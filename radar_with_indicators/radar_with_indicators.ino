#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

const int ECHO_PIN = 2;
const int TRIG_PIN = 3;

const int GREEN_LED = 5;
const int YELLOW_LED = 6;
const int RED_LED = 7;

const int BUZZER_PIN = 8;


const int JOYSTICK_X_PIN = A0;
const int SERVO_PIN = 10;

LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo myServo;
int servoAngle = 90;
const int JOYSTICK_THRESHOLD = 50;
const int minDelay = 5;
const int maxDelay = 75;

void setup() {
  myServo.attach(SERVO_PIN);
  myServo.write(servoAngle);

  lcd.init();
  lcd.backlight();

  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  int distance = getDistance();
  showDistance(distance);

  proximityLED(distance);
  proximityBuzzer(distance);
  changeServoAngle();
  
  delay(50);
}

int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  float duration = pulseIn(ECHO_PIN, HIGH);

  int distance = duration * 0.034 / 2; // 0.034 is the speed of sound

  return distance;
}

void showDistance(int distance) {
  lcd.setCursor(0, 0);
  lcd.print("Distance: ");
  lcd.print(getDistance());
  lcd.print(" cm      ");
}

void proximityLED(int distance) {
  if (distance > 30) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
  } else if (distance > 10) {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, LOW);
  } else {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);
  }
}

void proximityBuzzer(int distance) {
  static unsigned long previousBuzzerMillis = 0;
  static int buzzerState = LOW;
  unsigned long currentMillis = millis();

  int buzzerFrequency;
  int buzzerInterval;

  if (distance <= 10) {
    buzzerFrequency = 1000;
    buzzerInterval = 100;
  } else if (distance <= 30) {
    buzzerFrequency = 500;
    buzzerInterval = 300;
  } else {
    buzzerFrequency = 0;
  }

  if (buzzerFrequency > 0 && currentMillis - previousBuzzerMillis >= buzzerInterval) {
    previousBuzzerMillis = currentMillis;
    buzzerState = !buzzerState;

    if (buzzerState) {
      tone(BUZZER_PIN, buzzerFrequency);
    } else {
      noTone(BUZZER_PIN);
    }
  } else if (buzzerFrequency == 0) {
    noTone(BUZZER_PIN);
  }
}

void changeServoAngle() {
  int joystickValue = analogRead(JOYSTICK_X_PIN);
  
  int tiltIntensity = abs(joystickValue - 512);
  
  if (tiltIntensity > JOYSTICK_THRESHOLD) {
    int dynamicDelay = map(tiltIntensity, JOYSTICK_THRESHOLD, 512, maxDelay, minDelay);
    
    if (joystickValue < 512 - JOYSTICK_THRESHOLD) {
      servoAngle -= 10;
    } else if (joystickValue > 512 + JOYSTICK_THRESHOLD) {
      servoAngle += 10;
    }
    
    servoAngle = constrain(servoAngle, 0, 180);
    
    myServo.write(servoAngle);
    
    delay(dynamicDelay);
  }
  
  // Debugging information
  Serial.print("Joystick: ");
  Serial.print(joystickValue);
  Serial.print(" | Servo Angle: ");
  Serial.println(servoAngle);
}