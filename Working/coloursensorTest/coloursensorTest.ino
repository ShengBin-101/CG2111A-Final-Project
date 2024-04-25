#define TCS3200_COLOR_RED 1
#define TCS3200_COLOR_GREEN 2
#define TCS3200_COLOR_BLUE 3
#define TCS3200_COLOR_CLEAR 4

uint8_t s0_pin = 42;
uint8_t s1_pin = 44;
uint8_t s2_pin = 46;
uint8_t s3_pin = 48;
uint8_t out_pin = 20;

long min_r, min_g, min_b;
long max_r, max_g, max_b;

int integration_time = 3000;
float freqeuency_scaling = 2.0;
bool is_calibrated = false;



void begin_sensor() {
  pinMode(s0_pin, OUTPUT);
  pinMode(s1_pin, OUTPUT);
  pinMode(s2_pin, OUTPUT);
  pinMode(s3_pin, OUTPUT);
  pinMode(out_pin, INPUT);

  // 20% Freq Scaling
  digitalWrite(s0_pin, HIGH);
  digitalWrite(s1_pin, LOW);
}


void select_filter(uint8_t filter) {
    switch(filter) {
        case TCS3200_COLOR_RED:
            digitalWrite(s2_pin, LOW);
            digitalWrite(s3_pin, LOW);
            break;
        case TCS3200_COLOR_GREEN:
            digitalWrite(s2_pin, HIGH);
            digitalWrite(s3_pin, HIGH);
            break;
        case TCS3200_COLOR_BLUE:
            digitalWrite(s2_pin, LOW);
            digitalWrite(s3_pin, HIGH);
            break;
        case TCS3200_COLOR_CLEAR:
            digitalWrite(s2_pin, HIGH);
            digitalWrite(s3_pin, LOW);
            break;
    }
}

long read_red() {
    select_filter(TCS3200_COLOR_RED);

    long red = pulseIn(out_pin, LOW);

    return red;
}

long read_green() {
    select_filter(TCS3200_COLOR_GREEN);

    long green = pulseIn(out_pin, LOW);
    
    return green;
    
}

long read_blue() {
    select_filter(TCS3200_COLOR_BLUE);

    long blue = pulseIn(out_pin, LOW);
    
    return blue;
    
}

uint8_t read_clear() {
    select_filter(TCS3200_COLOR_CLEAR);
    return pulseIn(out_pin, LOW);
}

void calibrate() {
  Serial.println("Please face the sensor to light surface.");
  delay(1000);
  Serial.println("Starting white calibration");
  calibrate_light();
  Serial.println("Please face the sensor to dark surface.");
  delay(2000);
  Serial.println("Starting black calibration");
  calibrate_dark();
  Serial.println("Done calibration");
  is_calibrated = true;
}

void calibrate_light() {
    long r = 0, g = 0, b = 0, c = 0;

    delay(integration_time / 2);
    for(int i = 0; i < 10; i++) {
        r += read_red();
        g += read_green();
        b += read_blue();

        delay(integration_time / 10);
    }

    min_r = (long) (r / 10);
    min_g = (long) (g / 10);
    min_b = (long) (b / 10);
}

void calibrate_dark() {
    long r = 0, g = 0, b = 0, c = 0;

    delay(integration_time / 2);
    for(int i = 0; i < 10; i++) {
        r += read_red();
        g += read_green();
        b += read_blue();
        c += read_clear();

        delay(integration_time / 10);
    }
    max_r = (long) (r / 10);
    max_g = (long) (g / 10);
    max_b = (long) (b / 10);
}

void read_color() {
  long redFreq = read_red();
  Serial.print("redFreq: ");
  Serial.print(redFreq);
  long RedVal = map(redFreq, min_r, max_r, 255, 0);
  Serial.print(" RedVal: ");
  Serial.println(RedVal);
  long greenFreq = read_green();
  Serial.print("greenFreq: ");
  Serial.print(greenFreq);
  long GreenVal = map(greenFreq, min_g, max_g, 255, 0);
  Serial.print(" GreenVal: ");
  Serial.println(GreenVal);
  long blueFreq = read_blue();
  Serial.print("blueFreq: ");
  Serial.print(blueFreq);
  long BlueVal = map(blueFreq, min_b, max_b, 255, 0);
  Serial.print(" BlueVal: ");
  Serial.println(BlueVal);

  Serial.println(" ");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  begin_sensor();
  calibrate();

}

void loop() {
  // put your main code here, to run repeatedly:
  read_color();
  delay(2000);
}
