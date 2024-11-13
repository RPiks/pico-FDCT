///////////////////////////////////////////////////////////////////////////////
//
//  Roman Piksaykin [piksaykin@gmail.com], R2BDY
//  https://www.qrz.com/db/r2bdy
//
///////////////////////////////////////////////////////////////////////////////
//
//
//  test.c - The test & benchmark project of pico-FDCT library.
// 
//  DESCRIPTION
//      FDCT for Pico provides real-time forward and inverse DCT using integer 
//  arithmetic. It  can process  ~950k Samples/sec of  12-bit  data  providing 
//  1024-bins DCT. Throughtput  of  inverse  transform (IDCT)  quite the same. 
//  Using the library it is  possible to  process  realtime  audio streams in 
//  frequency domain as well as streams  of  higher frequencies,  for example, 
//  in digital radio receivers.
//
//  HOWTOSTART
//      ./build.sh; cp ./build/*.uf2 /media/Pico_Board/
//
//  PLATFORM
//      Raspberry Pi pico.
//
//  REVISION HISTORY
//      Rev 0.1   16 NOV 2024
//
//  PROJECT PAGE
//      https://github.com/RPiks/pico-FDCT
//
//  SUBMODULE PAGE
//      -
//
//  LICENCE
//      MIT License (http://www.opensource.org/licenses/mit-license.php)
//
//  Copyright (c) 2024 by Roman Piksaykin
//  
//  Permission is hereby granted, free of charge,to any person obtaining a copy
//  of this software and associated documentation files (the Software), to deal
//  in the Software without restriction,including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY,WHETHER IN AN ACTION OF CONTRACT,TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
///////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include <stdio.h>
#include "pico/stdlib.h"
//#include <stdlib.h>

#define DEBUG

#include <defines.h>
#include <init.h>
#include <StampPrintf.h>
#include <utility.h>
#include <PicoDCT.h>

int main()
{
    dbg::StampPrintf("\n");
    
    sleep_ms(5000);

    ft8::InitPicoHW();

    dbg::StampPrintf("PicoDCT module init...");
    sigproc::PicoDCT pdct;
    
    const int n2(10);
    const int len(1 << n2);

    dbg::StampPrintf("OK\nTesting forward + inverse conversion accuracy...\n");

    int32_t pvec_signal[len];
    int32_t pvec_temp[len];
    for(int i(0); i < len; ++i)
    {
        pvec_signal[i] = (int32_t)(0.5 + 500. * sin(2. * M_PI / 117. * (double)i));
    }

    uint32_t uinoise(0xCAFEC0DE);
    for(;;)
    {
        for(int i(0); i < len; ++i)
        {
            // Construct an additive mix of initial harmonic signal & noise.
            utl::PRN32(&uinoise);
            int32_t val = pvec_signal[i] + (int32_t)(uinoise % 500u) - 250u;
            pdct.SetBuf()[i] = val;
            pvec_temp[i] = val;
        }

        uint64_t tm_start = utl::GetUptime64();
        pdct.FwdFDCT(n2);
        uint64_t tm_finish = utl::GetUptime64();

        dbg::StampPrintf("Forward DCT conversion time: %ld us", (int32_t)(tm_finish - tm_start));

        for(int i(0); i < len; ++i)
        {
            //printf("%ld;", pdct.GetBuf()[i]);
            pdct.SetBuf()[i] >>= 3;
        }
        //printf("\n");

        tm_start = utl::GetUptime64();
        pdct.InvFDCT(n2);
        tm_finish = utl::GetUptime64();

        for(int i(0); i < len; ++i)
        {
            int32_t diff = (pdct.GetBuf()[i] >> 6) - pvec_temp[i];
            printf("%ld;", diff);
        }

        dbg::StampPrintf("Inverse DCT conversion time: %ld us", (int32_t)(tm_finish - tm_start));

        sleep_ms(100);
    }
}
