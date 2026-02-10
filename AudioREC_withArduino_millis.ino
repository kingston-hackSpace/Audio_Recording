int rec  = 2;
int play = 3;

unsigned long previousMillis = 0;
int state = 0;

void setup() {
  pinMode(rec, OUTPUT);
  pinMode(play, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  switch (state) {

    case 0: // start recording
      digitalWrite(rec, HIGH);
      previousMillis = currentMillis;
      state = 1;
      break;

    case 1: // recording for 3 seconds
      if (currentMillis - previousMillis >= 3000) {
        digitalWrite(rec, LOW);
        previousMillis = currentMillis;
        state = 2;
      }
      break;

    case 2: // play
      digitalWrite(play, HIGH);
      delay(100);               // short edge pulse
      digitalWrite(play, LOW);
      previousMillis = currentMillis;
      state = 3;
      break;

    case 3: // wait before next cycle
      if (currentMillis - previousMillis >= 4000) {
        state = 0;
      }
      break;
  }
}
