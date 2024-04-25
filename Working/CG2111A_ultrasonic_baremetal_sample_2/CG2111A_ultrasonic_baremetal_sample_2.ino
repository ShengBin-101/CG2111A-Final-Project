/*******************************/
/*** Created by Ovidiu Sabau ***/
/***	16th January 2022	 ***/
/*******************************/


// #include "Ultrasonic.h"
// From module: GPIO - General purpose Input/Output
// #include <gpio.h>

// // From module: Generic board support
// #include <board.h>

// // From module: IOPORT - General purpose I/O service
// #include <ioport.h>

// // From module: Interrupt management - megaAVR implementation
// #include <interrupt.h>

// // From module: MEGA compiler driver
// #include <compiler.h>
// #include <status_codes.h>

// // From module: Part identification macros
// #include <parts.h>


/*------------------ Global variables ---------------*/
// volatile uint16_t pulse;


uint8_t distance = 0;
uint8_t diagnostics = 0;
uint8_t iteration = 0;

void setup() {
  Serial.begin(9600);
	init_ultrasonic();
	
}

void loop() {
  iteration += 1;
		/* Ultrasonic sensor */
		distance = getDistance_main(&diagnostics);
		Serial.println(distance);
}

int main(void)
{
	/* Initialization */
	
	setup();
	
	while( 1 )
	{
		loop();
	
	}
}


/* *****************************************************************
Name:		Ultrasonic Interrupt
Inputs:		none
Outputs:	internal timer
Description:calculates elapsed time of a measurement
******************************************************************** */
