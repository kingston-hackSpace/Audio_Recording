# Teensy 4.1



---
### Recording WAV on Teensy

The Teensy Audio Library already includes:

- AudioInputI2S

- AudioRecordQueue

- SD writing helpers

- Typical flow:

  ```
  AudioInputI2S mic;
  AudioRecordQueue recorder;
  ```
With Teensy 4.1:

10–20 seconds → trivial

Minutes → normal

Hours → possible (SD size limited)

At 44.1 kHz / 16-bit mono:

~88 KB per second

1 minute ≈ 5.3 MB

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

