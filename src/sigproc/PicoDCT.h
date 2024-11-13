///////////////////////////////////////////////////////////////////////////////
//
//  Roman Piksaykin [piksaykin@gmail.com], R2BDY
//  https://www.qrz.com/db/r2bdy
//
///////////////////////////////////////////////////////////////////////////////
//
//
//  PicoDCT.h - Raspberry Pi Pico's fast Discrete Cosine Transform (FDCT) class.
//
//  DESCRIPTION
//      Provides highly optimized 1-D forward and reverse FDCT of length up to 
//  4096 frequency bins.
//      It uses only 32-bit integer arithmetics & pre-calculated trigonometric
//  table during operation. There is only one division in algorithm. It boasts
//  of quite decent linearity: the max spurious harmonic components is ~-15 dB.
//      So, it can process signals of about ~950 kHz sample rate (1024 f.bins,
//  no overlap). So, the conversion time of 1024-length forward DCT is ~1.1 ms.
//      The class  can be adopted to any other  platform  quite easily. It may
//  work outta the box outside of Pico; if doesn't - look at lines 68..76 for
//  help.
//
//  HOWTOSTART
//      1. Add the class into your project.
//      2. Provide a buffered queue to supply the class: please note that the
//  data in the input/output buffer mustn't vary during transform.
//      3. Process the data in any way you need (trim coeffs etc).
//      4. Divide every data value to 8 (or apply right shift by 3 bits).
//      5. Do reverse transform of data if your project requires so.
//
//  PLATFORM
//      Any.
//
//  REVISION HISTORY
//      v0.8    2024-11-09 Initial release.
//
//  PROJECT PAGE
//      https://github.com/RPiks/pico-FDCT
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
#pragma once

#include <math.h>
#include <stdint.h>

#ifdef PICO_DEFAULT_IRQ_PRIORITY
#define DCT_PICO_RAM  __not_in_flash_func
#else
#define DCT_PICO_RAM
#endif

#ifndef ASSERT_
#define ASSERT_(x) assert(x)
#endif

namespace sigproc
{

class PicoDCT final
{
public:
    PicoDCT(int n2max = 12)
    : _sin1exp(NULL)
    , _ptbuf(NULL)
    , _n2max(n2max)
    , _piobuf(NULL)
    {
        ASSERT_(n2max < 13);

        Init();
    }

    ~PicoDCT()
    {
        if(_ptbuf)
        {
            free(_ptbuf);
            _ptbuf = NULL;
        }

        if(_sin1exp)
        {
            free(_sin1exp);
            _sin1exp = NULL;
        }

        if(_piobuf)
        {
            free(_piobuf);
            _piobuf = NULL;
        }
    }

    const int32_t* GetBuf() const
    { 
        return _piobuf;
    }
    int32_t* SetBuf() const
    { 
        return _piobuf;
    }

    /// @brief Forward DCT transform of size 2^n.
    /// @param n Length of transform, 2^n values; [2...12] corresponds (4 to 4096).
    /// @return 0 OK; -1 n out of range; -2 no input array; -3 tr-size is too big.
    int FwdFDCT(int n)
    {
        if(n < 2 || n > _n2max)
        {
            return -1;
        }

        FwdTRstep(_piobuf, _ptbuf, n);

        return 0;
    }

    /// @brief Recurrent step of forward FDCT.
    /// @param vec Input & output vector.
    /// @param ptmp Temporary vector.
    /// @param n Length of transform, 2^n.
    inline void FwdTRstep(int32_t *vec, int32_t *ptmp, int n) 
    {
        // Recurrent call depth limit check.
        if(n <= 0)
        {
            return;
        }

        const int len(1 << n);
        const int halfLen(len >> 1);

        // Optimized Algorithm of Byeong Gi Lee, 1984.
        for(int i(0); i < halfLen; ++i)
        {
            const int32_t x = vec[i];
            const int32_t y = vec[len - 1 - i];
            ptmp[i] = x + y;
            
            const int32_t cos_m1 = Cos1Approx1024((i * 102943L + 51471L) >> (n + 2));
            ptmp[i + halfLen] = ((x - y) * cos_m1) >> 13;
        }

        // Recurrent calls.
        FwdTRstep(ptmp, vec, n - 1);
        FwdTRstep(ptmp + halfLen, vec, n - 1);

        for (int i(0); i < halfLen - 1; ++i)
        {
            vec[i << 1] = ptmp[i];
            vec[(i << 1) + 1] = ptmp[i + halfLen] + ptmp[i + halfLen + 1];
        }

        vec[len - 2] = ptmp[halfLen - 1];
        vec[len - 1] = ptmp[len - 1];
    }

    /// @brief Inverse DCT transform of size 2^n.
    /// @param n Length of transform, 2^n values; [2...12] corresponds (4 to 4096).
    /// @return 0 OK; -1 n out of range; -2 no input array; -3 tr-size is too big.
    int InvFDCT(int n)
    {
        if(n < 2 || n > _n2max)
        {
            return -1;
        }

        _piobuf[0] >>= 1;
        InvTRstep(_piobuf, _ptbuf, n);

        return 0;
    }

    /// @brief Recurrent step of inverse FDCT.
    /// @param vec Input & output vector.
    /// @param itmp Temporary vector.
    /// @param n Length of transform, 2^n.
    inline void InvTRstep(int32_t *vec, int32_t *itmp, int n) 
    {
        // Recurrent call depth limit check.
        if(n <= 0)
        {
            return;
        }

        const int len(1 << n);
        const int halfLen(len >> 1);

        itmp[0] = vec[0];
        itmp[halfLen] = vec[1];

        // Optimized Algorithm of Byeong Gi Lee, 1984.
        for(int i(1); i < halfLen; ++i)
        {
            itmp[i] = vec[i << 1];
            itmp[i + halfLen] = vec[(i << 1) - 1] + vec[(i << 1) + 1];
        }

        // Recurrent calls.
        InvTRstep(itmp, vec, n - 1);
        InvTRstep(itmp + halfLen, vec, n - 1);

        for (int i(0); i < halfLen; ++i)
        {
            const int32_t x = itmp[i];

            const int32_t cos_m1 = Cos1Approx1024((i * 102943L + 51471L) >> (n + 2));

            const int32_t y = (itmp[i + halfLen] * cos_m1) >> 13;

            vec[i] = x + y;
            vec[len - 1 - i] = x - y;
        }
    }

    /// @brief 1/Cosine approximation.
    /// @param  x an argument +-PI scaled by 2^13.
    /// @return value of 1/cos(x), scaled by 2^12.
    inline int32_t DCT_PICO_RAM (Cos1Approx1024)(int32_t x) const
    {
        x = 12868L - x;                       /* cos(pi/2 - phi) = sin(phi). */

        if(x < -25735L)           /* Get x in the range +-pi scaled by 2^13. */
        {
            x += 51471L;
        }
        else if(x > 25735L)
        {
            x -= 51471L;
        }

        int sign;
        if(x >= 0)                /* Get x in the range 0-pi scaled by 2^13 .*/
        {
            sign = 1;
        }
        else
        {
            sign = -1;
            x = -x;
        }

        if(x >= 12867L)                         /* Get x in the range 0-pi/2. */
        {
            x = 25735L - x;
        }

        const int index = x * 1025L / 12868L;

        return((int32_t)(sign * _sin1exp[index]));
    }

private:

    /// @brief Provides memory allocation & calculates look-up 1/sin(x) table.
    void Init()
    {
        _piobuf = (int32_t *)malloc((1 << _n2max) * sizeof(int32_t));
        ASSERT_(_piobuf);

        _ptbuf = (int32_t *)malloc((1 << _n2max) * sizeof(int32_t));
        ASSERT_(_ptbuf);

        // Calculate values & fill array of 1/sin(x).
        _sin1exp = (int32_t *)malloc(1025 * sizeof(int32_t));
        ASSERT_(_sin1exp);

        for(int i(0); i < 1025; ++i)
        {
            const double dangle = .5 * M_PI * (double)i / 1024.;
            const double dnom = (double)((1 << 12) - 1);
            const double ddenom = sin(dangle);

            _sin1exp[i] = fabs(ddenom) > 1e-12 ? (int32_t)(dnom / ddenom + .0) : 1 << 20;
        }
    }

    int32_t *_sin1exp;                                  /* 1 / sin(x) table. */
    int32_t *_ptbuf;                                   /* ptr to tmp buffer. */
    const int _n2max;                           /* max. transform size, 2^n. */
    int32_t *_piobuf;                                /* ptr to input buffer. */
};

}
