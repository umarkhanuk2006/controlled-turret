#include <Wire.h>
#include <math.h>
#include <Servo.h>

Servo panServo;
Servo tiltServo;
const byte MPU_ADDRESS = 0x68;
const int WINDOW_SIZE = 128;

int16_t axSamples[WINDOW_SIZE];
int nextSample = 0;
int sampleCount = 0;

int16_t readAxis() {
  uint16_t highByte = Wire.read();
  uint16_t lowByte = Wire.read();

  return (int16_t)((highByte << 8) | lowByte);
}

void printRollingStatistics(int16_t newSample) {

  axSamples[nextSample] = newSample;
  nextSample = (nextSample + 1) % WINDOW_SIZE;

  if (sampleCount < WINDOW_SIZE) {
    sampleCount++;
  }

  if (sampleCount < WINDOW_SIZE) {
    Serial.print("AX window: ");
    Serial.print(sampleCount);
    Serial.println("/128");
    return;
  }

  int32_t sum = 0;

  for (int i = 0; i < WINDOW_SIZE; i++) {
    sum += axSamples[i];
  }

  float mean = (float)sum / WINDOW_SIZE;

  float squaredDifferenceSum = 0;

  for (int i = 0; i < WINDOW_SIZE; i++) {
    float difference = (float)axSamples[i] - mean;
    squaredDifferenceSum += difference * difference;
  }

  float standardDeviation =
      sqrt(squaredDifferenceSum / WINDOW_SIZE);

  Serial.print("AX mean (128): ");
  Serial.print(mean, 1);
  Serial.print("  AX standard deviation (128): ");
  Serial.println(standardDeviation, 1);
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  panServo.attach(9);
  panServo.write(90);
  tiltServo.attach(10);
  tiltServo.write(90);

  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x6B);  
  Wire.write(0x00);

  if (Wire.endTransmission() != 0) {
    Serial.println("Sensor initialization failed.");
    while (true) {
      delay(1000);
    }
  }

  delay(100);
}

void loop() {

  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x3B);

  if (Wire.endTransmission(false) != 0) {
    Serial.println("could not select register");
    delay(500);
    return;
  }

  byte received = Wire.requestFrom(MPU_ADDRESS, (byte)6);

  if (received != 6) {
    Serial.println("incomplete reading");
    delay(500);
    return;
  }

  int16_t ax = readAxis();
  int16_t ay = readAxis();
  int16_t az = readAxis();

  Serial.print("Ax: ");
  Serial.print(ax);
  Serial.print("  Ay: ");
  Serial.print(ay);
  Serial.print("  Az: ");
  Serial.println(az);

  float x = ax;
  float y = ay;
  float z = az;

  float tiltX = atan2(x, sqrt(y * y + z * z)) * 180.0 / PI;
  float tiltY = atan2(y, sqrt(x * x + z * z)) * 180.0 / PI;

  float panAngle = 90.0 + tiltX;
  float tiltAngle = 90.0 + tiltY;


  panAngle = constrain(panAngle, 60.0, 120.0);
  tiltAngle = constrain(tiltAngle, 60.0, 120.0);

  panServo.write((int)panAngle);
  tiltServo.write((int)tiltAngle);

  Serial.print("Pan command: ");
  Serial.print(panAngle, 1);
  Serial.print("  Tilt command: ");
  Serial.println(tiltAngle, 1);

  Serial.print("Tilt X: ");
  Serial.print(tiltX, 1);
  Serial.print(" degrees  Tilt Y: ");
  Serial.print(tiltY, 1);
  Serial.println(" degrees");

  printRollingStatistics(ax);

  delay(100);
}
