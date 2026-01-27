/*
 * ============================================
 * REVERSE VENDING MACHINE (RVM) - Version 1.0
 * ============================================
 * 
 * Description:
 * A machine that accepts plastic and metal recyclables
 * in exchange for water credits. Users deposit materials
 * and earn points to redeem water.
 * 
 * Hardware:
 * - Arduino Microcontroller
 * - I2C LCD 20x4
 * - 4x SG90 Servo Motors (2 for shoot, 2 for flapper)
 * - LJ18A3-8-Z/BX NPN Inductive Proximity Sensor (Metal)
 * - LJC18A3-B-Z/BY PNP Capacitive Proximity Sensor (Plastic)
 * - MQ135 Gas Sensor (Biodegradable detection)
 * - Vibration Sensor (SW-420)
 * - Button Key Switch Sensor (Dispense button)
 * - Relay Module (Water pump control)
 * 
 * Author: RVM Development Team
 * Date: January 2026
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// ============================================
// PIN DEFINITIONS
// ============================================

// Sensor Pins
#define PIN_MQ135_SENSOR        A0  // Biodegradable/Gas detection (Analog)
#define PIN_INDUCTIVE_SENSOR    2   // Metal detection (NPN Inductive)
#define PIN_CAPACITIVE_SENSOR   3   // Plastic detection (Capacitive)
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

// Timing Configuration (in milliseconds)
const unsigned long SHOOT_OPEN_DELAY = 500;           // Delay after opening shoot
const unsigned long SHOOT_CLOSE_DELAY = 300;          // Delay after closing shoot
const unsigned long FLAPPER_MOVE_DELAY = 200;         // Delay for flapper movement
const unsigned long FLAPPER_RESET_DELAY = 500;        // Delay before flapper returns
const unsigned long WATER_DISPENSE_DURATION = 10000;   // Duration to dispense water (10 seconds)
const unsigned long SENSOR_READ_INTERVAL = 100;       // Interval between sensor readings
const unsigned long VIBRATION_TIMEOUT = 5000;         // Timeout waiting for vibration (5 seconds)
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
bool isMetalDetected();
bool isPlasticDetected();
bool isBiodegradableDetected();
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
  // Priority: Check for biodegradable first, then metal, then plastic
  if (isBiodegradableDetected()) {
    Serial.println(F("Biodegradable detected"));
    return MATERIAL_BIODEGRADABLE;
  }
  if (isMetalDetected()) {
    Serial.println(F("Metal detected"));
    return MATERIAL_METAL;
  }
  if (isPlasticDetected()) {
    Serial.println(F("Plastic detected"));
    return MATERIAL_PLASTIC;
  }
  return MATERIAL_NONE;
}

bool isMetalDetected() {
  // Inductive sensor: LOW when metal is detected (NPN type)
  return digitalRead(PIN_INDUCTIVE_SENSOR) == LOW;
}

bool isPlasticDetected() {
  // Capacitive sensor: HIGH when object detected (PNP type)
  // Note: Capacitive also detects metal, so check metal first
  return digitalRead(PIN_CAPACITIVE_SENSOR) == HIGH && !isMetalDetected();
}

bool isBiodegradableDetected() {
  // MQ135 sensor reads organic compounds/gases
  int gasValue = analogRead(PIN_MQ135_SENSOR);
  return gasValue > MQ135_BIODEGRADABLE_THRESHOLD;
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
  
  centerText(0, "REVERSE VENDING");
  centerText(1, "MACHINE");
  
  // Show credits and available water
  char line3[21];
  int availableWater = waterCredits / POINTS_REQUIRED_FOR_WATER;
  sprintf(line3, "Credits: %d", waterCredits);
  centerText(2, line3);
  
  char line4[21];
  if (availableWater > 0) {
    sprintf(line4, "Water Ready: %d", availableWater);
  } else {
    int needed = POINTS_REQUIRED_FOR_WATER - (waterCredits % POINTS_REQUIRED_FOR_WATER);
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
