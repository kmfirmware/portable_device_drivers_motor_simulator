/*
 * main.h
 *
 * Created: 11/27/2017 9:53:46 PM
 *  Author: kerry
 */ 


#ifndef MAIN_H_
#define MAIN_H_
#define SWPINDMASK 0x0F
#define SWPINJMASK 0x03
#define NOEMERG -1
#define EMERGENCY 1

#define SWPIN(pid,num)  PIN##pid##num
#define SW5PORT IOPORT_PORTJ
#define EMLEDPIN SWPIN(D,4) 
#define EMLEDPORT IOPORT_PORTD

#define DBNC_DLY_MS 15 //delay in milliseconds

volatile static bool swPressed=false;
volatile static bool btnTimeout=false;
volatile static uint8_t curSwNum=0;

#define DEBOUNCE_TMR_PRESCALER 8
/*.015*1000 to keep integer arithmetic*/
#define INT_CONV_VAL 1000
#define DBNC_DLY_TIME 0.015
#define DISABLE_DBNC_TMR() (TCCR1B=TCCR1B&~(1<<CS12)&~(1<<CS11)&~(1<<CS10))
#define DEBOUNCE() (btnTimeout=true)
#define TMR_TICKS(CPU_FREQ, DELAY_TIME,PRESCAL_VAL) \
((DELAY_TIME) * (INT_CONV_VAL) * ((CPU_FREQ)/(PRESCAL_VAL))/(INT_CONV_VAL))

#define ENABLE_DBNC_TMR()  (swPressed=true,btnTimeout=false,TCNT1=0,TCCR1B=(TCCR1B&0)|(1<<CS11))
#define IOPORT_CREATE_PIN(ioport,pin) ((ioport)*8 + (pin))

static void timer_dbnc_init();
static void switch_init();
void emergIndicator(int8_t);
static void emerg_led_init();
#endif /* MAIN_H_ */