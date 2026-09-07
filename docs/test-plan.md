# Hardware Test Plan

Use this checklist to reproduce the hardware validation process.

## 1. Visual Inspection

- [ ] Confirm Arduino Mega 2560 board
- [ ] Confirm HC-SR04 module
- [ ] Confirm OLED model and voltage requirements
- [ ] Confirm buzzer type
- [ ] Check jumper wires for damage
- [ ] Verify common ground

## 2. OLED Test

- [ ] Connect OLED only
- [ ] Upload firmware
- [ ] Confirm startup message appears
- [ ] Verify I2C address
- [ ] Verify SDA/SCL wiring
- [ ] Confirm display remains stable

## 3. Ultrasonic Sensor Test

Place a flat object at known distances.

| Target distance | Measured distance | Pass? |
|---:|---:|---|
| 5 cm |  |  |
| 10 cm |  |  |
| 15 cm |  |  |
| 20 cm |  |  |
| 30 cm |  |  |
| 50 cm |  |  |

Acceptance guideline:

- readings should be reasonably stable
- large repeated errors should be investigated
- timeout behaviour should not freeze the system

## 4. Buzzer Polarity Test

- [ ] Confirm buzzer is OFF in the safe zone
- [ ] Confirm active-low / active-high setting
- [ ] Change `BUZZER_ACTIVE_LOW` if necessary

## 5. Warning-Zone Test

### Above 20 cm
- [ ] No audible warning

### Greater than 10 cm and up to 20 cm
- [ ] Slow beep

### 3–10 cm
- [ ] Fast beep

### Below 3 cm
- [ ] Continuous warning

## 6. Timeout Test

Temporarily disconnect or obstruct the ultrasonic sensor.

Expected behaviour:

- [ ] OLED displays `NO READING`
- [ ] Buzzer turns off
- [ ] Serial Monitor reports timeout
- [ ] Program continues running

## 7. Continuous Operation Test

Run the system for at least 10 minutes.

- [ ] No freezes
- [ ] OLED continues refreshing
- [ ] Distance remains responsive
- [ ] Buzzer transitions correctly
- [ ] No components become abnormally hot

## 8. Documentation Update

Project documentation checklist:

- [ ] Add a project photo
- [ ] Add a wiring photo or diagram
- [ ] Add a demo video link
- [ ] Record hardware validation results and date
- [ ] Record any final pin changes
- [ ] Record known limitations
