/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// define colors
#define WHITE 0
#define RED 1
#define GREEN 2

// TCS230 or TCS3200 pins wiring to Arduino
#define S0 42
#define S1 44
#define S2 46
#define S3 48
#define sensorOut 20
#define VECT_INCR 100 //the number of vector increments for direction vector

//****************Variables************************

// Stores frequency read by the photodiodes
float redFrequency = 0;
float greenFrequency = 0;
float blueFrequency = 0;


// Max and min RGB readings for each colour
// Max distance is ~10cm from surface of coloured paper
float whiteFrequencyMin[3] = {118, 114, 91};
float whiteFrequencyMax[3] = {453, 461, 364};
float redFrequencyMin[3] = {223, 458, 355};
float redFrequencyMax[3] = {552, 723, 558};
float greenFrequencyMin[3] = {300, 214, 241};
float greenFrequencyMax[3] = {661, 586, 514};

float* colorFreqMin[3] = {whiteFrequencyMin, redFrequencyMin, greenFrequencyMin};
float* colorFreqMax[3] = {whiteFrequencyMax, redFrequencyMax, greenFrequencyMax};


float colorDirVect[3][3];

float colorTestArray[3];


// store the distances of test color from sample colors in this array
float colorDist[3] = {0};

//********************************************************

//return distance between test point and stored color point
float color_dist(float colourFrequencyMin[3], float colourDirVect[3], float testArray[3]) {
  float colourMainVect[3];
  float calc_dist = 0;
  float min_dist = 0;
  for (int i = 0; i < VECT_INCR; i += 1) {
    // add the direction vector to the main vector
    for (int j = 0; j < 3; j += 1) { 
      colourMainVect[j] = colourFrequencyMin[j] + i * colourDirVect[j];
    }
    
    float sum = 0;
    float dist_squared[3] = {0, 0, 0};
    for (int i = 0; i < 3; i += 1) {
      dist_squared[i] = colourMainVect[i] - testArray[i];
      dist_squared[i] *= dist_squared[i]; // square the distance value
      sum += dist_squared[i];
    }
    
    calc_dist = sqrt((double) sum);
    if (min_dist < 1 || calc_dist < min_dist) {
      min_dist = calc_dist;
    }
  }
//   Serial.print("returning color dist: ");
//   Serial.println(min_dist);
  return min_dist;
}

float getAvgReading(int iterations) {
  int colSum = 0;
  for (int i = 0; i < iterations; i += 1) {
    int colFreq = pulseIn(sensorOut, LOW); // actly returns period in microseconds
    colSum += colFreq;
  }
  return float(colSum) / iterations;
}

void setupColor() {
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

  // vector eqtn of red = redFrequencyMin - t
  for (int i = WHITE; i < 3; i += 1) {
    for (int j = 0; j < 3; j += 1) {
      colorDirVect[i][j] = (colorFreqMax[i][j] - colorFreqMin[i][j]) / VECT_INCR;
    }
  }
}


int getColor() {
  // Setting RED (R) filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  
  redFrequency = getAvgReading(5);
  colorTestArray[0] = redFrequency;
  
  // Setting GREEN (G) filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  
  // Reading the output frequency
  greenFrequency = getAvgReading(5);
  colorTestArray[1] = greenFrequency;
 
  // Setting BLUE (B) filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  
  // Reading the output frequency
  blueFrequency = getAvgReading(5);
  colorTestArray[2] = blueFrequency;
 
  // K-nearest neighbours
  for (int i = WHITE; i < 3; i += 1) { // start looping from WHITE (i.e. 0)
    colorDist[i] = color_dist(colorFreqMin[i], colorDirVect[i], colorTestArray);  
  };

  float minDist = colorDist[WHITE];
  int closestColor = WHITE;
  for (int color = WHITE; color < 3; color += 1) {
    if (colorDist[color] < minDist) {
      minDist = colorDist[color];
      closestColor = color;
    }
  }

  return closestColor;
}
