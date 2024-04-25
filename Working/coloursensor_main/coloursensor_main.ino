
int detected_color;

void setup() {
  Serial.begin(9600);
    setupColor();
    calibrate();
    delay(1000);
}

void loop() {
    getColor();
    delay(2000);
}
