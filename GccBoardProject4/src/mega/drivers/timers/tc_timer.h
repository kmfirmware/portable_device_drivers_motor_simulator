/**
 * \file
 *
 * \brief megaAVR Timer/Counter (TC) Timeout Driver defines
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef TC_TIMEOUT_H
#define TC_TIMEOUT_H

#include "compiler.h"
#include "conf_timer.h"
#include <avr/sfr_defs.h>
#include <stdint.h>

/**
 * \defgroup tc_timeout_group Timer/Counter (TC) Timeout Driver
 *
 * This timeout functionality driver uses the asynchronous Timer/Counter
 * (TC) as time source in order to have a system tick. Since this is the
 * asynchronous TC module, it's convenient to use a 32KHz crystal as timeout
 * source for real time timing. Typical tick rate is in the range 1 - 1000Hz.
 *
 * The timeout functionality is a configurable number of independent timeouts
 * that are specified in number of ticks. They can be both singleshot and
 * periodic.
 *
 * As this driver provides a software layer on top of the TC module it will
 * have some performance penalty, so for high performance it would be
 * recommended to implement a more specific use by implementing your own
 * interrupt handler based on this as a reference.
 *
 * \section tc_timeout_configuration Configuration
 * Configuration is done in the configuration file : conf_tc_timeout.h
 *
 * Configuration defines:
 * - \ref CONFIG_TC_TIMEOUT_COUNT : The number of separate timeouts to
 *                                  allocate. Maximum 8.
 * - \ref CONFIG_TC_TIMEOUT_TICK_HZ : Requested tick rate. Might not be
 *                                    possible to achieve exact rate, but
 *                                    actual rate is available in \ref
 *                                    TC_TIMEOUT_TICK_HZ.
 * - \ref CONFIG_TC_TIMEOUT_CLOCK_SOURCE_HZ : The TC clock source frequency.
 *                                            Typically this is from a 32KHz
 *                                            crystal connected to TOSC. But
 *                                            other external clocks or the IO
 *                                            clock is possible to use.
 * - \ref CONFIG_TC_TIMEOUT_CLOCK_SOURCE_TOSC : Define to use crystal on TOSC
 *                                              as clock source.
 * - \ref CONFIG_TC_TIMEOUT_CLOCK_SOURCE_EXCLK : Define to use external clock
 *                                               on TOSC1 as clock source.
 *                                               Not available on older
 *                                               megaAVR.
 *
 * If both \ref CONFIG_TC_TIMEOUT_CLOCK_SOURCE_TOSC and \ref
 * CONFIG_TC_TIMEOUT_CLOCK_SOURCE_EXCLK are undefined the IO clock is used as
 * clock source.
 *
 * \section tc_timeout_interface Interface
 * First, the timeout internal setup needs to be configured and this is done
 * by the function tc_timeout_init().
 *
 * There are different functions for starting a timer:
 * - tc_timeout_start_singleshot(): Start a singleshot timeout.
 * - tc_timeout_start_periodic(): Start a periodic timeout.
 * - tc_timeout_start_offset(): Start a periodic timeout with a specific start
 *                              offset.
 *
 * Polling for timer status can be done with
 * tc_timeout_test_and_clear_expired(), and this will also clear the expired
 * flag in case of periodic timer.
 *
 * A running timer can be stopped with tc_timeout_stop().
 *
 * Common to all the function arguments are a timeout identifier, this is a
 * number starting from 0 to identify the timeout. Maximum of this parameter is
 * controlled by the configuration \ref CONFIG_TC_TIMEOUT_COUNT.
 *
 * The start timeout function uses timeout values represented in number of
 * ticks.
 *
 * \subsection tc_timeout_usage Usage
 * First of all, the include file is needed:
 * \code #include "tc_timeout.h" \endcode
 *
 * Then the timeout internals need to be set up by calling:
 * \code tc_timeout_init(); \endcode
 *
 * For simple usage starting a singleshot timeout for timeout id 0 and a timeout
 * value of 100 ticks:
 * \code
	tc_timeout_start_singleshot(0, 100);
	while (!tc_timeout_test_and_clear_expired(0));
	// do whats needed after timeout has expired
\endcode
 *
 * \section tc_timeout_accuracy Accuracy
 * Since this is a timeout layer on top of a system tick; the trigger time of a
 * timeout is fully depending on this system tick. This means that you might
 * not know when the next tick will count down your timeout, and this inaccuracy
 * can be from 0 to 1 system tick.
 *
 * E.g.: If you want a timeout of 1 system tick and use 1 as your timeout
 * value, this might trigger immediately. So, if you have a requirement to wait
 * at least 1 system tick, it would be recommended to use the requested value
 * + 1.
 *
 * However, if you know the system tick has passed or are using periodic timeout
 * you can be confident in the timing.
 *
 * @{
 */

/**
 * \def CONFIG_TC_TIMEOUT_CLOCK_SOURCE_HZ
 * \brief Clock source speed in Hz
 *
 * This is typically either the IO clock (F_CPU) or clock crystal (32768Hz).
 */

/**
 * \def CONFIG_TC_TIMEOUT_CLOCK_SOURCE_TOSC
 * \brief If defined, use crystal on TOSC as clock source
 *
 * If this or \ref CONFIG_TC_TIMEOUT_CLOCK_SOURCE_EXCLK is undefined, the IO
 * clock will be used.
 */

/**
 * \def CONFIG_TC_TIMEOUT_CLOCK_SOURCE_EXCLK
 * \brief If defined, use external clock on TOSC1 as clock source
 *
 * If this or \ref CONFIG_TC_TIMEOUT_CLOCK_SOURCE_TOSC is undefined, the IO
 * clock will be used.
 */

/**
 * \def CONFIG_TC_TIMEOUT_TICK_HZ
 * \brief Ticks per second used as time base for timers
 */

/**
 * \def CONFIG_TC_TIMEOUT_COUNT
 * \brief Maximum number of TC based timers to allow
 *
 * This is limited by to 8 due to internal driver limits.
 */

// Test for missing configurations
#ifndef CONFIG_TC_TIMEOUT_CLOCK_SOURCE_HZ
# error "configuration define missing: CONFIG_TC_TIMEOUT_CLOCK_SOURCE_HZ"
#endif


#define TIMER_STOP 0


/**
 * \brief Initialize TC timeout
 *
 * Initializes Timer/Counter for desired tick rate and starts it.
 */
void tc_timer_init();
void tc_timer_set(uint32_t,void (*callback)(void));
void tc_timer_start();
void tc_timer_stop();
#endif