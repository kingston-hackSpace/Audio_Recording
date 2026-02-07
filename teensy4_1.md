# Teensy 4.1

Build a audio recorder with Teensy 4.1

---
### Teensy SET-UP

If you are using the Teensy 4.1 for the first time, you will need to install the Teensy board and library.

Follow the installation steps [here](https://github.com/kingston-hackSpace/Teensy_4-1/blob/main/README.md)

---
### SAVING .RAW FILES

Teensy uses RAW audio files (instead of WAV or MP3) because RAW is:

- Audio files with no compression

- Fast to write 

- Safe for long recordings

- Impossible to corrupt mid-recording

- Perfect for SD cards

This is why most professional embedded audio recorders record RAW internally and convert later.

In the Teensy file, the RAW file will store:

- Sample rate: 44.1 kHz

- Bit depth: 16-bit

- Channels: mono

- Format: signed PCM

---
### HARDWARE

- Teensy 4.1 

- Teensy Audio Shield

- USB‑A to USB‑Micro cable

- micro SD card (4GB or bigger)

- SparkFun I2S Audio Breakout - MAX98357A

- Adafruit I2S MEMS Microphone Breakout - SPH0645LM4H

- 4Ω speakers or headphones

- Push buttons (x3)

- Potentiometer

-----------
### WIRING

Insert the micro-SD card into the Teensy 4.1

Mount your Teensy Audio Shield on top of the Teensy 4.1

Wire as follows:

**SPH0645LM4H I2S Microphone** 
| Mic → Teensy 4.1 Pin                   | Notes / Connection                                           |
| -------------------------------------- | ------------------------------------------------------------ |
| **BCLK** → Pin 21                      | I2S Bit Clock                                                |
| **LRCLK** → Pin 20                     | I2S Word Select / Left-Right Clock                           |
| **DOUT** → Pin 9                       | I2S Data Out (to Teensy)                                     |
| **VDD** → 3.3 V                        | Power                                                        |
| **GND** → GND                          | Ground                                                       |

**MAX98357A I2S Amplifier** 
| AMP → Teensy 4.1 Pin                   | Notes / Connection                                           |
| -------------------------------------- | ------------------------------------------------------------ |
| **BCLK** → Teensy I2S BCLK (Pin 21)    | I2S Bit Clock                                                |
| **LRCLK** → Teensy I2S LRCLK (Pin 20)  | Word select                                                  |
| **DIN** → Teensy I2S Data Out          | Audio data from Teensy                                       |
| **VCC** → 3.3 V                        | Power                                                        |
| **GND** → GND                          | Ground                                                       |
    

BUTTON

| Teensy Pin → Other side of button | Function |
| -------------------- | ----------- |
| Pin **0**  → GND     |  **Record** |
| Pin **1**  → GND     |  **Stop**   |
| Pin **2**  → GND     |  **Play**   |




-----------
### CODE AND INSTRUCTIONS

Let's test Teensy by running one of the many Teensy Audio Library ready‑made examples:

- Open the Arduino IDE (Windows) or Teensyduino (macOS):

- Go to File → Examples → Audio → Recorder. 

- This sketch will record audio from your I2S microphone and save it as a .RAW file to the SD card, and play it back (mono).

- // A much more advanced sound recording and data logging project:
// https://github.com/WMXZ-EU/microSoundRecorder
// https://github.com/WMXZ-EU/microSoundRecorder/wiki/Hardware-setup
// https://forum.pjrc.com/threads/52175?p=185386&viewfull=1#post185386
