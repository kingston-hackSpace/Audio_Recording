/* 
Three-button recorder/player for Teensy 4 + Audio Shield (Rev D)

Pins:
   
   Record button : pin 2 (hold to record, release to stop recording)
   Play button   : pin 3 (press to play last recording)
   Stop button   : pin 4 (press to stop playback)
   LED           : pin 5


Note: You might need to PRESS THE ONBOARD button on the Teensy to enable uploading this code

*/

#include <Bounce.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

AudioInputI2S            i2s2;
AudioRecordQueue         queue1;
AudioPlaySdWav            playWav1;
AudioOutputI2S            i2s1;
AudioConnection           patchCord1(i2s2, 0, queue1, 0);
AudioConnection           patchCord2(playWav1, 0, i2s1, 0);
AudioConnection           patchCord3(playWav1, 1, i2s1, 1);
AudioControlSGTL5000      sgtl5000_1;

// Buttons — classic Bounce library, bundled with Teensyduino
Bounce buttonRecord = Bounce(2, 8);   // 8 = debounce time in ms
Bounce buttonPlay   = Bounce(3, 8);
Bounce buttonStop   = Bounce(4, 8);

const int ledPin = 5;

// which input on the audio shield will be used?
const int myInput = AUDIO_INPUT_MIC; 
//const int myInput = AUDIO_INPUT_LINEIN;

// SD card pins — Teensy 4.x + Audio Shield Rev D
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7    // Teensy 4 ignores this, uses pin 11 internally
#define SDCARD_SCK_PIN   14   // Teensy 4 ignores this, uses pin 13 internally

int mode = 0;  // 0 = stopped, 1 = recording, 2 = playing

// The file being recorded to / played from
File frec;
const char *recordingFile = "RECORD.WAV";
unsigned long dataBytesWritten = 0;

// Grace period after starting playback before we start checking
// isPlaying(), so a not-yet-started codec doesn't look "stopped"
unsigned long playStartTime = 0;
const unsigned long PLAY_GRACE_MS = 150;

void setup() {
  Serial.begin(9600);

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  AudioMemory(60);

  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.micGain(30);      // only relevant when using AUDIO_INPUT_MIC
  sgtl5000_1.volume(0.5);

  // Initialize the SD card
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  Serial.println("Ready. Hold Record (2) to record, Play (3) to play, Stop (4) to stop.");
}

void loop() {
  buttonRecord.update();
  buttonPlay.update();
  buttonStop.update();

  // Record button pressed -> immediately (re)start recording,
  // overwriting any previous take, stopping playback if it was running.
  if (buttonRecord.fallingEdge()) {
    Serial.println("Record button pressed");
    if (mode == 2) stopPlaying();
    if (mode == 1) stopRecording(); // safety: shouldn't normally happen on a press
    startRecording();
  }
  // Record button released -> stop recording
  if (buttonRecord.risingEdge()) {
    Serial.println("Record button released");
    if (mode == 1) stopRecording();
  }

  // Play button pressed -> (re)start playback
  if (buttonPlay.fallingEdge()) {
    Serial.print("Play button pressed, mode=");
    Serial.println(mode);
    if (mode != 1) {
      startPlaying();
    } else {
      Serial.println("  ignored: currently recording");
    }
  }

  // Stop button pressed -> stop playback
  if (buttonStop.fallingEdge()) {
    Serial.println("Stop button pressed");
    stopPlaying();
  }

  // Keep the current mode going
  if (mode == 1) {
    continueRecording();
  }
  if (mode == 2) {
    continuePlaying();
  }
}

// ---------------------------------------------------------------
// Recording
// ---------------------------------------------------------------

void startRecording() {
  Serial.println("startRecording");
  if (SD.exists(recordingFile)) {
    // Must delete the old file first, since writes append to the end
    SD.remove(recordingFile);
  }
  frec = SD.open(recordingFile, FILE_WRITE);
  if (frec) {
    writeWavHeader(frec, 0);   // placeholder header, patched once we know the size
    dataBytesWritten = 0;
    queue1.begin();
    mode = 1;
    digitalWrite(ledPin, HIGH);
  }
}

void continueRecording() {
  if (queue1.available() >= 2) {
    byte buffer[512];
    // Fetch 2 blocks from the audio library and copy into a 512 byte
    // buffer -- the SD library is most efficient at full sector writes.
    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer + 256, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    frec.write(buffer, 512);
    dataBytesWritten += 512;
  }
}

void stopRecording() {
  Serial.println("stopRecording");
  queue1.end();
  while (queue1.available() > 0) {
    byte buffer[256];
    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    frec.write(buffer, 256);
    dataBytesWritten += 256;
  }
  // Go back and patch the WAV header with the real data size
  frec.seek(0);
  writeWavHeader(frec, dataBytesWritten);
  frec.close();

  mode = 0;
  digitalWrite(ledPin, LOW);
}

// ---------------------------------------------------------------
// Playback
// ---------------------------------------------------------------

void startPlaying() {
  Serial.println("startPlaying");
  playWav1.stop();  // force a clean state before starting, no matter what came before
  if (SD.exists(recordingFile)) {
    playWav1.play(recordingFile);
    playStartTime = millis();
    mode = 2;
  } else {
    Serial.println("No recording yet.");
    mode = 0;
  }
}

void continuePlaying() {
  if (millis() - playStartTime < PLAY_GRACE_MS) {
    return; // give the codec a moment to actually start before checking
  }
  if (!playWav1.isPlaying()) {
    playWav1.stop();
    mode = 0;
  }
}

void stopPlaying() {
  Serial.println("stopPlaying");
  playWav1.stop();
  mode = 0;
}

// ---------------------------------------------------------------
// WAV header (mono, 16-bit, 44100 Hz PCM)
// ---------------------------------------------------------------

void writeWavHeader(File &f, unsigned long dataBytes) {
  unsigned long sampleRate = 44100;
  unsigned short numChannels = 1;
  unsigned short bitsPerSample = 16;
  unsigned long byteRate = sampleRate * numChannels * bitsPerSample / 8;
  unsigned short blockAlign = numChannels * bitsPerSample / 8;
  unsigned long chunkSize = 36 + dataBytes;

  f.write("RIFF", 4);
  writeUint32(f, chunkSize);
  f.write("WAVE", 4);

  f.write("fmt ", 4);
  writeUint32(f, 16);
  writeUint16(f, 1);               // PCM format
  writeUint16(f, numChannels);
  writeUint32(f, sampleRate);
  writeUint32(f, byteRate);
  writeUint16(f, blockAlign);
  writeUint16(f, bitsPerSample);

  f.write("data", 4);
  writeUint32(f, dataBytes);
}

void writeUint32(File &f, unsigned long val) {
  byte b[4];
  b[0] = val & 0xFF;
  b[1] = (val >> 8) & 0xFF;
  b[2] = (val >> 16) & 0xFF;
  b[3] = (val >> 24) & 0xFF;
  f.write(b, 4);
}

void writeUint16(File &f, unsigned short val) {
  byte b[2];
  b[0] = val & 0xFF;
  b[1] = (val >> 8) & 0xFF;
  f.write(b, 2);
}
