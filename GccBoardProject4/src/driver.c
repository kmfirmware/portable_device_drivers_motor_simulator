/*
 * driver.c
 *
 * Created: 11/10/2017 9:22:15 PM
 *  Author: kerry
 */ 
 #include <asf.h>
 #include <avr\interrupt.h>
 #include "..\services\ioport\ioport.h"
 #include "..\mega\drivers\timers\tc_timer.h"
 #include "..\config\motor_status.h"
 #include <driver.h>


 typedef struct 
 {
    uint8_t LEDs;
	uint8_t forwardPin;
	uint8_t reversePin;
	uint8_t emergSwPin;
	CallbType callback;
 } LEDConfigA;
 
typedef  struct
 {
    uint8_t LEDs;
	uint8_t forwardPin;
	uint8_t reversePin;
	uint8_t emergSwPin;
	CallbType callback;
 } LEDConfigB; 

 static union
 {
    LEDConfigA config;
	LEDConfigB configCopy;
 } currConfig;


 static int8_t emStatus=NOEMERG;

 static ErrStatus err=NOINIT;
 static Dir curDir=EQUAL;
 static Speed curSpeed=STONE;
  
 void MotorDriverInit(uint8_t config, CallbType callback)
 {
    cpu_irq_disable();
    if (config == 0)
	{
	   LEDConfigA configA;
	   configA.LEDs=IOPORT_PORTE;
	   configA.forwardPin=PINE1;
	   configA.reversePin=PINE2;
	   configA.emergSwPin=PINB7;
	   currConfig.config=configA;
	   configA.callback=callback;
	   PCMSK0=(1<<currConfig.config.emergSwPin); //set which pin interrupts is enabled
	   PCICR=(1<<PCINTMASK); //enable PCINT0
	}
	else if (config == 1)
	{
	   LEDConfigB configB;
	   configB.LEDs=IOPORT_PORTH;
	   configB.forwardPin=PINH5;
	   configB.reversePin=PINH6;
	   configB.emergSwPin=PINB6;
	   currConfig.configCopy=configB;
	   configB.callback=callback;
	   PCMSK0=(1<<currConfig.config.emergSwPin); //set which pin interrupt is enabled
	   PCICR=(1<<PCINTMASK); //enable PCINT0
	}
	else
	{
	   return;
	}
	ioport_init();
	// configure Leds
    ioport_configure_group(currConfig.config.LEDs,
	                         (pin_mask_t)((1<<currConfig.config.forwardPin)|(1<<currConfig.config.reversePin)),IOPORT_DIR_OUTPUT);
	
	// configure emergency switch
	ioport_configure_port_pin((void*)ioport_pin_to_port(IOPORT_CREATE_PIN(currConfig.config.LEDs,currConfig.config.emergSwPin)),
	                          (pin_mask_t)(1<<currConfig.config.emergSwPin),IOPORT_DIR_INPUT|IOPORT_PULL_UP);
	
	//configure timer
	tc_timer_init();
	
	cpu_irq_enable(); //enable global interrupts
	if (emStatus != NOEMERG) //emergency bit changed
	{
	   currConfig.config.callback(emStatus=NOEMERG); // call callback
	}					   
	err=READY;
	StopMotor();
	
 }
 
 //forward motor --forward led flash, reverse led stops
 void MotorDrive(Speed speed,Dir direction)
 {

	   if (speed == SLOW)
	   {
		  tc_timer_set(TMR_TICKS(CONFIG_TC_TIMEOUT_CLOCK_SOURCE_HZ,.4),&MotorRate);
		  tc_timer_start();
	   }
	   else if (speed == FAST)
	   {
	      tc_timer_set(TMR_TICKS(CONFIG_TC_TIMEOUT_CLOCK_SOURCE_HZ,.1),&MotorRate);
	      tc_timer_start();
	   }
	 
	   if (emStatus != NOEMERG) //emergency bit changed
	   {
		   currConfig.config.callback(emStatus=NOEMERG); // call callback
	   }
	   curSpeed=speed;
	   curDir=direction;

 }


 void StopMotor()
 {
    curDir=EQUAL;
	curSpeed=STONE;
    tc_timer_stop();
	if (emStatus != NOEMERG) //emergency bit changed
	{
		currConfig.config.callback(emStatus=NOEMERG); // call callback
	}
 }

 // callback function for timer code
 void MotorRate()
 {
    if (curDir == FORWARD)
	{
	   ioport_tgl_group(ioport_pin_to_port_id(currConfig.config.forwardPin),1<<currConfig.config.forwardPin);
	}
	else if (curDir == REVERSE)
	{
	   ioport_tgl_group(ioport_pin_to_port_id(currConfig.config.reversePin), 1<<currConfig.config.reversePin); 
	}
 }

DriverStatus GetStatus()
{
   return err | curDir | curSpeed;   
}



const int8_t GetEmStatus()
{
   return emStatus;
}
 // emergency switch isr
 ISR(INTERRUPTVECT,ISR_BLOCK)
 {
   if (!ioport_get_pin_level(IOPORT_CREATE_PIN(EMERGSWPORT,currConfig.config.emergSwPin)) && err == READY)
   { // switch closed
      emStatus=-emStatus;
      currConfig.config.callback(emStatus);
   } 
 }
 
 ISR(BADISR_vect) // don't let cpu reset
 {
 }
 