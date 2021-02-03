/** This project supports the rebuild of my logitech wingman extreme joystick.
     After 20+ years the potentiometers were worn out and needed replacement.
     I elected to use Hall Effect sensors and magnets for the axes instead of
     potentiometers and added a toggle switch as well.

     This project uses the Joystick library from; 
          https://github.com/MHeironimus/ArduinoJoystickLibrary

     I elected to manually scan the button matrix using the example at,
        https://www.baldengineer.com/arduino-keyboard-matrix-tutorial.html
     pretty much verbatim. As stated on that site there are optimizations possible
     but it is working pretty well now.

     As you can see from the code the rudder has been comented out. The magnet to
     sensor orientation needs to be pretty exact and I was unble to
     compensate for the slop in the stick rotation. I also found that the X axis
     still has travel beyond the maximum sensor reading. In the future magnet
     placement needs to be more precise.

     3FEB21 - Kyle Eberhart - First commit. Barred from work due to positive 
        COVID-19 test, though my quarantine has been complete since 26JAN21.
**/

#include <Joystick.h>

// Set up the Joystick Axes
#define X_PIN A0
#define X_MIN 485
#define X_MAX 1015
#define X_MID 772
#define X_TRIM 20
#define X_INVERT 1

#define Y_PIN A1
#define Y_MIN 181
#define Y_MAX 733
#define Y_MID 485
#define Y_TRIM 20
#define Y_INVERT 1

//#define RUD_PIN A2
//#define RUD_MIN 0
//#define RUD_MAX 1023
//#define RUD_MID 512
//#define RUD_TRIM 0
//#define RUD_INVERT 1

#define THROT_PIN A3
#define THROT_MIN 165
#define THROT_MAX 1012
#define THROT_MID 512
#define THROT_TRIM 0
#define THROT_INVERT 1

// Set up the Keypad bits
#define NUMROWS 4
#define NUMCOLS 3

// the pins used for the button rows and columns
byte rowPins[NUMROWS] = {15,14,16,10};
byte colPins[NUMCOLS] = {9,8,7};

byte keys[NUMCOLS][NUMROWS];
byte lastState[] = {0,0,0,0,0,0,0,0,0,0,0,0};
byte currState[] = {0,0,0,0,0,0,0,0,0,0,0,0};

int keymap[] = {0,1,2,3,4,5,6,7,8,9,10,11};
// Joystick button numbers are off by one from the physical buttons
// so button 0 is physical button 1.
int joymap[] = {5,11,6,0,8,3,7,10,4,2,9,1};

boolean switch_toggle;
unsigned long delay_start = 0;

// Initialize the Joystick
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_MULTI_AXIS,
  8, 1,                 // Button Count, Hat Switch Count
  true, true, false,  // X Axis, Y Axis, Z Axis
  false, false, false,  // RX Axis, RY Axis, RZ Axis
  false, true,         // Rudder, Throttle
  false, false, true); // Accellerator, Brake, Steering

int currHatButtonState[] = {0,0,0,0};
bool checkHatState = false;

void setup() {
  Serial.begin(115200);

  // reset the Analog in pins
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);

  // reset the button collumn pins
  pinMode(9, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);

  // reset the button row pins
  pinMode(15, INPUT);
  pinMode(14, INPUT);
  pinMode(16, INPUT);
  pinMode(10, INPUT);

  Joystick.begin();
}

void loop() {
  readMatrix();
  checkButtons();
  checkJoystick();
  if (checkHatState) {
    checkHat();
  }
  if (switch_toggle) {
    check_delay();
  }
} 

void check_delay(void) {
  if (delay_start == 0) {
    // press the button and set the start time
    
    // joystick.button set thingy here
    // 11 is actually button 12... count from 0
    Joystick.pressButton(7);
    //Serial.println("Press 12");
    
    delay_start = millis();
  }
  if (delay_start > 0) {
    // check our delay, release the button after 150ms duration
    if ((millis() - delay_start) > 150) {
      // joystick.button unset thingy here
      Joystick.releaseButton(7);
      //Serial.println("Release 12");
      
      // clear and reset our flags
      switch_toggle = false;
      delay_start = 0;
    }
  }
}

void readMatrix() {
  //iterate the columns
  for (int colIndex=0; colIndex < NUMCOLS; colIndex++) {
    // col: set to output low
    byte curCol = colPins[colIndex];
    pinMode(curCol, OUTPUT);
    digitalWrite(curCol, LOW);

    // row: iterate through the rows
    for (int rowIndex=0; rowIndex < NUMROWS; rowIndex++) {
      byte rowCol = rowPins[rowIndex];
      pinMode(rowCol, INPUT_PULLUP);
      int but_num = (rowIndex * NUMCOLS) + colIndex;
      currState[but_num] = !digitalRead(rowCol);
      pinMode(rowCol, INPUT);
    }
    // disable the column
    pinMode(curCol, INPUT);
  }
}

void checkButtons(void) {
  for (int i = 0; i<sizeof(lastState); i++) {
    if (currState[i] != lastState[i]) {
      if (i == 1){
        // toggle switch
        switch_toggle = true;
        lastState[i] = currState[i];
      }
      else if (i == 6) {
        // hat up
        currHatButtonState[0] = currState[i];
        lastState[i] = currState[i];
        checkHatState = true;
      }
      else if (i == 4) {
        // hat dn
        currHatButtonState[2] = currState[i];
        lastState[i] = currState[i];
        checkHatState = true;
      }
      else if (i == 10) {
        // hat L
        currHatButtonState[3] = currState[i];
        lastState[i] = currState[i];
        checkHatState = true;
      }
      else if (i == 7) {
        // hat R
        currHatButtonState[1] = currState[i];
        lastState[i] = currState[i];
        checkHatState = true;
      }
      else {
        Joystick.setButton(joymap[i], currState[i]);
        lastState[i] = currState[i];
      }
    }
  }
}


/**void checkKeypad(void) {
//  if (keypad.getKeys()) {
//    for (int i=0; i<LIST_MAX; i++) {
//     if (keypad.key[i].stateChanged) {
//        int keyNum = keypad.key[i].kcode;
//        Serial.println(keyNum);
//        switch (keypad.key[i].kstate) {
//            case PRESSED:
//              if (keyNum == 1) {
//               switch_toggle = true; 
//              }
//              else if (keyNum == 6) {
//                // hat up
//                currHatButtonState[0] = 1;
//                checkHatState = true;
//              }
 //             else if (keyNum == 4) {
//                // hat dn
//                currHatButtonState[2] = 1;
//                checkHatState = true;
//              }
//              else if (keyNum == 10) {
//                // hat L
//                currHatButtonState[3] = 1;
//                checkHatState = true;
//              }
//              else if (keyNum == 7) {
 //               // hat R
//                currHatButtonState[1] = 1;
//                checkHatState = true;
              }
              else {
                Joystick.pressButton(joymap[keyNum]);
              }
              break;
            case HOLD:
              break;
            case RELEASED:
              if (keyNum == 1) {
                switch_toggle = true; 
              }
             else if (keyNum == 6) {
                // hat up
                currHatButtonState[0] = 0;
                checkHatState = true;
              }
              else if (keyNum == 4) {
                // hat dn
                currHatButtonState[2] = 0;
                checkHatState = true;
              }
              else if (keyNum == 10) {
                // hat L
                currHatButtonState[3] = 0;
                checkHatState = true;
              }
              else if (keyNum == 7) {
                // hat R
                currHatButtonState[1] = 0;
                checkHatState = true;
              }
              else {
                Joystick.releaseButton(joymap[keyNum]);
              }
              break;
            case IDLE:
              break;
        }
      }
    }
  }
}
**/

void checkHat(void) {
  // The array is = {up '0', R '90', dn '180', L '270'}
  //int checkArray[] = {0,0,0,0};
  if (compareHatArray(currHatButtonState, 0, 0, 0, 0)) {
    Joystick.setHatSwitch(0, -1);
  }
  //checkArray = {1,0,0,0};
  else if (compareHatArray(currHatButtonState, 1, 0, 0, 0)) {
    Joystick.setHatSwitch(0, 0);
  }
  //checkArray = {1,1,0,0};
  else if (compareHatArray(currHatButtonState, 1, 1, 0, 0)) {
    Joystick.setHatSwitch(0, 45);
  }
  //checkArray = {0,1,0,0};
  else if (compareHatArray(currHatButtonState, 0, 1, 0, 0)) {
    Joystick.setHatSwitch(0, 90);
  }
  //checkArray = {0,1,1,0};
  else if (compareHatArray(currHatButtonState, 0, 1, 1, 0)) {
    Joystick.setHatSwitch(0, 135);
  }
  //checkArray = {0,0,1,0};
  else if (compareHatArray(currHatButtonState, 0, 0, 1, 0)) {
    Joystick.setHatSwitch(0, 180);
    //Serial.println("Down");
  }
  //checkArray = {0,0,1,1};
  else if (compareHatArray(currHatButtonState, 0, 0, 1, 1)) {
    Joystick.setHatSwitch(0, 225);
  }
  //checkArray = {0,0,0,1};
  else if (compareHatArray(currHatButtonState, 0, 0, 0, 1)) {
    Joystick.setHatSwitch(0, 270);
  }
  //checkArray = {1,0,0,1};
  else if (compareHatArray(currHatButtonState, 1, 0, 0, 1)) {
    Joystick.setHatSwitch(0, 315);
  }
  checkHatState = false;
}

int compareHatArray(int a[], int c, int d, int e, int f) {
  int b[] = {c,d,e,f};
  for (int i = 0; i<4; i++) {
    //Serial.print(a[i]);
    //Serial.print(" cmp ");
    //Serial.println(b[i]);
    if (a[i] != b[i]) {
      return 0;
    }
  }
  return 1;
}

void checkJoystick(void) {
  // set X Axis
  int x_value = analogRead(X_PIN);
  int x_map = 511;
  if (x_value < (X_MID - X_TRIM)) {
    //Serial.println("Trim up");
    x_map = map(x_value, X_MIN, (X_MID-X_TRIM), 0, 510);
  }
  else if (x_value > (X_MID + X_TRIM)) {
    //Serial.println("Trim dn");
    x_map = map(x_value, (X_MID+X_TRIM), X_MAX, 512, 1023); 
  }
  else {
    //Serial.println("Center");
    x_map = 511;
  }
  //Serial.print("x value ");
  //Serial.println(x_value);
  //Serial.println(x_map);
  Joystick.setXAxis(x_map);

  //set Y Axis
  int y_value = analogRead(Y_PIN);
  int y_map = 511;
  if (y_value < (Y_MID - Y_TRIM)) {
    //Serial.println("Trim up");
    y_map = map(y_value, Y_MIN, (Y_MID-Y_TRIM), 0, 510);
  }
  else if (y_value > (Y_MID + Y_TRIM)) {
    //Serial.println("Trim dn");
    y_map = map(y_value, (Y_MID+Y_TRIM), Y_MAX, 512, 1023);
  }
  else {
    //Serial.println("Center");
    y_map = 511;
  }
  //Serial.print("y value ");
  //Serial.println(y_value);
  //Serial.println(y_map);
  Joystick.setYAxis(y_map);

  // set rudder value
  //int rud_value = analogRead(RUD_PIN);
  //int rud_map = 511;
  //if (rud_value < (RUD_MID - RUD_TRIM)) {
    //Serial.println("Trim up");
 //   rud_map = map(rud_value, RUD_MIN, (RUD_MID-RUD_TRIM), 0, 510);
 // }
  //else if (rud_value > (RUD_MID + RUD_TRIM)) {
    //Serial.println("Trim dn");
   // rud_map = map(rud_value, (RUD_MID+RUD_TRIM), RUD_MAX, 512, 1023);
 // }
  //else {
    //Serial.println("Center");
   // rud_map = 511;
 // }
//  Serial.print("value ");
//  Serial.println(rud_value);
// Serial.print("map ");
//  Serial.println(rud_map);
//  Joystick.setRudder(rud_map);

  // set throttle value
  int throt_value = analogRead(THROT_PIN);
  //Serial.print("Throt value ");
  //Serial.println(throt_value);
  int throt_map = map(throt_value, THROT_MIN, THROT_MAX, 0, 1023);
  //Serial.print("Throt map ");
  //Serial.println(throt_map);
  Joystick.setThrottle(throt_map);
}
