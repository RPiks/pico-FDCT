# pico-FDCT
Fast discrete cosine transform (FDCT) library for Raspberry Pi Pico writen in C++.

FDCT for Pico provides real-time forward and inverse DCT using integer arithmetic. It can process ~950 kSamples/sec of 12-bit data providing 1024-bins DCT. Throughtput of inverse transform (IDCT) quite the same. Using the library it is possible to process realtime audio streams in frequency domain as well as streams of higher frequencies, for example, in digital radio receivers.
