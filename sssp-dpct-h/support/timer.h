/*
 * Copyright (c) 2016 University of Cordoba and University of Illinois
 * All rights reserved.
 *
 * Developed by:    IMPACT Research Group
 *                  University of Cordoba and University of Illinois
 *                  http://impact.crhc.illinois.edu/
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * with the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *      > Redistributions of source code must retain the above copyright notice,
 *        this list of conditions and the following disclaimers.
 *      > Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimers in the
 *        documentation and/or other materials provided with the distribution.
 *      > Neither the names of IMPACT Research Group, University of Cordoba, 
 *        University of Illinois nor the names of its contributors may be used 
 *        to endorse or promote products derived from this Software without 
 *        specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH
 * THE SOFTWARE.
 *
 */

#define DPCT_USM_LEVEL_NONE
#include <CL/sycl.hpp>
#include <dpct/dpct.hpp>
#include <time.h>

using namespace std;

struct Timer {

  map<string, clock_t> startTime;
  map<string, clock_t> stopTime;
  map<string, float>         time;

  void start(string name) {
    if(!time.count(name)) {
      time[name] = 0.0;
    }
        /*
    DPCT1008:0: clock function is not defined in the DPC++. This is a
         * hardware-specific feature. Consult with your hardware vendor to find
         * a replacement.
    */
        startTime[name] = clock();
  }

  void stop(string name) {
        /*
    DPCT1008:1: clock function is not defined in the DPC++. This is a
         * hardware-specific feature. Consult with your hardware vendor to find
         * a replacement.
    */
        stopTime[name] = clock();
    float part_time = (float)(stopTime[name] - startTime[name]) /
      CLOCKS_PER_SEC * 1000;
    time[name] += part_time;
  }

  void print(string name, unsigned int REP) { printf("%s Time (ms): %f\n", name.c_str(), time[name] / REP); }
};

