/*
 * motor_status.h
 *
 * Created: 11/11/2017 10:22:13 PM
 *  Author: kerry
 */ 


#ifndef MOTOR_STATUS_H_
#define MOTOR_STATUS_H_
typedef enum {READY=9, NOREADY,NOINIT} ErrStatus;

typedef enum {FORWARD=6, REVERSE, EQUAL} Dir;
typedef enum {FAST=3, SLOW, STONE} Speed;


#endif /* MOTOR_STATUS_H_ */