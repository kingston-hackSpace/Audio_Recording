# Teensy 4.1

Build a audio recorder with Teensy 4.1

---
### Teensy SET-UP

If you are using the Teensy 4.1 for the first time, you will need to install the Teensy board and library.

Follow the installation steps [here](https://github.com/kingston-hackSpace/Teensy_4-1/blob/main/README.md)

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

Let's test Teensy by running one of the many Teensy Audio Library ready‑made examples:

In Arduino IDE:

- File → Examples → Audio

Select:

- AudioRecordToWav
