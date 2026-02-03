/*
 * ============================================
 * SENSOR TEST PROGRAM FOR RVM
 * ============================================
 * 
 * Tests the following sensors:
 * - MQ135 Gas Sensor (Analog)
 * - TCRT5000 IR Sensor (Analog)
 * - LJ18A3 Inductive Proximity Sensor (Digital)
 * - LJC18A3 Capacitive Proximity Sensor (Digital)
 * 
 * Displays real-time readings on 20x4 LCD
 * 
 * Author: K1taru@github
 * Date: February 2026
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ============================================
// PIN DEFINITIONS (Same as RVM_v2.ino)
// ============================================

#define PIN_MQ135_SENSOR        A0  // Gas sensor (Analog)
#define PIN_TCRT5000_SENSOR     11  // IR sensor (Digital)
#define PIN_INDUCTIVE_SENSOR    2   // Metal detection (Digital)
#define PIN_CAPACITIVE_SENSOR   3   // Capacitive sensor (Digital)

// ============================================
// CONFIGURATION
// ============================================

const unsigned long UPDATE_INTERVAL = 200;  // LCD update interval (ms)
const int ANALOG_DISCONNECT_THRESHOLD = 10; // If reading < this, likely disconnected
const int ANALOG_MAX_THRESHOLD = 1020;      // If reading > this, might be disconnected

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
  pinMode(PIN_TCRT5000_SENSOR, INPUT);
  pinMode(PIN_INDUCTIVE_SENSOR, INPUT);
  pinMode(PIN_CAPACITIVE_SENSOR, INPUT);
  
  // Show startup message
  lcd.setCursor(0, 0);
  lcd.print("  SENSOR TEST MODE  ");
  lcd.setCursor(0, 1);
  lcd.print("   Initializing...  ");
  delay(2000);
  
  Serial.println(F("Sensors initialized"));
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
    
    // Read all sensors
    int gasValue = analogRead(PIN_MQ135_SENSOR);
    int irState = digitalRead(PIN_TCRT5000_SENSOR);
    int inductiveState = digitalRead(PIN_INDUCTIVE_SENSOR);
    int capacitiveState = digitalRead(PIN_CAPACITIVE_SENSOR);
    
    // Update LCD display
    updateLCD(gasValue, irState, inductiveState, capacitiveState);
    
    // Print to Serial Monitor
    printToSerial(gasValue, irState, inductiveState, capacitiveState);
  }
}

// ============================================
// DISPLAY FUNCTIONS
// ============================================

void updateLCD(int gasValue, int irValue, int inductiveState, int capacitiveState) {
  lcd.clear();
  
  // Line 0: Gas Sensor (MQ135)
  lcd.setCursor(0, 0);
  lcd.print("Gas:");
  if (isAnalogDisconnected(gasValue)) {
    lcd.print("No MQ135      ");
  } else {
    lcd.print(gasValue);
    lcd.print("    ");  // Clear remaining chars
  }
  
  // Line 1: IR Sensor (TCRT5000)
  lcd.setCursor(0, 1);
  lcd.print("IR:");
  lcd.print(irState == HIGH ? "HIGH  " : "LOW   ");
  
  // Line 2: Inductive Sensor
  lcd.setCursor(0, 2);
  lcd.print("Induct:");
  lcd.print(inductiveState == HIGH ? "HIGH " : "LOW  ");
  
  // Line 3: Capacitive Sensor
  lcd.setCursor(0, 3);
  lcd.print("Capac:");
  lcd.print(capacitiveState == HIGH ? "HIGH " : "LOW  ");
}

void printToSerial(int gasValue, int irValue, int inductiveState, int capacitiveState) {
  Serial.println(F("--- Sensor Readings ---"));
  
  // Gas Sensor
  Serial.print(F("MQ135 Gas:     "));
  if (isAnalogDisconnected(gasValue)) {
    Serial.println(F("NOT DETECTED"));
  } else {
    Serial.print(gasValue);
    Serial.println(F(" (0-1023)"));
  }
  
  // IR Sensor
  Serial.print(F("TCRT5000 IR:   "));
  Serial.println(irState == HIGH ? F("HIGH") : F("LOW"));
  
  // Inductive Sensor
  Serial.print(F("Inductive:     "));
  Serial.println(inductiveState == HIGH ? F("HIGH") : F("LOW (Metal if LOW)"));
  
  // Capacitive Sensor
  Serial.print(F("Capacitive:    "));
  Serial.println(capacitiveState == HIGH ? F("HIGH (Object detected)") : F("LOW"));
  
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
