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

- Download [this code](https://github.com/kingston-hackSpace/Audio_Recording/blob/main/teensy_40_rec-play_overwrite.ino) and open it on Arduino IDE. 

- Plug the Teensy to your computer. 

- Before uploading the code, remember go to **Tools** and select:

    - Board: Teensy 4.0
    
    - Port: (varies, but the one connected to usb)

- PRESS THE ON-BOARD RESET BUTTON. See [reference image here](https://github.com/kingston-hackSpace/Audio_Recording/blob/main/reset_teensy.png)

- If you get an error message, unplug the Teensy. Plug again and PRESS THE ON-BOARD RESET BUTTON. See [reference image here](https://github.com/kingston-hackSpace/Audio_Recording/blob/main/reset_teensy.png). Now try uploading the code again. 

- Ask a technician if you have any further issues. 

- Test your recording/player device! :)
    
-----------
### CODE : REC and PLAY MULTIPLE TRACKS

To record multiple tracks, we will add a small screen and a rotary encoder to visualize and select which track to play.

**HARDWARE to add**

- [Sparkfun rotary encoder](https://github.com/kingston-hackSpace/Rotary_encoder)

- [OLED Screen 0.96" (128x64)](https://github.com/kingston-hackSpace/All_Display_Screens_for_Arduino/blob/main/Seeed_display.md) 

**WIRING**

Encoder A > Teensy pin 6

Encoder B > Teensy pin 9

Encoder C > Teensy GND

OLED SCREEN GND > Teensy GND

OLED SCREEN VCC > Teensy 3.3V

OLED SCREEN SCL > Teensy pin 19

OLED SCREEN SDA > Teensy pin 18

[See wiring diagram here](https://github.com/kingston-hackSpace/Audio_Recording/blob/main/Teensy_40_rec_multipleTracks_bb.jpg)

**CODE AND INSTRUCTIONS**

- Install the screen library:

    - In the Arduino IDE go to Sketch -> Include Libraries -> Manage libraries...

    - Search for **Adafruit_SSD1306 by Adafruit** and choose to install the library. 

- **Test the encoder:** download and upload [this code](https://github.com/kingston-hackSpace/Audio_Recording/blob/main/Teensy_40_rec_encoder.ino) to your board.

- Open the **Serial Monitor** to confirm that the encoder is working correctly to select the tracks.

- **Test the OLED screen:** download and upload [this code] to visualize track selection on the OLED screen.

-----------
### ADD AN AMPLIFIER (PENDING)

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
