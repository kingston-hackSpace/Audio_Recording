int rec  = 2;   // REC pin on ISD1820
int play = 3;   // PLAYE pin on ISD1820

unsigned long previousMillis = 0;
unsigned long playStart = 0;

// Time settings (in milliseconds)
const unsigned long RECORD_TIME = 3000;   // 3 seconds recording
const unsigned long PAUSE_TIME  = 100;    // 100 ms pause after recording
const unsigned long PLAY_TIME   = 300;    // 300 ms PLAYE pulse
const unsigned long WAIT_TIME   = 6000;   // wait before next cycle

int state = 0; // 0=Record, 1=Pause after record, 2=Play, 3=Wait
bool playTriggered = false;

void setup() {
  pinMode(rec, OUTPUT);
  pinMode(play, OUTPUT);

  digitalWrite(rec, LOW);
  digitalWrite(play, LOW);
}

void loop() {
  unsigned long currentMillis = millis();

  // Record
  if(state == 0) { 
    digitalWrite(rec, HIGH);  // Start recording
    previousMillis = currentMillis; //start timer
    state = 1; // Go to next state
  }

  // Recording in progress
  else if(state == 1) { 
    if(currentMillis - previousMillis >= RECORD_TIME){
      digitalWrite(rec, LOW);  // Stop recording
      previousMillis = currentMillis; // update timer
      state = 2; // Go to next state
    }
  }

  // Pause after recording (needed waiting time to trigger play)
  else if(state == 2) {
    if(currentMillis - previousMillis >= PAUSE_TIME){
      playTriggered = false;     
      state = 3; // Go to next state
    }
  }

  // Play
  else if(state == 3) { 
    if(!playTriggered){
      digitalWrite(play, HIGH); // start PLAYE pulse
      playStart = currentMillis;
      playTriggered = true;
    }
    if(playTriggered && currentMillis - playStart >= PLAY_TIME){
      digitalWrite(play, LOW);  // end PLAYE pulse
      previousMillis = currentMillis;
      state = 4; // Go to next state
    }
  }

  // Wait for a complete playback
  else if(state == 4) { 
    if(currentMillis - previousMillis >= WAIT_TIME){
      state = 0; // start recording again
    }
  }
}
