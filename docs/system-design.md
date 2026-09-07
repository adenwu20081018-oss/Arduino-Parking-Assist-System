# System Design

## Objective

Create a simple reverse-parking assistance system that converts measured obstacle distance into visual and audible feedback.

## Inputs

The HC-SR04 provides the only sensor input.

The controller measures the duration of the ultrasonic echo and converts it to distance:

```text
distance = echo_time × speed_of_sound / 2
```

The division by two accounts for the round trip from sensor to obstacle and back.

## Warning State Machine

The measured distance is mapped into one of five states:

```text
SENSOR ERROR
    |
    +-- invalid / timeout

SAFE
    |
    +-- distance > 20 cm

SLOW WARNING
    |
    +-- 10 cm < distance <= 20 cm

FAST WARNING
    |
    +-- 3 cm <= distance <= 10 cm

DANGER
    |
    +-- distance < 3 cm
```

## Non-Blocking Buzzer Logic

The buzzer is controlled using `millis()` instead of long `delay()` calls.

This allows the program to continue:

- measuring distance
- updating the OLED
- printing diagnostic data

while the buzzer is cycling on and off.

### Slow Warning

Approximately:

```text
ON  180 ms
OFF 650 ms
```

### Fast Warning

Approximately:

```text
ON  100 ms
OFF 140 ms
```

### Danger

The buzzer remains continuously active.

## Sensor Timeout Handling

`pulseIn()` is given a timeout.

If no echo pulse is detected:

- the reading is marked invalid
- the OLED displays `NO READING`
- the system enters `SENSOR ERROR`
- the buzzer is disabled
- a timeout message is printed to Serial Monitor

This prevents a missing sensor reading from being interpreted as an extremely close obstacle.

## Display

The OLED shows:

- system title
- live distance in centimetres
- warning state
- proximity bar

The bar becomes fuller as the detected object moves closer.

## Software Architecture

The firmware is divided into small functions:

- `readDistanceCm()` — ultrasonic measurement
- `getWarningZone()` — state classification
- `setBuzzer()` — buzzer output abstraction
- `updateBuzzer()` — non-blocking warning timing
- `updateDisplay()` — OLED rendering
- `drawProgressBar()` — proximity visualization

This makes the code easier to debug and extend.
