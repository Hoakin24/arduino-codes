
const int LED_PIN = 9;

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  int potentiometerValue = analogRead(A0);  

  int calculatedBrightness = map(potentiometerValue, 78, 1023, 0, 255); 
  
  analogWrite(LED_PIN, calculatedBrightness);
  Serial.println(calculatedBrightness);

  delay(10);
}
