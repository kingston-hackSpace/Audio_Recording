# Teensy 4.1

Build an audio recorder with Teensy 4.1

---
### SET-UP

If you are using the Teensy 4.1 for the first time, you will need to install the Teensy board and library.

Follow the installation steps [here](https://github.com/kingston-hackSpace/Teensy_4-1/blob/main/README.md)

---
### HARDWARE

- Teensy 4.1 

- Teensy Audio Shield with electret microphone (soldered)

- USB‑A to USB‑Micro cable

- micro SD card (4GB or bigger)

- headphones

- Push buttons (x3)

- LED + 220 ohms resistor


---
### TEENSY AUDIO SHIELD

*note*: You do not need an external mic when you have the Audio Shield. The shield already provides an electret mic.

Read more about the audio shield [here](https://www.pjrc.com/store/teensy3_audio.html)


-----------
### WIRING

Insert the micro-SD card into the Teensy 4.1 board (not the audio shield)

Wire as follows:

| Wiring | Function |
| -------------------- | ----------- |
| Pin **24**  / Button 1 / GND     |  **Record** |
| Pin **25**  / Button 2 / GND     |  **Stop**   |
| Pin **28**  / Button 3 / GND     |  **Play**   |
| Pin **32**  / Resistor 220ohms / LED+ / LED- / GND     |  **LED REC**   |

Reference image [here](https://github.com/kingston-hackSpace/Audio_Recording/blob/main/Teensy41__audio_rec_bb.jpg)

-----------
### CODE : RECORDER EXAMPLE (record, play and overwrite)

- Go to File → Examples → Audio → Recorder. 

- This sketch will record audio from the electret microphone and save it as a .RAW file to the SD card, and play it back (mono).

- Just under the previous Bounce section, you need to select the right audio input. It should look like:

  ```
  //const int myInput = AUDIO_INPUT_LINEIN;
  const int myInput = AUDIO_INPUT_MIC;
  ```
  
- Just under the previous mic section, you need to select the right SD card pin. It should look like:

  ```
  #define SDCARD_CS_PIN BUILTIN_SDCARD
  ```
  
- Upload the code to your Teensy board. 

- Test by pressing the buttons to rec, play and stop.

-----------
### CODE : SAVING MULTIPLE WAV FILES

Download code [here](https://github.com/kingston-hackSpace/Audio_Recording/blob/main/recorder_WAV.ino)



----------
# NEXT SECTION IS IN WORK IN PROGRESS...

-----------
### CODE : ADD LED and POT

| Component                | Teensy Pin | Notes / Connection                                                                                               |
| ------------------------ | ---------- | ---------------------------------------------------------------------------------------------------------------- |
| **Recording LED**        | Pin **13** | Connect LED + resistor to 3.3 V pin through a 220 Ω resistor or connect cathode to GND depending on wiring style |
| **Volume potentiometer** | A0         | Connect middle pin to A0, one side to 3.3 V, the other to GND. The Teensy reads it and sets output volume.       |

Download code [here]


-----------
### Learn mmore about WAV and RAW audio format

https://github.com/kingston-hackSpace/Audio_Recording/blob/main/teensy_audio-file-format.md

-----------
### Further development

A much more advanced sound recording and data logging project:

https://github.com/WMXZ-EU/microSoundRecorder

https://github.com/WMXZ-EU/microSoundRecorder/wiki/Hardware-setup

https://forum.pjrc.com/threads/52175?p=185386&viewfull=1#post185386
