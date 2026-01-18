/*
 * REVERSE VENDING MACHINE (RVM) v1.0 - Optimized
 * Accepts plastic/metal recyclables for water credits
 * Author: RVM Development Team | Jan 2026
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// PIN DEFINITIONS
const uint8_t P_MQ135 = A0, P_INDUCT = 2, P_CAPAC = 3, P_VIBR = 4, P_BTN = 5;
const uint8_t P_SRV_SL = 6, P_SRV_SR = 7, P_SRV_FL = 8, P_SRV_FR = 9, P_PUMP = 10;

// CONFIGURATION
uint8_t credits = 0;
const uint8_t PTS_PER_ITEM = 1, PTS_FOR_WATER = 3;
const uint16_t MQ135_THRESH = 400;

// TIMING (ms)
const uint16_t T_SHOOT_OPEN = 500, T_SHOOT_CLOSE = 300, T_FLAP_MOVE = 200;
const uint16_t T_FLAP_RESET = 500, T_WATER = 10000, T_SENSE = 100;
const uint16_t T_VIB_TIMEOUT = 5000, T_MSG = 2000, T_DEBOUNCE = 50;

// SERVO POSITIONS
const uint8_t SRV_SL_CLOSE = 0, SRV_SL_OPEN = 90, SRV_SR_CLOSE = 180, SRV_SR_OPEN = 90;
const uint8_t SRV_FLAP_CTR = 90, SRV_FLAP_OFS = 45;

// MATERIAL TYPES
enum MatType : uint8_t { MAT_NONE, MAT_METAL, MAT_PLASTIC, MAT_BIO };

// OBJECTS
LiquidCrystal_I2C lcd(0x27, 20, 4);
Servo srvSL, srvSR, srvFL, srvFR;

// STATE
uint32_t lastRead = 0, vibStart = 0;
bool waitVib = false;
MatType detMat = MAT_NONE;

// FUNCTION PROTOTYPES
void initSys(), initLCD(), initServos(), initSensors();
MatType detectMat();
bool isMetal(), isPlastic(), isBio(), isVibration(), isBtnPressed();
void openShoot(), closeShoot(), flapMetal(), flapPlastic(), resetFlap();
void dispenseWater(), addCredit();
void lcdIdle(), lcdDetect(MatType m), lcdProcess(), lcdDispense();
void showMsg(const char* l1, const char* l2, const char* l3, const char* l4);
void centerTxt(uint8_t row, const char* txt);

// SETUP
void setup() {
  Serial.begin(9600);
  Serial.println(F("RVM Starting..."));
  initSys();
  Serial.println(F("RVM Ready!"));
}

// MAIN LOOP
void loop() {
  uint32_t now = millis();
  
  if (!waitVib) lcdIdle();
  
  if (isBtnPressed()) {
    if (credits >= PTS_FOR_WATER) {
      dispenseWater();
    } else {
      showMsg("  NOT ENOUGH CREDITS", "", "    Need 3 points", "    to get water");
      delay(T_MSG);
    }
  }
  
  if (now - lastRead >= T_SENSE) {
    lastRead = now;
    
    if (waitVib) {
      if (isVibration()) {
        Serial.println(F("Material deposited!"));
        addCredit();
        delay(T_FLAP_RESET);
        resetFlap();
        closeShoot();
        waitVib = false;
        detMat = MAT_NONE;
        showMsg("   THANK YOU!", "", "  Material accepted", "");
        delay(T_MSG);
      } else if (now - vibStart >= T_VIB_TIMEOUT) {
        Serial.println(F("Timeout - No material"));
        resetFlap();
        closeShoot();
        waitVib = false;
        detMat = MAT_NONE;
        showMsg("    TIMEOUT", "", " No material detected", " Please try again");
        delay(T_MSG);
      }
    } else {
      MatType mat = detectMat();
      if (mat != MAT_NONE) {
        detMat = mat;
        lcdDetect(mat);
        delay(T_MSG);
        
        if (mat == MAT_BIO) {
          showMsg("     REJECTED!", "", "  Biodegradables", "  not accepted");
          delay(T_MSG);
          detMat = MAT_NONE;
        } else {
          lcdProcess();
          openShoot();
          delay(T_SHOOT_OPEN);
          
          if (mat == MAT_METAL) {
            flapMetal();
            Serial.println(F("Flapper: METAL"));
          } else {
            flapPlastic();
            Serial.println(F("Flapper: PLASTIC"));
          }
          
          waitVib = true;
          vibStart = now;
          showMsg(" PLEASE INSERT YOUR", "   ITEM NOW", "", "  Waiting...");
        }
      }
    }
  }
}

// INITIALIZATION
void initSys() {
  initLCD();
  initServos();
  initSensors();
  showMsg("    REVERSE", "    VENDING", "    MACHINE", "   Starting...");
  delay(2000);
  closeShoot();
  resetFlap();
}

void initLCD() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void initServos() {
  srvSL.attach(P_SRV_SL);
  srvSR.attach(P_SRV_SR);
  srvFL.attach(P_SRV_FL);
  srvFR.attach(P_SRV_FR);
}

void initSensors() {
  pinMode(P_INDUCT, INPUT);
  pinMode(P_CAPAC, INPUT);
  pinMode(P_VIBR, INPUT);
  pinMode(P_BTN, INPUT_PULLUP);
  pinMode(P_PUMP, OUTPUT);
  digitalWrite(P_PUMP, LOW);
}

// SENSORS
MatType detectMat() {
  if (isBio()) return MAT_BIO;
  if (isMetal()) return MAT_METAL;
  if (isPlastic()) return MAT_PLASTIC;
  return MAT_NONE;
}

bool isMetal() { return digitalRead(P_INDUCT) == LOW; }
bool isPlastic() { return digitalRead(P_CAPAC) == LOW && !isMetal(); }
bool isBio() { return analogRead(P_MQ135) > MQ135_THRESH; }
bool isVibration() { return digitalRead(P_VIBR) == HIGH; }

bool isBtnPressed() {
  static bool lastState = HIGH;
  static uint32_t lastDebounce = 0;
  bool reading = digitalRead(P_BTN);
  
  if (reading != lastState) lastDebounce = millis();
  
  if ((millis() - lastDebounce) > T_DEBOUNCE) {
    if (reading == LOW && lastState == HIGH) {
      lastState = reading;
      return true;
    }
  }
  lastState = reading;
  return false;
}

// SERVO CONTROL
void openShoot() {
  srvSL.write(SRV_SL_OPEN);
  srvSR.write(SRV_SR_OPEN);
  delay(T_SHOOT_OPEN);
}

void closeShoot() {
  srvSL.write(SRV_SL_CLOSE);
  srvSR.write(SRV_SR_CLOSE);
  delay(T_SHOOT_CLOSE);
}

void flapMetal() {
  srvFL.write(SRV_FLAP_CTR - SRV_FLAP_OFS);
  srvFR.write(SRV_FLAP_CTR - SRV_FLAP_OFS);
  delay(T_FLAP_MOVE);
}

void flapPlastic() {
  srvFL.write(SRV_FLAP_CTR + SRV_FLAP_OFS);
  srvFR.write(SRV_FLAP_CTR + SRV_FLAP_OFS);
  delay(T_FLAP_MOVE);
}

void resetFlap() {
  srvFL.write(SRV_FLAP_CTR);
  srvFR.write(SRV_FLAP_CTR);
  delay(T_FLAP_MOVE);
}

// WATER DISPENSE
void dispenseWater() {
  credits -= PTS_FOR_WATER;
  lcdDispense();
  digitalWrite(P_PUMP, HIGH);
  delay(T_WATER);
  digitalWrite(P_PUMP, LOW);
  showMsg("    ENJOY YOUR", "      WATER!", "", "   Thank you!");
  delay(T_MSG);
}

// CREDIT SYSTEM
void addCredit() {
  credits += PTS_PER_ITEM;
}

// LCD FUNCTIONS
void lcdIdle() {
  lcd.clear();
  centerTxt(0, "REVERSE VENDING");
  centerTxt(1, "MACHINE");
  
  char buf[21];
  sprintf(buf, "Credits: %d", credits);
  centerTxt(2, buf);
  
  uint8_t avail = credits / PTS_FOR_WATER;
  if (avail > 0) {
    sprintf(buf, "Water Ready: %d", avail);
  } else {
    sprintf(buf, "Need %d more items", PTS_FOR_WATER - (credits % PTS_FOR_WATER));
  }
  centerTxt(3, buf);
}

void lcdDetect(MatType m) {
  lcd.clear();
  centerTxt(0, "MATERIAL DETECTED!");
  
  switch (m) {
    case MAT_METAL:
      centerTxt(1, "Type: METAL");
      centerTxt(2, "Status: ACCEPTED");
      break;
    case MAT_PLASTIC:
      centerTxt(1, "Type: PLASTIC");
      centerTxt(2, "Status: ACCEPTED");
      break;
    case MAT_BIO:
      centerTxt(1, "Type: ORGANIC");
      centerTxt(2, "Status: REJECTED");
      break;
    default:
      centerTxt(1, "Type: UNKNOWN");
      break;
  }
  centerTxt(3, "");
}

void lcdProcess() {
  lcd.clear();
  centerTxt(0, "PROCESSING...");
  centerTxt(1, "");
  centerTxt(2, "Opening shoot");
  centerTxt(3, "Please wait...");
}

void lcdDispense() {
  lcd.clear();
  centerTxt(0, "DISPENSING WATER");
  centerTxt(1, "");
  centerTxt(2, "Please wait...");
  centerTxt(3, "");
}

void showMsg(const char* l1, const char* l2, const char* l3, const char* l4) {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print(l1);
  lcd.setCursor(0, 1); lcd.print(l2);
  lcd.setCursor(0, 2); lcd.print(l3);
  lcd.setCursor(0, 3); lcd.print(l4);
}

void centerTxt(uint8_t row, const char* txt) {
  uint8_t len = strlen(txt);
  uint8_t pad = (20 - len) / 2;
  lcd.setCursor(0, row);
  lcd.print("                    ");
  lcd.setCursor(pad > 0 ? pad : 0, row);
  lcd.print(txt);
}
