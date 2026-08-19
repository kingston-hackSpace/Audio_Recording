# Teensy 4.0

Build an audio recorder with Teensy 4.0

---
### SET-UP

If you are using the Teensy 4.0 for the first time, you will need to install the Teensy board and library.

Follow the installation steps [here](https://github.com/kingston-hackSpace/Teensy_4-1/blob/main/README.md)

---
### HARDWARE

- Teensy 4.0

- Teensy Audio Shield with electret microphone

- USB‑A to USB‑Micro cable

- micro SD card (4GB or bigger)

- headphones

- Push buttons (x2)

- LED + 220 ohms resistor


---
### TEENSY AUDIO SHIELD

*note*: You do not need an external mic when you have the Audio Shield. The shield already provides an electret mic.

Read more about the audio shield [here](https://www.pjrc.com/store/teensy3_audio.html)


-----------
### PINS NOTE

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
### 

Wire as follows:

| Wiring | Function |
| -------------------- | ----------- |
| Pin **2**  / Button 1 / GND     |  **Record** |
| Pin **3**  / Button 2 / GND     |  **Play**   |
| Pin **4**  / Resistor 220ohms / LED+ / LED- / GND     |  **LED REC**   |

Reference image [here](https://github.com/kingston-hackSpace/Audio_Recording/blob/main/Teensy_40_rec_bb.jpg)

-----------
### CODE : TESTING SD CARD

- Open the Arduino IDE (Windows) or Teensyduino (macOS):

- Go to File → Examples → SD → Card Info

- Upload the code to your board and open the Serial Monitor

    - If you have library error messages: The teensy uses its own libraries. If you have a duplicated library, such as the SdFat, you might have a library conflict. Try fixing the issue by removing the conflicting libraries from your Ardudino/Documents/libraries

    - If you get a error message in the Serial Monitor: check that you selected the correct *chipSelect*. You cannot use 2 SD cards, you must select of only one: the built-in SD ard of your board, or the one in the audio shield.
 
    - A sucessfull SD result should show your the SD card info. 
 
-----------
### CODE : RECORDER EXAMPLE (1 TRACK THAT OVERWRITES)

- Go to File → Examples → Audio → Recorder. 

- This sketch will record audio from the electret microphone and save it as a .RAW file to the SD card, and play it back (mono).

- Scroll down the sketch and type the right output pins. It should look like:

  ```
  Bounce buttonRecord = Bounce(24, 8);
  Bounce buttonStop = Bounce(25, 8);
  Bounce buttonPlay = Bounce(28, 8);
  ```

- Just under the previous Bounce section, you need to select the right audio input. It should look like:

  ```
  //const int myInput = AUDIO_INPUT_LINEIN;
  const int myInput = AUDIO_INPUT_MIC;
  ```
- Just under the previous mic section, you need to select the right SD card pin. It should look like:

  ```
  #define SDCARD_CS_PIN BUILTIN_SDCARD
  ```

- Scroll down the sketch and replace the push button pin numbers. It should look like:

  ```
  pinMode(24, INPUT_PULLUP);
  pinMode(25, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
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
