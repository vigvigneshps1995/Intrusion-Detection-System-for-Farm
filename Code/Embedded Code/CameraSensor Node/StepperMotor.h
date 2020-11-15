#define FORWARD 0
#define BACKWARD 1

class StepperMotor
{

  private:
    int Steps = 0;
    int Direction;
    int currentPosition = 0;
    int cumAngle = 0;
    int INPUT1, INPUT2, INPUT3, INPUT4;
    int stepsPerRevolution = 4096;

    void SetDirection()
    {
      if (Direction == 1)
        Steps++;

      if (Direction == 0)
        Steps--;

      if (Steps > 7)
        Steps = 0;

      if (Steps < 0)
        Steps = 7;
    }

    void stepperStep()
    {
      switch (Steps)
      {
        case 0:
          digitalWrite(INPUT1, LOW);
          digitalWrite(INPUT2, LOW);
          digitalWrite(INPUT3, LOW);
          digitalWrite(INPUT4, HIGH);
          break;
        case 1:
          digitalWrite(INPUT1, LOW);
          digitalWrite(INPUT2, LOW);
          digitalWrite(INPUT3, HIGH);
          digitalWrite(INPUT4, HIGH);
          break;
        case 2:
          digitalWrite(INPUT1, LOW);
          digitalWrite(INPUT2, LOW);
          digitalWrite(INPUT3, HIGH);
          digitalWrite(INPUT4, LOW);
          break;
        case 3:
          digitalWrite(INPUT1, LOW);
          digitalWrite(INPUT2, HIGH);
          digitalWrite(INPUT3, HIGH);
          digitalWrite(INPUT4, LOW);
          break;
        case 4:
          digitalWrite(INPUT1, LOW);
          digitalWrite(INPUT2, HIGH);
          digitalWrite(INPUT3, LOW);
          digitalWrite(INPUT4, LOW);
          break;
        case 5:
          digitalWrite(INPUT1, HIGH);
          digitalWrite(INPUT2, HIGH);
          digitalWrite(INPUT3, LOW);
          digitalWrite(INPUT4, LOW);
          break;
        case 6:
          digitalWrite(INPUT1, HIGH);
          digitalWrite(INPUT2, LOW);
          digitalWrite(INPUT3, LOW);
          digitalWrite(INPUT4, LOW);
          break;
        case 7:
          digitalWrite(INPUT1, HIGH);
          digitalWrite(INPUT2, LOW);
          digitalWrite(INPUT3, LOW);
          digitalWrite(INPUT4, HIGH);
          break;
        default:
          digitalWrite(INPUT1, LOW);
          digitalWrite(INPUT2, LOW);
          digitalWrite(INPUT3, LOW);
          digitalWrite(INPUT4, LOW);
          break;
      }
      SetDirection();
    }

    void stepperMove(float angle, int dir)
    {
      float stepstomove;

      stepstomove = round((angle / 360) * stepsPerRevolution);

      if (dir == FORWARD)
      {
        Direction = FORWARD;
        while (stepstomove > 0)
        {
          stepperStep();
          delay(2);
          stepstomove--;
        }
      }

      if (dir == BACKWARD)
      {
        Direction = BACKWARD;
        while (stepstomove > 0)
        {
          stepperStep();
          delay(2);
          stepstomove--;
        }
      }
    }

  public:
    void attachPins(short in1, short in2, short in3, short in4)
    {
      INPUT1 = in1;
      INPUT2 = in2;
      INPUT3 = in3;
      INPUT4 = in4;
    }

    void stepperPan(int degree)
    {
      int angleforward;
      int anglebackward;
      int stepstomove;

      if ( degree > currentPosition)
      {
        angleforward = degree - currentPosition;
        anglebackward = currentPosition + (360 - degree);
      }

      else if ( degree < currentPosition)
      {
        anglebackward = currentPosition - degree;
        angleforward = (360 - currentPosition) + degree;
      }
      else if (degree == currentPosition)
      {
        angleforward = 0;
        anglebackward = 0;
      }

      if (angleforward < anglebackward)
      {
        if ((cumAngle + angleforward) >= 360 )
        {
          stepperMove(anglebackward, BACKWARD);
          cumAngle = cumAngle - anglebackward;
        }
        else if (cumAngle - anglebackward <= -360 )
        {
          stepperMove(angleforward, FORWARD);
          cumAngle = cumAngle + angleforward;
        }
        else
        {
          stepperMove(angleforward, FORWARD);
          cumAngle = cumAngle + angleforward;
        }
      }
      else if (angleforward > anglebackward)
      {
        if ((cumAngle + angleforward) >= 360)
        {
          stepperMove(anglebackward, BACKWARD);
          cumAngle = cumAngle - anglebackward;
        }
        else if (cumAngle - anglebackward <= -360 )
        {
          stepperMove(angleforward, FORWARD);
          cumAngle = cumAngle + angleforward;
        }
        else
        {
          stepperMove(anglebackward, BACKWARD);
          cumAngle = cumAngle - anglebackward;
        }
      }
      else if (angleforward == anglebackward)
      {
        if ((cumAngle + angleforward) >= 360)
        {
          stepperMove(anglebackward, BACKWARD);
          cumAngle = cumAngle - anglebackward;
        }
        else if (cumAngle - anglebackward <= -360 )
        {
          stepperMove(angleforward, FORWARD);
          cumAngle = cumAngle + angleforward;
        }
      }

      currentPosition = degree;
    }
    void stepperCalibrate(int angle)
    {
      if(angle < 0)
        stepperMove(abs(angle), BACKWARD);
      if(angle > 0)
        stepperMove(abs(angle), FORWARD);
    }
};
