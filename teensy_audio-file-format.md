# Teensy - audio file format

Teensy can play and record using WAV format, however, some examples might use RAW as default. 

Raw:

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

