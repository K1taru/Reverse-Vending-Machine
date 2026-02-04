/*
 * ============================================
 * SENSOR TEST PROGRAM FOR RVM v5
 * ============================================
 * 
 * Tests the following sensors with noise reduction:
 * - MQ135 Gas Sensor (Analog) - averaged readings
 * - TCRT5000 IR Sensor (Digital) - debounced with filtering
 * - LJ18A3 Inductive Proximity Sensor (Digital NPN) - debounced
 * - LJC18A3 Capacitive Proximity Sensor (Digital PNP) - debounced
 * 
 * Displays real-time readings on 20x4 LCD
 * Shows both raw and filtered values for debugging
 * 
 * Author: K1taru@github
 * Date: February 2026
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ============================================
// PIN DEFINITIONS (Same as RVM_v5.ino)
// ============================================
// Pin layout optimized to reduce electrical noise

#define PIN_MQ135_SENSOR        A0  // Gas sensor (Analog)
#define PIN_CAPACITIVE_SENSOR   2   // Capacitive sensor (Digital PNP - normally HIGH)
#define PIN_INDUCTIVE_SENSOR    4   // Metal detection (Digital NPN - normally HIGH)
#define PIN_TCRT5000_SENSOR     12  // IR sensor (Digital - normally HIGH)

// ============================================
// CONFIGURATION
// ============================================

const unsigned long UPDATE_INTERVAL = 500;          // LCD update interval (ms)
const int ANALOG_DISCONNECT_THRESHOLD = 10;         // If reading < this, likely disconnected
const int ANALOG_MAX_THRESHOLD = 1020;              // If reading > this, might be disconnected

// Noise Reduction Configuration (same as RVM_v5)
const int SENSOR_STABLE_READINGS = 7;               // Number of consecutive readings
const unsigned long SENSOR_DEBOUNCE_DELAY = 15;     // Delay between readings (ms)
const int NOISE_FILTER_THRESHOLD = 5;               // Minimum consistent readings (out of 7)

// ============================================
// GLOBAL OBJECTS
// ============================================

LiquidCrystal_I2C lcd(0x27, 20, 4);

// ============================================
// SETUP
// ============================================

void setup() {
  Serial.begin(9600);
  Serial.println(F("=== RVM Sensor Test ==="));
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  // Initialize sensor pins
  pinMode(PIN_CAPACITIVE_SENSOR, INPUT);
  pinMode(PIN_INDUCTIVE_SENSOR, INPUT);
  pinMode(PIN_TCRT5000_SENSOR, INPUT);
  
  // Show startup message
  lcd.setCursor(0, 0);
  lcd.print("  SENSOR TEST v5.0  ");
  lcd.setCursor(0, 1);
  lcd.print("   Initializing...  ");
  lcd.setCursor(0, 2);
  lcd.print(" Stabilizing sensors");
  delay(1000);  // Sensor stabilization (noise reduction)
  
  lcd.setCursor(0, 2);
  lcd.print("                    ");
  lcd.setCursor(0, 2);
  lcd.print("      Ready!        ");
  delay(1000);
  
  Serial.println(F("Sensors initialized with noise reduction"));
  Serial.println(F("Pin Layout: Cap(D2), Ind(D4), IR(D12), Gas(A0)"));
  Serial.println(F("Starting continuous monitoring...\n"));
}

// ============================================
// MAIN LOOP
// ============================================

void loop() {
  static unsigned long lastUpdate = 0;
  unsigned long currentMillis = millis();
  
  // Update display at set interval
  if (currentMillis - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = currentMillis;
    
    // Read all sensors with noise filtering
    int gasValue = readGasSensorFiltered();
    bool irDetected = readIRSensorFiltered();
    bool inductiveDetected = readInductiveSensorFiltered();
    bool capacitiveDetected = readCapacitiveSensorFiltered();
    
    // Update LCD display
    updateLCD(gasValue, irDetected, inductiveDetected, capacitiveDetected);
    
    // Print to Serial Monitor
    printToSerial(gasValue, irDetected, inductiveDetected, capacitiveDetected);
  }
}

// ============================================
// DISPLAY FUNCTIONS
// ============================================

void updateLCD(int gasValue, bool irDetected, bool inductiveDetected, bool capacitiveDetected) {
  lcd.clear();
  
  // Line 0: Gas Sensor (MQ135) - averaged
  lcd.setCursor(0, 0);
  lcd.print("Gas(avg):");
  if (isAnalogDisconnected(gasValue)) {
    lcd.print("DISC    ");
  } else {
    lcd.print(gasValue);
    lcd.print("    ");  // Clear remaining chars
  }
  
  // Line 1: Capacitive Sensor (PNP)
  lcd.setCursor(0, 1);
  lcd.print("Cap(D2):");
  lcd.print(capacitiveDetected ? "DETECT" : "NONE  ");
  
  // Line 2: Inductive Sensor (NPN)
  lcd.setCursor(0, 2);
  lcd.print("Ind(D4):");
  lcd.print(inductiveDetected ? "METAL " : "NONE  ");
  
  // Line 3: IR Sensor
  lcd.setCursor(0, 3);
  lcd.print("IR(D12):");
  lcd.print(irDetected ? "DETECT" : "NONE  ");
}

void printToSerial(int gasValue, bool irDetected, bool inductiveDetected, bool capacitiveDetected) {
  Serial.println(F("=== Filtered Sensor Readings (7-sample avg/debounce) ==="));
  
  // Gas Sensor
  Serial.print(F("MQ135 Gas (A0):       "));
  if (isAnalogDisconnected(gasValue)) {
    Serial.println(F("DISCONNECTED"));
  } else {
    Serial.print(gasValue);
    Serial.print(F(" (avg of 7) "));
    Serial.println(gasValue > 400 ? F("[BIODEGRADABLE]") : F("[Clean air]"));
  }
  
  // Capacitive Sensor (PNP)
  Serial.print(F("Capacitive (D2) PNP:  "));
  Serial.println(capacitiveDetected ? F("OBJECT DETECTED") : F("No object"));
  
  // Inductive Sensor (NPN)
  Serial.print(F("Inductive (D4) NPN:   "));
  Serial.println(inductiveDetected ? F("METAL DETECTED") : F("No metal"));
  
  // IR Sensor
  Serial.print(F("IR Sensor (D12):      "));
  Serial.println(irDetected ? F("OBJECT DETECTED") : F("No object"));
  
  Serial.println();
}

// ============================================
// HELPER FUNCTIONS
// ============================================

bool isAnalogDisconnected(int value) {
  // Check if sensor might be disconnected
  // Disconnected sensors often read 0 or very low values
  // Some might float to max value
  return (value < ANALOG_DISCONNECT_THRESHOLD);
}

// ============================================
// NOISE FILTERING FUNCTIONS (same as RVM_v5)
// ============================================

int readGasSensorFiltered() {
  // Average multiple readings to reduce noise
  long gasSum = 0;
  
  for (int i = 0; i < SENSOR_STABLE_READINGS; i++) {
    gasSum += analogRead(PIN_MQ135_SENSOR);
    if (i < SENSOR_STABLE_READINGS - 1) {
      delay(SENSOR_DEBOUNCE_DELAY);
    }
  }
  
  return gasSum / SENSOR_STABLE_READINGS;
}

bool readCapacitiveSensorFiltered() {
  // PNP type: Normally HIGH, goes LOW when object detected
  // Count how many readings show LOW (detection)
  int detectCount = 0;
  
  for (int i = 0; i < SENSOR_STABLE_READINGS; i++) {
    if (digitalRead(PIN_CAPACITIVE_SENSOR) == LOW) {
      detectCount++;
    }
    if (i < SENSOR_STABLE_READINGS - 1) {
      delay(SENSOR_DEBOUNCE_DELAY);
    }
  }
  
  // Object detected if sufficient readings agree (noise filtering)
  return detectCount >= NOISE_FILTER_THRESHOLD;
}

bool readInductiveSensorFiltered() {
  // NPN type: Normally HIGH, goes LOW when metal detected
  // Count how many readings show LOW (metal detection)
  int detectCount = 0;
  
  for (int i = 0; i < SENSOR_STABLE_READINGS; i++) {
    if (digitalRead(PIN_INDUCTIVE_SENSOR) == LOW) {
      detectCount++;
    }
    if (i < SENSOR_STABLE_READINGS - 1) {
      delay(SENSOR_DEBOUNCE_DELAY);
    }
  }
  
  // Metal detected if sufficient readings agree (noise filtering)
  return detectCount >= NOISE_FILTER_THRESHOLD;
}

bool readIRSensorFiltered() {
  // Normally HIGH, goes LOW when object detected
  // Use stricter threshold (6 out of 7) for primary sensor
  int detectCount = 0;
  
  for (int i = 0; i < SENSOR_STABLE_READINGS; i++) {
    if (digitalRead(PIN_TCRT5000_SENSOR) == LOW) {
      detectCount++;
    }
    if (i < SENSOR_STABLE_READINGS - 1) {
      delay(SENSOR_DEBOUNCE_DELAY);
    }
  }
  
  // Object detected with strict filtering (allow 1 noise spike)
  return detectCount >= (SENSOR_STABLE_READINGS - 1);
}
