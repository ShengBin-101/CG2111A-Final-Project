// Ultrasounic Sensor done in BareMetal!
#define TRIGGER DDA2  // 24
#define ECHO DDD0     // 21 (Which is also tied to External Interrupt 0)
volatile uint16_t pulse;
volatile uint8_t edge = 0; 
void setupUltrasonic() {
    DDRA |= (1 << TRIGGER); // Set TRIGGER as output
    DDRD &= ~(1 << ECHO);   // Set ECHO as input
    PORTA &= ~(1 << TRIGGER);
    //setup INT0 to measure for FALLING edge
    EICRA |= (1<< ISC00);
    EICRA &= ~(1<< ISC01);
}

float readPulse() {
  return pulse / 2.0f;
}



ISR(INT0_vect)
{
    switch (edge)
    {
      case 0:
        edge = 1;
        TCCR5B |= (1 << CS51);
        break;
      case 1:
        edge = 0;
        TCCR5B &= ~(1 << CS51);
        pulse = TCNT5;
        TCNT5 = 0;
        break;
    }
}

void triggerUltrasonic()
{
  PORTA |= (1 << TRIGGER);
  _delay_us(15);
  PORTA &= ~(1 << TRIGGER);
  _delay_ms(20);
}

float getDistance() {
    float duration;
    EIMSK |= (1 << INT0);
    triggerUltrasonic();
    duration = readPulse();
    EIMSK &= ~(1 << INT0);
    return (duration / 2) * (0.0343f); // in cm
}
