# pico-FDCT
Fast discrete cosine transform (FDCT) library for Raspberry Pi Pico & other platforms writen in C++.

FDCT for Pico provides real-time forward and inverse DCT using integer arithmetic. It processes ~950k Samples/sec of 12-bit data providing 1024-bins DCT. Throughtput of inverse transform (IDCT) quite the same. Using the library it is possible to process realtime audio streams in frequency domain, having a lot of time to implement sophisticated algorithms of spectral components' manipulation. It is possible to process streams of higher frequencies, which can be suitable for SDR projects.
