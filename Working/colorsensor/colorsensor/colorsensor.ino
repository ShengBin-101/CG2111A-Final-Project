/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// TCS230 or TCS3200 pins wiring to Arduino
#define S0 42
#define S1 44
#define S2 46
#define S3 48
#define sensorOut 20

// Stores frequency read by the photodiodes
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

// Store lowest and highest frequency values for each colour
// Min value is taken with colour sensor touching the surface
// Max value is taken with base of colour sensor ~10cm from surface
int redFrequencyMin[3] = { 154, 404, 309};
int redFrequencyMax[3] = { 406, 760, 586};
int greenFrequencyMin[3] = { 290, 200, 239};
int greenFrequencyMax[3] = { 603, 524, 502};
int whiteFrequencyMin[3] = { 84, 84, 66};
int whiteFrequencyMax[3] = { 295, 347, 295 };

int getAvgReading(int iterations) {
  int colSum = 0;
  for (int i = 0; i < iterations; i += 1) {
    int colFreq = pulseIn(sensorOut, LOW); // actly returns period in microseconds
    colSum += colFreq;
  }
  return int(round(float(colSum) / iterations));
}


void setup() {
  // Setting the outputs
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  
  // Setting the sensorOut as an input
  pinMode(sensorOut, INPUT);
  
  // Setting frequency scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);
  
   // Begins serial communication 
  Serial.begin(9600);
}
void loop() {
  // Setting RED (R) filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  

  // Reading the output frequency
  redFrequency = getAvgReading(5);
   // Printing the RED (R) value
  Serial.print("R = ");
  Serial.print(redFrequency);
  delay(100);
  
  // Setting GREEN (G) filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  
  // Reading the output frequency
  greenFrequency = getAvgReading(5);
  
  // Printing the GREEN (G) value  
  Serial.print(" G = ");
  Serial.print(greenFrequency);
  delay(100);
 
  // Setting BLUE (B) filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  
  // Reading the output frequency
  blueFrequency = getAvgReading(5);
  
  // Printing the BLUE (B) value 
  Serial.print(" B = ");
  Serial.println(blueFrequency);
  delay(100);
}