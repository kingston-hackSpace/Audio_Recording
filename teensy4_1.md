# Teensy 4.1

How audio flows:

Mic converts sound → digital samples

Teensy receives audio via I2S

Audio Library buffers it in RAM

Teensy writes chunks to built-in SD card

You remove SD → play WAV on computer 🎧

---
### HARDWARE

- Teensy 4.1 

- micro SD card

-----------
### Teensy set up

-----------
### WIRING

| Mic pin        | Meaning     | Teensy 4.1         |
| -------------- | ----------- | ------------------ |
| **3V**         | Power       | 3.3V               |
| **GND**        | Ground      | GND                |
| **BCLK**       | Bit clock   | Pin 21             |
| **LRCLK / WS** | Word select | Pin 20             |
| **DOUT**       | Audio data  | Pin 8              |
| **SEL**        | L/R select  | GND (left channel) |



-----------
### CODE

