#include <Joystick.h>
#include <StringSplitter.h>
#include <Wire.h>
#include <WString.h>

Joystick_ Joystick(0x04, 
  JOYSTICK_TYPE_JOYSTICK, 13, 0,
  true, true, false, true, false, false,
  true, true, false, true, false);
int XAxis = 0;
int YAxis = 0;
int rudderL = 200;
int rudderR = 200;
int lastrudderL = 200;
int lastrudderR = 200;
int avgRudderL = 200;
int avgRudderR = 200;

#define CLK 4
#define DT 3
#define SW 2
int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir ="";
unsigned long lastButtonPress = 0;

int parked = 1;
int lastParked = 0;

int PA5 = 0;
int PA1 = 0;
int throttle = 0;

const bool initAutoSendState = true;

//StringSplitter *splitter;

// A0 - Joy Down
// A1 - IR Sensor Throttle Low
// A5 - IR Sensor Throttle High
// 2  - SDA - Red
// 3  - SCL - White
// 4  - Trim Dn
// 5  - Joy Up
// 6  - Joy Right
// 7  - Flaps Up
// 8  - Flaps Dn
// 9  - Joy Left
// 10 - POV
// 11 - Parking Bake
// 12 - Gear Down
// 13 - Trim Up

void setup() {
  Wire.begin();
  Wire.setClock(10000);
  delay(100);
  Serial.begin(9600);
  //while (!Serial){};
  delay(100);
  //pinMode(2, INPUT_PULLUP);  // Not needed I2C
  //pinMode(3, INPUT_PULLUP);  // Not needed I2C
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT);
  pinMode(A5, INPUT);

  Joystick.setXAxisRange(-1, 1);
  Joystick.setYAxisRange(-1, 1);
  Joystick.setBrakeRange(0, 1);
  Joystick.setThrottleRange(-320, 200);
  Joystick.setRxAxisRange(-100, 100);
  Joystick.begin(true);

  /* Rotary Encoder Throttle Setup
  pinMode(CLK,INPUT);
  pinMode(DT,INPUT);
  pinMode(SW, INPUT_PULLUP);
  lastStateCLK = digitalRead(CLK);  */
}

/*void processIncomingByte(const byte inByte)
{
  static unsigned int input_pos = 0;

  switch (inByte) {
    case '\n':   // end of text
      incString [input_pos] = 0;  // terminating null byte
       // reset buffer for next time
      input_pos = 0;  
      break;
    case '\r':   // discard carriage return
      break;
    default:
      // keep adding if not full ... allow for terminating null byte
      if (input_pos < (7))
        incString [input_pos++] = inByte;
      break;
   }
}*/

void loop() {
  // Rudder
  Wire.requestFrom(10, 6);
  if (Wire.available() >= 6) {
    //Serial.println("incoming I2C avalible " + String(Wire.available()));
    int byteNum = 1;
    char RL[4];
    char RR[4];
    RL[3] = "\0";
    RR[3] = "\0";
    while (byteNum <= 6) {
      if (byteNum <= 3) {
        RL[byteNum-1] = Wire.read();
        //Serial.print(String(RL[byteNum-1]));
      } else if (byteNum > 3 && byteNum <= 6) {
        RR[byteNum-4] = Wire.read();
        //Serial.print(String(RR[(byteNum-4)]));
      } else {
        Wire.read();
      }
      byteNum++;
    }

    /*Serial.println();
    Serial.println("End Raw Bytes");
    Serial.print(String(RL));
    Serial.print(F(" "));
    Serial.println(String(RR) + " ");
    Serial.println("End Char Arrays");*/
    
    rudderL = atoi(RL);
    rudderR = atoi(RR);

    avgRudderL = (int)((rudderL + lastrudderL) / 2);
    avgRudderR = (int)((rudderR + lastrudderR) / 2);
    
    //Serial.println("Final: " + String(avgRudderL) + " " + String(avgRudderR));

    if (avgRudderR - avgRudderL > 30 && avgRudderL < 180 && (double)((180 - avgRudderL) - 20) * 2 > 0 && (double)((180 - avgRudderL) - 20) * 2 <= 160) { //left down 20 70
      double tempV = ((180 - avgRudderL) - 20) * 2;
      Joystick.setRxAxis((int)(tempV));
      //Serial.println("Left down " + String((int)(tempV)));
    } else if (avgRudderR - avgRudderL < -25 && avgRudderR < 180 && (double)((180 - avgRudderR) - 10) * 10/4 > 0 && (double)((180 - avgRudderR) - 10) * 10/4 <= 160) { //right down -10 -50
      double tempV = ((180 - avgRudderR) - 10) * 10/4;
      Joystick.setRxAxis((int)(-1 * tempV));
      //Serial.println("Right down " + String((int)(-1 * tempV)));
    } else {
      Joystick.setRxAxis(0);
    }

    lastrudderL = rudderL;
    lastrudderR = rudderR;
  }
  
  /* Rudder - Serial
  if (Serial1.available() > 0) {
    // Read the incoming bytes
    Serial.println(F("incoming serial avalible"));
    strcpy(incString, resetString);
    while(Serial1.available() > 0) {
      processIncomingByte(Serial1.read());
    }
    //Serial.print(F("Got Message: "));
    //Serial.println(incString);

    splitter = new StringSplitter(incString, ';', 2);
    
    for(int i = 0; i < 2; i++){
      char item[3];
      strcpy(item, splitter->getItemAtIndex(i).c_str());
      if (i == 0) {
        rudderL = atoi(item);
      } else {
        rudderR = atoi(item);
      }
    }
    Joystick.setRudder((rudderR - rudderL));
  }
  */

  // Trim Up
  if (digitalRead(13) == LOW) {
    Joystick.pressButton(0);
  } else {
    Joystick.releaseButton(0);
  }

  // Trim Down
  if (digitalRead(4) == LOW) {
    Joystick.pressButton(1);
  } else {
    Joystick.releaseButton(1);
  }

  // Flaps Up
  if (digitalRead(7) == LOW) {
    Joystick.pressButton(5);
  } else {
    Joystick.releaseButton(5);
  }
  
  // Flaps Down
  if (digitalRead(8) == LOW) {
    Joystick.pressButton(6);
  } else {
    Joystick.releaseButton(6);
  }

  // Parking Brake
  if (digitalRead(11) == LOW && parked == 0 && lastParked == 0) {
    Joystick.setBrake(1);
    parked = 1;
    lastParked = 1;
  } else if (digitalRead(11) == HIGH && parked == 0 && lastParked == 1) {
    Joystick.setBrake(1);
    parked = 1;
    lastParked = 0;
  } else if (parked == 1) {
    delay(100);
    parked = 0;
    Joystick.setBrake(0);
  }

  // Gear Down
  if (digitalRead(12) == LOW) {
    Joystick.pressButton(11);
    Joystick.releaseButton(10);
  } else {
    Joystick.pressButton(10);
    Joystick.releaseButton(11);
  }

  // POV
  if (digitalRead(10) == LOW) {
    Joystick.pressButton(8);
    Joystick.releaseButton(7);
  } else {
    Joystick.pressButton(7);
    Joystick.releaseButton(8);
  }

  // Joystick X
  if (digitalRead(9) == LOW) {
    Joystick.setXAxis(-1);
  } else if (digitalRead(6) == LOW) {
    Joystick.setXAxis(1);
  } else {
    Joystick.setXAxis(0);
  }

  // Joystick Y
  if (digitalRead(5) == LOW) {
    Joystick.setYAxis(-1);
  } else if (digitalRead(A0) == LOW) {
    Joystick.setYAxis(1);
  } else {
    Joystick.setYAxis(0);
  }
  
  // Throttle - IR
  PA5 = analogRead(A5);
  PA1 = analogRead(A1);
  throttle = ((PA5 - PA1 + 10/2) / 10) * 10;
  Joystick.setThrottle(throttle);
  //Serial.println(String(PA5) + " " + String(PA1) + " :: " + String(throttle));
  
  /*
  //Throttle - Rotary
  currentStateCLK = digitalRead(CLK);
  if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
    if (digitalRead(DT) != currentStateCLK) {
      if (counter > 0) {
        counter --;
      }
      currentDir ="CCW";
    } else {
      if (counter < 4) {
        counter ++;
      }
      currentDir ="CW";
    }
    
    Joystick.setThrottle(counter);

    Serial.print("Direction: ");
    Serial.print(currentDir);
    Serial.print(" | Counter: ");
    Serial.println(counter);
  }

  lastStateCLK = currentStateCLK;
  int btnState = digitalRead(SW);

  if (btnState == LOW) {
    if (millis() - lastButtonPress > 50) {
      Serial.println("Button pressed!");
    }
    lastButtonPress = millis();
  }
  //End Throttle - Rotary
  */
  
  delay(100);
}
