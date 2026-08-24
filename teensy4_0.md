# Teensy 4.0

Build an audio recorder/player with Teensy 4.0

---
### SET-UP

If you are using the Teensy 4.0 for the first time, you will need to install the Teensy board and library.

Follow the installation steps [here](https://github.com/kingston-hackSpace/Teensy_4-1/blob/main/README.md)

---
### HARDWARE

- Teensy 4.0

- Teensy Audio Shield REV-D with an electret microphone

- USB‑A to USB‑Micro cable

- micro SD card (4GB or bigger)

- capsule speaker

- Push buttons (x3)

- LED + 220 ohms resistor


---
### TEENSY AUDIO SHIELD

*note*: Our hackSpace kit provides an electric mic with the Teensy and Audio Shield. 

Read more about the audio shield [here](https://www.pjrc.com/store/teensy3_audio.html)


-----------
### NOTE ABOUT TEENSY PINS

[Teensy 4.0 PINOUT here](https://github.com/kingston-hackSpace/Teensy/blob/main/Teensy4_0_pinout.pdf)

NOTE: The Teensy 4.0 has 40 pins available, however, some of them will be in used by the Audio Shield and SD card. See below:

Busy pins:
    
    - 7, 8 —> Audio Shield I2S data

    - 18, 19 —> Audio Shield I2C (codec control)

    - 20, 21, 23 —> Audio Shield I2S clocks

    - 10, 11, 12, 13 —> SPI to the onboard SD card

Pins available:

    - 0–6, 9, 14, 16, 17, 22

-----------
### WIRING

Wire your electronics as follows:

| Wiring | Function |
| -------------------- | ----------- |
| Pin **2**  / Button 1 / GND     |  **Record** |
| Pin **3**  / Button 2 / GND     |  **Play**   |
| Pin **4**  / Button 3 / GND     |  **Stop**   |
| Pin **5**  / Resistor 220ohms / LED+ / LED- / GND     |  **LED REC**   |

Reference image [here](https://github.com/kingston-hackSpace/Audio_Recording/blob/main/Teensy_40_rec.jpg)

-----------
### CODE : REC and PLAY ONE TRACK THAT OVERWRITES

Record, play and overwrite: Download and upload [this code](https://github.com/kingston-hackSpace/Audio_Recording/blob/main/teensy_40_rec-play_overwrite.ino) to your board. 

-----------
### CODE : REC and PLAY MULTIPLE TRACKS


 POT AND SCREEN

| Component                | Teensy Pin | Notes / Connection                                                                                               |
| ------------------------ | ---------- | ---------------------------------------------------------------------------------------------------------------- |
| **Recording LED**        | Pin **13** | Connect LED + resistor to 3.3 V pin through a 220 Ω resistor or connect cathode to GND depending on wiring style |
| **Volume potentiometer** | A0         | Connect middle pin to A0, one side to 3.3 V, the other to GND. The Teensy reads it and sets output volume.       |

Download code [here]()

-----------
### ADD AN AMPLIFIER

hardware:

- Jack 3.5mm

- Sparkfun Amplifier

- Speaker xx

- Power supply xx

- Barrel connector

- Barrel to USB cable

- mini screw driver

-----------
### Learn mmore about WAV and RAW audio format

https://github.com/kingston-hackSpace/Audio_Recording/blob/main/teensy_audio-file-format.md

-----------
### Further development

A much more advanced sound recording and data logging project:

https://github.com/WMXZ-EU/microSoundRecorder

https://github.com/WMXZ-EU/microSoundRecorder/wiki/Hardware-setup

https://forum.pjrc.com/threads/52175?p=185386&viewfull=1#post185386
