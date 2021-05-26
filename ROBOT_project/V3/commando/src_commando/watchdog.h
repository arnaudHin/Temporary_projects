/**
 * @file watchdog.h
 *
 * @author team FORMATO, ESEO
 *
 * @brief Watchdog implementation based on POSIX timers
 *
 * At the delay expiration, a function (callback) is executed in a POSIX thread.
 * This watchdog can be canceled or destroyed at any time.
 *
 * @section License
 *
 * The MIT License
 *
 * Copyright (c) 2016, Jonathan ILIAS-PILLET (ESEO)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef WATCHDOG_H_
#define WATCHDOG_H_
#include <stdint.h>

/**
 * The watchdog structure
 */
typedef struct Watchdog_t Watchdog;

/**
 * Function that will be called by the watchdog
 *
 * @param this the watchdog which have called this function
 */
typedef void (*WatchdogCallback)(Watchdog *this);

/**
 * Watchdog's constructor.
 *
 * @param delay expressed in microseconds
 * @param callback function to be called at expiration
 */
extern Watchdog *Watchdog_construct(uint32_t delay, WatchdogCallback callback);

/**
 * Arms the watchdog.
 *
 * @param this watchdog's instance
 */
extern void Watchdog_start(Watchdog *this);

/**
 * Disarms the watchdog.
 *
 * @param this watchdog's instance
 */
extern void Watchdog_cancel(Watchdog *this);

/**
 * watchdog's destructor
 *
 * @param this watchdog's instance
 */
extern void Watchdog_destroy(Watchdog *this);

#endif /* WATCHDOG_H_ */
