/*Arduino speaker song tutorial
* This program will play the theme song of the Malayalam movie
* 'Ennu Ninte Moideen'. The song is 'Mukkathe Penne'.
* The song is played on the speaker connected to pin 3 and GND.
* 
* Created 26 Oct 2015
* by Akshay James
* Video at https://www.youtube.com/watch?v=LgtcUxe8fmA
*/
#define BUZZER 33

unsigned int C[8] = {0, 0, 0, 131, 262, 524, 1048, 2096};
unsigned int Cs[8] = {0, 0, 0, 139, 277, 554, 1108, 2217};
unsigned int D[8] = {0, 0, 0, 147, 294, 588, 1176, 2352};
unsigned int Ds[8] = {0, 0, 0, 156, 311, 622, 1244, 2489};
unsigned int E[8] = {0, 0, 0, 165, 330, 660, 1320, 2640};
unsigned int F[8] = {0, 0, 0, 175, 350, 700, 1400, 2800};
unsigned int Fs[8] = {0, 0, 0, 185, 370, 740, 1480, 2960};
unsigned int G[8] = {0, 0, 0, 196, 392, 784, 1568, 3136};
unsigned int Gs[8] = {0, 0, 0, 208, 415, 831, 1661, 3322};
unsigned int A[8] = {0, 0, 0, 220, 440, 880, 1760, 3520};
unsigned int As[8] = {0, 0, 0, 233, 466, 932, 1865, 3729};
unsigned int B[8] = {0, 0, 0, 247, 494, 988, 1976, 3952};

void low_hp() {
  int speed=40;  //higher value, slower notes
  int no_of_beats = 100;

  unsigned int frequency[no_of_beats] = {
    0, Ds[5], As[4]
    };
  
  int noteDurations[] = {
    8, 4, 4
  };
  for (int thisNote = 0; thisNote < 15; thisNote += 1) {

    int noteDuration = speed*noteDurations[thisNote % 3];
    tone(BUZZER, frequency[thisNote % 3], noteDuration*.95);
    delay(noteDuration);
    noTone(BUZZER);
  }
}

void start_tone() {
  int speed = 20;
  int no_of_beats = 100;
  unsigned int frequency[no_of_beats] = {
    A[5], Gs[5], G[5], Fs[5], A[5], E[5], F[5], E[5], A[5], Ds[5], E[5], Ds[5], 
    A[5], D[5], Ds[5], D[5], A[5], Cs[5], D[5], Cs[5], A[5], C[5], Cs[5], C[5], 
    A[5], B[4], C[5], B[4], A[5], As[4], B[4], As[4], B[5], B[5], B[5], B[5]
   };
  
  for (int thisNote = 0; thisNote < 40; thisNote += 1) {

    int noteDuration = speed*4;
    tone(BUZZER, frequency[thisNote], noteDuration*.95);
    delay(noteDuration);
    noTone(BUZZER);
  }
  
}

void celebrate() {
  int speed=60;  //higher value, slower notes
  int no_of_beats = 100;

  unsigned int frequency[no_of_beats] = {
    C[5], G[4], C[5], G[5], F[5], E[5], D[5], B[4], 
    B[4], G[4], B[4], E[5], D[5], B[4], C[5], E[5],
    C[5], G[4], C[5], G[5], F[5], E[5], D[5], B[4],
    B[4], G[4], B[4], E[5], D[5], B[4], C[5], C[5], 
    E[5], F[5], G[5], 
    E[5], G[5], F[5], G[5], F[5], E[5], D[5], 
    B[4], D[5], E[5], F[5], E[5], D[5], C[5], 
    E[5], G[5], F[5], E[5], F[5], G[5], A[5], 
    G[5], F[5], E[5], F[5], 
    E[5], F[5], E[5], D[5], C[5],
    };
  
  int noteDurations[] = {
    4, 4, 4, 8, 8, 4, 4, 32, 
    4, 4, 4, 8, 8, 4, 4, 32, 
    4, 4, 4, 8, 8, 4, 4, 32, 
    4, 4, 4, 8, 8, 4, 4, 16, 
    4, 4, 8, 
    16, 16, 4, 4, 4, 4, 16, 
    16, 16, 4, 4, 4, 4, 16, 
    16, 16, 4, 4, 4, 4, 16, 
    8, 4, 4, 16, 
    4, 4, 4, 4, 16
  };
  for (int thisNote = 0; thisNote < 65; thisNote += 1) {

    int noteDuration = speed*noteDurations[thisNote];
    tone(BUZZER, frequency[thisNote], noteDuration*.95);
    delay(noteDuration);
    noTone(BUZZER);
  }
}

void caught() {
  int speed=90;  //higher value, slower notes
  int no_of_beats = 100;

  unsigned int frequency[no_of_beats] = {
    A[5], F[5], C[5], 
    As[5], As[5], As[5], G[5], As[5], A[5]
    };
  
  int noteDurations[] = {
    4, 4, 8, 
    1, 1, 2, 3, 1, 8
  };
  for (int thisNote = 0; thisNote < 9; thisNote += 1) {

    int noteDuration = speed*noteDurations[thisNote];
    tone(BUZZER, frequency[thisNote], noteDuration*.95);
    delay(noteDuration);
    noTone(BUZZER);
  }
}

void tokyo_drift() {
  int speed=115;  //higher value, slower notes
  int no_of_beats = 100;

  unsigned int frequency[] = {
    As[5], 0, As[5], 0, As[5], 0, As[5], 0,
    As[5], B[5], Ds[6], As[5], 0, As[5], 0,
    As[5], B[5], Ds[6], As[5], 0, As[5], 0,
    As[5], B[5], Ds[6], As[5], 0, As[5], 0,
    As[5], B[5], Ds[6], As[5], 0, As[5], 0,
    };
  
  int noteDurations[] = { 
    2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 2, 2, 2, 2, 2,  
    3, 3, 2, 2, 2, 2, 2, 
    3, 3, 2, 2, 2, 2, 2, 
    3, 3, 2, 2, 2, 2, 2
  };

  for (int thisNote = 0; thisNote < 29; thisNote += 1) {
    int noteDuration = speed*noteDurations[thisNote];
    tone(BUZZER, frequency[thisNote], noteDuration*.95);
    delay(noteDuration);
    noTone(BUZZER);
  }
}