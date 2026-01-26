# Reverse Vending Machine (RVM) - Wiring Guide

Complete wiring documentation for RVM v2.0

---

## Table of Contents
1. [Components Overview](#components-overview)
2. [Power Requirements](#power-requirements)
3. [Arduino Pin Assignments](#arduino-pin-assignments)
4. [Wiring Diagrams](#wiring-diagrams)
5. [Step-by-Step Wiring Instructions](#step-by-step-wiring-instructions)
6. [Sensor Connections](#sensor-connections)
7. [Servo Motor Connections](#servo-motor-connections)
8. [Relay & Pump Connections](#relay--pump-connections)
9. [Testing & Verification](#testing--verification)
10. [Troubleshooting](#troubleshooting)

---

## Components Overview

### Control & Display
| Component | Quantity | Purpose |
|-----------|----------|---------|
| Arduino Uno/Mega | 1 | Main microcontroller |
| I2C LCD 20x4 | 1 | User interface display |
| Breadboard (Full-size) | 2 | Prototyping connections |
| Jumper Wires | 30+ | Signal connections |
| Solid Core Wires | 20-30 | Power distribution |

### Sensors
| Component | Quantity | Type | Purpose |
|-----------|----------|------|---------|
| LJ18A3-8-Z/BX NPN Inductive Sensor | 1 | Digital (NPN) | Metal detection |
| LJC18A3-B-Z/BY Capacitive Sensor | 1 | Digital | Conductive material detection |
| TCRT5000 IR Reflective Sensor | 1 | Analog | Plastic detection (primary) |
| MQ135 Gas Sensor | 1 | Analog | Biodegradable detection |
| SW-420 Vibration Sensor | 1 | Digital | Material drop confirmation |
| Button Key Switch Sensor | 2 | Digital | User input (dispense button) |

### Actuators
| Component | Quantity | Purpose |
|-----------|----------|---------|
| SG90 Servo Motor | 4 | Shoot door (2) + Flapper (2) |
| Relay Module | 1 | Water pump control |
| Water Pump (12V DC) | 1 | Water dispensing |

### Power
| Component | Quantity | Purpose |
|-----------|----------|---------|
| 12V Rechargeable Battery | 1 | Main power supply |
| 5V Voltage Regulator (Optional) | 1 | Arduino/sensor power |

---

## Power Requirements

### Voltage Requirements
| Component | Voltage | Current (Typical) |
|-----------|---------|-------------------|
| Arduino Uno | 5V (via USB) or 7-12V (Vin) | 50mA |
| I2C LCD 20x4 | 5V | 20-100mA (with backlight) |
| SG90 Servo (×4) | 5V | 100-250mA each (under load) |
| Proximity Sensors | 6-36V DC (use 12V) | 10-15mA each |
| TCRT5000 IR Sensor | 5V | 20-60mA |
| MQ135 Gas Sensor | 5V | 150mA (heater active) |
| Vibration Sensor | 3.3-5V | 5mA |
| Relay Module | 5V (coil) | 70-80mA |
| Water Pump | 12V DC | 500-1000mA |

### Power Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                   12V RECHARGEABLE BATTERY                  │
└───────────────────────────┬─────────────────────────────────┘
                            │
            ┌───────────────┼───────────────┐
            │               │               │
            ▼               ▼               ▼
     ┌──────────┐    ┌──────────┐    ┌──────────┐
     │ Arduino  │    │ Proximity│    │  Water   │
     │   Vin    │    │ Sensors  │    │  Pump    │
     │ (7-12V)  │    │  (12V)   │    │ (via     │
     └────┬─────┘    └──────────┘    │ Relay)   │
          │                          └──────────┘
          │ 5V Output
          │
    ┌─────┴─────┬───────────┬───────────┬───────────┐
    │           │           │           │           │
    ▼           ▼           ▼           ▼           ▼
┌───────┐  ┌───────┐   ┌───────┐   ┌───────┐   ┌───────┐
│  LCD  │  │Servos │   │ MQ135 │   │ Vibr. │   │ Relay │
│ (5V)  │  │ (5V)  │   │ (5V)  │   │ (5V)  │   │ (5V)  │
└───────┘  └───────┘   └───────┘   └───────┘   └───────┘
```

### Power Recommendations

**Option 1: USB + 12V Battery (Recommended)**
```
USB → Arduino (logic power)
12V Battery → Proximity sensors + Water pump (via relay)
Arduino 5V → Servos, LCD, MQ135, vibration sensor, relay coil
```

**Option 2: Single 12V Battery**
```
12V Battery → Arduino Vin (7-12V input)
           → Proximity sensors
           → Water pump (via relay)
Arduino 5V out → All 5V components
```

### ⚠️ Power Warnings
- **Servo Current**: 4 servos can draw up to 1A peak - external 5V supply recommended
- **MQ135 Heater**: Requires warm-up time (20+ seconds) and draws 150mA
- **Never power servos directly from Arduino 5V pin** if using more than 2 servos
- **Common Ground**: All power supplies must share a common GND

---

## Arduino Pin Assignments

### Complete Pin Map

| Pin | Type | Function | Component | Wire Color (Suggested) |
|-----|------|----------|-----------|------------------------|
| **A0** | Analog Input | Gas Sensor | MQ135 Analog Out | Yellow |
| **A1** | Analog Input | IR Sensor | TCRT5000 Analog Out | Orange |
| **A4** | I2C (SDA) | LCD Data | LCD SDA | Blue |
| **A5** | I2C (SCL) | LCD Clock | LCD SCL | Green |
| **D2** | Digital Input | Metal Detection | Inductive Sensor OUT | Orange |
| **D3** | Digital Input | Conductive Detection | Capacitive Sensor OUT | Purple |
| **D4** | Digital Input | Vibration Detection | SW-420 OUT | White |
| **D5** | Digital Input (PULLUP) | Dispense Button | Button Switch | Gray |
| **D6** | PWM Output | Shoot Servo Left | SG90 Signal | Brown |
| **D7** | Digital Output | Shoot Servo Right | SG90 Signal | Brown |
| **D8** | Digital Output | Flapper Servo Left | SG90 Signal | Brown |
| **D9** | PWM Output | Flapper Servo Right | SG90 Signal | Brown |
| **D10** | Digital Output | Water Pump Control | Relay IN | Red |
| **5V** | Power Output | +5V Supply | Sensors, LCD, Servos | Red |
| **GND** | Ground | Common Ground | All components | Black |
| **Vin** | Power Input | 7-12V DC Input | External power | Red |

### Pin Summary by Function

**Analog Inputs:**
- A0: MQ135 Gas Sensor
- A1: TCRT5000 IR Sensor (Plastic Detection)

**Digital Inputs:**
- D2: Inductive Proximity Sensor (Metal)
- D3: Capacitive Proximity Sensor (Conductive materials)
- D4: Vibration Sensor
- D5: Dispense Button (with internal pull-up)

**Digital Outputs (Servos):**
- D6: Shoot Servo Left
- D7: Shoot Servo Right
- D8: Flapper Servo Left
- D9: Flapper Servo Right

**Digital Outputs (Relay):**
- D10: Water Pump Relay

**I2C:**
- A4: SDA (LCD)
- A5: SCL (LCD)

### Available Pins for Expansion
- D0, D1 (Serial - reserved for USB communication)
- D11, D12, D13
- A2, A3, A6, A7 (if using Arduino Nano/Mega)

---

## Wiring Diagrams

### System Overview

```
                                ┌───────────────────────────┐
                                │       ARDUINO UNO         │
                                │                           │
   ┌────────────────────────────┤ A0   MQ135 Gas Sensor     │
   │  ┌─────────────────────────┤ A1   TCRT5000 IR Sensor   │
   │  │  ┌──────────────────────┤ A4   LCD SDA              │
   │  │  │  ┌───────────────────┤ A5   LCD SCL              │
   │  │  │  │                   │                           │
   │  │  │  │  ┌────────────────┤ D2   Inductive Sensor     │─────┐
   │  │  │  │  │  ┌─────────────┤ D3   Capacitive Sensor    │─────┤
   │  │  │  │  │  │  ┌──────────┤ D4   Vibration Sensor     │     │
   │  │  │  │  │  │  │  ┌───────┤ D5   Dispense Button      │     │
   │  │  │  │  │  │  │  │ ┌─────┤ D6   Shoot Servo L        │     │
   │  │  │  │  │  │  │  │ │┌────┤ D7   Shoot Servo R        │     │
   │  │  │  │  │  │  │  │ ││┌───┤ D8   Flapper Servo L      │     │
   │  │  │  │  │  │  │  │ │││┌──┤ D9   Flapper Servo R      │     │
   │  │  │  │  │  │  │  │ ││││  │                           │     │
   │  │  │  │  │  │  │  │ │││││┌┤ D10  Relay Module         │     │
   │  │  │  │  │  │  │  │ ││││││|                           │     │
   │  │  │  │  │  │  │  │ │││││││ 5V ────┬──────────────────┼─┐   │
   │  │  │  │  │  │  │  │ │││││││ GND ───┼──────────────────┼─┼─┐ │
   │  │  │  │  │  │  │  │ │││││││ Vin ◄──┼── 12V Battery    │ │ │ │
   │  │  │  │  │  │  │  │ │││││││────────┼──────────────────┘ │ │ │
   │  │  │  │  │  │  │  │ ││││││└────────┼────────────────────┘ │ │
   │  │  │  │  │  │  │  │ ││││││         │                      │ │
   │  │  │  │  │  │  │  │ │││││└────►┌───┴────────┐             │ │
   │  │  │  │  │  │  │  │ │││││      │Relay Module│             │ │
   │  │  │  │  │  │  │  │ │││││      │IN  VCC GND │◄────────────┘ │
   │  │  │  │  │  │  │  │ │││││      │COM  NO  NC │               │
   │  │  │  │  │  │  │  │ │││││      └─┬───┬──────┘               │
   │  │  │  │  │  │  │  │ │││││        │   │                      │
   │  │  │  │  │  │  │  │ │││││      12V+  └──► Water Pump (+)    │
   │  │  │  │  │  │  │  │ │││││                  Water Pump (-) ──┼─┘
   │  │  │  │  │  │  │  │ │││││                                   │
   │  │  │  │  │  │  │  │ ││││└──► Flapper Servo R ◄──────────────┤
   │  │  │  │  │  │  │  │ │││└───► Flapper Servo L ◄──────────────┤
   │  │  │  │  │  │  │  │  │└────► Shoot Servo R ◄────────────────┤
   │  │  │  │  │  │  │  │  └─────► Shoot Servo L ◄────────────────┤
   │  │  │  │  │  │  │  │                                         │
   │  │  │  │  │  │  │  └──────► Button (other pin to GND) ◄──────┤
   │  │  │  │  │  │  └─────────► SW-420 VCC:5V GND ◄──────────────┤
   │  │  │  │  │  │                                               │
   │  │  │  │  │  └───────────────────────────────────────────┐   │
   │  │  │  │  └──────────────────────────────────────────┐   │   │
   │  │  │  │                  PROXIMITY SENSORS          │   │   │
   │  │  │  │                ┌────────────────────────┐   │   │   │
   │  │  │  │                │ Inductive   Capacitive │   │   │   │
   │  │  │  │                │ LJ18A3      LJC18A3    │   │   │   │
   │  │  │  │                │                        │   │   │   │
   │  │  │  │                │ Brown ──► 12V+         │   │   │   │
   │  │  │  │                │ Blue ───► 12V GND      │   │   │   │
   │  │  │  │                │ Black ──► Arduino D2/D3│◄──┴───┘   │
   │  │  │  │                └────────────────────────┘           │
   │  │  │  │                                                     │
   │  │  │  │    ┌───────────────┐                                │
   │  │  │  └───►│  20x4 LCD     │                                │
   │  │  └──────►│  I2C Module   │ VCC ◄──────────────────────────┤
   │  │          │               │ GND ◄──────────────────────────┘
   │  │          └───────────────┘
   │  │
   │  │          ┌───────────────┐
   │  └─────────►│   TCRT5000    │
   │             │   IR Sensor   │ VCC → 5V
   │             │ (Plastic Det) │ GND → GND
   │             │               │ AO → A1
   │             └───────────────┘
   │
   │             ┌───────────────┐
   └────────────►│    MQ135      │
                 │  Gas Sensor   │ VCC → 5V
                 │               │ GND → GND
                 │               │ AO → A0
                 └───────────────┘
```

### Simplified Connection Diagram

```
┌──────────────────────────────────────────────────────────────────┐
│                         SENSORS SECTION                          │
├──────────────────────────────────────────────────────────────────┤
│                                                                  │
│  [Inductive Sensor]     [Capacitive Sensor]    [MQ135 Sensor]    │
│   LJ18A3-8-Z/BX          LJC18A3-B-Z/BY                          │
│   ┌─────────┐            ┌─────────┐           ┌─────────┐       │
│   │ Brown   │──► 12V+    │ Brown   │──► 12V+   │ VCC     │──► 5V │
│   │ Blue    │──► 12V-    │ Blue    │──► 12V-   │ GND     │──► GND│
│   │ Black   │──► D2      │ Black   │──► D3     │ AO      │──► A0 │
│   └─────────┘            └─────────┘           └─────────┘       │
│                                                                  │
│  [TCRT5000 IR Sensor]    [Vibration Sensor]    [Button Switch]   │
│   (Plastic Detection)     SW-420                                 │
│   ┌─────────┐            ┌─────────┐           ┌─────────┐       │
│   │ VCC     │──► 5V      │ VCC     │──► 5V     │ Pin 1   │──► D5 │
│   │ GND     │──► GND     │ GND     │──► GND    │ Pin 2   │──► GND│
│   │ AO      │──► A1      │ DO      │──► D4     └─────────┘       │
│   └─────────┘            └─────────┘                             │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────────────────┐
│                         SERVO SECTION                            │
├──────────────────────────────────────────────────────────────────┤
│                                                                  │
│  All SG90 Servos (×4):                                           │
│  ┌──────────────────────────────────────────┐                    │
│  │ Wire Color    │ Connection               │                    │
│  ├───────────────┼──────────────────────────┤                    │
│  │ Brown/Black   │ GND                      │                    │
│  │ Red           │ 5V (External recommended)│                    │
│  │ Orange/Yellow │ Signal Pin               │                    │
│  └──────────────────────────────────────────┘                    │
│                                                                  │
│  Shoot Left  → D6     Shoot Right  → D7                          │
│  Flapper Left → D8    Flapper Right → D9                         │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────────────────┐
│                    RELAY & PUMP SECTION                          │
├──────────────────────────────────────────────────────────────────┤
│                                                                  │
│  [Relay Module]                    [Water Pump]                  │
│   ┌─────────────┐                  ┌───────────┐                 │
│   │ VCC   ──────┼──► 5V            │           │                 │
│   │ GND   ──────┼──► GND           │    (+)────┼──► Relay NO     │
│   │ IN    ──────┼──► D10           │    (-)────┼──► 12V GND      │
│   │             │                  │           │                 │
│   │ COM   ──────┼──► 12V+          └───────────┘                 │
│   │ NO    ──────┼──► Pump (+)                                    │
│   │ NC    ──────┼──  (unused)                                    │
│   └─────────────┘                                                │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────────────────┐
│                      LCD DISPLAY SECTION                         │
├──────────────────────────────────────────────────────────────────┤
│                                                                  │
│  [20x4 I2C LCD] (Address: 0x27)                                  │
│   ┌─────────────┐                                                │
│   │ VCC   ──────┼──► 5V                                          │
│   │ GND   ──────┼──► GND                                         │
│   │ SDA   ──────┼──► A4                                          │
│   │ SCL   ──────┼──► A5                                          │
│   └─────────────┘                                                │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘
```

---

## Step-by-Step Wiring Instructions

### Phase 1: Power Distribution Setup

**Step 1: Prepare Breadboard Power Rails**
1. Connect Arduino 5V to breadboard positive rail (red)
2. Connect Arduino GND to breadboard negative rail (blue)
3. Connect second breadboard rails to first (if using 2 breadboards)

**Step 2: 12V Power Distribution**
1. Connect 12V battery positive to a separate section of breadboard
2. Connect 12V battery negative to common GND rail
3. **Important**: Share GND between Arduino and 12V supply

```
Breadboard Layout:
┌────────────────────────────────────────┐
│ (+5V Rail) ═══════════════════════════ │ ◄── Arduino 5V
│ (GND Rail) ═══════════════════════════ │ ◄── Arduino GND + 12V GND
│                                        │
│ [Component Connection Area]            │
│                                        │
│ (+12V Rail) ══════════════════════════ │ ◄── 12V Battery +
│ (GND Rail) ═══════════════════════════ │ ◄── Shared with above
└────────────────────────────────────────┘
```

### Phase 2: I2C LCD Display

**Components Needed:**
- 20x4 I2C LCD module
- 4 jumper wires

**Connections:**
| LCD Pin | Arduino Pin | Description |
|---------|-------------|-------------|
| VCC | 5V | Power (Red wire) |
| GND | GND | Ground (Black wire) |
| SDA | A4 | Data line (Blue wire) |
| SCL | A5 | Clock line (Green wire) |

**Verification:**
- LCD backlight should turn on when powered
- Run I2C scanner sketch to verify address (0x27 or 0x3F)

### Phase 3: Proximity Sensors (Metal & Conductive Detection)

**Components Needed:**
- LJ18A3-8-Z/BX NPN Inductive Proximity Sensor (Metal)
- LJC18A3-B-Z/BY Capacitive Proximity Sensor (Conductive)
- 6 jumper wires

#### Inductive Sensor (Metal Detection) - Pin D2

**Wire Colors (Standard 3-wire):**
| Wire Color | Connection | Description |
|------------|------------|-------------|
| Brown | 12V+ | Power supply (6-36V DC) |
| Blue | 12V GND | Ground |
| Black | Arduino D2 | Signal output (NPN) |

**Sensor Behavior:**
- **No metal present**: Output HIGH (pulled up)
- **Metal detected**: Output LOW (NPN sinks current)

#### Capacitive Sensor (Conductive Detection) - Pin D3

**Wire Colors (Standard 3-wire):**
| Wire Color | Connection | Description |
|------------|------------|-------------|
| Brown | 12V+ | Power supply (6-36V DC) |
| Blue | 12V GND | Ground |
| Black | Arduino D3 | Signal output |

**Sensor Behavior:**
- **No object present**: Output HIGH
- **Object detected**: Output LOW
- **Note**: Used to detect conductive materials. In RVM v2, plastic is detected by TCRT5000 IR sensor instead.

**Mounting Tips:**
- Mount both sensors side-by-side above the shoot opening
- Detection range: ~8mm for inductive, ~8mm for capacitive
- Adjust sensitivity potentiometer on sensor if available
- Keep sensors 50mm apart to avoid interference

### Phase 3.5: TCRT5000 IR Sensor (Plastic Detection)

**Components Needed:**
- TCRT5000 IR Reflective Sensor Module
- 3 jumper wires

**Connections:**
| TCRT5000 Pin | Arduino Pin | Description |
|--------------|-------------|-------------|
| VCC | 5V | Power supply |
| GND | GND | Ground |
| AO (Analog Out) | A1 | Analog signal output |

**Sensor Behavior:**
- **No object present**: High analog value (>500)
- **Object detected**: Low analog value (<500)
- **Detection Logic**: Plastic is detected when IR sensor is triggered AND metal sensor is NOT triggered AND capacitive sensor is NOT triggered AND gas sensor is NOT triggered

**Mounting Tips:**
- Mount facing the detection area, perpendicular to material path
- Optimal detection distance: 2-15mm
- Keep sensor clean - dust affects IR readings
- Avoid direct sunlight on sensor

**Calibration:**
1. Read baseline value with no object (Serial Monitor)
2. Test with plastic bottle - value should drop significantly
3. Adjust `TCRT5000_DETECTION_THRESHOLD` in code if needed (default: 500)

### Phase 4: MQ135 Gas Sensor (Biodegradable Detection)

**Components Needed:**
- MQ135 Gas Sensor Module
- 3 jumper wires

**Connections:**
| MQ135 Pin | Arduino Pin | Description |
|-----------|-------------|-------------|
| VCC | 5V | Power (draws ~150mA) |
| GND | GND | Ground |
| AO (Analog Out) | A0 | Analog signal output |
| DO (Digital Out) | (unused) | Threshold-based digital output |

**Important Notes:**
- MQ135 requires 20+ seconds warm-up time for accurate readings
- Default threshold in code: 400 (adjust based on testing)
- Sensor detects organic compounds, ammonia, benzene, alcohol, smoke
- High readings indicate biodegradable/organic materials

**Calibration:**
1. Power on system and wait 1-2 minutes for sensor warm-up
2. Read baseline value with clean air (Serial Monitor)
3. Test with organic material (banana peel, paper)
4. Adjust `MQ135_BIODEGRADABLE_THRESHOLD` in code if needed

### Phase 5: Vibration Sensor (SW-420)

**Components Needed:**
- SW-420 Vibration Sensor Module
- 3 jumper wires

**Connections:**
| SW-420 Pin | Arduino Pin | Description |
|------------|-------------|-------------|
| VCC | 5V | Power supply |
| GND | GND | Ground |
| DO | D4 | Digital output |

**Sensor Behavior:**
- **No vibration**: Output LOW
- **Vibration detected**: Output HIGH

**Sensitivity Adjustment:**
- Turn potentiometer on module to adjust sensitivity
- Test by tapping the flapper board
- Should trigger reliably when material drops

**Mounting:**
- Attach firmly to the flapper board
- Use double-sided tape or screws
- Ensure good mechanical coupling

### Phase 6: Dispense Button

**Components Needed:**
- Button Key Switch Sensor
- 2 jumper wires

**Connections:**
| Button Pin | Arduino Pin | Description |
|------------|-------------|-------------|
| Pin 1 (Signal) | D5 | Digital input (uses internal pull-up) |
| Pin 2 (Ground) | GND | Ground |

**Button Behavior:**
- Uses `INPUT_PULLUP` - no external resistor needed
- **Not pressed**: Pin reads HIGH
- **Pressed**: Pin reads LOW

**Alternative: Standard Push Button**
```
Button wiring with internal pull-up:

    D5 ──┬── Button ── GND
         │
   (Internal 20kΩ pull-up to 5V)
```

### Phase 7: SG90 Servo Motors (×4)

**Components Needed:**
- 4× SG90 Micro Servo Motors
- 12 jumper wires (3 per servo)
- External 5V power supply (recommended for 4 servos)

**Servo Wire Colors (Standard):**
| Wire Color | Connection | Description |
|------------|------------|-------------|
| Brown/Black | GND | Ground |
| Red | 5V | Power (500mA peak per servo) |
| Orange/Yellow | Signal Pin | PWM control signal |

**Individual Servo Connections:**

| Servo | Function | Signal Pin | Position |
|-------|----------|------------|----------|
| Shoot Left | Left door of shoot | D6 | Top entrance |
| Shoot Right | Right door of shoot | D7 | Top entrance |
| Flapper Left | Left side of flapper | D8 | Internal divider |
| Flapper Right | Right side of flapper | D9 | Internal divider |

**Power Warning ⚠️:**
4 servos × 250mA = 1A peak current!
- Arduino 5V pin can only supply ~400mA
- **Recommended**: Use external 5V 2A power supply for servos
- Connect external supply GND to Arduino GND

**External Servo Power Setup:**
```
External 5V Supply (+) ──► All Servo Red wires
External 5V Supply (-) ──┬─► All Servo Brown wires
                         └─► Arduino GND (common ground!)

Arduino D6 ──► Shoot Left Signal (Orange)
Arduino D7 ──► Shoot Right Signal (Orange)
Arduino D8 ──► Flapper Left Signal (Orange)
Arduino D9 ──► Flapper Right Signal (Orange)
```

**Servo Position Reference:**
| Servo | Closed/Center | Open/Offset |
|-------|---------------|-------------|
| Shoot Left | 0° | 90° |
| Shoot Right | 180° | 90° |
| Flapper (both) | 90° (center) | ±45° (directional) |

### Phase 8: Relay Module & Water Pump

**Components Needed:**
- 1-Channel Relay Module (5V)
- Water Pump (12V DC)
- 5 jumper wires

**Relay Module Connections:**
| Relay Pin | Connection | Description |
|-----------|------------|-------------|
| VCC | Arduino 5V | Module power |
| GND | Arduino GND | Module ground |
| IN | Arduino D10 | Control signal |
| COM | 12V Battery + | Common terminal |
| NO | Water Pump (+) | Normally Open contact |
| NC | (unused) | Normally Closed contact |

**Water Pump Connections:**
| Pump Wire | Connection |
|-----------|------------|
| Positive (+) | Relay NO terminal |
| Negative (-) | 12V Battery GND |

**Relay Operation:**
- **D10 LOW**: Relay OFF → Pump OFF (NO contact open)
- **D10 HIGH**: Relay ON → Pump ON (NO contact closed)

**Wiring Diagram:**
```
12V Battery (+) ──► Relay COM
                         │
                    ┌────┴────┐
                    │  RELAY  │
                    │    NO ──┼──► Water Pump (+)
                    │    NC   │    (unused)
                    └─────────┘
                    
Water Pump (-) ──────────────────► 12V Battery (-)
```

**Safety Notes:**
- Never exceed relay contact rating (usually 10A)
- Use flyback diode across pump if experiencing relay damage
- Check pump amperage before connecting

---

## Sensor Connections

### Proximity Sensor Detailed Wiring

#### NPN Sensor Output Explanation

The LJ18A3 and LJC18A3 are NPN-type sensors:
- They "sink" current when activated
- Output goes LOW when object detected
- Require pull-up resistor (internal or external)

```
NPN Sensor Operation:

                    12V
                     │
              ┌──────┴──────┐
              │   Sensor    │
              │   (NPN)     │
              │             │
              │  ┌──────┐   │
              │  │ OUT  │───┼──► To Arduino (D2/D3)
              │  └──────┘   │    (with internal pull-up enabled in code)
              │             │
              └──────┬──────┘
                     │
                    GND
```

**Arduino Code Configuration:**
```cpp
pinMode(PIN_INDUCTIVE_SENSOR, INPUT);  // Uses default (floating)
// Sensor has internal pull-up in most cases

// Detection logic:
if (digitalRead(PIN_INDUCTIVE_SENSOR) == LOW) {
    // Metal detected!
}
```

### MQ135 Sensor Module Pinout

```
┌─────────────────────┐
│       MQ135         │
│     Gas Sensor      │
│   ┌─────────────┐   │
│   │  Sensing    │   │
│   │  Element    │   │
│   └─────────────┘   │
│                     │
│  VCC  GND  DO  AO   │
│   │    │   │   │    │
└───┼────┼───┼───┼────┘
    │    │   │   │
    5V  GND  NC  A0 (Arduino)
```

**Threshold Calibration:**
| Environment | Typical Reading |
|-------------|-----------------|
| Clean air | 100-200 |
| Paper/cardboard | 300-500 |
| Organic waste | 500-800 |
| Strong organic | 800+ |

---

## Servo Motor Connections

### Mechanical Setup

**Shoot Door Assembly:**
```
Top View (Shoot Opening):
┌───────────────────────┐
│                       │
│    [Shoot Opening]    │
│                       │
├───────────┬───────────┤
│  Servo L  │  Servo R  │
│    (D6)   │    (D7)   │
└───────────┴───────────┘
     │           │
     ▼           ▼
 Closed: 0°   Closed: 180°
 Open: 90°    Open: 90°
```

**Flapper Assembly:**
```
Side View (Internal):
         ┌─────────────┐
         │   Flapper   │ ◄── Vibration Sensor attached
         │   Board     │
         └──────┬──────┘
                │
        ┌───────┴───────┐
        │               │
   Servo L (D8)    Servo R (D9)
        │               │
        ▼               ▼
   Center: 90°     Center: 90°
   Metal: 45°      Metal: 45°
   Plastic: 135°   Plastic: 135°
```

**Flapper Movement:**
- **Center (90°)**: Aligned with vertical divider
- **Metal (-45° offset)**: Directs to RIGHT bin
- **Plastic (+45° offset)**: Directs to LEFT bin

### Servo Wiring Bundle

```
Recommended wire bundling:

Servo Power Bundle (Red + Brown wires):
┌─────────────────────────────┐
│ All 4 Brown → Common GND    │
│ All 4 Red → 5V Power Bus    │
└─────────────────────────────┘

Servo Signal Bundle (Orange wires):
┌─────────────────────────────┐
│ Shoot L Orange → D6         │
│ Shoot R Orange → D7         │
│ Flapper L Orange → D8       │
│ Flapper R Orange → D9       │
└─────────────────────────────┘
```

---

## Relay & Pump Connections

### Relay Module Detail

```
┌─────────────────────────────────────┐
│           RELAY MODULE              │
│  ┌─────────────────────────────┐    │
│  │    ┌─────┐                  │    │
│  │    │RELAY│  LED  Optocoupler│    │
│  │    │COIL │   ●              │    │
│  │    └─────┘                  │    │
│  └─────────────────────────────┘    │
│                                     │
│   Low Voltage Side (Arduino)        │
│  ┌──────┬──────┬──────┐             │
│  │ VCC  │  IN  │ GND  │             │
│  └──┬───┴──┬───┴──┬───┘             │
│     │      │      │                 │
│    5V     D10    GND                │
│                                     │
│   High Voltage Side (Pump Power)    │
│  ┌──────┬──────┬──────┐             │
│  │ COM  │  NO  │  NC  │             │
│  └──┬───┴──┬───┴──────┘             │
│     │      │                        │
│   12V+   Pump(+)                    │
│                                     │
└─────────────────────────────────────┘
```

### Complete Pump Circuit

```
12V Battery
    │
    ├──(+)───► Relay COM terminal
    │                │
    │           [RELAY SWITCH]
    │                │
    │                └──► NO terminal ──► Water Pump (+)
    │                                          │
    │                                          │
    └──(-)──────────────────────► Water Pump (-)
```

### Water Pump Specifications

**Typical 12V DC Water Pump:**
- Voltage: 12V DC
- Current: 0.5-1.5A
- Flow rate: 1-3 L/min
- Head height: 1-3m

**Tubing Setup:**
```
[Water Container] ──► [Pump Intake] ══► [Pump Outlet] ──► [Dispenser Nozzle]
                                                               │
                                                               ▼
                                                          [Paper Cup]
```

---

## Testing & Verification

### Pre-Power Checklist

**Visual Inspection:**
- [ ] No bare wires touching each other
- [ ] Correct polarity on all power connections
- [ ] Servo wires not tangled with other components
- [ ] LCD and sensors properly seated
- [ ] Relay module connections secure
- [ ] 12V and 5V rails not connected together (except GND)

### Progressive Testing Sequence

#### Test 1: Arduino Power
1. Connect Arduino via USB only (no external power)
2. Verify power LED illuminates
3. Upload basic blink sketch to confirm operation

#### Test 2: LCD Display
1. Connect LCD (VCC, GND, SDA, SCL)
2. Upload I2C scanner sketch:
```cpp
#include <Wire.h>
void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("I2C Scanner");
}
void loop() {
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found: 0x");
      Serial.println(address, HEX);
    }
  }
  delay(5000);
}
```
3. Should find address 0x27 (or 0x3F)
4. Test LCD hello world sketch

#### Test 3: Sensors Individual Test

**MQ135 Gas Sensor:**
```cpp
void setup() {
  Serial.begin(9600);
}
void loop() {
  int gasValue = analogRead(A0);
  Serial.print("MQ135: ");
  Serial.println(gasValue);
  delay(500);
}
```
- Normal air: 100-300
- Hold organic material near sensor: should increase

**Proximity Sensors:**
```cpp
void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT);  // Inductive
  pinMode(3, INPUT);  // Capacitive
}
void loop() {
  Serial.print("Inductive (Metal): ");
  Serial.print(digitalRead(2));
  Serial.print("  Capacitive: ");
  Serial.println(digitalRead(3));
  delay(200);
}
```
- Place metal object: Inductive should read 0
- Place conductive object: Capacitive should read 0

**TCRT5000 IR Sensor (Plastic Detection):**
```cpp
void setup() {
  Serial.begin(9600);
}
void loop() {
  int irValue = analogRead(A1);
  Serial.print("TCRT5000 IR: ");
  Serial.println(irValue);
  delay(200);
}
```
- No object: High reading (>500)
- Object present: Low reading (<500)
- Plastic detected when: IR triggered AND NOT metal AND NOT capacitive AND NOT gas

**Vibration Sensor:**
```cpp
void setup() {
  Serial.begin(9600);
  pinMode(4, INPUT);
}
void loop() {
  Serial.print("Vibration: ");
  Serial.println(digitalRead(4));
  delay(100);
}
```
- Tap sensor: should read 1

**Button:**
```cpp
void setup() {
  Serial.begin(9600);
  pinMode(5, INPUT_PULLUP);
}
void loop() {
  Serial.print("Button: ");
  Serial.println(digitalRead(5));
  delay(100);
}
```
- Not pressed: 1
- Pressed: 0

#### Test 4: Servos Individual Test
```cpp
#include <Servo.h>
Servo testServo;

void setup() {
  Serial.begin(9600);
  testServo.attach(6);  // Change pin for each servo: 6, 7, 8, 9
}

void loop() {
  testServo.write(0);
  Serial.println("Position: 0");
  delay(1000);
  
  testServo.write(90);
  Serial.println("Position: 90");
  delay(1000);
  
  testServo.write(180);
  Serial.println("Position: 180");
  delay(1000);
}
```
- Test each servo individually
- Verify full range of motion
- Check for binding or obstruction

#### Test 5: Relay & Pump Test
1. **Relay click test** (no pump connected):
```cpp
void setup() {
  pinMode(10, OUTPUT);
}
void loop() {
  digitalWrite(10, HIGH);
  delay(1000);
  digitalWrite(10, LOW);
  delay(1000);
}
```
- Relay should click ON/OFF every second

2. **Pump test** (with pump connected):
- Verify pump runs when relay clicks ON
- Check for water flow
- Verify pump stops when relay OFF

#### Test 6: Full System Test
1. Upload RVM_v1.ino
2. Open Serial Monitor (9600 baud)
3. Verify startup messages
4. Test material detection sequence
5. Test dispense button functionality
6. Monitor for any errors

### Verification Checklist

**Sensors:**
- [ ] LCD displays startup message
- [ ] MQ135 readings change with organic materials
- [ ] Inductive sensor detects metal (LOW signal)
- [ ] Capacitive sensor detects plastic (LOW signal)
- [ ] Vibration sensor triggers on tap (HIGH signal)
- [ ] Button press registers (LOW when pressed)

**Actuators:**
- [ ] Shoot servos open/close correctly
- [ ] Flapper servos move left/right
- [ ] All servos return to home position
- [ ] Relay clicks when triggered
- [ ] Water pump runs when relay active

**Integration:**
- [ ] Metal detection opens shoot, moves flapper right
- [ ] Plastic detection opens shoot, moves flapper left
- [ ] Biodegradable rejection (shoot stays closed)
- [ ] Vibration triggers credit addition
- [ ] Button dispenses water when credits available

---

## Troubleshooting

### LCD Issues

**Problem: LCD backlight on but no text**
- **Solution**: Adjust contrast potentiometer on I2C backpack

**Problem: LCD not detected**
- **Solution 1**: Check I2C wiring (SDA→A4, SCL→A5)
- **Solution 2**: Try alternate address (0x3F instead of 0x27)
- **Solution 3**: Check 5V power supply

**Problem: Garbled characters**
- **Solution**: Check for loose connections, reduce I2C wire length

### Sensor Issues

**Problem: Proximity sensors always triggered**
- **Solution 1**: Check power supply voltage (needs 6-36V, recommend 12V)
- **Solution 2**: Increase distance between sensors
- **Solution 3**: Reduce sensitivity (if adjustable)

**Problem: MQ135 always reads high**
- **Solution 1**: Allow 2-5 minute warm-up time
- **Solution 2**: Calibrate in clean air environment
- **Solution 3**: Adjust threshold in code

**Problem: Vibration sensor too sensitive/not sensitive**
- **Solution**: Adjust potentiometer on module

**Problem: Button doesn't register**
- **Solution 1**: Check INPUT_PULLUP is enabled
- **Solution 2**: Verify wiring (signal to D5, other to GND)

### Servo Issues

**Problem: Servos jittering**
- **Solution 1**: Use external power supply (not Arduino 5V)
- **Solution 2**: Add decoupling capacitor (100µF) near servos
- **Solution 3**: Check for mechanical binding

**Problem: Servos not moving**
- **Solution 1**: Verify signal wire connection
- **Solution 2**: Check power supply capacity
- **Solution 3**: Test servo individually

**Problem: Servos moving erratically at startup**
- **Solution**: Add small delay in setup() before attaching servos

### Relay/Pump Issues

**Problem: Relay not clicking**
- **Solution 1**: Check 5V supply to relay module
- **Solution 2**: Verify IN pin connected to D10
- **Solution 3**: Test relay with direct HIGH signal

**Problem: Relay clicks but pump doesn't run**
- **Solution 1**: Check 12V supply to relay COM
- **Solution 2**: Verify pump connected to NO (not NC)
- **Solution 3**: Test pump directly with 12V

**Problem: Pump runs continuously**
- **Solution**: Check code - ensure relay pin initialized LOW

### Power Issues

**Problem: System resets when servos move**
- **Solution**: Use external 5V supply for servos (voltage sag)

**Problem: Arduino resets when pump starts**
- **Solution**: Separate power supplies for logic and high-current devices

**Problem: Sensors giving erratic readings**
- **Solution**: Add decoupling capacitors, improve grounding

---

## Safety Warnings

### Electrical Safety

⚠️ **12V Battery Precautions:**
- Never short-circuit battery terminals
- Use appropriate fuse (5A recommended)
- Ensure proper polarity before connecting
- Keep battery terminals insulated

⚠️ **Water + Electronics:**
- Keep electronics in separate enclosure from water pump
- Use waterproof enclosure for Arduino and sensors
- Route pump tubing away from electrical components
- Use drip loops on cables

⚠️ **Servo Power:**
- Never exceed servo voltage rating (typically 6V max)
- Use adequate power supply for multiple servos
- Avoid mechanical stalling (causes overcurrent)

### Mechanical Safety

⚠️ **Moving Parts:**
- Keep fingers away from shoot door and flapper
- Ensure enclosure prevents user contact with servos
- Add guards around moving mechanisms

⚠️ **Sensor Placement:**
- Mount sensors securely to prevent false readings
- Protect sensors from moisture and debris
- Keep sensor wiring organized and secured

---

## Wiring Checklist

### Complete Installation Checklist

**Power:**
- [ ] 12V battery connected with correct polarity
- [ ] Arduino powered (USB or Vin)
- [ ] Common ground between all power supplies
- [ ] External 5V supply for servos (if used)

**LCD Display:**
- [ ] VCC → 5V
- [ ] GND → GND
- [ ] SDA → A4
- [ ] SCL → A5

**Proximity Sensors:**
- [ ] Inductive: Brown→12V+, Blue→12V GND, Black→D2
- [ ] Capacitive: Brown→12V+, Blue→12V GND, Black→D3

**TCRT5000 IR Sensor:**
- [ ] VCC → 5V
- [ ] GND → GND
- [ ] AO → A1

**MQ135 Gas Sensor:**
- [ ] VCC → 5V
- [ ] GND → GND
- [ ] AO → A0

**Vibration Sensor:**
- [ ] VCC → 5V
- [ ] GND → GND
- [ ] DO → D4

**Button:**
- [ ] One pin → D5
- [ ] Other pin → GND

**Servos (×4):**
- [ ] All Brown → GND
- [ ] All Red → 5V (external recommended)
- [ ] Shoot Left Orange → D6
- [ ] Shoot Right Orange → D7
- [ ] Flapper Left Orange → D8
- [ ] Flapper Right Orange → D9

**Relay & Pump:**
- [ ] Relay VCC → 5V
- [ ] Relay GND → GND
- [ ] Relay IN → D10
- [ ] Relay COM → 12V+
- [ ] Relay NO → Pump (+)
- [ ] Pump (-) → 12V GND

---

## Appendix: Component Specifications

### LJ18A3-8-Z/BX NPN Inductive Proximity Sensor
- Operating Voltage: 6-36V DC
- Output Type: NPN (Normally Open)
- Detection Distance: 8mm (adjustable)
- Output Current: 200mA max
- Detection Object: Ferrous metals

### LJC18A3-B-Z/BY Capacitive Proximity Sensor
- Operating Voltage: 6-36V DC
- Output Type: NPN
- Detection Distance: 1-10mm (adjustable)
- Output Current: 200mA max
- Detection Object: Metal, plastic, liquid, powder
- Note: Used to detect conductive materials in RVM v2

### TCRT5000 IR Reflective Sensor
- Operating Voltage: 5V DC
- Operating Current: 20-60mA
- Detection Distance: 1-25mm (optimal ~2-15mm)
- Output: Analog (0-1023)
- Detection Object: Any reflective surface
- Note: Primary plastic detection sensor in RVM v2

### MQ135 Gas Sensor
- Operating Voltage: 5V DC
- Heater Current: ~150mA
- Detection: NH3, NOx, alcohol, benzene, smoke, CO2
- Preheat Time: 20+ seconds (24 hours for best accuracy)

### SG90 Micro Servo
- Operating Voltage: 4.8-6V DC
- Stall Torque: 1.8 kg-cm (4.8V)
- Operating Speed: 0.1 sec/60° (4.8V)
- Rotation Range: 0-180°
- Dimensions: 23 × 12.2 × 29mm

### SW-420 Vibration Sensor
- Operating Voltage: 3.3-5V DC
- Output: Digital (High/Low)
- Sensitivity: Adjustable via potentiometer

---

**Document Version**: 2.0  
**Last Updated**: January 26, 2026  
**Compatible with**: RVM_v2.ino  
**Author**: K1taru

---

## Quick Reference Card

### Pin Assignments Summary
```
SENSORS:           SERVOS:            RELAY:
A0 - MQ135         D6 - Shoot L       D10 - Water Pump
D2 - Inductive     D7 - Shoot R
D3 - Capacitive    D8 - Flapper L
D4 - Vibration     D9 - Flapper R
D5 - Button

I2C LCD: A4 (SDA), A5 (SCL)
```

### Sensor Signal Logic
| Sensor | No Detection | Detection |
|--------|--------------|-----------|
| Inductive (Metal) | HIGH | LOW |
| Capacitive (Plastic) | HIGH | LOW |
| Vibration | LOW | HIGH |
| Button (w/ pullup) | HIGH | LOW |
| MQ135 | <400 | >400 (organic) |

### Servo Positions
| Servo | Closed/Center | Open/Active |
|-------|---------------|-------------|
| Shoot L | 0° | 90° |
| Shoot R | 180° | 90° |
| Flapper | 90° | 45° or 135° |

---

**END OF WIRING GUIDE**