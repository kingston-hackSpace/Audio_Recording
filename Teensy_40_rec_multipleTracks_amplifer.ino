/* Multi-track recorder/player for Teensy 4 + Audio Shield (Rev D)
with a rotary encoder for track selection,
and an SSD1306 OLED showing the current track and status.

Pins:
   Record button    : pin 2  (hold to record a NEW track, release to stop)
   Play button       : pin 3  (press to play the currently selected track)
   Stop button       : pin 4  (press to stop playback, hold 3+ seconds to delete the selected track)
   Red LED           : pin 5, through a ~220ohm resistor, to GND

  Rotary encoder:
   A / CLK           : pin 6
   B / DT             : pin 9
   GND               : Teensy GND

 OLED (Seeed SSD1306, 128x64, I2C):
   SDA               : pin 18  (shared with the Audio Shield's codec bus)
   SCL               : pin 19  (shared with the Audio Shield's codec bus)
   VCC               : 3.3V
   GND               : Teensy GND

 TPA2016D2 amp 
   GND               : Teensy GND
   SDA               : pin 18  (shared bus)
   SCL               : pin 19  (shared bus)

*/

#include <Bounce.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_TPA2016.h>

Adafruit_TPA2016 audioAmp = Adafruit_TPA2016();

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

// --- Rotary encoder pins (rotation only) ---
#define ENC_A  6
#define ENC_B  9

int encA_currentState;
int encA_lastState;
unsigned long encLastDebounceTime = 0;
const unsigned long encDebounceDelay = 100;

// --- OLED display ---
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1     // reset pin not used
#define SCREEN_ADDRESS 0x3C   // common I2C address; try 0x3D if this doesn't work
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int8_t AMP_GAIN = 28; // TPA2016D2 fixed gain, dB

#define VOL_POT_PIN 15 // A1
unsigned long lastPotCheck = 0;
const unsigned long POT_CHECK_INTERVAL_MS = 100;
int lastLineOutLevel = -1; // sentinel so the first reading always applies

int trackCount = 0;       // how many tracks exist on the SD card
int selectedTrack = 1;    // which track number is currently selected (1-based)

// Hold Stop for 3+ seconds to delete the currently selected track
unsigned long stopPressStart = 0;
bool deleteTriggered = false;
const unsigned long DELETE_HOLD_MS = 3000;

// Remember which mode we're in
int mode = 0;  // 0 = stopped, 1 = recording, 2 = playing

// The file being recorded to / played from
File frec;
unsigned long dataBytesWritten = 0;
int recordingTrackNumber = 0; // which track number the current recording targets

// Grace period after starting playback before we start checking
// isPlaying(), so a not-yet-started codec doesn't look "stopped"
unsigned long playStartTime = 0;
const unsigned long PLAY_GRACE_MS = 150;

// Throttled display updates: encoder reading stays instant even if
// the screen redraw (I2C transfer) is comparatively slow.
bool displayDirty = false;
unsigned long lastDisplayUpdate = 0;
const unsigned long DISPLAY_UPDATE_INTERVAL_MS = 80;

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
  sgtl5000_1.lineOutLevel(22); // starting level -- the pot takes over immediately in loop()
  sgtl5000_1.unmuteLineout();  // line-out starts muted by default

  // Initialize the SD card
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  // Initialize the OLED (shares the I2C bus already used by the codec)
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    // Don't halt -- the recorder can still work without the screen.
  }
  Wire.setClock(400000); // fast-mode I2C, speeds up display.display() calls

  // Initialize the amp. AGC and limiter are disabled so the fixed gain
  // below actually holds instead of being adjusted by the amp itself.
  if (!audioAmp.begin()) {
    Serial.println("TPA2016D2 not detected on I2C bus!");
  }
  audioAmp.enableChannel(true, true);          // right, left -- both on
  audioAmp.setAGCCompression(TPA2016_AGC_OFF); // stop AGC from readjusting our gain
  audioAmp.setLimitLevelOff();                 // disable the output limiter cap
  audioAmp.setGain(AMP_GAIN);

  // Verify what actually got applied, rather than assuming the calls worked
  Serial.print("Amp gain readback: ");
  Serial.println(audioAmp.getGain());

  scanExistingTracks();

  Serial.println("Ready. Hold Record (2) to record a new track.");
  Serial.println("Turn the encoder to select a track, Play (3) to play, Stop (4) to stop.");
  printSelection();
  updateDisplay();
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

  // Stop button pressed -> stop playback (unconditional), and start
  // timing the hold in case it's held long enough to delete a track
  if (buttonStop.fallingEdge()) {
    Serial.println("Stop button pressed");
    stopPlaying();
    stopPressStart = millis();
    deleteTriggered = false;
  }
  // While Stop is held down, check if we've crossed the delete threshold
  if (digitalRead(4) == LOW && !deleteTriggered) {
    if (millis() - stopPressStart >= DELETE_HOLD_MS) {
      deleteSelectedTrack();
      deleteTriggered = true;
    }
  }
  if (buttonStop.risingEdge()) {
    deleteTriggered = false;
  }

  handleEncoderRotation();

  // Volume pot: controls the codec's line-out level (confirmed working),
  // only pushed to the codec when it actually changes.
  if (millis() - lastPotCheck > POT_CHECK_INTERVAL_MS) {
    lastPotCheck = millis();
    int reading = analogRead(VOL_POT_PIN); // 0-1023 by default on Teensy
    int levelValue = map(reading, 0, 1023, 31, 13); // inverted: quiet -> loud
    if (levelValue != lastLineOutLevel) {
      sgtl5000_1.lineOutLevel(levelValue);
      lastLineOutLevel = levelValue;
    }
  }

  if (mode == 1) {
    continueRecording();
  }
  if (mode == 2) {
    continuePlaying();
  }

  // Redraw the screen at most every DISPLAY_UPDATE_INTERVAL_MS, and only
  // if something actually changed. Keeps I2C draw time from blocking
  // encoder/button reading when things change rapidly.
  if (displayDirty && (millis() - lastDisplayUpdate > DISPLAY_UPDATE_INTERVAL_MS)) {
    updateDisplay();
    displayDirty = false;
    lastDisplayUpdate = millis();
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
        displayDirty = true;
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
  Serial.print(trackCount);
  if (!SD.exists(trackFilename(selectedTrack).c_str())) {
    Serial.print("  (empty -- hold Record to fill this slot)");
  }
  Serial.println();
}

// Deletes the currently selected track's file from the SD card, then
// rescans so trackCount/selectedTrack reflect what's actually left.
// Leaves a gap in numbering rather than renumbering later tracks.
void deleteSelectedTrack() {
  if (trackCount == 0) {
    Serial.println("Nothing to delete.");
    return;
  }
  String filename = trackFilename(selectedTrack);
  if (SD.exists(filename.c_str())) {
    SD.remove(filename.c_str());
    Serial.print("Deleted ");
    Serial.println(filename);
  } else {
    Serial.print(filename);
    Serial.println(" was already missing.");
  }
  scanExistingTracks();
  printSelection();
  displayDirty = true;
}

// ---------------------------------------------------------------
// OLED display
// ---------------------------------------------------------------

void updateDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Track line, large text
  display.setTextSize(2);
  display.setCursor(0, 0);
  if (trackCount == 0) {
    display.println("No tracks");
  } else {
    display.print("Track ");
    display.setCursor(0, 20);
    display.print(selectedTrack);
    display.print("/");
    display.println(trackCount);
  }

  // Status line, smaller text
  display.setTextSize(1);
  display.setCursor(0, 40);
  if (trackCount > 0 && !SD.exists(trackFilename(selectedTrack).c_str())) {
    display.println("Empty slot");
  } else {
    switch (mode) {
      case 1: display.println("Recording..."); break;
      case 2: display.println("Playing...");   break;
      default: display.println("Idle");        break;
    }
  }

  display.display();
}

// ---------------------------------------------------------------
// Recording
// ---------------------------------------------------------------

void startRecording() {
  // If the currently selected track is an empty slot (e.g. left behind
  // by a deletion), record into that slot instead of always appending
  // a brand-new track at the end.
  if (trackCount > 0 && !SD.exists(trackFilename(selectedTrack).c_str())) {
    recordingTrackNumber = selectedTrack;
  } else {
    recordingTrackNumber = trackCount + 1;
  }

  String filename = trackFilename(recordingTrackNumber);
  Serial.print("startRecording: ");
  Serial.println(filename);

  frec = SD.open(filename.c_str(), FILE_WRITE);
  if (frec) {
    writeWavHeader(frec, 0);   // placeholder header, patched once we know the size
    dataBytesWritten = 0;
    queue1.begin();
    mode = 1;
    digitalWrite(ledPin, HIGH);
    displayDirty = true;
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

  if (recordingTrackNumber > trackCount) {
    trackCount = recordingTrackNumber; // genuinely new track, grows the count
  }
  selectedTrack = recordingTrackNumber; // auto-select whichever slot we just filled
  mode = 0;
  digitalWrite(ledPin, LOW);

  Serial.print("Saved ");
  Serial.println(trackFilename(recordingTrackNumber));
  printSelection();
  displayDirty = true;
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
    displayDirty = true;
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
    displayDirty = true;
  }
}

void stopPlaying() {
  Serial.println("stopPlaying");
  playWav1.stop();
  mode = 0;
  displayDirty = true;
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
