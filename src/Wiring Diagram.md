# RVM v5.0 - Complete Wiring Diagram

## Noise-Optimized Pin Layout

**Design Rationale:**
- Capacitive sensor (PNP, normally HIGH) isolated on D2 to reduce interference
- Sensors spread across pins to minimize cross-talk
- All servo pins are PWM-capable (D5, D6, D9, D10) for reliable control
- High-frequency signals (IR, vibration) placed away from servo PWM pins
- Decoupling capacitors recommended on sensor power lines

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
│    │   D2   │◄─────── Capacitive Sensor (Black wire) [PNP - isolated for noise]          │      │
│    │   D4   │◄─────── Inductive Sensor (Black wire) [NPN]                                │      │
│    │   D5   │◄─────── Shoot Servo Left (Orange wire) [PWM]                               │      │
│    │   D6   │◄─────── Shoot Servo Right (Orange wire) [PWM]                              │      │
│    │   D7   │◄─────── Relay Module (IN)                                                  │      │
│    │   D8   │◄─────── Dispense Button (Pin 1)                                            │      │
│    │   D9   │◄─────── Flapper Servo Left (Orange wire) [PWM]                             │      │
│    │  D10   │◄─────── Flapper Servo Right (Orange wire) [PWM]                            │      │
│    │  D12   │◄─────── TCRT5000 IR Sensor (DO)                                            │      │
│    │  D13   │◄─────── SW-420 Vibration Sensor (DO)                                       │      │
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
│    │   DO ────┼───────────┼──► D12   │             │
│    │          │           │          │             │
│    │   AO     │  (unused) │          │             │
│    │          │           │          │             │
│    └──────────┘           └──────────┘             │
│                                                    │
│    Signal: LOW = Object detected | HIGH = No object│
│    Plastic = IR triggered + NO metal + NO cap + NO gas │
│    Placed on D12 - away from servo PWM to reduce noise│
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
│    │  BLACK ──┼───────────┼──► D4    │             │
│    │ (Signal) │           │          │             │
│    └──────────┘           └──────────┘             │
│                                                    │
│    Signal: HIGH = No metal | LOW = Metal detected  │
│    NPN type sensor - normally HIGH                 │
│                                                    │
└────────────────────────────────────────────────────┘
```

---

## Capacitive Proximity Sensor (Conductive Detection)

```
┌────────────────────────────────────────────────────┐
│       LJC18A3-B-Z/BY CAPACITIVE SENSOR (PNP)       │
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
│    │  BLACK ──┼───────────┼──► D2    │             │
│    │ (Signal) │           │          │             │
│    └──────────┘           └──────────┘             │
│                                                    │
│    Signal: HIGH = No object | LOW = Object detected│
│    PNP type - normally HIGH, isolated on D2        │
│    ⚠️ Add 10kΩ pull-down resistor for noise immunity│
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
│    │   DO ────┼───────────┼──► D13   │             │
│    │          │           │          │             │
│    └──────────┘           └──────────┘             │
│                                                    │
│    Signal: LOW = No vibration | HIGH = Vibration   │
│    Placed on D13 - away from PWM pins              │
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
│    │  PIN 1 ──┼───────────┼──► D8    │             │
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
│   │   SHOOT SERVO LEFT (D5) [PWM]   │    │   SHOOT SERVO RIGHT (D6) [PWM]  │       │
│   │                                 │    │                                 │       │
│   │  ┌────────┐      ┌────────┐     │    │  ┌────────┐      ┌────────┐     │       │
│   │  │  SG90  │      │ARDUINO │     │    │  │  SG90  │      │ARDUINO │     │       │
│   │  │        │      │        │     │    │  │        │      │        │     │       │
│   │  │ BROWN ─┼──────┼─► GND  │     │    │  │ BROWN ─┼──────┼─► GND  │     │       │
│   │  │  RED ──┼──────┼─► 5V   │     │    │  │  RED ──┼──────┼─► 5V   │     │       │
│   │  │ ORANGE─┼──────┼─► D5   │     │    │  │ ORANGE─┼──────┼─► D6   │     │       │
│   │  └────────┘      └────────┘     │    │  └────────┘      └────────┘     │       │
│   └─────────────────────────────────┘    └─────────────────────────────────┘       │
│                                                                                    │
│   ┌─────────────────────────────────┐    ┌─────────────────────────────────┐       │
│   │  FLAPPER SERVO LEFT (D9) [PWM]  │    │  FLAPPER SERVO RIGHT (D10)[PWM] │       │
│   │                                 │    │                                 │       │
│   │  ┌────────┐      ┌────────┐     │    │  ┌────────┐      ┌────────┐     │       │
│   │  │  SG90  │      │ARDUINO │     │    │  │  SG90  │      │ARDUINO │     │       │
│   │  │        │      │        │     │    │  │        │      │        │     │       │
│   │  │ BROWN ─┼──────┼─► GND  │     │    │  │ BROWN ─┼──────┼─► GND  │     │       │
│   │  │  RED ──┼──────┼─► 5V   │     │    │  │  RED ──┼──────┼─► 5V   │     │       │
│   │  │ ORANGE─┼──────┼─► D9   │     │    │  │ ORANGE─┼──────┼─► D10  │     │       │
│   │  └────────┘      └────────┘     │    │  └────────┘      └────────┘     │       │
│   └─────────────────────────────────┘    └─────────────────────────────────┘       │
│                                                                                    │
│   ✓ All servo pins are now PWM-capable for reliable control                        │
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
│   │    D7 ───┼────────┼──► IN           │        │             │    │           │   │
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
│   • D7 = LOW  → Relay OFF → NO open     → Pump OFF                                  │
│   • D7 = HIGH → Relay ON  → NO closed   → Pump ON                                   │
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
│    D2    │ Capacitive Sensor (PNP)  │ Black wire - isolated for noise immunity      │
│    D4    │ Inductive Sensor (NPN)   │ Black wire (signal) - Brown to 12V+           │
│    D5    │ Shoot Servo Left [PWM]   │ Orange wire (signal) - Brown GND, Red 5V      │
│    D6    │ Shoot Servo Right [PWM]  │ Orange wire (signal) - Brown GND, Red 5V      │
│    D7    │ Relay Module             │ IN pin - VCC to 5V, GND to GND                │
│    D8    │ Dispense Button          │ Pin 1 (uses INPUT_PULLUP) - Pin 2 to GND      │
├──────────┼──────────────────────────┼───────────────────────────────────────────────┤
│    D9    │ Flapper Servo Left [PWM] │ Orange wire (signal) - Brown GND, Red 5V      │
│   D10    │ Flapper Servo Right [PWM]│ Orange wire (signal) - Brown GND, Red 5V      │
│   D12    │ TCRT5000 IR Sensor       │ DO pin (away from PWM for noise reduction)    │
│   D13    │ SW-420 Vibration         │ DO pin (digital out)                          │
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
│  [ ] Capacitive (PNP): Brown→12V+ | Blue→GND | Black→D2 (+ 10kΩ pull-down)          │
│  [ ] Inductive (NPN): Brown→12V+ | Blue→GND | Black→D4                              │
│                                                                                     │
│  DIGITAL SENSORS (5V):                                                              │
│  [ ] TCRT5000: VCC→5V | GND→GND | DO→D12                                            │
│  [ ] SW-420: VCC→5V | GND→GND | DO→D13                                              │
│  [ ] Button: Pin1→D8 | Pin2→GND                                                     │
│                                                                                     │
│  SERVO MOTORS (5V) - All PWM pins:                                                  │
│  [ ] Shoot Left: Brown→GND | Red→5V | Orange→D5  [PWM]                              │
│  [ ] Shoot Right: Brown→GND | Red→5V | Orange→D6 [PWM]                              │
│  [ ] Flapper Left: Brown→GND | Red→5V | Orange→D9 [PWM]                             │
│  [ ] Flapper Right: Brown→GND | Red→5V | Orange→D10 [PWM]                           │
│                                                                                     │
│  LCD DISPLAY (5V):                                                                  │
│  [ ] LCD: VCC→5V | GND→GND | SDA→A4 | SCL→A5                                        │
│                                                                                     │
│  RELAY & PUMP:                                                                      │
│  [ ] Relay: VCC→5V | GND→GND | IN→D7                                                │
│  [ ] Relay: COM→12V Battery(+)                                                      │
│  [ ] Relay: NO→Water Pump(+)                                                        │
│  [ ] Water Pump: (-)→12V Battery(-)                                                 │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

---

**Document Version**: 5.0  
**Compatible with**: RVM_v5.ino  
**Last Updated**: February 4, 2026

---

## Noise Reduction Tips

1. **Decoupling Capacitors**: Add 100nF ceramic capacitors between VCC and GND on each sensor
2. **Pull-down Resistor**: Add 10kΩ pull-down on D2 (capacitive sensor) for stable LOW state
3. **Twisted Pairs**: Use twisted pair wiring for sensor signal lines
4. **Shielded Cables**: Consider shielded cables for proximity sensors
5. **Separation**: Keep sensor wires away from servo/motor power lines
6. **Software Filtering**: Code includes 7-sample debouncing with noise threshold filtering
