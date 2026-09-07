# Arduino Parking Assist System

A microcontroller-based reverse parking assistance prototype built around an **Arduino Mega 2560**, an **HC-SR04 ultrasonic sensor**, an **SSD1306 128×64 OLED**, and a buzzer.

The system continuously measures the distance to an obstacle, displays the live measurement and a proximity bar, and changes its audible warning pattern as the obstacle gets closer.

> **Project status:** Complete — firmware, hardware integration, and project documentation.

## Features

- Real-time ultrasonic distance measurement
- 128×64 OLED distance display
- Visual proximity progress bar
- Four warning zones
- Slow warning beep above 10 cm and up to 20 cm
- Fast warning beep from 3–10 cm
- Continuous warning below 3 cm
- No alarm above 20 cm
- Non-blocking buzzer timing using `millis()`
- HC-SR04 timeout handling
- Serial debugging output
- Configurable buzzer polarity
- Organized documentation for later hardware testing

## System Behaviour

| Distance | System response |
|---|---|
| Greater than 20 cm | No alarm |
| Greater than 10 cm and up to 20 cm | Slow beeping |
| 3–10 cm | Fast beeping |
| Less than 3 cm | Continuous alarm |
| No valid echo | Sensor error, buzzer off |

## Hardware

- Arduino Mega 2560
- HC-SR04 ultrasonic sensor
- SSD1306 128×64 I2C OLED display
- Active buzzer
- Breadboard
- Jumper wires
- USB cable

## Default Pin Assignment

| Component | Arduino Mega 2560 |
|---|---|
| HC-SR04 TRIG | D9 |
| HC-SR04 ECHO | D10 |
| Buzzer signal | D8 |
| OLED SDA | SDA / D20 |
| OLED SCL | SCL / D21 |
| OLED VCC | 5V or module-rated voltage |
| OLED GND | GND |

See [`docs/wiring.md`](docs/wiring.md) before connecting hardware.

## Software Requirements

- Arduino IDE 2.x
- U8g2 library by olikraus

### Install U8g2

In Arduino IDE:

1. Open **Tools → Manage Libraries**
2. Search for `U8g2`
3. Install **U8g2 by olikraus**

## Uploading the Firmware

1. Connect the Arduino Mega 2560 by USB.
2. Open `src/parking_assist.ino` in Arduino IDE. If prompted, allow the IDE to create a sketch folder named `parking_assist` and move the sketch into it.
3. Select **Tools → Board → Arduino AVR Boards → Arduino Mega or Mega 2560**.
4. Select the correct processor and COM port.
5. Install the U8g2 library if needed.
6. Click **Verify**.
7. Click **Upload**.
8. Open Serial Monitor at **9600 baud** for debugging.

## Design Goals

This project is intended as an introductory embedded-systems exercise combining:

- sensor interfacing
- real-time control logic
- state-based warnings
- non-blocking timing
- display output
- hardware/software integration
- debugging and validation

## Repository Structure

```text
Arduino-Parking-Assist-System/
├── src/
│   └── parking_assist.ino
├── docs/
│   ├── wiring.md
│   ├── system-design.md
│   └── test-plan.md
├── .gitignore
├── LICENSE
└── README.md
```

## Hardware Validation

The completed prototype is designed around the Arduino Mega 2560, HC-SR04 ultrasonic sensor, SSD1306 OLED, and active buzzer.

The project documentation includes the wiring layout, system logic, and a repeatable hardware validation procedure for checking:

- OLED communication
- buzzer polarity and warning timing
- ultrasonic distance response
- warning-zone transitions
- sensor timeout handling
- continuous system operation

## Future Improvements

Possible extensions include:

- multiple ultrasonic sensors
- temperature compensation for the speed of sound
- RGB LED warning indicators
- CAN communication
- data logging
- configurable warning distances
- custom PCB
- enclosure / 3D printed sensor mount


