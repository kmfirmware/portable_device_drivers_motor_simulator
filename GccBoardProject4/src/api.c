/*
 * api.c
 *
 * Created: 11/11/2017 10:36:30 PM
 *  Author: kerry
 */ 
 #include <asf.h>
 #include <avr\interrupt.h>
 #include <api.h>
 #include <driver.h>
 #include "..\services\ioport\ioport.h"


 Status DrvrInit(LEDConfig config, CallbType callback)
 {
    if (GetStatus() & NOINIT)
	{
       if (config == LEDConfigA)
	   {
	      MotorDriverInit(LEDConfigA,callback);
	   }
	   else if (config == LEDConfigB)
	   {
	      MotorDriverInit(LEDConfigB, callback);
	   }
	}

	return MotorStatus();
 }
 
 Status MotorForward(Speed speed)
 {
    DriverStatus status=GetStatus();
    if ((status&READY) == READY && (status&REVERSE) == REVERSE)
	{
	    // stop motor before going forward
		MotorStop();
		MotorDrive(speed,FORWARD);
	}
	
	return MotorStatus();
 }

 Status MotorReverse(Speed speed)
 {
	 DriverStatus status=GetStatus();
	 if ((status&READY) == READY && (status&FORWARD) == FORWARD)
	 {
		 // stop motor before going forward
		 MotorStop();
		 MotorDrive(speed,REVERSE);
	 }
	
	 
	 return MotorStatus();
 }

 Status MotorStop()
 {
     DriverStatus status=GetStatus();
	 if (!(status&NOREADY) && !(status&NOINIT))
	 {
	    MotorStop();
	 }
	
	 return MotorStatus();
 }
 
 Status MotorStatus()
 {
    Status result=NONE;
	DriverStatus status=GetStatus();
	
	if (status & NOREADY)
	{
	   result=ERR;
	}
	else if (status & READY) 
	{
	   if (status & FORWARD && status & FAST)
	   {
	      result=FORWARD_FAST;
	   }
	   else if (status & FORWARD && status & SLOW)
	   {
	      result=FORWARD_SLOW;
	   }
	   else if (status & REVERSE && status & FAST )
	   {
	      result=REVERSE_FAST;
	   }
	   else if (status & REVERSE && status & SLOW)
	   {
	      result=REVERSE_SLOW;
	   }
	 }
	 else if (status & EQUAL || status & STONE)
	 {
	    result=STOP;
	 }
	 else if (GetEmStatus() == EMERGENCY)
	 {
	    result=EMERG;
	 }
	 return result;
 }