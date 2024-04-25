#ifndef __CONSTANTS_INC__
#define __CONSTANTS_INC__

/* 
 *  This file containts all the packet types, commands
 *  and status constants
 *  
 */

// Packet types
typedef enum
{
  PACKET_TYPE_COMMAND = 0,
  PACKET_TYPE_RESPONSE = 1,
  PACKET_TYPE_ERROR = 2,
  PACKET_TYPE_ULTRASONIC_MESSAGE = 3,
  PACKET_TYPE_HELLO = 4,
  PACKET_TYPE_COLOR_MESSAGE = 5,
  PACKET_TYPE_BUZZER = 6,
  PACKET_TYPE_MESSAGE = 7, // for debugging
  PACKET_TYPE_SPEED = 8,
  PACKET_TYPE_MANUAL = 9,
} TPacketType;

// Response types. This goes into the command field
typedef enum
{
  RESP_OK = 0,
  RESP_STATUS=1,
  RESP_BAD_PACKET = 2,
  RESP_BAD_CHECKSUM = 3,
  RESP_BAD_COMMAND = 4,
  RESP_BAD_RESPONSE = 5,
  RESP_COLOR = 6,
  RESP_ULTRASONIC = 7,
  RESP_MODE = 8,
  RESP_BUZZER = 9,
} TResponseType;


// Commands
// For direction commands, param[0] = distance in cm to move
// param[1] = speed
typedef enum
{
  COMMAND_FORWARD = 0,
  COMMAND_REVERSE = 1,
  COMMAND_TURN_LEFT = 2,
  COMMAND_TURN_RIGHT = 3,
  COMMAND_STOP = 4,
  COMMAND_GET_STATS = 5,
  COMMAND_CLEAR_STATS = 6,
  COMMAND_GET_ULTRASONIC = 7,
  COMMAND_GET_COLOR = 8,
  COMMAND_BUZZER_1 = 9,
  COMMAND_TOGGLE_SPEED = 10,
  COMMAND_BUZZER_2 = 11,
  COMMAND_BUZZER_3 = 12,
  COMMAND_BUZZER_4 = 13,
  COMMAND_BUZZER_5 = 14,
} TCommandType;
#endif

