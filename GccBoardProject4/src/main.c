/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
 
#include <asf.h>
#include "..\services\ioport\ioport.h"
#include "..\services\clock\sysclk.h"
#include "api.h"
#include "main.h"
#define CurConfig LEDConfigA

int main (void)
{
	board_init();
	sysclk_init();
	ioport_init();
	switch_init();
	timer_dbnc_init();
	emerg_led_init();
	Status status=DrvrInit(CurConfig,&emergIndicator);
	while (true)
	{
	   if (swPressed && btnTimeout && status!=ERR)
	   {
	      switch(curSwNum)
		  {
		     case 1: status=MotorStop(); curSwNum=0; swPressed=false; break;
			 case 2: status=MotorForward(SLOW); curSwNum=0; swPressed=false; break;
			 case 3: status=MotorForward(FAST); curSwNum=0; swPressed=false; break;
			 case 4: status=MotorReverse(SLOW); curSwNum=0; swPressed=false; break;
			 case 5: status=MotorReverse(FAST); curSwNum=0; swPressed=false; break;  
		  }
	   }
	}
}

static void timer_dbnc_init()
{
   cpu_irq_disable();
   TCCR1B=(1<<WGM12); // normal operation
   OCR1A=TMR_TICKS(sysclk_get_cpu_hz(),DBNC_DLY_TIME,8);
   TIMSK1=(1<<OCIE1A); // enable timer1 interrupt
   swPressed=btnTimeout=false;
   cpu_irq_enable(); 
}

static void switch_init()
{
   cpu_irq_disable();
   // set interrupt on rising edge
   EICRA=(1<<ISC31)|(1<<ISC30)|(1<<ISC21)|(1<<ISC20)|(1<<ISC11)|(1<<ISC10)|(1<<ISC01)|(1<<ISC00);
   // turn on interrupts on change of pins 0-3
   EIMSK=(1<<INT3)|(1<<INT2)|(1<<INT1)|(1<<INT0);
   // turn on pin change interrupt for PCINT1
   PCICR=(1<<PCIE1);
   PCMSK1=(1<<PCINT9);
   ioport_configure_group(ioport_pin_to_port_id(SWPIN(D,0)),SWPINDMASK,IOPORT_PULL_UP|IOPORT_DIR_INPUT);
   ioport_configure_group(ioport_pin_to_port_id(SWPIN(J,0)),SWPINJMASK,IOPORT_PULL_UP|IOPORT_DIR_INPUT);
   btnTimeout=swPressed=false;
   cpu_irq_enable();
}
static void emerg_led_init()
{
   ioport_configure_port_pin((void*)ioport_pin_to_port(EMLEDPIN),(pin_mask_t)(1<<EMLEDPIN),
                             IOPORT_DIR_OUTPUT);
}
void emergIndicator(int8_t status)
{
   if (status == EMERGENCY)
   {
      ioport_set_pin_high(IOPORT_CREATE_PIN(EMLEDPORT,EMLEDPIN));
   }
   else if (status == NOEMERG)
   {
      ioport_set_pin_low(IOPORT_CREATE_PIN(EMLEDPORT,EMLEDPIN));
   }
}
ISR (TIMER1_COMPA_vect,ISR_BLOCK)
{
   DEBOUNCE();
   DISABLE_DBNC_TMR();
}
ISR (INT0_vect, ISR_BLOCK)
{
   curSwNum=1;
   ENABLE_DBNC_TMR();
}
ISR (INT1_vect, ISR_BLOCK)
{
   curSwNum=2;
   ENABLE_DBNC_TMR();
}
ISR (INT2_vect,ISR_BLOCK)
{
   curSwNum=3;
   ENABLE_DBNC_TMR();
}
ISR (INT3_vect,ISR_BLOCK)
{
	curSwNum=4;
	ENABLE_DBNC_TMR();
}

ISR (PCINT1_vect,ISR_BLOCK)
{
    if (!ioport_get_pin_level(IOPORT_CREATE_PIN(SW5PORT,SWPIN(J,0))))
	{
	   curSwNum=5;
	   //if switch pressed, enable debounce timer
	   ENABLE_DBNC_TMR();
	}
	else if (!ioport_get_pin_level(IOPORT_CREATE_PIN(SW5PORT,SWPIN(J,0))))
	{
	   curSwNum=6;
	   ENABLE_DBNC_TMR();
	}
}