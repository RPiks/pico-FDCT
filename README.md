# pico-FDCT
Fast discrete cosine transform (FDCT) library for Raspberry Pi Pico & other platforms writen in C++.

FDCT for Pico provides real-time forward and inverse DCT using integer arithmetic. It processes ~950k Samples/sec of 12-bit data providing 1024-bins DCT. Throughtput of inverse transform (IDCT) quite the same. Using the library it is possible to process realtime audio streams in frequency domain, having a lot of time to implement sophisticated algorithms of spectral components' manipulation. It is possible to process streams of higher frequencies, which can be suitable for SDR projects.

![two-tone](https://github.com/user-attachments/assets/5ba3f165-6995-4a53-808a-651707ce3c06)

Here an example of test terminal output from Pico.
![two-tone-term](https://github.com/user-attachments/assets/3add0041-408f-47d5-8751-e25406308f39)

# For what?
This is an experimental project of amateur radio hobby and it is devised by me in order to experiment with analog and digital modes. I am licensed radio amateur who is keen on experiments in the area of the digital modes on HF. My QRZ page is https://www.qrz.com/db/R2BDY

# Quick start
1. Install Raspberry Pi Pico SDK. Configure environment variables. Test whether it is built successfully.

2. git clone https://github.com/RPiks/pico-FDCT

3. cd pico-FDCT

4. ./build.sh

5. Check whether output file ./build/pico-FDCT.uf2 appears.

6. Load the .uf2 file into the Pico board.

7. minicom -D /dev/ttyXXXX (here is the serial port of Pico, usually ttyACM0).

8. Provide the feedback by clicking like on the github page of the project :).

Cheers, Roman Piksaykin, amateur radio callsign R2BDY https://www.qrz.com/db/R2BDY 
piksaykin@gmail.com
