#include <serialize.h>
#include <stdarg.h> // week9studio1 activity 0
#include <math.h>

#include "packet.h"
#include "constants.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"

volatile TDirection dir;
bool manualMode = true;

/*
 *    Alex's State Variables
 */

/*
   Alex's configuration constants
*/
#define ALEX_LENGTH 25.7
#define ALEX_BREADTH 15.0

#define PI 3.141592654
#define COUNTS_PER_REV 4.0
// Number of ticks per revolution from the
// wheel encoder.
// We will use this to calculate forward/backward distance traveled
// by taking revs * WHEEL_CIRC

#define WHEEL_CIRC 21.3628300444

// #define BAUD 9600
// #define SIZE 200

float alexDiagonal = 0.0;
float alexCirc = 0.0;

// right encoders.
volatile unsigned long leftForwardTicks;
volatile unsigned long rightForwardTicks;
volatile unsigned long leftReverseTicks;
volatile unsigned long rightReverseTicks;

// Left and right encoder ticks for turning
volatile unsigned long leftForwardTicksTurns;
volatile unsigned long rightForwardTicksTurns;
volatile unsigned long leftReverseTicksTurns;
volatile unsigned long rightReverseTicksTurns;

// Store the revolutions on Alex's left
// and right wheels
volatile unsigned long leftRevs;
volatile unsigned long rightRevs;

// Forward and backward distance traveled
volatile unsigned long forwardDist;
volatile unsigned long reverseDist;
unsigned long deltaDist;
unsigned long newDist;
unsigned long deltaTicks;
unsigned long targetTicks;

unsigned long computeDeltaTicks(float ang)
{
  unsigned long ticks = (unsigned long)((ang * alexCirc * COUNTS_PER_REV) / (360.0 * WHEEL_CIRC));
  return ticks;
}

void left(float ang, float speed)
{
  if (ang == 0)
    deltaTicks = 9999999;
  else
    deltaTicks = computeDeltaTicks(ang);
  targetTicks = leftReverseTicksTurns + deltaTicks;
  ccw(ang, speed);
}

void right(float ang, float speed)
{
  if (ang == 0)
    deltaTicks = 9999999;
  else
    deltaTicks = computeDeltaTicks(ang);
  targetTicks = rightReverseTicksTurns + deltaTicks;
  cw(ang, speed);
}

/*
 *
 * Alex Communication Routines.
 *
 */
TResult readPacket(TPacket *packet)
{
  // Reads in data from the serial port and
  // deserializes it.Returns deserialized
  // data in "packet".
  char buffer[PACKET_SIZE];
  int len = readSerial(buffer);
  if (len == 0)
    return PACKET_INCOMPLETE;
  else
    return deserialize(buffer, len, packet);
}

void sendStatus()
{
  // Implement code to send back a packet containing key
  // information like leftTicks, rightTicks, leftRevs, rightRevs
  // forwardDist and reverseDist
  // Use the params array to store this information, and set the
  // packetType and command files accordingly, then use sendResponse
  // to send out the packet. See sendMessage on how to use sendResponse.
  //
  TPacket statusPacket;
  statusPacket.packetType = PACKET_TYPE_RESPONSE;
  statusPacket.command = RESP_STATUS;
  statusPacket.params[0] = leftForwardTicks;
  statusPacket.params[1] = rightForwardTicks;
  statusPacket.params[2] = leftReverseTicks;
  statusPacket.params[3] = rightReverseTicks;
  statusPacket.params[4] = leftForwardTicksTurns;
  statusPacket.params[5] = rightForwardTicksTurns;
  statusPacket.params[6] = leftReverseTicksTurns;
  statusPacket.params[7] = rightReverseTicksTurns;
  statusPacket.params[8] = forwardDist;
  statusPacket.params[9] = reverseDist;
  sendResponse(&statusPacket);
}

void sendColor(int color)
{
  TPacket colorPacket;
  colorPacket.packetType = PACKET_TYPE_COLOR_MESSAGE;
  colorPacket.command = RESP_COLOR;
  colorPacket.params[0] = color;
  sendResponse(&colorPacket);
}

void sendDistance(int distance)
{
  TPacket distancePacket;
  distancePacket.packetType = PACKET_TYPE_ULTRASONIC_MESSAGE;
  distancePacket.command = RESP_ULTRASONIC;
  distancePacket.params[0] = distance;
  sendResponse(&distancePacket);
}

void sendMode(bool manualMode)
{
  TPacket modePacket;
  modePacket.packetType = PACKET_TYPE_MANUAL;
  modePacket.command = RESP_MODE;
  modePacket.params[0] = manualMode;
  sendResponse(&modePacket);
}

void sendBuzzer()
{
  TPacket buzzerPacket;
  buzzerPacket.packetType = PACKET_TYPE_BUZZER;
  buzzerPacket.command = RESP_BUZZER;
  sendResponse(&buzzerPacket);
}

void sendMessage(const char *message)
{
  // Sends text messages back to the Pi. Useful
  // for debugging.

  TPacket messagePacket;
  messagePacket.packetType = PACKET_TYPE_MESSAGE;
  strncpy(messagePacket.data, message, MAX_STR_LEN);
  sendResponse(&messagePacket);
}

void dbprintf(char *format, ...)
{
  va_list args;
  char buffer[128];
  va_start(args, format);
  vsprintf(buffer, format, args);
  sendMessage(buffer);
}

void sendBadPacket()
{
  // Tell the Pi that it sent us a packet with a bad
  // magic number.

  TPacket badPacket;
  badPacket.packetType = PACKET_TYPE_ERROR;
  badPacket.command = RESP_BAD_PACKET;
  sendResponse(&badPacket);
}

void sendBadChecksum()
{
  // Tell the Pi that it sent us a packet with a bad
  // checksum.

  TPacket badChecksum;
  badChecksum.packetType = PACKET_TYPE_ERROR;
  badChecksum.command = RESP_BAD_CHECKSUM;
  sendResponse(&badChecksum);
}

void sendBadCommand()
{
  // Tell the Pi that we don't understand its
  // command sent to us.

  TPacket badCommand;
  badCommand.packetType = PACKET_TYPE_ERROR;
  badCommand.command = RESP_BAD_COMMAND;
  sendResponse(&badCommand);
}

void sendBadResponse()
{
  TPacket badResponse;
  badResponse.packetType = PACKET_TYPE_ERROR;
  badResponse.command = RESP_BAD_RESPONSE;
  sendResponse(&badResponse);
}

void sendOK()
{
  TPacket okPacket;
  okPacket.packetType = PACKET_TYPE_RESPONSE;
  okPacket.command = RESP_OK;
  sendResponse(&okPacket);
}

void sendResponse(TPacket *packet)
{
  // Takes a packet, serializes it then sends it out
  // over the serial port.
  char buffer[PACKET_SIZE];
  int len;
  len = serialize(buffer, packet, sizeof(TPacket));
  writeSerial(buffer, len);
}

/*
 * Setup and start codes for external interrupts and
 * pullup resistors.
 *
 */
// Enable pull up resistors on pins 18 and 19
void enablePullups()
{
  // Use bare-metal to enable the pull-up resistors on pins
  // 19 and 18. These are pins PD2 and PD3 respectively.
  // We set bits 2 and 3 in DDRD to 0 to make them inputs.
  DDRD &= ~(0b00001100);
  PORTD |= 0b00001100;
}

// By driving it high, the interupt pins as left high, as the encoders turn every quarter, some magnet magic happens which pulls the voltage on
// interrupt pin low, triggering the falling edge interrupt.

// Functions to be called by INT2 and INT3 ISRs.
void leftISR()
{

  if (dir == FORWARD)
  {
    leftForwardTicks++;
    forwardDist = (unsigned long)((float)leftForwardTicks / COUNTS_PER_REV * WHEEL_CIRC);
  }
  if (dir == BACKWARD)
  {
    leftReverseTicks++;
    reverseDist = (unsigned long)((float)leftReverseTicks / COUNTS_PER_REV * WHEEL_CIRC);
  }
  if (dir == LEFT)
  {
    leftReverseTicksTurns++;
  }
  if (dir == RIGHT)
  {
    leftForwardTicksTurns++;
  }
}

void rightISR()
{
  if (dir == FORWARD)
  {
    rightForwardTicks++;
  }
  if (dir == BACKWARD)
  {
    rightReverseTicks++;
  }
  if (dir == LEFT)
  {
    rightForwardTicksTurns++;
  }
  if (dir == RIGHT)
  {
    rightReverseTicksTurns++;
  }
}

// Set up the external interrupt pins INT2 and INT3
// for falling edge triggered. Use bare-metal.
void setupEINT()
{
  // Use bare-metal to configure pins 18 and 19 to be
  // falling edge triggered. Remember to enable
  // the INT2 and INT3 interrupts.
  // Hint: Check pages 110 and 111 in the ATmega2560 Datasheet.
  EICRA = 0b10100000; // 10 is falling edge for INT3 and INT2
  EIMSK = 0b00001100; // Enable interrupts on INT3 and INT2
}

// Implement the external interrupt ISRs below.
// INT3 ISR should call leftISR while INT2 ISR
// should call rightISR.
ISR(INT3_vect)
{
  leftISR();
}

ISR(INT2_vect)
{
  rightISR();
}
// Implement INT2 and INT3 ISRs above.

/*
 * Setup and start codes for serial communications
 */
// Bare-metal Serial Implemented
void setupSerial()
{
  // serial is done in baremetal (see uart.cpp)
  serial.begin(9600);
}


int readSerial(char *buffer)
{
  int count = 0;
  // serial is done in baremetal (see uart.cpp)
  while (serial.available())
    buffer[count++] = serial.read();
  return count;
}

void writeSerial(const char *buffer, int len)
{
  serial.write(buffer, len);
  // Change Serial to Serial2/Serial3/Serial4 in later labs when using other UARTs
}

/*
 * Alex's setup and run codes
 *
 */

// Clears all our counters
void clearCounters()
{
  leftForwardTicks = 0;
  rightForwardTicks = 0;
  leftReverseTicks = 0;
  rightReverseTicks = 0;
  leftForwardTicksTurns = 0;
  rightForwardTicksTurns = 0;
  leftReverseTicksTurns = 0;
  rightReverseTicksTurns = 0;
  leftRevs = 0;
  rightRevs = 0;
  forwardDist = 0;
  reverseDist = 0;
}

// Clears one particular counter
void clearOneCounter(int which)
{
  /**
  switch(which)
  {
    case 0:
      clearCounters();
      break;

    case 1:
      leftForwardTicks=0;
      break;

    case 2:
      rightForwardTicks=0;
      break;

    case 3:
      leftRevs=0;
      break;

    case 4:
      rightRevs=0;
      break;

    case 5:
      forwardDist=0;
      break;

    case 6:
      reverseDist=0;
      break;
  }
  */
  clearCounters();
}
// Intialize Alex's internal states

void initializeState()
{
  clearCounters();
}

void handleCommand(TPacket *command)
{
  switch (command->command)
  {
  // For movement commands, param[0] = distance, param[1] = speed.
  case COMMAND_FORWARD:
    sendOK();
    forward((double)command->params[0], (float)command->params[1]);
    break;
  case COMMAND_REVERSE:
    sendOK();
    backward((double)command->params[0], (float)command->params[1]);
    break;
  case COMMAND_TURN_LEFT:
    sendOK();
    left((double)command->params[0], (float)command->params[1]);
    break;
  case COMMAND_TURN_RIGHT:
    sendOK();
    right((double)command->params[0], (float)command->params[1]);
    break;
  case COMMAND_STOP:
    sendOK();
    stop();
    break;
  case COMMAND_GET_STATS:
    // sendOK();
    sendStatus();
    break;
  case COMMAND_CLEAR_STATS:
    sendOK();
    clearOneCounter(command->params[0]);
    break;

  /*
   * Implement code for other commands here.
   *
   */
  case COMMAND_GET_ULTRASONIC:
  {
    sendOK();
    // execute code for ultrasonic
    uint32_t distance = getDistance();
    sendDistance(distance);
    break;
  }
  case COMMAND_GET_COLOR:
  {
    sendOK();
    // execute code for color
    int color = getColor();
    sendColor(color);
    break;
  }
  case COMMAND_BUZZER_1:  // start_tone
  {
    sendOK();
    start_tone();
    // execute code for buzzer
    sendBuzzer();
    break;
  }
  case COMMAND_BUZZER_2:  // low_hp
  {
    sendOK();
    low_hp();
    // execute code for buzzer
    sendBuzzer();
    break;
  }
  case COMMAND_BUZZER_3:  // caught
  {
    sendOK();
    caught();
    // execute code for buzzer
    sendBuzzer();
    break;
  }
  case COMMAND_BUZZER_4: // celebrate
  {
    sendOK();
    celebrate();
    // execute code for buzzer
    sendBuzzer();
    break;
  }
  case COMMAND_BUZZER_5: // tokyo_drift
  {
    sendOK();
    tokyo_drift();
    // execute code for buzzer
    sendBuzzer();
    break;
  }

  case COMMAND_TOGGLE_MANUAL:
  {
    manualMode = !manualMode; // Flipping this toggles control flow for free teleop and hall effect based movement.
    sendOK();
    if (manualMode){
      dbprintf("manualMode is ON");
    }
    else {
      dbprintf("manualMode is OFF");
    }
    sendMode(manualMode);
    // execute code to toggle between slow and fast speed
    break;
  }
  default:
    sendBadCommand();
  }
}

void waitForHello()
{
  int exit = 0;

  while (!exit)
  {
    TPacket hello;
    TResult result;

    do
    {
      result = readPacket(&hello);
    } while (result == PACKET_INCOMPLETE);

    if (result == PACKET_OK)
    {
      if (hello.packetType == PACKET_TYPE_HELLO)
      {
        sendOK();
        exit = 1;
      }
      else
        sendBadResponse();
    }
    else if (result == PACKET_BAD)
    {
      sendBadPacket();
    }
    else if (result == PACKET_CHECKSUM_BAD)
      sendBadChecksum();
  } // !exit
}

void setup()
{
  // put your setup code here, to run once:
  cli();
  setupEINT();
  setupColor(); 
  setupUltrasonic();
  setupSerial();
  enablePullups();
  initializeState();
  sei();
  waitForHello();
  alexDiagonal = sqrt((ALEX_LENGTH * ALEX_LENGTH) + (ALEX_BREADTH * ALEX_BREADTH));
  alexCirc = PI * alexDiagonal;
  
}

void handlePacket(TPacket *packet)
{
  switch (packet->packetType)
  {
  case PACKET_TYPE_COMMAND:
    handleCommand(packet);
    break;

  case PACKET_TYPE_RESPONSE:
    break;

  case PACKET_TYPE_ERROR:
    break;

  case PACKET_TYPE_MESSAGE:
    break;

  case PACKET_TYPE_HELLO:

    break;
  }
}

void loop()
{
  TPacket recvPacket; // This holds commands from the Pi

  TResult result = readPacket(&recvPacket);

  if (result == PACKET_OK)
    handlePacket(&recvPacket);
  else if (result == PACKET_BAD)
  {
    sendBadPacket();
  }
  else if (result == PACKET_CHECKSUM_BAD)
  {
    sendBadChecksum();
  }

  if (manualMode)
  {
    // Manual Forward/Backward Movement
    if (deltaDist > 0)
    {
      if (dir == FORWARD)
      {
        if (forwardDist > newDist)
        {
          deltaDist = 0;
          newDist = 0;
          stop();
        }
      }
      else if (dir == BACKWARD)
      {
        if (reverseDist > newDist)
        {
          deltaDist = 0;
          newDist = 0;
          stop();
        }
      }
      else if (dir == STOP)
      {
        deltaDist = 0;
        newDist = 0;
        stop();
      }
    }
    // Manual Turning
    if (deltaTicks > 0)
    {
      if (dir == LEFT)
      {
        if (leftReverseTicksTurns >= targetTicks)
        {
          deltaTicks = 0;
          targetTicks = 0;
          stop();
        }
      }
      else if (dir == RIGHT)
      {
        if (rightReverseTicksTurns >= targetTicks)
        {
          deltaTicks = 0;
          targetTicks = 0;
          stop();
        }
      }
      else if (dir == STOP)
      {
        deltaTicks = 0;
        targetTicks = 0;
        stop();
      }
    }
  }
  else
  {
    
    clearCounters();
  }
}
