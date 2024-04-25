int main(void)
{
    cli();
    setupUltrasonic();
    sei();
    Serial.begin(9600);
    while (1)
    {
        Serial.println(getDistance()); 
        _delay_ms(500);   
    }
}
