/*
 * ============================================
 * REVERSE VENDING MACHINE (RVM) - Version 4.0
 * ============================================
 * 
 * Description:
 * A machine that accepts plastic and metal recyclables
 * in exchange for water credits. Users deposit materials
 * and earn points to redeem water.
 * 
 * Detection Algorithm (inspired by WiseWaste):
 * - IR sensor MUST detect object for any valid reading
 * - Gas sensor triggers → BIODEGRADABLE (rejected)
 * - Inductive sensor triggers → METAL (accepted)
 * - Only IR triggered → PLASTIC (accepted)
 * - Triple-check validation with configurable delay
 * 
 * Hardware:
 * - Arduino Microcontroller
 * - I2C LCD 20x4
 * - 4x SG90 Servo Motors (2 for shoot, 2 for flapper)
 * - LJ18A3-8-Z/BX NPN Inductive Proximity Sensor (Metal)
 * - LJC18A3-B-Z/BY PNP Capacitive Proximity Sensor (Plastic)
 * - MQ135 Gas Sensor (Biodegradable detection)
 * - TCRT5000 IR Sensor (Object presence detection)
 * - Vibration Sensor (SW-420)
 * - Button Key Switch Sensor (Dispense button)
 * - Relay Module (Water pump control)
 * 
 * Author: K1taru@github
 * Date: February 2026
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// ============================================
// PIN DEFINITIONS
// ============================================

// Sensor Pins
#define PIN_MQ135_SENSOR        A0  // Biodegradable/Gas detection (Analog)
#define PIN_TCRT5000_SENSOR     11  // TCRT5000 IR sensor for plastic detection (Digital)
#define PIN_INDUCTIVE_SENSOR    2   // Metal detection (NPN Inductive)
#define PIN_CAPACITIVE_SENSOR   3   // Capacitive proximity sensor
#define PIN_VIBRATION_SENSOR    4   // Vibration sensor on flapper
#define PIN_DISPENSE_BUTTON     5   // Water dispense button

// Servo Pins
#define PIN_SHOOT_SERVO_LEFT    6   // Left servo for shoot door
#define PIN_SHOOT_SERVO_RIGHT   7   // Right servo for shoot door
#define PIN_FLAPPER_SERVO_LEFT  8   // Left servo for flapper
#define PIN_FLAPPER_SERVO_RIGHT 9   // Right servo for flapper

// Relay/Pump Pin
#define PIN_WATER_PUMP_RELAY    10  // Relay for water pump

// ============================================
// GLOBAL CONFIGURATION VARIABLES
// ============================================

// Points System
int waterCredits = 0;                       // Current accumulated points
const int POINTS_PER_MATERIAL = 1;          // Points earned per valid material
const int POINTS_REQUIRED_FOR_WATER = 3;    // Points needed to dispense water

// Sensor Thresholds
const int MQ135_BIODEGRADABLE_THRESHOLD = 400;  // Analog threshold for biodegradable detection

// Triple-Check Detection Configuration
const unsigned long TRIPLE_CHECK_DELAY = 150;     // Delay between each check (ms) - adjustable
const int TRIPLE_CHECK_REQUIRED_COUNT = 3;        // Number of consistent detections required

// Sensor Debounce Configuration
const int SENSOR_STABLE_READINGS = 5;             // Number of consecutive stable readings required
const unsigned long SENSOR_DEBOUNCE_DELAY = 20;   // Delay between debounce readings (ms)

// Timing Configuration (in milliseconds)
const unsigned long SHOOT_OPEN_DELAY = 500;           // Delay after opening shoot
const unsigned long SHOOT_CLOSE_DELAY = 300;          // Delay after closing shoot
const unsigned long FLAPPER_MOVE_DELAY = 200;         // Delay for flapper movement
const unsigned long FLAPPER_RESET_DELAY = 500;        // Delay before flapper returns
const unsigned long WATER_DISPENSE_DURATION = 10000;   // Duration to dispense water (10 seconds)
const unsigned long SENSOR_READ_INTERVAL = 100;       // Interval between sensor readings
const unsigned long VIBRATION_TIMEOUT = 15000;         // Timeout waiting for vibration (15 seconds)
const unsigned long LCD_MESSAGE_DELAY = 2000;         // Duration for LCD messages
const unsigned long DEBOUNCE_DELAY = 50;              // Button debounce delay

// Servo Positions (in degrees)
const int SHOOT_SERVO_LEFT_CLOSED = 0;
const int SHOOT_SERVO_LEFT_OPEN = 90;
const int SHOOT_SERVO_RIGHT_CLOSED = 180;
const int SHOOT_SERVO_RIGHT_OPEN = 90;

const int FLAPPER_SERVO_LEFT_CENTER = 90;     // Center position (aligned with divider)
const int FLAPPER_SERVO_RIGHT_CENTER = 90;
const int FLAPPER_ANGLE_OFFSET = 45;          // Offset for directing materials

// Material Types
enum MaterialType {
  MATERIAL_NONE,
  MATERIAL_METAL,
  MATERIAL_PLASTIC,
  MATERIAL_BIODEGRADABLE
};

// ============================================
// OBJECT DECLARATIONS
// ============================================

// LCD Display (I2C address 0x27, 20 columns, 4 rows)
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Servo Motors
Servo shootServoLeft;
Servo shootServoRight;
Servo flapperServoLeft;
Servo flapperServoRight;

// ============================================
// STATE VARIABLES
// ============================================

bool isShootOpen = false;
bool isFlapperMoved = false;
unsigned long lastSensorRead = 0;
unsigned long vibrationStartTime = 0;
bool waitingForVibration = false;
MaterialType detectedMaterial = MATERIAL_NONE;

// ============================================
// FUNCTION PROTOTYPES
// ============================================

void initializeSystem();
void initializeLCD();
void initializeServos();
void initializeSensors();
MaterialType detectMaterial();
MaterialType detectMaterialRaw();
void printMaterialType(MaterialType material);
bool isMetalDetected();
bool isBiodegradableDetected();
bool isIRObjectDetected();
bool isVibrationDetected();
bool isDispenseButtonPressed();
void openShoot();
void closeShoot();
void moveFlapperForMetal();
void moveFlapperForPlastic();
void resetFlapper();
void dispenseWater();
void addCredit();
void updateLCDIdle();
void updateLCDDetection(MaterialType material);
void updateLCDProcessing();
void updateLCDDispensing();
void showMessage(const char* line1, const char* line2, const char* line3, const char* line4);
void centerText(int row, const char* text);

// ============================================
// SETUP FUNCTION
// ============================================

void setup() {
  Serial.begin(9600);
  Serial.println(F("RVM System Starting..."));
  
  initializeSystem();
  
  Serial.println(F("RVM System Ready!"));
}

// ============================================
// MAIN LOOP
// ============================================

void loop() {
  unsigned long currentMillis = millis();
  
  // Update idle display
  if (!waitingForVibration) {
    updateLCDIdle();
  }
  
  // Check for dispense button press
  if (isDispenseButtonPressed()) {
    if (waterCredits >= POINTS_REQUIRED_FOR_WATER) {
      dispenseWater();
    } else {
      showMessage("  NOT ENOUGH CREDITS", "", "    Need 3 points", "    to get water");
      delay(LCD_MESSAGE_DELAY);
    }
  }
  
  // Sensor reading with interval
  if (currentMillis - lastSensorRead >= SENSOR_READ_INTERVAL) {
    lastSensorRead = currentMillis;
    
    // If waiting for vibration (material in transit)
    if (waitingForVibration) {
      if (isVibrationDetected()) {
        // Material successfully deposited
        Serial.println(F("Vibration detected - Material deposited!"));
        addCredit();
        
        // Reset flapper and close shoot
        delay(FLAPPER_RESET_DELAY);
        resetFlapper();
        closeShoot();
        
        waitingForVibration = false;
        detectedMaterial = MATERIAL_NONE;
        
        showMessage("   THANK YOU!", "", "  Material accepted", "");
        delay(LCD_MESSAGE_DELAY);
        
      } else if (currentMillis - vibrationStartTime >= VIBRATION_TIMEOUT) {
        // Timeout - no material deposited
        Serial.println(F("Vibration timeout - No material deposited"));
        
        resetFlapper();
        closeShoot();
        
        waitingForVibration = false;
        detectedMaterial = MATERIAL_NONE;
        
        showMessage("    TIMEOUT", "", " No material detected", " Please try again");
        delay(LCD_MESSAGE_DELAY);
      }
    } else {
      // Normal operation - detect materials
      MaterialType material = detectMaterial();
      
      if (material != MATERIAL_NONE) {
        detectedMaterial = material;
        updateLCDDetection(material);
        delay(LCD_MESSAGE_DELAY);
        
        if (material == MATERIAL_BIODEGRADABLE) {
          // Reject biodegradable materials
          showMessage("     REJECTED!", "", "  Biodegradables", "  not accepted");
          delay(LCD_MESSAGE_DELAY);
          detectedMaterial = MATERIAL_NONE;
          
        } else {
          // Valid material - process it
          updateLCDProcessing();
          openShoot();
          delay(SHOOT_OPEN_DELAY);
          
          if (material == MATERIAL_METAL) {
            moveFlapperForMetal();
            Serial.println(F("Flapper moved for METAL (right bin)"));
          } else {
            moveFlapperForPlastic();
            Serial.println(F("Flapper moved for PLASTIC (left bin)"));
          }
          
          // Start waiting for vibration
          waitingForVibration = true;
          vibrationStartTime = currentMillis;
          
          showMessage(" PLEASE INSERT YOUR", "   ITEM NOW", "", "  Waiting...");
        }
      }
    }
  }
}

// ============================================
// INITIALIZATION FUNCTIONS
// ============================================

void initializeSystem() {
  initializeLCD();
  initializeServos();
  initializeSensors();
  
  // Show startup message
  showMessage("    REVERSE", "    VENDING", "    MACHINE", "   Starting...");
  delay(2000);
  
  // Close shoot and center flapper
  closeShoot();
  resetFlapper();
}

void initializeLCD() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  Serial.println(F("LCD Initialized"));
}

void initializeServos() {
  shootServoLeft.attach(PIN_SHOOT_SERVO_LEFT);
  shootServoRight.attach(PIN_SHOOT_SERVO_RIGHT);
  flapperServoLeft.attach(PIN_FLAPPER_SERVO_LEFT);
  flapperServoRight.attach(PIN_FLAPPER_SERVO_RIGHT);
  Serial.println(F("Servos Initialized"));
}

void initializeSensors() {
  pinMode(PIN_INDUCTIVE_SENSOR, INPUT);
  pinMode(PIN_CAPACITIVE_SENSOR, INPUT);
  pinMode(PIN_TCRT5000_SENSOR, INPUT);      // IR sensor digital input
  pinMode(PIN_VIBRATION_SENSOR, INPUT);
  pinMode(PIN_DISPENSE_BUTTON, INPUT_PULLUP);
  pinMode(PIN_WATER_PUMP_RELAY, OUTPUT);
  
  // Ensure pump is off initially
  digitalWrite(PIN_WATER_PUMP_RELAY, LOW);
  
  Serial.println(F("Sensors Initialized"));
}

// ============================================
// SENSOR FUNCTIONS
// ============================================

MaterialType detectMaterial() {
  // Triple-check detection algorithm (inspired by WiseWaste)
  // Requires 3 consistent readings with delay between each check
  // IR sensor MUST be triggered for any valid detection
  
  MaterialType firstCheck = detectMaterialRaw();
  
  // If no object detected by IR, return immediately
  if (firstCheck == MATERIAL_NONE) {
    return MATERIAL_NONE;
  }
  
  Serial.print(F("Check 1/3: "));
  printMaterialType(firstCheck);
  
  // Wait before second check
  delay(TRIPLE_CHECK_DELAY);
  
  MaterialType secondCheck = detectMaterialRaw();
  
  // If second check doesn't match or no object, abort
  if (secondCheck != firstCheck) {
    Serial.println(F("Check 2/3: MISMATCH - Detection aborted"));
    return MATERIAL_NONE;
  }
  
  Serial.print(F("Check 2/3: "));
  printMaterialType(secondCheck);
  
  // Wait before third check
  delay(TRIPLE_CHECK_DELAY);
  
  MaterialType thirdCheck = detectMaterialRaw();
  
  // If third check doesn't match, abort
  if (thirdCheck != firstCheck) {
    Serial.println(F("Check 3/3: MISMATCH - Detection aborted"));
    return MATERIAL_NONE;
  }
  
  Serial.print(F("Check 3/3: "));
  printMaterialType(thirdCheck);
  
  // All 3 checks passed with consistent readings
  Serial.print(F("Material CONFIRMED after triple-check: "));
  printMaterialType(thirdCheck);
  
  return thirdCheck;
}

void printMaterialType(MaterialType material) {
  switch (material) {
    case MATERIAL_METAL: Serial.println(F("METAL")); break;
    case MATERIAL_PLASTIC: Serial.println(F("PLASTIC")); break;
    case MATERIAL_BIODEGRADABLE: Serial.println(F("BIODEGRADABLE")); break;
    default: Serial.println(F("NONE")); break;
  }
}

MaterialType detectMaterialRaw() {
  // Raw single detection - inspired by WiseWaste algorithm
  // IR sensor MUST be triggered for any valid reading
  // Capacitive sensor is PNP type: LOW when object detected, HIGH when no object
  // Priority: Gas → (Capacitive + Metal combo) → Metal → Plastic
  
  // First, check if IR sensor detects an object
  bool objectDetected = isIRObjectDetected();
  
  if (!objectDetected) {
    // No object present - return NONE
    return MATERIAL_NONE;
  }
  
  // Object is present (IR triggered)
  // Now independently read other sensors
  bool gasDetected = isBiodegradableDetected();
  bool metalDetected = isMetalDetected();
  bool capacitiveDetected = isCapacitiveDetected();  // LOW (0) when object detected
  
  // Classification logic:
  
  // Priority 1: Gas sensor override - if gas detected, it's biodegradable
  if (gasDetected) {
    return MATERIAL_BIODEGRADABLE;
  }
  
  // Priority 2: Capacitive sensor logic (detects conductive/organic materials)
  if (capacitiveDetected) {
    // Capacitive is true (object detected)
    
    if (metalDetected) {
      // Both capacitive AND metal detected → METAL
      return MATERIAL_METAL;
    } else {
      // Capacitive detected but NO metal → Biodegradable (no smell but organic/conductive)
      return MATERIAL_BIODEGRADABLE;
    }
  }
  
  // Priority 3: Metal sensor alone
  if (metalDetected) {
    return MATERIAL_METAL;
  }
  
  // Priority 4: If only IR triggered (no gas, no capacitive, no metal) → Plastic
  // This is the default for non-metal, non-biodegradable, non-conductive objects
  return MATERIAL_PLASTIC;
}

bool isMetalDetected() {
  // Inductive sensor: LOW when metal is detected (NPN type)
  // Use debouncing for stable reading
  
  int detectCount = 0;
  
  for (int i = 0; i < SENSOR_STABLE_READINGS; i++) {
    if (digitalRead(PIN_INDUCTIVE_SENSOR) == LOW) {
      detectCount++;
    }
    if (i < SENSOR_STABLE_READINGS - 1) {
      delay(SENSOR_DEBOUNCE_DELAY);
    }
  }
  
  // Metal detected only if most readings show detection (threshold: 80%)
  return detectCount >= (SENSOR_STABLE_READINGS * 4 / 5);
}

bool isCapacitiveDetected() {
  // Capacitive sensor: LOW when object detected (PNP type)
  // Returns true when object is present
  // Use debouncing for stable reading
  
  int detectCount = 0;
  
  for (int i = 0; i < SENSOR_STABLE_READINGS; i++) {
    if (digitalRead(PIN_CAPACITIVE_SENSOR) == LOW) {
      detectCount++;
    }
    if (i < SENSOR_STABLE_READINGS - 1) {
      delay(SENSOR_DEBOUNCE_DELAY);
    }
  }
  
  // Object detected only if most readings show detection (threshold: 80%)
  return detectCount >= (SENSOR_STABLE_READINGS * 4 / 5);
}

bool isIRObjectDetected() {
  // TCRT5000 IR sensor (Digital): LOW when object is detected
  // This is the PRIMARY sensor - must be triggered for any valid detection
  // Use debouncing to ensure stable reading and avoid false triggers
  
  int detectCount = 0;
  
  // Take multiple readings to ensure stability
  for (int i = 0; i < SENSOR_STABLE_READINGS; i++) {
    if (digitalRead(PIN_TCRT5000_SENSOR) == LOW) {
      detectCount++;
    }
    if (i < SENSOR_STABLE_READINGS - 1) {
      delay(SENSOR_DEBOUNCE_DELAY);
    }
  }
  
  // Object detected only if ALL readings show detection
  // This prevents noise and false triggers
  return detectCount == SENSOR_STABLE_READINGS;
}

bool isBiodegradableDetected() {
  // MQ135 sensor reads organic compounds/gases
  // Automatically flags object as biodegradable if threshold exceeded
  // Use averaging to reduce noise
  
  long gasSum = 0;
  
  for (int i = 0; i < SENSOR_STABLE_READINGS; i++) {
    gasSum += analogRead(PIN_MQ135_SENSOR);
    if (i < SENSOR_STABLE_READINGS - 1) {
      delay(SENSOR_DEBOUNCE_DELAY);
    }
  }
  
  int gasAverage = gasSum / SENSOR_STABLE_READINGS;
  return gasAverage > MQ135_BIODEGRADABLE_THRESHOLD;
}

bool isVibrationDetected() {
  return digitalRead(PIN_VIBRATION_SENSOR) == HIGH;
}

bool isDispenseButtonPressed() {
  static bool lastButtonState = HIGH;
  static unsigned long lastDebounceTime = 0;
  
  bool reading = digitalRead(PIN_DISPENSE_BUTTON);
  
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading == LOW && lastButtonState == HIGH) {
      lastButtonState = reading;
      return true;
    }
  }
  
  lastButtonState = reading;
  return false;
}

// ============================================
// SERVO CONTROL FUNCTIONS
// ============================================

void openShoot() {
  shootServoLeft.write(SHOOT_SERVO_LEFT_OPEN);
  shootServoRight.write(SHOOT_SERVO_RIGHT_OPEN);
  isShootOpen = true;
  Serial.println(F("Shoot OPENED"));
  delay(SHOOT_OPEN_DELAY);
}

void closeShoot() {
  shootServoLeft.write(SHOOT_SERVO_LEFT_CLOSED);
  shootServoRight.write(SHOOT_SERVO_RIGHT_CLOSED);
  isShootOpen = false;
  Serial.println(F("Shoot CLOSED"));
  delay(SHOOT_CLOSE_DELAY);
}

void moveFlapperForMetal() {
  // Move flapper -45 degrees (right bin)
  flapperServoLeft.write(FLAPPER_SERVO_LEFT_CENTER - FLAPPER_ANGLE_OFFSET);
  flapperServoRight.write(FLAPPER_SERVO_RIGHT_CENTER - FLAPPER_ANGLE_OFFSET);
  isFlapperMoved = true;
  delay(FLAPPER_MOVE_DELAY);
}

void moveFlapperForPlastic() {
  // Move flapper +45 degrees (left bin)
  flapperServoLeft.write(FLAPPER_SERVO_LEFT_CENTER + FLAPPER_ANGLE_OFFSET);
  flapperServoRight.write(FLAPPER_SERVO_RIGHT_CENTER + FLAPPER_ANGLE_OFFSET);
  isFlapperMoved = true;
  delay(FLAPPER_MOVE_DELAY);
}

void resetFlapper() {
  // Return flapper to center position (90 degrees)
  flapperServoLeft.write(FLAPPER_SERVO_LEFT_CENTER);
  flapperServoRight.write(FLAPPER_SERVO_RIGHT_CENTER);
  isFlapperMoved = false;
  Serial.println(F("Flapper RESET to center"));
  delay(FLAPPER_MOVE_DELAY);
}

// ============================================
// WATER DISPENSE FUNCTION
// ============================================

void dispenseWater() {
  // Deduct credits
  waterCredits -= POINTS_REQUIRED_FOR_WATER;
  Serial.print(F("Dispensing water! Credits remaining: "));
  Serial.println(waterCredits);
  
  // Show dispensing message
  updateLCDDispensing();
  
  // Activate water pump
  digitalWrite(PIN_WATER_PUMP_RELAY, HIGH);
  delay(WATER_DISPENSE_DURATION);
  digitalWrite(PIN_WATER_PUMP_RELAY, LOW);
  
  showMessage("    ENJOY YOUR", "      WATER!", "", "   Thank you!");
  delay(LCD_MESSAGE_DELAY);
}

// ============================================
// CREDIT SYSTEM FUNCTIONS
// ============================================

void addCredit() {
  waterCredits += POINTS_PER_MATERIAL;
  Serial.print(F("Credit added! Total credits: "));
  Serial.println(waterCredits);
  
  // Check if user can claim water
  if (waterCredits >= POINTS_REQUIRED_FOR_WATER) {
    int availableWater = waterCredits / POINTS_REQUIRED_FOR_WATER;
    Serial.print(F("Water available: "));
    Serial.println(availableWater);
  }
}

// ============================================
// LCD DISPLAY FUNCTIONS
// ============================================

void updateLCDIdle() {
  lcd.clear();
  
  // Line 1: Title
  centerText(0, "RVM");
  
  // Line 2: Credits and progress (X/n)
  char line2[21];
  int progress = waterCredits % POINTS_REQUIRED_FOR_WATER;
  int availableWater = waterCredits / POINTS_REQUIRED_FOR_WATER;
  sprintf(line2, "Credits:%d  (%d/%d)", waterCredits, progress, POINTS_REQUIRED_FOR_WATER);
  centerText(1, line2);
  
  // Line 3: Prompt
  if (availableWater > 0) {
    centerText(2, "Water Ready!");
  } else {
    centerText(2, "Please scan item");
  }
  
  // Line 4: Additional info
  char line4[21];
  if (availableWater > 0) {
    sprintf(line4, "Press to dispense");
  } else {
    int needed = POINTS_REQUIRED_FOR_WATER - progress;
    if (needed == 0) needed = POINTS_REQUIRED_FOR_WATER;
    sprintf(line4, "Need %d more items", needed);
  }
  centerText(3, line4);
}

void updateLCDDetection(MaterialType material) {
  lcd.clear();
  
  centerText(0, "MATERIAL DETECTED!");
  
  switch (material) {
    case MATERIAL_METAL:
      centerText(1, "Type: METAL");
      centerText(2, "Status: ACCEPTED");
      break;
    case MATERIAL_PLASTIC:
      centerText(1, "Type: PLASTIC");
      centerText(2, "Status: ACCEPTED");
      break;
    case MATERIAL_BIODEGRADABLE:
      centerText(1, "Type: ORGANIC");
      centerText(2, "Status: REJECTED");
      break;
    default:
      centerText(1, "Type: UNKNOWN");
      break;
  }
  
  centerText(3, "");
}

void updateLCDProcessing() {
  lcd.clear();
  centerText(0, "PROCESSING...");
  centerText(1, "");
  centerText(2, "Opening shoot");
  centerText(3, "Please wait...");
}

void updateLCDDispensing() {
  lcd.clear();
  centerText(0, "DISPENSING WATER");
  centerText(1, "");
  centerText(2, "Please wait...");
  centerText(3, "");
}

void showMessage(const char* line1, const char* line2, const char* line3, const char* line4) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  lcd.setCursor(0, 2);
  lcd.print(line3);
  lcd.setCursor(0, 3);
  lcd.print(line4);
}

void centerText(int row, const char* text) {
  int len = strlen(text);
  int padding = (20 - len) / 2;
  
  lcd.setCursor(0, row);
  
  // Clear the row first
  lcd.print("                    ");
  
  // Position and print centered text
  lcd.setCursor(padding > 0 ? padding : 0, row);
  lcd.print(text);
}
