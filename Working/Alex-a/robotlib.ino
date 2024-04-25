  #include <AFMotor.h>

// Motor control
#define FRONT_LEFT 2  // M4 on the driver shield
#define FRONT_RIGHT 3 // M1 on the driver shield
#define BACK_LEFT 1   // M3 on the driver shield
#define BACK_RIGHT 4  // M2 on the driver shield
#define FB_SPEED 70
#define FB_DELAY 170 
#define LR_SPEED 100
#define LR_DELAY 110  // 500 gives 90 degrees turn
AF_DCMotor motorFL(FRONT_LEFT);
AF_DCMotor motorFR(FRONT_RIGHT);
AF_DCMotor motorBL(BACK_LEFT);
AF_DCMotor motorBR(BACK_RIGHT);

void move(float speed, int direction)
{
  int speed_scaled = (speed / 100.0) * 255;
  motorFL.setSpeed(speed_scaled);
  motorFR.setSpeed(speed_scaled);
  motorBL.setSpeed(speed_scaled);
  motorBR.setSpeed(speed_scaled);

  switch (direction)
  {
  case BACK:
    motorFL.run(BACKWARD);
    motorFR.run(BACKWARD);
    motorBL.run(FORWARD);
    motorBR.run(FORWARD);
    break;
  case GO:
    motorFL.run(FORWARD);
    motorFR.run(FORWARD);
    motorBL.run(BACKWARD);
    motorBR.run(BACKWARD);
    break;
  case CW:
    motorFL.run(BACKWARD);
    motorFR.run(FORWARD);
    motorBL.run(FORWARD);
    motorBR.run(BACKWARD);
    break;
  case CCW:
    motorFL.run(FORWARD);
    motorFR.run(BACKWARD);
    motorBL.run(BACKWARD);
    motorBR.run(FORWARD);
    break;
  case STOP:
  default:
    motorFL.run(STOP);
    motorFR.run(STOP);
    motorBL.run(STOP);
    motorBR.run(STOP);
  }
}

void forward(float dist, float speed)
{
  dir = (TDirection)FORWARD;
  if (manualMode)
  {
    if (dist > 0)
      deltaDist = dist;
    else
      deltaDist = 9999999;
    newDist = forwardDist + deltaDist;
    move(speed, FORWARD);
  }
  else
  {
    move(FB_SPEED, FORWARD);
    delay(FB_DELAY);
    stop();
  }
}

void backward(float dist, float speed)
{
  dir = (TDirection)BACKWARD;
  if (manualMode)
  {
    if (dist > 0)
      deltaDist = dist;
    else
      deltaDist = 9999999;
    newDist = reverseDist + deltaDist;
    move(speed, BACKWARD);
  }
  else
  {
    move(FB_SPEED, BACKWARD);
    delay(FB_DELAY);
    stop();
  }
}

void ccw(float dist, float speed)
{
  dir = (TDirection)CCW;
  if (manualMode)
  {
    move(speed, CCW);
  }
  else
  {
    move(LR_SPEED, CCW);
    delay(LR_DELAY);
    stop();
  }
}

void cw(float dist, float speed)
{
  dir = (TDirection)CW;
  if (manualMode)
  {
    move(speed, CW);
  }
  else
  {
    move(LR_SPEED, CW);
    delay(LR_DELAY);
    stop();
  }
}

void stop()
{
  move(0, STOP);
}
