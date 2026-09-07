# Wiring Guide

## Important

This document describes the wiring used for the prototype.

Always disconnect USB/power while changing wiring.

## Arduino Mega 2560 Connections

### HC-SR04 Ultrasonic Sensor

| HC-SR04 pin | Mega 2560 pin |
|---|---|
| VCC | 5V |
| GND | GND |
| TRIG | D9 |
| ECHO | D10 |

The HC-SR04 measures distance by sending an ultrasonic pulse and measuring the time required for the echo to return.

### Buzzer

| Buzzer pin | Mega 2560 pin |
|---|---|
| Signal / IN | D8 |
| VCC | Module-rated supply |
| GND | GND |

The firmware defaults to an **active-low buzzer**.

If the buzzer turns on when the signal is HIGH instead, change:

```cpp
const bool BUZZER_ACTIVE_LOW = true;
```

to:

```cpp
const bool BUZZER_ACTIVE_LOW = false;
```

### SSD1306 OLED

The Arduino Mega 2560 hardware-I2C pins are:

| OLED pin | Mega 2560 pin |
|---|---|
| SDA | D20 / SDA |
| SCL | D21 / SCL |
| GND | GND |
| VCC | Module-rated supply |

Most SSD1306 I2C displays use address `0x3C`, but some use `0x3D`.

The current U8g2 constructor uses the display's default I2C address. If the display is not detected during hardware testing, verify its address with an I2C scanner.

## Planned Signal Flow

```text
Obstacle
   |
   v
HC-SR04
   |
   | distance measurement
   v
Arduino Mega 2560
   |             |
   |             +----> SSD1306 OLED
   |
   +------------------> Buzzer
```

## Pin Changes

The firmware is intentionally easy to modify.

At the top of `src/parking_assist.ino`:

```cpp
const uint8_t TRIG_PIN = 9;
const uint8_t ECHO_PIN = 10;
const uint8_t BUZZER_PIN = 8;
```

Change these constants if different pins are needed during assembly.
