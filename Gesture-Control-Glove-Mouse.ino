#include <Wire.h>
#include <Mouse.h>
#include <MPU6050.h>

#define leftButtonPin 6    // Left mouse button pin
#define rightButtonPin 7   // Right mouse button pin
#define flexSensorPin A0   // Flex sensor pin

MPU6050 mpu;

// Function: Sliding Average Filter to smooth sensor readings
int slidingAverageFilterTime(int rawValue) {
  static const int numReadings = 3;
  static int readings[numReadings] = {0};
  static int index = 0;
  static int total = 0;

  total -= readings[index];
  readings[index] = rawValue;
  total += readings[index];
  index = (index + 1) % numReadings;

  int smoothedValue = total / numReadings;
  return smoothedValue;
}

// Variables for Flex Sensor
int flexBaseline = 0;
bool flexScrolledUp = false;
bool flexScrolledDown = false;

// Variables for Button States
int leftButtonState = HIGH;
int lastLeftButtonState = HIGH;

int rightButtonState = HIGH;
int lastRightButtonState = HIGH;

// Mouse event timing control
unsigned long lastMouseEventTime = 0;
const unsigned long mouseEventInterval = 10; // Minimum interval in ms

void setup() {
  Serial.begin(9600);

  // Initialize buttons with internal pull-up resistors
  pinMode(leftButtonPin, INPUT_PULLUP);
  pinMode(rightButtonPin, INPUT_PULLUP);
  pinMode(flexSensorPin, INPUT);

  // Calibrate baseline value for flex sensor
  flexBaseline = analogRead(flexSensorPin);
  // Serial.print("Flex Baseline: ");
  // Serial.println(flexBaseline);

  // Initialize MPU6050
  Wire.begin();
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    while (1);
  }

  // Initialize Mouse HID
  Mouse.begin();
}

void loop() {
  unsigned long currentTime = millis();

  // --- Handle Left Mouse Button ---
  int readingLeft = digitalRead(leftButtonPin);
  if (readingLeft != lastLeftButtonState) {
    if (readingLeft == LOW) {
      // Button Pressed
      Mouse.press(MOUSE_LEFT);
    } else if(readingLeft == HIGH) {
      // Button Released
      Mouse.release(MOUSE_LEFT);
    }
  }
  lastLeftButtonState = readingLeft;

  // --- Handle Right Mouse Button ---
  int readingRight = digitalRead(rightButtonPin);
  if (readingRight != lastRightButtonState) {
    if (readingRight == LOW) {
      Mouse.press(MOUSE_RIGHT);
    } else if(readingRight == HIGH) {
      Mouse.release(MOUSE_RIGHT);
    }
  }
  lastRightButtonState = readingRight;

  // --- Handle Scrolling (Flex Sensor + Palm Orientation) ---
  int flexValue = slidingAverageFilterTime(analogRead(flexSensorPin));
  int flexThreshold = flexBaseline + 60;  // Threshold for detecting a "fist" or bent finger

  if (flexValue > flexThreshold) { // Finger is bent
    // Read accelerometer data to determine palm orientation
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);

    // Convert Z-axis value to g-force
    float az_g = az / 16384.0;

    float orientationThreshold = 0.5; // Threshold for orientation detection

    if (az_g > orientationThreshold && !flexScrolledUp) { // Palm facing UP -> Scroll UP
      if (currentTime - lastMouseEventTime > mouseEventInterval) {
        Mouse.move(0, 0, 1); // Scroll up
        lastMouseEventTime = currentTime;
      }
      flexScrolledUp = true;
      flexScrolledDown = false;
    } else if (az_g < -orientationThreshold && !flexScrolledDown) { // Palm facing DOWN -> Scroll DOWN
      if (currentTime - lastMouseEventTime > mouseEventInterval) {
        Mouse.move(0, 0, -1); // Scroll down
        lastMouseEventTime = currentTime;
      }
      flexScrolledDown = true;
      flexScrolledUp = false;
    }
  } else {
    // Reset scroll states when finger is straight
    flexScrolledUp = false;
    flexScrolledDown = false;
  }

  // --- Handle Cursor Movement (MPU6050 Gyroscope) ---
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Sensitivity adjustment (Higher value = Slower movement)
  float sensitivity = 260.0;

  int moveX = gx / sensitivity; // Map Gyro X to Mouse X
  int moveY = gy / sensitivity; // Map Gyro Y to Mouse Y

  int threshold = 2; // Deadzone to ignore small jitters
  if (abs(moveX) > threshold || abs(moveY) > threshold) {
    if (currentTime - lastMouseEventTime > mouseEventInterval) {
      Mouse.move(moveX, moveY);
      lastMouseEventTime = currentTime;
    }
  }

  // Short delay to prevent CPU overload
  delay(10);
}