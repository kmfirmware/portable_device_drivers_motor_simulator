
/*
 * api.h
 *
 * Created: 11/11/2017 10:46:28 PM
 *  Author: kerry
 */ 


#ifndef API_H_
#define API_H_
#include "..\config\motor_status.h"
#include <stdint.h>

typedef void (*CallbType)(int8_t);
typedef enum  {EMERG,FORWARD_FAST, FORWARD_SLOW, REVERSE_FAST,REVERSE_SLOW, STOP, ERR,NONE} Status;
typedef enum  {LEDConfigA=0,LEDConfigB} LEDConfig;

Status DrvrInit(LEDConfig, CallbType);
Status MotorForward(Speed);
Status MotorReverse(Speed);
Status MotorStop();
Status MotorStatus();

#endif /* API_H_ */