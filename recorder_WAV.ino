// Record sound as WAV data to a SD card, and play it back.

#include <Bounce.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s2;           //xy=105,63
AudioAnalyzePeak         peak1;          //xy=278,108
AudioRecordQueue         queue1;         //xy=281,63
AudioPlaySdWav           playWav1;       //xy=302,157
AudioOutputI2S           i2s1;           //xy=470,120
AudioConnection          patchCord1(i2s2, 0, queue1, 0);
AudioConnection          patchCord2(i2s2, 0, peak1, 0);
AudioConnection          patchCord3(playWav1, 0, i2s1, 0);
AudioConnection          patchCord4(playWav1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=265,212
// GUItool: end automatically generated code

// For a stereo recording version, see this forum thread:
// https://forum.pjrc.com/threads/46150?p=158388&viewfull=1#post158388

// A much more advanced sound recording and data logging project:
// https://github.com/WMXZ-EU/microSoundRecorder
// https://github.com/WMXZ-EU/microSoundRecorder/wiki/Hardware-setup
// https://forum.pjrc.com/threads/52175?p=185386&viewfull=1#post185386

// Bounce objects to easily and reliably read the buttons
Bounce buttonRecord = Bounce(24, 8);
Bounce buttonStop =   Bounce(25, 8);  // 8 = 8 ms debounce time
Bounce buttonPlay =   Bounce(28, 8);

// which input on the audio shield will be used?
//const int myInput = AUDIO_INPUT_LINEIN;
const int myInput = AUDIO_INPUT_MIC;


// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  7   // Teensy 4 ignores this, uses pin 11
#define SDCARD_SCK_PIN   14  // Teensy 4 ignores this, uses pin 13

//#define SDCARD_CS_PIN    10 // audio Shield SD card
//#define SDCARD_CS_PIN    BUILTIN_SDCARD

int mode = 0;  // 0=stopped, 1=recording, 2=playing

// The file where data is recorded
File frec;
uint32_t totalBytes = 0;

String currentFilename;

int LED_pin = 32;


// --- WAV header helper ---
void writeWavHeader(File &file, uint32_t sampleRate, uint16_t bitsPerSample, uint16_t channels, uint32_t dataSize) {
  file.seek(0);
  file.write("RIFF", 4);
  uint32_t chunkSize = 36 + dataSize;
  file.write((uint8_t*)&chunkSize, 4);
  file.write("WAVE", 4);

  file.write("fmt ", 4);
  uint32_t fmtChunkSize = 16;
  file.write((uint8_t*)&fmtChunkSize, 4);
  uint16_t audioFormat = 1;
  file.write((uint8_t*)&audioFormat, 2);
  file.write((uint8_t*)&channels, 2);
  file.write((uint8_t*)&sampleRate, 4);
  uint32_t byteRate = sampleRate * channels * bitsPerSample/8;
  file.write((uint8_t*)&byteRate, 4);
  uint16_t blockAlign = channels * bitsPerSample/8;
  file.write((uint8_t*)&blockAlign, 2);
  file.write((uint8_t*)&bitsPerSample, 2);

  file.write("data", 4);
  file.write((uint8_t*)&dataSize, 4);
}

String getNextFilename() {
  char filename[14]; // 8.3 format
  for (int i = 1; i < 1000; i++) {
    sprintf(filename, "RECORD%03d.WAV", i);
    if (!SD.exists(filename)) {
      return String(filename);
    }
  }
  return String("RECORD999.WAV"); // fallback
}

String getLatestRecording() {
  char filename[14];
  for (int i = 999; i >= 1; i--) {  // check from high to low
    sprintf(filename, "RECORD%03d.WAV", i);
    if (SD.exists(filename)) {
      return String(filename);
    }
  }
  return "";  // no file found
}



void setup() {
  // LED pins
  pinMode(LED_pin, OUTPUT);
  
  // Button pins
  pinMode(24, INPUT_PULLUP);
  pinMode(25, INPUT_PULLUP);
  pinMode(28, INPUT_PULLUP);

  // Audio connections require memory, and the record queue
  // uses this memory to buffer incoming audio.
  AudioMemory(60);

  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.5);

  // Initialize the SD card
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here if no SD card, but print a message
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}


void loop() {
  // First, read the buttons
  buttonRecord.update();
  buttonStop.update();
  buttonPlay.update();

  // Respond to button presses
  if (buttonRecord.fallingEdge()) {
    Serial.println("Record Button Press");
    if (mode == 2) stopPlaying();
    if (mode == 0) startRecording();
  }
  if (buttonStop.fallingEdge()) {
    Serial.println("Stop Button Press");
    if (mode == 1) stopRecording();
    if (mode == 2) stopPlaying();
  }
  if (buttonPlay.fallingEdge()) {
    Serial.println("Play Button Press");
    if (mode == 1) stopRecording();
    if (mode == 0) {
      currentFilename = getLatestRecording(); //  latest file
      startPlaying();
    }
  }

  // If we're playing or recording, carry on...
  if (mode == 1) {
    continueRecording();
  }
  if (mode == 2) {
    continuePlaying();
  }

  // when using a microphone, continuously adjust gain
  if (myInput == AUDIO_INPUT_MIC) adjustMicLevel();
}


void startRecording() {
  totalBytes = 0;
  Serial.println("startRecording");
  digitalWrite(LED_pin, HIGH);
  currentFilename = getNextFilename();
  frec = SD.open(currentFilename.c_str(), FILE_WRITE);
  writeWavHeader(frec, 44100, 16, 1, 0); // placeholder header
  if (frec) {
    queue1.begin();
    mode = 1;
    Serial.print("Recording to: ");
    Serial.println(currentFilename);
  }
}

void continueRecording() {
  if (queue1.available() >= 2) {
    byte buffer[512];
    // Fetch 2 blocks from the audio library and copy
    // into a 512 byte buffer.  The Arduino SD library
    // is most efficient when full 512 byte sector size
    // writes are used.
    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer+256, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    // write all 512 bytes to the SD card
    //elapsedMicros usec = 0;
    frec.write(buffer, 512);
    // Uncomment these lines to see how long SD writes
    // are taking.  A pair of audio blocks arrives every
    // 5802 microseconds, so hopefully most of the writes
    // take well under 5802 us.  Some will take more, as
    // the SD library also must write to the FAT tables
    // and the SD card controller manages media erase and
    // wear leveling.  The queue1 object can buffer
    // approximately 301700 us of audio, to allow time
    // for occasional high SD card latency, as long as
    // the average write time is under 5802 us.
    //Serial.print("SD write, us=");
    //Serial.println(usec);
    totalBytes += 512;
  }
}

void stopRecording() {
  Serial.println("stopRecording");
  digitalWrite(LED_pin, LOW);
  queue1.end();
  if (mode == 1) {
    while (queue1.available() > 0) {
      frec.write((byte*)queue1.readBuffer(), 256);
      queue1.freeBuffer();
      totalBytes += 256;
    }
    // Rewrite WAV header with correct size
    writeWavHeader(frec, 44100, 16, 1, totalBytes);
    frec.close();
  }
  mode = 0;
}


void startPlaying() {
  Serial.println("startPlaying");
  if (currentFilename.length() > 0 && SD.exists(currentFilename.c_str())) {
    playWav1.play(currentFilename.c_str());
    delay(25);
    mode = 2;
    Serial.print("Playing: ");
    Serial.println(currentFilename);
  } else {
    Serial.println("No recording available to play.");
  }
}


void continuePlaying() {
  if (!playWav1.isPlaying()) {
    playWav1.stop();
    mode = 0;
  }
}

void stopPlaying() {
  Serial.println("stopPlaying");
  if (mode == 2) playWav1.stop();
  mode = 0;
}


void adjustMicLevel() {
  // TODO: read the peak1 object and adjust sgtl5000_1.micGain()
  // if anyone gets this working, please submit a github pull request :-)
}
