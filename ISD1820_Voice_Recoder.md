# Voice / Sound Recorder (ISD1820)

----
### What it does

- Records short audio clips (typically set up to ~10 seconds)

- Plays back the recorded audio through an on-board speaker (or an external amplifier/speaker)

- Recording is done using the on-board microphone

- Audio is overwritten every time you record (there is no file system, audio datam and no way to store multiple recordings)

- The ISD1820 has no programmable logic, however, it can interact with a microcontroller under its pinout logig (HIGH/LOW), but all audio handling happens inside the ISD1820 itself.

----
### Module control

**VCC:** +5V power supply

**GND:** GND power supply 

**FT:** Feed Through. Does not play recorded audio. Routes the microphone input directly to the speaker output. Acts like a crude live microphone. Ignores the recorded memory completely

**PLAYL:** Plays the recorded sound. Level-triggered. Push and hold the PLAYL button and the message starts playing and if you want to stop the playback at any time, simply release the button.

**PLAYE:** Plays the recorded sound. Edge-triggered. Press once to play the entire voice message. 

**REC:** Push and start recording. Continue to push the button until you record the complete message.


----
### Activating Endless-Loop-Playback

To force infinite looping: connect PE to PL via a jumper or bridge.

Infinite looping will work like:

- if the PLAYE button is pressed once, the track will loop.

- while the PLEYL button is pressed, the track will loop.

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
### interfase with Arduino

**Wiring**: Connect the ISD1820 with Arduino as follows:

- VCC to 5V

- GND to GND

- PE to Digital pin 3

- REC to Digital pin 2

**Code:** Trigger pins sending a LOW → HIGH transition

----
### More tutorials

[ISD1820 with Arduino and sensors](https://www.electroniclinic.com/arduino-isd1820-voice-recorder-playback-module/)

[Play sound if IR sensor detects movement](https://www.electronicshub.org/interfacing-isd1820-voice-recorder-module-with-arduino/)

