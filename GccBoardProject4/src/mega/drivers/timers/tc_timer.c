/**
 * \file
 *
 * \brief megaAVR Timer/Counter (TC) timeout driver implementation
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
#include "compiler.h"
#include "tc_timer.h"

/**
 * \ingroup tc_timeout_group
 * \defgroup tc_timeout_internals_group Timer/Counter (TC) Timeout Driver \
 *                                      internals
 *
 * @{
 */



// Validate number of timeouts configured
#if CONFIG_TC_TIMEOUT_COUNT > 1
# error "Too many timeouts configured! Maximum is 1"
#endif

//! \brief Bitmask of active timeouts
static bool tc_timer_inited=false;

//! \brief bitmask of expired timeouts
static bool tc_timer_setted=false;

static bool tc_timer_started=false;

static uint8_t tc_timer_prescale_mask=1<<CS20; // prescaler of 1
static void (*timer_callback)();

// Resolve mask to set in ASSR register based on configuration
#ifdef CONFIG_TC_TIMEOUT_CLOCK_SOURCE_TOSC
# ifdef AS0 // Older mega have got the asynchronous TC on TC0
#  define TC_TIMEOUT_ASSR_MASK (1 << AS0)
# else
#  define TC_TIMEOUT_ASSR_MASK (1 << AS2)
# endif
#elif defined(CONFIG_TC_TIMEOUT_CLOCK_SOURCE_EXCLK)
# define TC_TIMEOUT_ASSR_MASK ((1 << EXCLK) | (1 << AS2))
#else
# define TC_TIMEOUT_ASSR_MASK 0
#endif

// Resolve which TIMSK register to use
#ifdef TIMSK // Older mega have got a common TIMSK register
# define TC_TIMEOUT_TIMSK TIMSK
#else
# define TC_TIMEOUT_TIMSK TIMSK2
#endif

// Resolve which TC registers to use
#ifdef AS0 // Older mega have got the asynchronous TC on TC0
# define TC_TIMEOUT_OCR OCR0
# define TC_TIMEOUT_OCIE OCIE0
# define TC_TIMEOUT_TCCRA TCCR0
# define TC_TIMEOUT_COMP_vect TIMER0_COMP_vect
#elif defined(OCR2) // A bit newer mega got single compare on TC
# define TC_TIMEOUT_OCR OCR2
# define TC_TIMEOUT_OCIE OCIE2
# define TC_TIMEOUT_TCCRA TCCR2
# define TC_TIMEOUT_COMP_vect TIMER2_COMP_vect
#elif !defined(OCR2B) // LCD mega got single compare on TC called COMPA
# define TC_TIMEOUT_OCR OCR2A
# define TC_TIMEOUT_OCIE OCIE2A
# define TC_TIMEOUT_TCCRA TCCR2A
# define TC_TIMEOUT_COMP_vect TIMER2_COMP_vect
#else
# define TC_TIMEOUT_OCR OCR2A
# define TC_TIMEOUT_OCIE OCIE2A
# define TC_TIMEOUT_TCCRA TCCR2A
# define TC_TIMEOUT_TCCRB TCCR2B
# define TC_TIMEOUT_COMP_vect TIMER2_COMPA_vect
#endif

//! \brief Interrupt handler for TC compare
ISR(TC_TIMEOUT_COMP_vect)
{
   if (tc_timer_started)
   {
	  timer_callback();
   }
}

/** @} */

void tc_timer_init()
{
    if (!tc_timer_inited && !tc_timer_setted && !tc_timer_started)
	{

	// Set up clock source according to configuration
	ASSR = TC_TIMEOUT_ASSR_MASK;

	// Configure Timer/Counter to CTC mode, and set desired prescaler
#ifdef TC_TIMEOUT_TCCRB // Only newer mega devices have two control register
	TC_TIMEOUT_TCCRA = 1 << WGM21;
	TC_TIMEOUT_TCCRB = TIMER_STOP;
#else
	TC_TIMEOUT_TCCRA = (1 << WGM21) | TIMER_STOP;
#endif
    
	// Enable interrupt for compare match
	TC_TIMEOUT_TIMSK = 1 << TC_TIMEOUT_OCIE;

	tc_timer_inited=true;
	}
}



void tc_timer_set(uint32_t val,void (*callback)(void))
{
   if (tc_timer_inited && !tc_timer_started)
   {
    timer_callback=callback;  
   uint16_t tc_timer_prescaler=1;

   if (val > 0xFF*2<<7)
   {
      tc_timer_prescale_mask|=(1<<CS21)|(1<<CS22);
	  tc_timer_prescaler=1024;
   }
   else if (val > 0xFF*2<<6)
   {
      tc_timer_prescale_mask=(1 << CS22) | (1 << CS21);
	  tc_timer_prescaler=256;
   }
   else if (val > 0xFF*2<<5)
   {
       tc_timer_prescale_mask|=(1<<CS22);
	   tc_timer_prescaler=128;
   }
   else if (val > 0xFF*2<<4)
   {
       tc_timer_prescale_mask=(1<<CS22);
	   tc_timer_prescaler=64;
   }
   else if (val > 0xFF*2<<2)
   {
       tc_timer_prescale_mask|=(1<<CS21);
	   tc_timer_prescaler=32;
   }
   else if (val > 0xFF)
   {
       tc_timer_prescale_mask=(1<<CS21);
	   tc_timer_prescaler=8;
   }
   TC_TIMEOUT_OCR=val/tc_timer_prescaler - 1;
   if (TC_TIMEOUT_OCR < 0)
      TC_TIMEOUT_OCR=0;
   else if (TC_TIMEOUT_OCR > 0xFF)
      TC_TIMEOUT_OCR=255;
   tc_timer_setted=true;
   }
}
void tc_timer_start()
{
   if (tc_timer_setted)
   {

   #ifdef TC_TIMEOUT_TCCRB // Only newer mega devices have two control register
   TC_TIMEOUT_TCCRB = tc_timer_prescale_mask;
   #else
   TC_TIMEOUT_TCCRA |= (1 << WGM21) | tc_timer_prescale_mask;
   #endif
   tc_timer_started=true;
   }
}
void tc_timer_stop()
{
    if (tc_timer_started)
	{
	    #ifdef TC_TIMEOUT_TCCRB // Only newer mega devices have two control register
	    TC_TIMEOUT_TCCRB = 0;
	    #else
	    TC_TIMEOUT_TCCRA &= ~(1 << WGM21) ;
	    #endif
	}
}

