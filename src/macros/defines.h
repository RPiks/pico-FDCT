///////////////////////////////////////////////////////////////////////////////
//
//  Roman Piksaykin [piksaykin@gmail.com], R2BDY
//  https://www.qrz.com/db/r2bdy
//
///////////////////////////////////////////////////////////////////////////////
//
//
//  defines.h - Project macros.
// 
//  DESCRIPTION
//      -
//
//  HOWTOSTART
//      -
//
//  PLATFORM
//      Raspberry Pi pico.
//
//  REVISION HISTORY
// 
//      Rev 0.1   21 Sep 2024
//  Initial release.
//
//  PROJECT PAGE
//      https://github.com/RPiks/pico-ft8
//
//  LICENCE
//      MIT License (http://www.opensource.org/licenses/mit-license.php)
//
//  Copyright (c) 2023,2024 by Roman Piksaykin
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

#ifdef DEBUG
#define DEBUGPRINTF(x) dbg::StampPrintf(x);
#else
#define DEBUGPRINTF(x) { }
#endif

/* Assertion macro. */
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1       \
: __FILE__)
#define ASSERT_(x) { if(!(x)) for(;;) { dbg::StampPrintf("%s:%s", __FILENAME__, \
__LINE__); gpio_put(PICO_DEFAULT_LED_PIN, 1); sleep_ms(100);                    \
gpio_put(PICO_DEFAULT_LED_PIN, 0); sleep_ms(900); } }

#define FALSE 0                                     /* Something is false. */
#define TRUE 1                                       /* Something is true. */
#define BAD 0                                         /* Something is bad. */
#define GOOD 1                                       /* Something is good. */
#define INVALID 0                                 /* Something is invalid. */
#define VALID 1                                     /* Something is valid. */
#define NO 0                                          /* The answer is no. */
#define YES 1                                        /* The answer is yes. */
#define OFF 0                                       /* Turn something off. */
#define ON 1                                         /* Turn something on. */
#define ZERO 0                                 /* Something in zero state. */

     /* A macro for arithmetic right shifts, with casting of the argument. */
#define iSAR(arg, rcount) (((int32_t)(arg)) >> (rcount))

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

#define clip(x,a,b) min((b), max((x), (a)))

#define MUL(x,y) (__mul_instruction((x), (y)))
#define SQR(x) MUL((x), (x))
