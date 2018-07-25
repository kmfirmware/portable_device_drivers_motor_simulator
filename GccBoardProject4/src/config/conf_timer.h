/*
 * conf_timer.h
 *
 * Created: 10/28/2017 12:59:56 PM
 *  Author: kerry
 */ 


#ifndef CONF_TIMER_H_
#define CONF_TIMER_H_


#define INT_CONV_VAL 1000 // used to convert ms to seconds
/* 1/(CPU_FREQ/_PRESCALER)=1_cycle_time*/
/* delay_time=timer_count*1_cycle_time*/
/* timer_count=delay_time/1_cycle_time*/
// DELAY_TIME in seconds
#define TMR_TICKS(CPU_FREQ,DELAY_TIME) \
((DELAY_TIME) * (CPU_FREQ))/(INT_CONV_VAL) 

// Using 32KHz clock on STK600 as clock source
#define CONFIG_TC_TIMEOUT_CLOCK_SOURCE_HZ 32768
#define CONFIG_TC_TIMEOUT_CLOCK_SOURCE_TOSC 1
#define CONFIG_TC_TIMEOUT_COUNT 1

#endif /* CONF_TIMER_H_ */