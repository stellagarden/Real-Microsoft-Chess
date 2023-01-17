void setup() {  
  Serial.begin(9600);
}

void loop() {
  for(int i=0;i<16;i++){
    Serial.print(i);
    Serial.print(" : ");
    Serial.println(analogRead(i));
  }
  delay(10);
}

