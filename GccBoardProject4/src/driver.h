/*
 * driver.h
 *
 * Created: 11/11/2017 10:41:58 PM
 *  Author: kerry
 */ 


#ifndef DRIVER_H_
#define DRIVER_H_
#include <stdint.h>
#include <avr/sfr_defs.h>
typedef void (*CallbType)(int8_t);
typedef uint8_t DriverStatus;
#define NOEMERG -1
#define EMERGENCY 1
#define EMERGSWPORT IOPORT_PORTB  //defined in io library- for portability
#define PCINTMASK 0
#define INTERRUPTVECT PCINT0_vect

DriverStatus GetStatus();
void MotorDriverInit(uint8_t, CallbType);

const int8_t GetEmStatus();
void StopMotor();
void MotorDrive(Speed,Dir);
void MotorRate();

#define IOPORT_CREATE_PIN(ioport,pin) ((ioport)*8 + (pin))
#endif /* DRIVER_H_ */