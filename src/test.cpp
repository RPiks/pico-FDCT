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
#define DEBUG

#include <string.h>
#include <stdio.h>

#include <climits>
#include <utility>

#include "pico/stdlib.h"

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
    
    const int n2(12);
    const int len(1 << n2);

    dbg::StampPrintf("OK\nTesting forward + inverse conversion accuracy...\n");

    int32_t pvec_signal[len];
    int32_t pvec_temp[len];
    int32_t pvec_fdct[len];
    for(int i(0); i < len; ++i)
    {
        pvec_signal[i] = (int32_t)(0.5 + 1024. * sin(2. * M_PI / 11.f * (double)i));
        pvec_signal[i]+= (int32_t)(0.5 + 1024. * sin(2. * M_PI / 133.f * (double)i));
    }

    uint32_t uinoise(0xCAFEC0DE);
    for(;;)
    {
        std::pair<int32_t, int32_t> pr_minmax = { INT_MAX, INT_MIN };
        for(int i(0); i < len; ++i)
        {
            // Construct an additive mix of initial signal & pseudo-random noise.
            utl::PRN32(&uinoise);
            pdct.SetBuf()[i] = pvec_temp[i] = pvec_signal[i] + (int32_t)(uinoise % 1024u) - 512u;
            pr_minmax.first = min(pr_minmax.first, pdct.GetBuf()[i]);
            pr_minmax.second = max(pr_minmax.second, pdct.GetBuf()[i]);
        }

        uint64_t tm_start = utl::GetUptime64();
        pdct.FwdFDCT(n2);
        uint64_t tm_finish = utl::GetUptime64();
        const int32_t ifwdcdt = tm_finish - tm_start;

        // Apply scale to output in order to normalize result.
        for(int i(0); i < len; ++i)
        {
            pvec_fdct[i] = pdct.SetBuf()[i];
            pdct.SetBuf()[i] >>= 3;
        }

        tm_start = utl::GetUptime64();
        pdct.InvFDCT(n2);
        tm_finish = utl::GetUptime64();

        float f_acc2(0.f);
        for(int i(0); i < len; ++i)
        {
            const int32_t scl_val = (pdct.GetBuf()[i] + (1<<5)) >> 6;
            const int32_t diff = scl_val - pvec_temp[i];
            f_acc2 += SQR(diff);
            printf("%ld;%ld;%ld;%ld\n", abs(pvec_fdct[i]), pvec_temp[i], scl_val, diff);
        }

        // Standard deviation of full cycle conversion error.
        f_acc2 = sqrt(f_acc2 / (float)len);
        
        // Peak-to-peak range of the input signal.
        const int32_t dpkpk = pr_minmax.second - pr_minmax.first;
        
        // Calculate an error in dB relative to full scale signal range.
        // Edit this formula if you do prefer other method of quality assessment.
        const float errdb = 20.f * std::log10(f_acc2 / (float)dpkpk);

        dbg::StampPrintf("Forward DCT-%ld conversion time: %ld micros.", len, ifwdcdt);
        dbg::StampPrintf("Inverse DCT-%ld conversion time: %ld micros.", len, (int32_t)(tm_finish - tm_start));
        dbg::StampPrintf("Forward -> inverse transform error stats: Vpk-pk: %ld, Std.dev:%f, SNR:%.1f dBFS", dpkpk, f_acc2, errdb);

        sleep_ms(3000);
    }
}
