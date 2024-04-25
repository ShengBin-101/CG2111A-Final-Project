// Color Sensor Code is done in baremetal!

// define colors
#define WHITE 0
#define RED 1
#define GREEN 2

// TCS230 or TCS3200 pins wiring to Arduino
#define S0 DDL7 // PL7
#define S1 DDL5 // PL5
#define S2 DDL3 // PL3
#define S3 DDL1 // PL1
#define sensorOut DDD1  // 20 pin is used for INT1
#define VECT_INCR 100 //the number of vector increments for direction vector

//****************Variables************************

// Stores frequency read by the photodiodes
float redFrequency = 0;
float greenFrequency = 0;
float blueFrequency = 0;

int lower_red = 0;
int upper_red = 0;
int lower_green = 0;
int upper_green = 0;
int lower_blue = 0;
int upper_blue = 0;
bool calibrated = 0; // 0 - Not Calibrated, 1 - Calibrated


float getAvgReading(int iterations) {
  delay(1000);
  int colSum = 0;
  for (int i = 0; i < iterations; i += 1) {
    int colFreq = pulseIn(sensorOut, LOW); // actly returns period in microseconds
    colSum += colFreq;
  }
  return float(colSum) / iterations;
}

void setupColor() {
  // Setting the outputs
  DDRL |= (1 << S0) | (1 << S1) | (1 << S2) | (1 << S3);
  
  // Setting the sensorOut as an input
  DDRD &= ~(1 << sensorOut);
  
  // Setting frequency scaling to 20%
  PORTL |= (1 << S0);
  PORTL &= ~(1 << S1);
}

int read_red() {
    long frequency;
    PORTL &= ~(1 << S2);
    PORTL &= ~(1 << S3);

    frequency = getAvgReading(5);
    if (!calibrated){
        return map(frequency, 0, 255, 255, 0);
    }
    return map(frequency, lower_red, upper_red, 255, 0);
}

int read_green() {
   long frequency;
    PORTL |= (1 << S2);
    PORTL |= (1 << S3);

    frequency = getAvgReading(5);
    if (!calibrated){
        return map(frequency, 0, 255, 255, 0);
    }
    return map(frequency, lower_green, upper_green, 255, 0);
    
}

int read_blue() {
    long frequency;
    
    PORTL &= ~(1 << S2);
    PORTL |= (1 << S3);

    frequency = getAvgReading(5);
    if (!calibrated){
        return map(frequency, 0, 255, 255, 0);
    }
    return map(frequency, lower_blue, upper_blue, 255, 0);
}

void getColor() {
  int red, green, blue;
  red = read_red();
  green = read_green();
  blue = read_blue();

  //TODO: Add mapping
  Serial.print("Red: ");
  Serial.println(red);
  Serial.print("Green: ");
  Serial.println(green);
  Serial.print("Blue: ");
  Serial.println(blue);
  
}

void calibrate_black(){
    uint8_t r = 0, g = 0, b = 0, c = 0;

    delay(50);
    
    r = read_red();
    g = read_green();
    b = read_blue();

    upper_red = r;
    upper_green = g;
    upper_blue = b;
}

void calibrate_white(){
    uint8_t r = 0, g = 0, b = 0, c = 0;
    delay(50);
    
    r = read_red();
    g = read_green();
    b = read_blue();

    lower_red = r;
    lower_green = g;
    lower_blue = b;
}

void calibrate() {
  // calibrate white
  Serial.println("Prepare White Paper.");
  delay(5000);
  Serial.println("Calibrating White");
  calibrate_white();
  Serial.println("Prepare Black Paper");
  delay(5000);
  Serial.println("Calibrating Black");
  calibrate_black();
  Serial.println("Done Calibrating");
  delay(2000);
  
  // calibrate black
  calibrated = 1;
}
