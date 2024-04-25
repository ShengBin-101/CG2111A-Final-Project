
uint8_t distance = 0;
uint8_t duration = 0;
uint8_t iteration = 0;

void setup() {
  Serial.begin(9600);
	configPorts();
  enableInterrupts();
	
}

void loop() {
  iteration += 1;
		/* Ultrasonic sensor */
		// distance = getDistance_main();
    duration = getPulseMicroseconds();
    // _delay_ms(50);
		Serial.println(duration);
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