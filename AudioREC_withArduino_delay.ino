int rec=2;
int play=3;

void setup() {
Serial.begin(9600);
pinMode(rec,OUTPUT);
pinMode(play,OUTPUT);

digitalWrite(rec,LOW);
digitalWrite(play,LOW);

}

void loop() {
  digitalWrite(rec,HIGH);
  delay(3000);
  digitalWrite(rec,LOW);

  delay(500); // small gap

  digitalWrite(play,HIGH);
  delay (100);
  digitalWrite(play,LOW);

  delay(4000);
}
