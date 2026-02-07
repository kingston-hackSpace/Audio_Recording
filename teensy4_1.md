# Teensy 4.1

- **600 MHz Cortex-M7 processor**: extremely fast, capable of handling real-time audio processing without glitches.

- **1 MB of RAM**: Huge RAM, this large memory allows huge buffers, so audio can be streamed and written to storage without dropouts. Can record minutes or hours. 

- **Built-in microSD socket**: native SDIO interface is much faster than SPI, allowing continuous audio recording for minutes or hours.

- **3.3 V logic**: compatible with most I2S microphones and peripherals without level-shifting.

- **Supports 44.1 kHz, 16-bit audio**: enabling high-quality recordings.

- Works with both I2S digital microphones and analog microphones.

- **Audio Library**: Integrated support via the Teensy Audio Library makes setup and coding much simpler compared to other microcontrollers.


Read more [here](https://www.sparkfun.com/teensy-4-1.html)


---
### Recording WAV with the Teensy Audio Library 

It already includes:

- AudioInputI2S

- AudioRecordQueue

- SD writing helpers

- Typical flow:

  ```
  AudioInputI2S mic;
  AudioRecordQueue recorder;
  ```

---
### HARDWARE

- Teensy 4.1 

- Teensy Audio Shield

- USB‑A to USB‑Micro cable

- micro SD card (4GB or bigger)

- SparkFun I2S Audio Breakout - MAX98357A

- Adafruit I2S MEMS Microphone Breakout - SPH0645LM4H

- 4Ω speakers or headphones

- Push buttons

- Potentiometer

-----------
### Teensy set up (for Arduino IDE)

If you are using the Teensy 4.1 for the first time, you will need to follow the next set-up steps:

- Open your Arduino IDE, and click File > Preferences (on MacOS, click Arduino IDE > Settings).

- In "Additional boards manager URLs", copy this link: https://www.pjrc.com/teensy/package_teensy_index.json

- In the main Arduino window, open Boards Manager by clicking the left-side board icon, search for "teensy", and click "Install".

- Install the Teensyduino Library. See more [here](https://www.pjrc.com/teensy/td_download.html)

- done! Plug the Teensy 4.1 to your computer.

- Select the teensy as your board.

- If the Teensy is not turning on, you might need to press the BOOST button. Reboot the board. 

- Upload Arduino's Blink example to test communication




-----------
### WIRING

| Component                      | Teensy 4.1 Pin                         | Notes / Connection                                           |
| ------------------------------ | -------------------------------------- | ------------------------------------------------------------ |
| **SPH0645LM4H I2S Microphone** | **BCLK** → Pin 21                      | I2S Bit Clock                                                |
|                                | **LRCLK** → Pin 20                     | I2S Word Select / Left-Right Clock                           |
|                                | **DOUT** → Pin 9                       | I2S Data Out (to Teensy)                                     |
|                                | **VDD** → 3.3 V                        | Power                                                        |
|                                | **GND** → GND                          | Ground                                                       |
|                                |                                        |                                                              |
|                                |                                        |                                                              |
| **MAX98357A I2S Amplifier**    | **BCLK** → Teensy I2S BCLK (Pin 21)    | I2S Bit Clock                                                |
|                                | **LRCLK** → Teensy I2S LRCLK (Pin 20)  | Word select                                                  |
|                                | **DIN** → Teensy I2S Data Out          | Audio data from Teensy                                       |
|                                | **VCC** → 3.3 V                        | Power                                                        |
|                                | **GND** → GND                          | Ground                                                       |




-----------
### CODE AND INSTRUCTIONS

Let's test our Teensy by running one of the many Teensy Audio Library ready‑made examples:

In Arduino IDE:

- File → Examples → Audio

Select:

- AudioRecordToWav
