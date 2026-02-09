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

Insert the micro-SD card into the Teensy Audio Shield

Mount your Teensy Audio Shield on top of the Teensy 4.1

Wire as follows:
    
BUTTONS

| Teensy Pin / Other side of button | Function |
| -------------------- | ----------- |
| Pin **24**  / GND     |  **Record** |
| Pin **25**  / GND     |  **Stop**   |
| Pin **28**  / GND     |  **Play**   |


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
### CODE : SAVING AUDIO AS A WAV FILE

-----------
### CODE : SAVING MULTIPLE WAV FILES

Download code [here]



////////
NEXT SECTION IS IN WORK IN PROGRESS...

-----------
### CODE : ADD LED and POT

| Component                | Teensy Pin | Notes / Connection                                                                                               |
| ------------------------ | ---------- | ---------------------------------------------------------------------------------------------------------------- |
| **Recording LED**        | Pin **13** | Connect LED + resistor to 3.3 V pin through a 220 Ω resistor or connect cathode to GND depending on wiring style |
| **Volume potentiometer** | A0         | Connect middle pin to A0, one side to 3.3 V, the other to GND. The Teensy reads it and sets output volume.       |

Download code [here]



-----------
### Further development

A much more advanced sound recording and data logging project:

https://github.com/WMXZ-EU/microSoundRecorder

https://github.com/WMXZ-EU/microSoundRecorder/wiki/Hardware-setup

https://forum.pjrc.com/threads/52175?p=185386&viewfull=1#post185386
