
// #include "TCS3200.h"
// #define S0 42//DDL7 // PL7
// #define S1 44//DDL5 // PL5
// #define S2 46//DDL3 // PL3
// #define S3 48//DDL1 // PL1
// #define sensorOut 20 //DDD1  // 20 pin is used for INT1
// TCS3200 colorSensor(S0, S1, S2, S3, sensorOut);

// void setup() {
//     Serial.begin(9600);
//     colorSensor.begin();
//     // Set integration time to 2500 milliseconds (2.5 seconds)
//     colorSensor.integration_time(2000);
 
//     // Set frequency scaling to 20%
//     colorSensor.frequency_scaling(TCS3200_OFREQ_20P);
//     Serial.println("calibrate white");
//     colorSensor.calibrate_light();
//     delay(2000);
//     Serial.println("calibrate black");
//     colorSensor.calibrate_dark();
//     delay(2000);
//     Serial.println("done");
//     colorSensor.calibrate();
// }

// void loop() {
//     // Read RGB values from the color sensor
//     uint8_t red = colorSensor.read_red();
//     uint8_t green = colorSensor.read_green();
//     uint8_t blue = colorSensor.read_blue();

//     // Print the RGB values to the serial monitor
//     Serial.print("Red: ");
//     Serial.print(red);
//     Serial.print(" Green: ");
//     Serial.print(green);
//     Serial.print(" Blue: ");
//     Serial.println(blue);

//     delay(1000); // Wait for 1 second before reading again
// }
