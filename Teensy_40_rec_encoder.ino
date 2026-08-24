/* Multi-track recorder/player for Teensy 4 + Audio Shield (Rev D)
 with a rotary encoder for track selection. 

Pins:
   Record button    : pin 2  (hold to record a NEW track, release to stop)
   Play button       : pin 3  (press to play the currently selected track)
   Stop button       : pin 4  (press to stop playback)
   Red LED           : pin 5, through a ~220 ohm resistor, to GND

 Rotary encoder:
    A / CLK           : pin 6
    B / DT             : pin 9
    GND               : Teensy GND
    
 Behaviour:
- Multi-track recording (TRACK1.WAV, TRACK2.WAV, ...)
- The newly recorded track becomes the selected track automatically.
- Use the encoder to select the track to play 
- Monitor selection using the the Serial Monitor.
- Use push-buttons to rec, play and stop playing
*/

#include <Bounce.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s2;
AudioRecordQueue         queue1;
AudioPlaySdWav            playWav1;
AudioOutputI2S            i2s1;
AudioConnection           patchCord1(i2s2, 0, queue1, 0);
AudioConnection           patchCord2(playWav1, 0, i2s1, 0);
AudioConnection           patchCord3(playWav1, 1, i2s1, 1);
AudioControlSGTL5000      sgtl5000_1;
// GUItool: end automatically generated code

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

// --- Rotary encoder pins (rotation only) ---
#define ENC_A  6
#define ENC_B  9

int encA_currentState;
int encA_lastState;
unsigned long encLastDebounceTime = 0;
const unsigned long encDebounceDelay = 100;

// --- Track bookkeeping ---
int trackCount = 0;       // how many tracks exist on the SD card
int selectedTrack = 1;    // which track number is currently selected (1-based)

// Remember which mode we're in
int mode = 0;  // 0 = stopped, 1 = recording, 2 = playing

// The file being recorded to / played from
File frec;
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

  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  encA_lastState = digitalRead(ENC_A);

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

  scanExistingTracks();

  Serial.println("Ready. Hold Record (2) to record a new track.");
  Serial.println("Turn the encoder to select a track, Play (3) to play, Stop (4) to stop.");
  printSelection();
}

void loop() {
  buttonRecord.update();
  buttonPlay.update();
  buttonStop.update();

  // Record button pressed -> start a brand-new track, stopping playback first if needed
  if (buttonRecord.fallingEdge()) {
    Serial.println("Record button pressed");
    if (mode == 2) stopPlaying();
    startRecording();
  }
  // Record button released -> stop and save the new track
  if (buttonRecord.risingEdge()) {
    Serial.println("Record button released");
    if (mode == 1) stopRecording();
  }

  // Play button pressed -> play the selected track
  if (buttonPlay.fallingEdge()) {
    Serial.println("Play button pressed");
    if (mode != 1) {
      startPlaying();
    } else {
      Serial.println("  ignored: currently recording");
    }
  }

  // Stop button pressed -> stop playback (unconditional)
  if (buttonStop.fallingEdge()) {
    Serial.println("Stop button pressed");
    stopPlaying();
  }

  handleEncoderRotation();

  if (mode == 1) {
    continueRecording();
  }
  if (mode == 2) {
    continuePlaying();
  }
}

// ---------------------------------------------------------------
// Rotary encoder (rotation only)
// ---------------------------------------------------------------

void handleEncoderRotation() {
  encA_currentState = digitalRead(ENC_A);
  if (encA_currentState != encA_lastState && encA_currentState == LOW) {
    if ((millis() - encLastDebounceTime) > encDebounceDelay) {
      int bState = digitalRead(ENC_B);
      if (trackCount > 0) {
        if (bState != encA_currentState) {
          selectedTrack++;
        } else {
          selectedTrack--;
        }
        // wrap around between 1 and trackCount
        if (selectedTrack > trackCount) selectedTrack = 1;
        if (selectedTrack < 1) selectedTrack = trackCount;
        printSelection();
      }
      encLastDebounceTime = millis();
    }
  }
  encA_lastState = encA_currentState;
}

// ---------------------------------------------------------------
// Track bookkeeping
// ---------------------------------------------------------------

// Scans the SD card root for existing TRACKn.WAV files and sets
// trackCount to the highest track number found, so recording
// continues numbering from where it left off after a power cycle.
void scanExistingTracks() {
  trackCount = 0;
  File root = SD.open("/");
  while (true) {
    File entry = root.openNextFile();
    if (!entry) break;
    String name = entry.name();
    entry.close();
    if (name.startsWith("TRACK") && name.endsWith(".WAV")) {
      String numberPart = name.substring(5, name.length() - 4);
      int n = numberPart.toInt();
      if (n > trackCount) trackCount = n;
    }
  }
  root.close();
  if (trackCount > 0) {
    selectedTrack = trackCount; // start on the most recent track
    Serial.print("Found ");
    Serial.print(trackCount);
    Serial.println(" existing track(s) on the SD card.");
  } else {
    Serial.println("No tracks found on the SD card yet.");
  }
}

String trackFilename(int trackNumber) {
  return "TRACK" + String(trackNumber) + ".WAV";
}

void printSelection() {
  if (trackCount == 0) {
    Serial.println("No tracks recorded yet.");
    return;
  }
  Serial.print("Selected track: ");
  Serial.print(selectedTrack);
  Serial.print(" / ");
  Serial.println(trackCount);
}

// ---------------------------------------------------------------
// Recording
// ---------------------------------------------------------------

void startRecording() {
  int newTrackNumber = trackCount + 1;
  String filename = trackFilename(newTrackNumber);
  Serial.print("startRecording: ");
  Serial.println(filename);

  frec = SD.open(filename.c_str(), FILE_WRITE);
  if (frec) {
    writeWavHeader(frec, 0);   // placeholder header, patched once we know the size
    dataBytesWritten = 0;
    queue1.begin();
    mode = 1;
    digitalWrite(ledPin, HIGH);
  } else {
    Serial.println("  could not open file for recording");
  }
}

void continueRecording() {
  if (queue1.available() >= 2) {
    byte buffer[512];
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

  trackCount++;
  selectedTrack = trackCount; // auto-select the track just recorded
  mode = 0;
  digitalWrite(ledPin, LOW);

  Serial.print("Saved ");
  Serial.println(trackFilename(trackCount));
  printSelection();
}

// ---------------------------------------------------------------
// Playback
// ---------------------------------------------------------------

void startPlaying() {
  if (trackCount == 0) {
    Serial.println("No tracks to play yet.");
    return;
  }
  String filename = trackFilename(selectedTrack);
  Serial.print("startPlaying: ");
  Serial.println(filename);

  playWav1.stop();  // force a clean state before starting, no matter what came before
  if (SD.exists(filename.c_str())) {
    playWav1.play(filename.c_str());
    playStartTime = millis();
    mode = 2;
  } else {
    Serial.println("  file not found");
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
