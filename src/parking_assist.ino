/*
  Arduino Parking Assist System
  Board: Arduino Mega 2560
  Sensor: HC-SR04 ultrasonic distance sensor
  Display: 128x64 SSD1306 I2C OLED
  Buzzer: Active buzzer (supports active-low or active-high trigger)

  Features:
  - Real-time distance display
  - OLED progress bar
  - Four warning zones
  - Non-blocking buzzer control using millis()
  - Ultrasonic timeout handling
  - Easy pin and threshold configuration

*/

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// -------------------------
// Pin configuration
// -------------------------
const uint8_t TRIG_PIN = 9;
const uint8_t ECHO_PIN = 10;
const uint8_t BUZZER_PIN = 8;

// Change to false if your buzzer turns on when the pin is HIGH.
const bool BUZZER_ACTIVE_LOW = true;

// -------------------------
// Distance thresholds (cm)
// -------------------------
const float NO_ALARM_DISTANCE = 20.0;
const float SLOW_BEEP_DISTANCE = 10.0;
const float FAST_BEEP_DISTANCE = 3.0;

// Maximum valid reading shown on the progress bar.
const float MAX_DISPLAY_DISTANCE = 50.0;

// HC-SR04 echo timeout.
// 30,000 us corresponds to roughly 5 m, longer than needed here.
const unsigned long ECHO_TIMEOUT_US = 30000UL;

// Screen refresh interval.
const unsigned long DISPLAY_INTERVAL_MS = 80UL;

// Buzzer timing.
const unsigned long SLOW_BEEP_ON_MS = 180UL;
const unsigned long SLOW_BEEP_OFF_MS = 650UL;
const unsigned long FAST_BEEP_ON_MS = 100UL;
const unsigned long FAST_BEEP_OFF_MS = 140UL;

// -------------------------
// OLED configuration
// -------------------------
// Mega 2560 hardware I2C:
// SDA = pin 20
// SCL = pin 21
//
// The common 0x3C address is handled by U8g2 automatically for this display.
// If your module uses another address, see docs/wiring.md.
U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(
  U8G2_R0,
  U8X8_PIN_NONE
);

// -------------------------
// Runtime state
// -------------------------
float currentDistanceCm = -1.0;
bool sensorTimedOut = false;

unsigned long lastDisplayUpdate = 0;
unsigned long lastBuzzerToggle = 0;
bool buzzerIsOn = false;

enum WarningZone {
  ZONE_SAFE,
  ZONE_SLOW,
  ZONE_FAST,
  ZONE_DANGER,
  ZONE_SENSOR_ERROR
};

WarningZone currentZone = ZONE_SENSOR_ERROR;

// -------------------------
// Buzzer helpers
// -------------------------
void setBuzzer(bool on) {
  buzzerIsOn = on;

  if (BUZZER_ACTIVE_LOW) {
    digitalWrite(BUZZER_PIN, on ? LOW : HIGH);
  } else {
    digitalWrite(BUZZER_PIN, on ? HIGH : LOW);
  }
}

void stopBuzzer() {
  setBuzzer(false);
}

void updateBuzzer(WarningZone zone) {
  const unsigned long now = millis();

  if (zone == ZONE_SAFE || zone == ZONE_SENSOR_ERROR) {
    stopBuzzer();
    return;
  }

  if (zone == ZONE_DANGER) {
    setBuzzer(true);
    return;
  }

  unsigned long onTime = SLOW_BEEP_ON_MS;
  unsigned long offTime = SLOW_BEEP_OFF_MS;

  if (zone == ZONE_FAST) {
    onTime = FAST_BEEP_ON_MS;
    offTime = FAST_BEEP_OFF_MS;
  }

  const unsigned long currentInterval = buzzerIsOn ? onTime : offTime;

  if (now - lastBuzzerToggle >= currentInterval) {
    lastBuzzerToggle = now;
    setBuzzer(!buzzerIsOn);
  }
}

// -------------------------
// Distance measurement
// -------------------------
float readDistanceCm(bool &timedOut) {
  timedOut = false;

  // Ensure a clean LOW pulse first.
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Trigger the HC-SR04 with a 10 us HIGH pulse.
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo duration.
  const unsigned long duration =
      pulseIn(ECHO_PIN, HIGH, ECHO_TIMEOUT_US);

  if (duration == 0) {
    timedOut = true;
    return -1.0;
  }

  // Speed of sound ~= 0.0343 cm/us.
  // Divide by 2 because the sound travels to the object and back.
  return (duration * 0.0343f) / 2.0f;
}

WarningZone getWarningZone(float distanceCm, bool timedOut) {
  if (timedOut || distanceCm < 0.0) {
    return ZONE_SENSOR_ERROR;
  }

  if (distanceCm > NO_ALARM_DISTANCE) {
    return ZONE_SAFE;
  }

  if (distanceCm > SLOW_BEEP_DISTANCE) {
    return ZONE_SLOW;
  }

  if (distanceCm >= FAST_BEEP_DISTANCE) {
    return ZONE_FAST;
  }

  return ZONE_DANGER;
}

// -------------------------
// OLED drawing
// -------------------------
const char* zoneLabel(WarningZone zone) {
  switch (zone) {
    case ZONE_SAFE:         return "SAFE";
    case ZONE_SLOW:         return "SLOW WARNING";
    case ZONE_FAST:         return "FAST WARNING";
    case ZONE_DANGER:       return "STOP";
    case ZONE_SENSOR_ERROR: return "SENSOR ERROR";
    default:                return "";
  }
}

void drawProgressBar(float distanceCm) {
  const int x = 8;
  const int y = 49;
  const int width = 112;
  const int height = 10;

  oled.drawFrame(x, y, width, height);

  if (distanceCm < 0.0) {
    return;
  }

  // Nearer obstacle = fuller warning bar.
  float clamped = constrain(distanceCm, 0.0f, MAX_DISPLAY_DISTANCE);
  float proximity = 1.0f - (clamped / MAX_DISPLAY_DISTANCE);

  int fillWidth = (int)((width - 4) * proximity);
  fillWidth = constrain(fillWidth, 0, width - 4);

  if (fillWidth > 0) {
    oled.drawBox(x + 2, y + 2, fillWidth, height - 4);
  }
}

void updateDisplay(float distanceCm,
                   WarningZone zone,
                   bool timedOut) {
  oled.clearBuffer();

  oled.setFont(u8g2_font_6x12_tf);
  oled.drawStr(0, 10, "Parking Assist");

  if (timedOut || distanceCm < 0.0) {
    oled.setFont(u8g2_font_7x14B_tf);
    oled.drawStr(0, 31, "NO READING");
  } else {
    char distanceText[24];
    dtostrf(distanceCm, 5, 1, distanceText);

    oled.setFont(u8g2_font_7x14B_tf);
    oled.drawStr(0, 30, distanceText);
    oled.drawStr(48, 30, "cm");
  }

  oled.setFont(u8g2_font_6x12_tf);
  oled.drawStr(0, 43, zoneLabel(zone));

  drawProgressBar(distanceCm);

  oled.sendBuffer();
}

// -------------------------
// Setup and main loop
// -------------------------
void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  stopBuzzer();

  Serial.begin(9600);

  oled.begin();

  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tf);
  oled.drawStr(0, 18, "Parking Assist");
  oled.drawStr(0, 34, "Starting...");
  oled.sendBuffer();

  delay(500);
}

void loop() {
  // pulseIn() has a short timeout, so the loop remains responsive.
  currentDistanceCm = readDistanceCm(sensorTimedOut);
  currentZone = getWarningZone(currentDistanceCm, sensorTimedOut);

  updateBuzzer(currentZone);

  const unsigned long now = millis();

  if (now - lastDisplayUpdate >= DISPLAY_INTERVAL_MS) {
    lastDisplayUpdate = now;

    updateDisplay(
      currentDistanceCm,
      currentZone,
      sensorTimedOut
    );

    if (sensorTimedOut) {
      Serial.println("Sensor timeout");
    } else {
      Serial.print("Distance: ");
      Serial.print(currentDistanceCm, 1);
      Serial.print(" cm | Zone: ");
      Serial.println(zoneLabel(currentZone));
    }
  }
}
