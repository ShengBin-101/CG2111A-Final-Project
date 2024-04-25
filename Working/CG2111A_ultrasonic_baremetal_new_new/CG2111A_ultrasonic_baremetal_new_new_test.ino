

// This is based of of CG2111A_ultrasonic_baremetal_2, https://github.com/Ovidiu22/HC-SR04/blob/main/src/Ultrasonic.c


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include <stdlib.h>
#include <stdio.h>

#define trigPin PINB4
#define echoPin PIND2
#define powerPin PINB5

uint16_t volatile pulse;


void configPorts() {
  // config the echo pin
  DDRD &= ~(1<<echoPin); // set echoPin (PD2) as input
  PORTD &= ~(1<<echoPin); // ensure that PD2 is LOW
  // config the trig pin
  DDRB |= (1<<trigPin); // set trigPin (PB4) as output
  PORTB &= ~(1<<trigPin); // set trigPin to LOW (for now)
  // config the power pin
  DDRB |= (1<<powerPin);
  PORTB &= ~(1<<powerPin); // set powerPin to LOW (for now)
}

void enableInterrupts() {
  EIMSK |= (1<<INT0); // INT0 is enabled
	EICRA |= (1<<ISC00); // INT0 is triggered on any logical change 
	sei();
}

uint8_t getDistance_main() {
  uint8_t distance = 0;
  float dist = 0;
  float sum = 0;
  PORTB |= (1<<powerPin); // switch on powerPin
  _delay_ms(150);

  for (uint8_t j = 0; j < 2; j++) {
    triggerUltrasonic();
    sum += getDistance();
    _delay_ms(50);
  }
  dist = sum/2;
	distance = dist;
  return distance;
}

void triggerUltrasonic() {
  PORTB |= (1<<trigPin); // write HIGH to trigPin, send pulse
  _delay_us(15);
  PORTB &= ~(1<<trigPin); // write LOW to trigPin, stop pulse
  _delay_ms(20);
}

float getDistance() {
  return pulse / 116.618;
}

uint8_t getPulseMicroseconds() {
  PORTB |= (1<<powerPin); // switch on powerPin
  _delay_ms(150);
  triggerUltrasonic();
  _delay_ms(50);
  // f = speedOfSound (343 m/s) * 100 * 8 (prescale factor) / 2 (way-back) / F_CPU;  ( ~0.008575 );
  // distance = pulse * factor
  return pulse * 100 * 8 / 2 / (1 / 16000000);
}

volatile uint8_t iIRC = 0;
ISR(INT0_vect)
{
  // Serial.println("INT0 triggered");
	switch (iIRC)
	{
		case 0: //when logic changes from LOW to HIGH
		{
			iIRC = 1;
			TCCR1B |= (1<<CS11); // start timer 1, prescaler of 8
			break;
		}
		case 1:
		{
			iIRC = 0; // reset iIRC flag
			TCCR1B &= ~(1<<CS11); // stop timer 1
			pulse = TCNT1; // store TCNT1 counter value into pulse
			TCNT1=0; // reset TCNT1 counter
			break;
		}
	}
}
