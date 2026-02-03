# RVM v3.0 - Complete Wiring Diagram

## System Overview

```
┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                        ARDUINO UNO                                              │
│                                                                                                 │
│    ┌─────────────────────────────────────────────────────────────────────────────────────┐      │
│    │  PIN   │                           CONNECTION                                       │      │
│    ├────────┼────────────────────────────────────────────────────────────────────────────┤      │
│    │   A0   │◄─────── MQ135 Gas Sensor (AO)                                              │      │
│    │   A4   │◄─────── LCD I2C (SDA)                                                      │      │
│    │   A5   │◄─────── LCD I2C (SCL)                                                      │      │
│    │   D2   │◄─────── Inductive Sensor (Black wire)                                      │      │
│    │   D3   │◄─────── Capacitive Sensor (Black wire)                                     │      │
│    │   D4   │◄─────── SW-420 Vibration Sensor (DO)                                       │      │
│    │   D5   │◄─────── Dispense Button (Pin 1)                                            │      │
│    │   D6   │◄─────── Shoot Servo Left (Orange wire)                                     │      │
│    │   D7   │◄─────── Shoot Servo Right (Orange wire)                                    │      │
│    │   D8   │◄─────── Flapper Servo Left (Orange wire)                                   │      │
│    │   D9   │◄─────── Flapper Servo Right (Orange wire)                                  │      │
│    │  D10   │◄─────── Relay Module (IN)                                                  │      │
│    │  D11   │◄─────── TCRT5000 IR Sensor (DO)                                            │      │
│    │   5V   │────────► 5V Power Bus (all 5V components)                                  │      │
│    │  GND   │────────► Common Ground Bus (all components)                                │      │
│    │  Vin   │◄─────── 12V Battery (+)                                                    │      │
│    └────────┴────────────────────────────────────────────────────────────────────────────┘      │
│                                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────────────────────┘
```

---

## MQ135 Gas Sensor (Biodegradable Detection)

```
┌────────────────────────────────────────────────────┐
│                  MQ135 GAS SENSOR                  │
│                                                    │
│    ┌──────────┐           ┌──────────┐             │
│    │  MQ135   │           │ ARDUINO  │             │
│    │  MODULE  │           │          │             │
│    │          │           │          │             │
│    │   VCC ───┼───────────┼──► 5V    │             │
│    │          │           │          │             │
│    │   GND ───┼───────────┼──► GND   │             │
│    │          │           │          │             │
│    │   AO ────┼───────────┼──► A0    │             │
│    │          │           │          │             │
│    │   DO     │  (unused) │          │             │
│    │          │           │          │             │
│    └──────────┘           └──────────┘             │
│                                                    │
│    Threshold: >400 = Biodegradable detected        │
│                                                    │
└────────────────────────────────────────────────────┘
```

---

## TCRT5000 IR Sensor (Plastic Detection)

```
┌────────────────────────────────────────────────────┐
│               TCRT5000 IR SENSOR                   │
│                                                    │
│    ┌──────────┐           ┌──────────┐             │
│    │ TCRT5000 │           │ ARDUINO  │             │
│    │  MODULE  │           │          │             │
│    │          │           │          │             │
│    │   VCC ───┼───────────┼──► 5V    │             │
│    │          │           │          │             │
│    │   GND ───┼───────────┼──► GND   │             │
│    │          │           │          │             │
│    │   DO ────┼───────────┼──► D11   │             │
│    │          │           │          │             │
│    │   AO     │  (unused) │          │             │
│    │          │           │          │             │
│    └──────────┘           └──────────┘             │
│                                                    │
│    Signal: LOW = Object detected | HIGH = No object│
│    Plastic = IR triggered + NO metal + NO cap + NO gas │
│    Adjust sensitivity with onboard potentiometer   │
│                                                    │
└────────────────────────────────────────────────────┘
```

---

## Inductive Proximity Sensor (Metal Detection)

```
┌────────────────────────────────────────────────────┐
│        LJ18A3-8-Z/BX INDUCTIVE SENSOR (NPN)        │
│                                                    │
│    ┌──────────┐           ┌──────────┐             │
│    │INDUCTIVE │           │ ARDUINO  │             │
│    │  SENSOR  │           │          │             │
│    │          │           │          │             │
│    │  BROWN ──┼───────────┼──► 12V+  │             │
│    │          │           │  (Battery)             │
│    │          │           │          │             │
│    │  BLUE ───┼───────────┼──► GND   │             │
│    │          │           │ (Common) │             │
│    │          │           │          │             │
│    │  BLACK ──┼───────────┼──► D2    │             │
│    │ (Signal) │           │          │             │
│    └──────────┘           └──────────┘             │
│                                                    │
│    Signal: HIGH = No metal | LOW = Metal detected  │
│                                                    │
└────────────────────────────────────────────────────┘
```

---

## Capacitive Proximity Sensor (Conductive Detection)

```
┌────────────────────────────────────────────────────┐
│       LJC18A3-B-Z/BY CAPACITIVE SENSOR (NPN)       │
│                                                    │
│    ┌──────────┐           ┌──────────┐             │
│    │CAPACITIVE│           │ ARDUINO  │             │
│    │  SENSOR  │           │          │             │
│    │          │           │          │             │
│    │  BROWN ──┼───────────┼──► 12V+  │             │
│    │          │           │  (Battery)             │
│    │          │           │          │             │
│    │  BLUE ───┼───────────┼──► GND   │             │
│    │          │           │ (Common) │             │
│    │          │           │          │             │
│    │  BLACK ──┼───────────┼──► D3    │             │
│    │ (Signal) │           │          │             │
│    └──────────┘           └──────────┘             │
│                                                    │
│    Signal: HIGH = No object | LOW = Object detected│
│                                                    │
└────────────────────────────────────────────────────┘
```

---

## SW-420 Vibration Sensor

```
┌────────────────────────────────────────────────────┐
│              SW-420 VIBRATION SENSOR               │
│                                                    │
│    ┌──────────┐           ┌──────────┐             │
│    │  SW-420  │           │ ARDUINO  │             │
│    │  MODULE  │           │          │             │
│    │          │           │          │             │
│    │   VCC ───┼───────────┼──► 5V    │             │
│    │          │           │          │             │
│    │   GND ───┼───────────┼──► GND   │             │
│    │          │           │          │             │
│    │   DO ────┼───────────┼──► D4    │             │
│    │          │           │          │             │
│    └──────────┘           └──────────┘             │
│                                                    │
│    Signal: LOW = No vibration | HIGH = Vibration   │
│                                                    │
└────────────────────────────────────────────────────┘
```

---

## Dispense Button

```
┌────────────────────────────────────────────────────┐
│                  DISPENSE BUTTON                   │
│                                                    │
│    ┌──────────┐           ┌──────────┐             │
│    │  BUTTON  │           │ ARDUINO  │             │
│    │  SWITCH  │           │          │             │
│    │          │           │          │             │
│    │  PIN 1 ──┼───────────┼──► D5    │             │
│    │          │           │(INPUT_PULLUP)          │
│    │          │           │          │             │
│    │  PIN 2 ──┼───────────┼──► GND   │             │
│    │          │           │          │             │
│    └──────────┘           └──────────┘             │
│                                                    │
│    Signal: HIGH = Not pressed | LOW = Pressed      │
│    (Uses internal pull-up resistor)                │
│                                                    │
└────────────────────────────────────────────────────┘
```

---

## Servo Motors (4x SG90)

```
┌────────────────────────────────────────────────────────────────────────────────────┐
│                              4x SG90 SERVO MOTORS                                  │
│                                                                                    │
│   ┌─────────────────────────────────┐    ┌─────────────────────────────────┐       │
│   │     SHOOT SERVO LEFT (D6)       │    │     SHOOT SERVO RIGHT (D7)      │       │
│   │                                 │    │                                 │       │
│   │  ┌────────┐      ┌────────┐     │    │  ┌────────┐      ┌────────┐     │       │
│   │  │  SG90  │      │ARDUINO │     │    │  │  SG90  │      │ARDUINO │     │       │
│   │  │        │      │        │     │    │  │        │      │        │     │       │
│   │  │ BROWN ─┼──────┼─► GND  │     │    │  │ BROWN ─┼──────┼─► GND  │     │       │
│   │  │  RED ──┼──────┼─► 5V   │     │    │  │  RED ──┼──────┼─► 5V   │     │       │
│   │  │ ORANGE─┼──────┼─► D6   │     │    │  │ ORANGE─┼──────┼─► D7   │     │       │
│   │  └────────┘      └────────┘     │    │  └────────┘      └────────┘     │       │
│   └─────────────────────────────────┘    └─────────────────────────────────┘       │
│                                                                                    │
│   ┌─────────────────────────────────┐    ┌─────────────────────────────────┐       │
│   │     FLAPPER SERVO LEFT (D8)     │    │     FLAPPER SERVO RIGHT (D9)    │       │
│   │                                 │    │                                 │       │
│   │  ┌────────┐      ┌────────┐     │    │  ┌────────┐      ┌────────┐     │       │
│   │  │  SG90  │      │ARDUINO │     │    │  │  SG90  │      │ARDUINO │     │       │
│   │  │        │      │        │     │    │  │        │      │        │     │       │
│   │  │ BROWN ─┼──────┼─► GND  │     │    │  │ BROWN ─┼──────┼─► GND  │     │       │
│   │  │  RED ──┼──────┼─► 5V   │     │    │  │  RED ──┼──────┼─► 5V   │     │       │
│   │  │ ORANGE─┼──────┼─► D8   │     │    │  │ ORANGE─┼──────┼─► D9   │     │       │
│   │  └────────┘      └────────┘     │    │  └────────┘      └────────┘     │       │
│   └─────────────────────────────────┘    └─────────────────────────────────┘       │
│                                                                                    │
│   ⚠️  WARNING: 4 servos can draw ~1A. Use external 5V 2A supply recommended!       │
│                                                                                    │
└────────────────────────────────────────────────────────────────────────────────────┘
```

---

## 20x4 I2C LCD Display

```
┌────────────────────────────────────────────────────┐
│              20x4 I2C LCD (Address: 0x27)          │
│                                                    │
│    ┌──────────┐           ┌──────────┐             │
│    │  LCD     │           │ ARDUINO  │             │
│    │  I2C     │           │          │             │
│    │  MODULE  │           │          │             │
│    │          │           │          │             │
│    │   VCC ───┼───────────┼──► 5V    │             │
│    │          │           │          │             │
│    │   GND ───┼───────────┼──► GND   │             │
│    │          │           │          │             │
│    │   SDA ───┼───────────┼──► A4    │             │
│    │          │           │          │             │
│    │   SCL ───┼───────────┼──► A5    │             │
│    │          │           │          │             │
│    └──────────┘           └──────────┘             │
│                                                    │
└────────────────────────────────────────────────────┘
```

---

## Relay Module & Water Pump

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                            RELAY MODULE & 12V WATER PUMP                            │
│                                                                                     │
│                                                                                     │
│   ┌──────────┐        ┌─────────────────┐        ┌─────────────┐    ┌───────────┐   │
│   │ ARDUINO  │        │  RELAY MODULE   │        │ WATER PUMP  │    │12V BATTERY│   │
│   │          │        │                 │        │   (12V DC)  │    │           │   │
│   │          │        │                 │        │             │    │           │   │
│   │   D10 ───┼────────┼──► IN           │        │             │    │           │   │
│   │          │        │                 │        │             │    │           │   │
│   │    5V ───┼────────┼──► VCC          │        │             │    │           │   │
│   │          │        │                 │        │             │    │           │   │
│   │   GND ───┼────────┼──► GND          │        │             │    │           │   │
│   │          │        │                 │        │             │    │           │   │
│   └──────────┘        │   COM ◄─────────┼────────┼─────────────┼────┼─── (+)    │   │
│                       │                 │        │             │    │           │   │
│                       │   NO ───────────┼────────┼──► (+)      │    │           │   │
│                       │                 │        │             │    │           │   │
│                       │   NC            │(unused)│   (─) ──────┼────┼─── (─)    │   │
│                       │                 │        │             │    │           │   │
│                       └─────────────────┘        └─────────────┘    └───────────┘   │
│                                                                                     │
│                                                                                     │
│   Operation:                                                                        │
│   • D10 = LOW  → Relay OFF → NO open     → Pump OFF                                 │
│   • D10 = HIGH → Relay ON  → NO closed   → Pump ON                                  │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

---

## Power Distribution

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                 POWER DISTRIBUTION                                  │
│                                                                                     │
│                         ┌─────────────────────────────┐                             │
│                         │     12V RECHARGEABLE        │                             │
│                         │         BATTERY             │                             │
│                         │                             │                             │
│                         │    (+) ────────┬────────────┼─────────────────────┐       │
│                         │                │            │                     │       │
│                         │    (─) ────────┼────────────┼──► COMMON GND ◄─────┼───┐   │
│                         │                │            │                     │   │   │
│                         └────────────────┼────────────┘                     │   │   │
│                                          │                                  │   │   │
│          ┌───────────────────────────────┼──────────────────────────────────┤   │   │
│          │                               │                                  │   │   │
│          ▼                               ▼                                  ▼   │   │
│   ┌─────────────┐               ┌─────────────┐                    ┌────────────┴┐  │
│   │   Arduino   │               │  Proximity  │                    │   Relay     │  │
│   │     Vin     │               │   Sensors   │                    │    COM      │  │
│   │   (7-12V)   │               │    (12V)    │                    │   (12V+)    │  │
│   │             │               │             │                    │             │  │
│   │   Inductive │◄──────────────┤ Brown wire  │                    └──────┬──────┘  │
│   │   Capacitive│◄──────────────┤ Brown wire  │                           │         │
│   └──────┬──────┘               └─────────────┘                           ▼         │
│          │                                                           Water Pump     │
│          │ 5V OUTPUT                                                      │         │
│          │                                                                │         │
│    ┌─────┴─────┬──────────┬──────────┬──────────┬──────────┬──────────┐   │         │
│    │           │          │          │          │          │          │   │         │
│    ▼           ▼          ▼          ▼          ▼          ▼          ▼   │         │
│ ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐  │   │         │
│ │ LCD  │  │Servo │  │Servo │  │Servo │  │Servo │  │MQ135 │  │TCRT  │  │   │         │
│ │ VCC  │  │  L1  │  │  R1  │  │  L2  │  │  R2  │  │ VCC  │  │5000  │  │   │         │
│ └──┬───┘  └──┬───┘  └──┬───┘  └──┬───┘  └──┬───┘  └──┬───┘  │ VCC  │  │   │         │
│    │         │         │         │         │         │      └──┬───┘  │   │         │
│    │         │         │         │         │         │         │      │   │         │
│    └─────────┴─────────┴─────────┴─────────┴─────────┴─────────┴──────┴───┴─────────┘
│                                          │                                          │
│                                     COMMON GND                                      │
│                                                                                     │
│   ⚠️  CRITICAL: Arduino GND + 12V Battery (─) must connect to same common ground!   │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

---

## Complete Connection Summary

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                           PIN-BY-PIN CONNECTION TABLE                               │
├──────────┬──────────────────────────┬───────────────────────────────────────────────┤
│ ARDUINO  │       COMPONENT          │              WIRING DETAILS                   │
│   PIN    │                          │                                               │
├──────────┼──────────────────────────┼───────────────────────────────────────────────┤
│    A0    │ MQ135 Gas Sensor         │ Yellow wire from AO pin                       │
│    A4    │ 20x4 LCD I2C             │ SDA (Blue wire)                               │
│    A5    │ 20x4 LCD I2C             │ SCL (Green wire)                              │
├──────────┼──────────────────────────┼───────────────────────────────────────────────┤
│    D2    │ Inductive Sensor         │ Black wire (signal) - Brown to 12V+           │
│    D3    │ Capacitive Sensor        │ Black wire (signal) - Brown to 12V+           │
│    D4    │ SW-420 Vibration         │ DO pin (digital out)                          │
│    D5    │ Dispense Button          │ Pin 1 (uses INPUT_PULLUP) - Pin 2 to GND      │
├──────────┼──────────────────────────┼───────────────────────────────────────────────┤
│    D6    │ Shoot Servo Left         │ Orange wire (signal) - Brown GND, Red 5V      │
│    D7    │ Shoot Servo Right        │ Orange wire (signal) - Brown GND, Red 5V      │
│    D8    │ Flapper Servo Left       │ Orange wire (signal) - Brown GND, Red 5V      │
│    D9    │ Flapper Servo Right      │ Orange wire (signal) - Brown GND, Red 5V      │
├──────────┼──────────────────────────┼───────────────────────────────────────────────┤
│   D10    │ Relay Module             │ IN pin - VCC to 5V, GND to GND                │
│   D11    │ TCRT5000 IR Sensor       │ DO pin (digital out)                          │
│          │                          │ COM to 12V+, NO to Pump(+), Pump(-) to 12V-   │
├──────────┼──────────────────────────┼───────────────────────────────────────────────┤
│    5V    │ Power Output             │ LCD, MQ135, TCRT5000, SW-420, 4x Servos,      │
│          │                          │ Relay VCC                                     │
│   GND    │ Common Ground            │ ALL components + 12V Battery (-)              │
│   Vin    │ Power Input              │ 12V Battery (+)                               │
└──────────┴──────────────────────────┴───────────────────────────────────────────────┘
```

---

## Wiring Checklist

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                              VERIFICATION CHECKLIST                                 │
├─────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                     │
│  POWER CONNECTIONS:                                                                 │
│  [ ] 12V Battery (+) ────► Arduino Vin                                              │
│  [ ] 12V Battery (+) ────► Inductive Sensor (Brown wire)                            │
│  [ ] 12V Battery (+) ────► Capacitive Sensor (Brown wire)                           │
│  [ ] 12V Battery (+) ────► Relay COM terminal                                       │
│  [ ] 12V Battery (-) ────► Common GND bus                                           │
│  [ ] Arduino GND ────────► Common GND bus                                           │
│  [ ] Arduino 5V ─────────► 5V power bus                                             │
│                                                                                     │
│  ANALOG SENSORS (5V):                                                               │
│  [ ] MQ135: VCC→5V | GND→GND | AO→A0                                                │
│                                                                                     │
│  PROXIMITY SENSORS (12V):                                                           │
│  [ ] Inductive: Brown→12V+ | Blue→GND | Black→D2                                    │
│  [ ] Capacitive: Brown→12V+ | Blue→GND | Black→D3                                   │
│                                                                                     │
│  DIGITAL SENSORS (5V):                                                              │
│  [ ] TCRT5000: VCC→5V | GND→GND | DO→D11                                            │
│  [ ] SW-420: VCC→5V | GND→GND | DO→D4                                               │
│  [ ] Button: Pin1→D5 | Pin2→GND                                                     │
│                                                                                     │
│  SERVO MOTORS (5V):                                                                 │
│  [ ] Shoot Left: Brown→GND | Red→5V | Orange→D6                                     │
│  [ ] Shoot Right: Brown→GND | Red→5V | Orange→D7                                    │
│  [ ] Flapper Left: Brown→GND | Red→5V | Orange→D8                                   │
│  [ ] Flapper Right: Brown→GND | Red→5V | Orange→D9                                  │
│                                                                                     │
│  LCD DISPLAY (5V):                                                                  │
│  [ ] LCD: VCC→5V | GND→GND | SDA→A4 | SCL→A5                                        │
│                                                                                     │
│  RELAY & PUMP:                                                                      │
│  [ ] Relay: VCC→5V | GND→GND | IN→D10                                               │
│  [ ] Relay: COM→12V Battery(+)                                                      │
│  [ ] Relay: NO→Water Pump(+)                                                        │
│  [ ] Water Pump: (-)→12V Battery(-)                                                 │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

---

**Document Version**: 3.0  
**Compatible with**: RVM_v3.ino  
**Last Updated**: February 3, 2026
