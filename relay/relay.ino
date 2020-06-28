int AC_WaterPump = 8;

void setup() {
  // put your setup code here, to run once:
  pinMode(AC_WaterPump, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i=0;i<=10;i++){
    digitalWrite(AC_WaterPump, HIGH);
    delay(1000);
  }
  digitalWrite(AC_WaterPump, LOW);//dev/ttyACM0
    delay(3000);
}
