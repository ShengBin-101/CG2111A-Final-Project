/*
 * Ultrasonic.h
 *
 * Created: 14.03.2020 17:19:25
 *  Author: lenovo
 */ 


// #ifndef ULTRASONIC_H
// #define ULTRASONIC_H

// #ifndef F_CPU
// #define F_CPU 16000000ul
// #endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/* Configuration */
#define DIST_MAX 250			// Upper limit of the sensor
#define DIST_MIN 10				// Lower limit of the sensor
#define ULTRAS_ITER_MAX 3		// max iterations in the case measurements are not consistent
#define DISTANCE_E_NOK 0xFF		
#define SNSR_TOLERANCE 5		// cm

/* Macros */
/* Trigger pin */
#define U_Port_trig		PORTB
#define U_DDR_trig		DDRB
#define U_TRIG_pin		PINB4
/* Echo pin */
#define U_Port_echo		PORTD
#define U_DDR_echo		DDRD
#define U_ECHO_pin		PIND2
/* Power pin */
#define U_Port_pwr		PORTB
#define U_DDR_pwr		DDRB
#define U_PWR_pin		PINB5

#define ULTR_PWR_ON		U_Port_pwr |= (1<<U_PWR_pin)
#define ULTR_PWR_OFF	U_Port_pwr &= ~(1<<U_PWR_pin)


/* Prototype functions */
// void configUltrasonicPorts(void);
// void enable_interrupts(void);
// void disableInterrupts(void);
// uint8_t getLevel_main (void);
// uint8_t getDistance_main(uint8_t *);
// uint16_t getLevel(uint16_t);
// float getDistance(void);
// void triggerUltrasonic(void);
// void init_ultrasonic(void);


#include <stdlib.h>
#include <stdio.h>
// #include "Ultrasonic.h"

// #ifndef F_CPU
// #define F_CPU 16000000ul
// #endif

uint16_t volatile pulse;

/* *****************************************************************
Name:		configUltrasonicPorts()
Inputs:		none
Outputs:	none
Description:configures the pins for the ultrasonic sensor
******************************************************************** */
void configUltrasonicPorts()
{
	/* ECHO - input - port ECHO_pin */
	U_DDR_echo &= ~(1<<U_ECHO_pin);
	U_Port_echo &= ~(1<<U_ECHO_pin);
	
	/* TRIG - output - port TRIG_pin */
	U_DDR_trig |= (1<<U_TRIG_pin);
	U_Port_trig &= ~(1<<U_TRIG_pin);
	
	/* PWR - output - port TRIG_pin */
	U_DDR_pwr |= (1<<U_PWR_pin);
	U_Port_pwr &= ~(1<<U_PWR_pin);
}

/* *****************************************************************
Name:		enable_interrupts()
Inputs:		none
Outputs:	none
Description:configures the interrupts and enables the global interrupts
******************************************************************** */
 void enable_interrupts()
 {
	EIMSK |= (1<<INT0);
	EICRA |= (1<<ISC00);
	sei();
 }

/* *****************************************************************
Name:		init_ultrasonic()
Inputs:		none
Outputs:	none
Description:main function for the ultrasonic initialization
******************************************************************** */
void init_ultrasonic(void)
{
	configUltrasonicPorts();
	enable_interrupts();
}

/* *****************************************************************
Name:		getDistance_main()
Inputs:		pointer to diagnosis
Outputs:	distance measurements
Description:triggers measurements and plausibility checks
******************************************************************** */
 uint8_t getDistance_main(uint8_t * diag)
 {
 	uint8_t distance = 0;
	float dist = 0;
	float distance_array[2];
	float sum = 0;
	
	ULTR_PWR_ON; _delay_ms(150);
	
	/* re-measurements for confirmation of unexpected results */
	for (uint8_t i = 0; i <= ULTRAS_ITER_MAX; i++)
	{
		/* Reset before measurement */
		sum = 0;
		* diag = 0;
		
		/* Execute two measurements and calculate mean value for redundancy */
		for (uint8_t j = 0; j < 2; j++)
		{
			triggerUltrasonic();
			distance_array[j] = getDistance();
			sum +=distance_array[j];
			_delay_ms(50);
		}

		dist = sum/2;
		distance = dist;
		
		if ( dist <= 0 ) // Check if sensor defect or not connected (in which case it shows 0)
		{
			* diag = 13; // DTC: Defect sensor 
		}
		else if ( dist <= 20 ) // Too low range for the sensor (in case of sensor AJ-SR04M)
		{
			* diag = 15; // DTC: Low sensor range
		}		
		else if ( abs(distance_array[0] - distance_array[1]) > SNSR_TOLERANCE ) /* Check measurements tolerance */
		{	
			* diag = 12;		// DTC: Imprecise meas.
		}
		else if ( dist >= 250 ) // distance too high
		{
			* diag = 14; // DTC: Sensor out of range
		}
		else // Exit loop if no fault found
		{ 
			break;
		}
	}
	//ULTR_PWR_OFF;
	return distance;
 }

/* *****************************************************************
Name:		triggerUltrasonic()
Inputs:		none
Outputs:	none
Description:Triggers the TRIG pin of the ultrasonic sensor
******************************************************************** */
 void triggerUltrasonic()
 {
		/* trigger ultrasonic */
		U_Port_trig |= (1<<U_TRIG_pin);
		_delay_us(15);

		/* stop trigger ultrasonic */
		U_Port_trig &= ~(1<<U_TRIG_pin);
		_delay_ms(20);
 }

/* *****************************************************************
Name:		getDistance()
Inputs:		none
Outputs:	measured distance
Description:converts the pulse length to measurements in cm
******************************************************************** */
float getDistance()
{
	/* factor to be multiplied by pulse
	f = speedOfSound (343 m/s) * 100 * 8 (prescale factor) / 2 (way-back) / F_CPU;  ( ~0.008575 ); */
	/* distance = pulse * factor or
	distance = pulse / (1/factor) */
	return pulse / 116.618;
}

// #endif /* ULTRASONIC_H_ */


volatile uint8_t iIRC = 0;
volatile int f_wdt = 1;
ISR(INT0_vect)
{
  // Serial.println("INT0 triggered");
	switch (iIRC)
	{
		case 0: //when logic changes from LOW to HIGH
		{
			iIRC = 1;
			TCCR1B |= (1<<CS11);
			break;
		}
		case 1:
		{
			/* reset iIRC */
			iIRC = 0;
			/* stop counter */
			TCCR1B &= ~(1<<CS11);
			/* assign counter value to pulse */
			pulse = TCNT1;
			/* reset counter */
			TCNT1=0;
			break;
		}
	}
}

/* *****************************************************************
Name:		Watchdog Interrupt
Inputs:		none
Outputs:	f_wdt
Description:wakes up processor after internal timer limit reached (8 sec)
******************************************************************** */
ISR(WDT_vect)
{
	/* set the flag. */
	if(f_wdt == 0)
	{
		f_wdt = 1;
	}
	//else there is an error -> flag was not cleared
}