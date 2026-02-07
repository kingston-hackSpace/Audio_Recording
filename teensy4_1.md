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

- Teensy Audio Shield with electret microphone (soldered)

- USB‑A to USB‑Micro cable

- micro SD card (4GB or bigger)

- headphones

- Push buttons (x3)


---
### TEENSY AUDIO SHIELD

*note*: You do not need an external mic when you have the Audio Shield. The shield already gives you a electret mic.

Read more about the audio shield [here](https://www.pjrc.com/store/teensy3_audio.html)


-----------
### WIRING

Insert the micro-SD card into the Teensy 4.1

Mount your Teensy Audio Shield on top of the Teensy 4.1

Wire as follows:
    
BUTTONS

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




-----------
### Further development

A much more advanced sound recording and data logging project:

https://github.com/WMXZ-EU/microSoundRecorder

https://github.com/WMXZ-EU/microSoundRecorder/wiki/Hardware-setup

https://forum.pjrc.com/threads/52175?p=185386&viewfull=1#post185386
