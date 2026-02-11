# Voice / Sound Recorder (ISD1820)

----
### What it does

- Records short audio clips (typically set up to ~10 seconds)

- Plays back the recorded audio through an on-board speaker (or an external amplifier/speaker)

- Recording is done using the on-board microphone

- Audio is overwritten every time you record. The module has no file system, no way to access the audio data, and cannot store multiple recordings.

- The ISD1820 has no programmable logic, however, it can interact with a microcontroller under its pinout logig (HIGH/LOW), but all audio handling happens inside the ISD1820 itself.

----
### Module control

**VCC:** +5V power supply

**GND:** GND power supply 

**FT:** Feed Through. Does not play recorded audio. Routes the microphone input directly to the speaker output. Acts like a crude live microphone. Ignores the recorded memory completely

**PLAYL:** Audio plays while the pin is held HIGH, stops when released.

**PLAYE:** Plays the recorded sound. Press once to play the entire voice message. 

**REC:** Push and start recording. Continue to push the button until you record the complete message.


----
### Activating Endless-Loop-Playback

To force infinite looping: connect PE to PL via a jumper or bridge.

Infinite looping will work like:

- Pressing PLAYE triggers the audio, which now loops continuously.

- Holding PLAYL also plays the audio in a continuous loop, stopping only when the button is released.

----
### Change storage capacity

Change the audio storage capacity in a range from 8 to 20 seconds.

See tutorial [here](https://www.electroniclinic.com/arduino-isd1820-voice-recorder-playback-module/)

----
### Change speakers

If you want higher volume, you will need a speaker amplifier like the LM386 module. This will need external power.

Check that the amplifier is suitable for your speaker, or you might damage the equipment.

See wiring [here](https://github.com/kingston-hackSpace/Audio_Recording/blob/main/addAmp.png)

----
### Interface with Arduino

**Wiring**: Connect the ISD1820 with Arduino as follows:

- VCC to 5V

- GND to GND

- PE to Digital pin 3

- REC to Digital pin 2

**Code:** 

The following codes record audio for 3 seconds, then play (4 seconds). They rec and play in loop. 

[Automatic REC & PLAY with Arduino using delay](https://github.com/kingston-hackSpace/Audio_Recording/blob/main/AudioREC_withArduino_delay.ino)

If you haven't used millis() before, follow [this tutorial before continue using the recorder](https://github.com/kingston-hackSpace/millis)

[Automatic REC & PLAY with Arduino using millis](https://github.com/kingston-hackSpace/Audio_Recording/blob/main/AudioREC_withArduino_millis.ino)


----
### More tutorials

[ISD1820 with Arduino and sensors](https://www.electroniclinic.com/arduino-isd1820-voice-recorder-playback-module/)

[Play sound if IR sensor detects movement](https://www.electronicshub.org/interfacing-isd1820-voice-recorder-module-with-arduino/)

