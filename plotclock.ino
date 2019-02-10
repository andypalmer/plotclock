// Plotclock
// cc - by Johannes Heberlein 2014
// v 1.02
// thingiverse.com/joo   wiki.fablab-nuernberg.de
// units: mm; microseconds; radians
// origin: bottom left of drawing surface
// time library see http://playground.arduino.cc/Code/time
// RTC  library see http://playground.arduino.cc/Code/time
//               or http://www.pjrc.com/teensy/td_libs_DS1307RTC.html
// Change log:
// 1.01  Release by joo at https://github.com/9a/plotclock
// 1.02  Additional features implemented by Dave:
//       - added ability to calibrate servofaktor seperately for left and right servos
//       - added code to support DS1307, DS1337 and DS3231 real time clock chips
//       - see http://www.pjrc.com/teensy/td_libs_DS1307RTC.html for how to hook up the real time clock

// delete or mark the next line as comment if you don't need these
#define REALTIMECLOCK    // enable real time clock

#define SERVOPINLIFT  2
#define SERVOPINLEFT  3
#define SERVOPINRIGHT 4

#define COLON 11

// lift positions of lifting servo
#define LIFT0 840 // on drawing surface
#define LIFT1 950  // between numbers
#define LIFT2 1300  // going towards sweeper

// speed of liftimg arm, higher is slower
#define LIFTSPEED 1500

// length of arms
#define L1 35
#define L2 45
#define L3 15

#define WIPER_X 75
#define WIPER_Y 40


#include <Time.h> // see http://playground.arduino.cc/Code/time 
#include <TimeLib.h>
#include <Servo.h>

#include <Wire.h>
#include <DS1307RTC.h> // see http://playground.arduino.cc/Code/time   

#include "include/geometry/point.hpp"
Point point = Point(3,5);

Servo servo1;  //
Servo servo2;  //
Servo servo3;  //


int last_min = 0;


volatile double lastX = WIPER_X;
volatile double lastY = WIPER_Y;


#include <SerialUI.h>
SUI::SerialUI mySUI;

void setup() {
  tmElements_t tm;
  if (RTC.read(tm)) {
    setTime(tm.Hour, tm.Minute, tm.Second, tm.Day, tm.Month, tm.Year);
  }
  else {
    setTime(19, 38, 0, 0, 0, 0);
  }

  servo1.attach(SERVOPINLIFT);  //  lifting servo
  lift(2);
  servo2.attach(SERVOPINLEFT);  //  left servo
  servo3.attach(SERVOPINRIGHT);  //  right servo
  rest();
  delay(1000);
  setupMenu();
  mySUI.println(F("Init"));
}

void loop() {
  test_loop();
}

void setupMenu()
{
  mySUI.setGreeting(F("+++ Welcome to Plot Clock +++\r\nEnter ? for help."));
  mySUI.begin(115200);
  mySUI.setTimeout(20000);      // timeout for reads (in ms), same as for Serial.
  mySUI.setMaxIdleMs(30000);    // timeout for user (in ms)

  SUI::Menu * mainMenu = mySUI.topLevelMenu();
  mainMenu->setName(SUI_STR("Main Menu"));
  mainMenu->addCommand(SUI_STR("wipe"), wipe, SUI_STR("erase the board"));
  mainMenu->addCommand(SUI_STR("move"), move, SUI_STR("move the pen"));
  mainMenu->addCommand(SUI_STR("time"), showTime, SUI_STR("show the time on the RTC"));
  mainMenu->addCommand(SUI_STR("left"), servoLeft, SUI_STR("set the left servo"));
  mainMenu->addCommand(SUI_STR("right"), servoRight, SUI_STR("set the right servo"));
  mainMenu->addCommand(SUI_STR("write"), writeTime, SUI_STR("write the time"));
  mainMenu->addCommand(SUI_STR("rest"), rest, SUI_STR("rest the pen in the wiper"));
  mainMenu->addCommand(SUI_STR("dots"), dots, SUI_STR("draw calibration dots"));
}

void writeTime() {
  draw_time(hour(),minute());
}

void move() {
  mySUI.print(F("Enter x coordinate: "));
  mySUI.showEnterNumericDataPrompt();
  int x = mySUI.parseInt();
  mySUI.println(x);
  mySUI.print(F("Enter y coordinate: "));
  mySUI.showEnterNumericDataPrompt();
  int y = mySUI.parseInt();
  mySUI.println(y);
  calculate_angles(x, y);
}

void servoLeft() {
  mySUI.print(F("Set left PWM: "));
  mySUI.showEnterNumericDataPrompt();
  int x = mySUI.parseInt();
  mySUI.println(x);
  servo2.writeMicroseconds(x);
}

void servoRight() {
  mySUI.print(F("Set right PWM: "));
  mySUI.showEnterNumericDataPrompt();
  int x = mySUI.parseInt();
  mySUI.println(x);
  servo3.writeMicroseconds(x);
}

void showTime()
{
  tmElements_t tm;
  if (RTC.read(tm))
  {
    mySUI.print(F("The time is: "));
    mySUI.print(tm.Hour);
    mySUI.print(F(":"));
    mySUI.println(tm.Minute);
  }
}


void wipe() {
  lift(2);
  drawTo(WIPER_X, WIPER_Y);
  lift(0);
  for(int y=WIPER_Y; y >= 20; y -=10) {
    drawTo(5,y);
    drawTo(5,y-5);
    drawTo(65,y-5);
  }

  drawTo(WIPER_X - 20, WIPER_Y);
  lift(1);
  drawTo(WIPER_X + 5, WIPER_Y);
  drawTo(WIPER_X, WIPER_Y);
}

void rest() {
  lift(2);
  drawTo(WIPER_X, WIPER_Y);
  lift(1);
}

void colon() {
  number(28, 25, 11, 0.9);
}

void test_loop() {
  if (mySUI.checkForUser(150))
  {
    mySUI.enter();
    while (mySUI.userPresent())
    {
      mySUI.handleRequests();
    }
  }
}

void dots() {
  lift(2);
  
  for (double y = 45; y >= 20; y -= 5) {
    for (double x = 5; x <= 65; x += 5) {
      drawTo(x, y);
      lift(0);
      lift(1);
    }
  }
  
  rest();
}

void draw_time(int hour, int minute) {
    lift(2);
    
    number(5, 25, hour / 10, 0.9);
    number(19, 25, hour % 10, 0.9);
    colon();
    number(34, 25, minute / 10, 0.9);
    number(48, 25, minute % 10, 0.9);
    
    rest();
}

void original_loop() {
  int i = 0;
  if (last_min != minute()) {

    if (!servo1.attached()) servo1.attach(SERVOPINLIFT);
    if (!servo2.attached()) servo2.attach(SERVOPINLEFT);
    if (!servo3.attached()) servo3.attach(SERVOPINRIGHT);

    lift(0);

    hour();
    while ((i + 1) * 10 <= hour())
    {
      i++;
    }

    wipe();
    number(5, 25, i, 0.9);
    number(19, 25, (hour() - i * 10), 0.9);
    colon();

    i = 0;
    while ((i + 1) * 10 <= minute())
    {
      i++;
    }
    number(34, 25, i, 0.9);
    number(48, 25, (minute() - i * 10), 0.9);
    lift(2);
    drawTo(WIPER_X, WIPER_Y);
    lift(1);
    last_min = minute();

    servo1.detach();
    servo2.detach();
    servo3.detach();
  }
}

// Writing numeral with bx by being the bottom left originpoint. Scale 1 equals a 20 mm high font.
// The structure follows this principle: move to first startpoint of the numeral, lift down, draw numeral, lift up
// ANDY: the curve functions are called with doubles, but the method takes ints, so we're losing precision
void number(float bx, float by, int num, float scale) {

  switch (num) {

    case 0:
      drawTo(bx + 12 * scale, by + 6 * scale);
      lift(0);
      bogenGZS(bx + 7 * scale, by + 10 * scale, 10 * scale, -0.8, 6.7, 0.5);
      lift(1);
      break;
    case 1:

      drawTo(bx + 3 * scale, by + 15 * scale);
      lift(0);
      drawTo(bx + 10 * scale, by + 20 * scale);
      drawTo(bx + 10 * scale, by + 0 * scale);
      lift(1);
      break;
    case 2:
      drawTo(bx + 2 * scale, by + 12 * scale);
      lift(0);
      bogenUZS(bx + 8 * scale, by + 14 * scale, 6 * scale, 3, -0.8, 1);
      drawTo(bx + 1 * scale, by + 0 * scale);
      drawTo(bx + 12 * scale, by + 0 * scale);
      lift(1);
      break;
    case 3:
      drawTo(bx + 2 * scale, by + 17 * scale);
      lift(0);
      bogenUZS(bx + 5 * scale, by + 15 * scale, 5 * scale, 3, -2, 1);
      bogenUZS(bx + 5 * scale, by + 5 * scale, 5 * scale, 1.57, -3, 1);
      lift(1);
      break;
    case 4:
      drawTo(bx + 10 * scale, by + 0 * scale);
      lift(0);
      drawTo(bx + 10 * scale, by + 20 * scale);
      drawTo(bx + 2 * scale, by + 6 * scale);
      drawTo(bx + 12 * scale, by + 6 * scale);
      lift(1);
      break;
    case 5:
      drawTo(bx + 2 * scale, by + 5 * scale);
      lift(0);
      bogenGZS(bx + 5 * scale, by + 6 * scale, 6 * scale, -2.5, 2, 1);
      drawTo(bx + 5 * scale, by + 20 * scale);
      drawTo(bx + 12 * scale, by + 20 * scale);
      lift(1);
      break;
    case 6:
      drawTo(bx + 2 * scale, by + 10 * scale);
      lift(0);
      bogenUZS(bx + 7 * scale, by + 6 * scale, 6 * scale, 2, -4.4, 1);
      drawTo(bx + 11 * scale, by + 20 * scale);
      lift(1);
      break;
    case 7:
      drawTo(bx + 2 * scale, by + 20 * scale);
      lift(0);
      drawTo(bx + 12 * scale, by + 20 * scale);
      drawTo(bx + 2 * scale, by + 0);
      lift(1);
      break;
    case 8:
      drawTo(bx + 5 * scale, by + 10 * scale);
      lift(0);
      bogenUZS(bx + 5 * scale, by + 15 * scale, 5 * scale, 4.7, -1.6, 1);
      bogenGZS(bx + 5 * scale, by + 5 * scale, 5 * scale, -4.7, 2, 1);
      lift(1);
      break;

    case 9:
      drawTo(bx + 9 * scale, by + 11 * scale);
      lift(0);
      bogenUZS(bx + 7 * scale, by + 15 * scale, 5 * scale, 4, -0.5, 1);
      drawTo(bx + 5 * scale, by + 0);
      lift(1);
      break;

    case COLON:
      drawTo(bx + 5 * scale, by + 15 * scale);
      lift(0);
      bogenGZS(bx + 5 * scale, by + 15 * scale, 0.1 * scale, 1, -1, 1);
      lift(1);
      drawTo(bx + 5 * scale, by + 5 * scale);
      lift(0);
      bogenGZS(bx + 5 * scale, by + 5 * scale, 0.1 * scale, 1, -1, 1);
      lift(1);
      break;

  }
}


int servoLift = 1500; // make this local
void lift(char lift) {
  int lifts[3] = { LIFT0, LIFT1, LIFT2 };

  int difference = lifts[lift] - servoLift;
  int direction = difference / abs(difference);
  while(servoLift != lifts[lift]) {
    servoLift += direction;
    servo1.writeMicroseconds(servoLift);
    delayMicroseconds(LIFTSPEED);
  }
}


void bogenUZS(float bx, float by, float radius, int start, int ende, float sqee) {
  // start = start angle, ende = end angle (from horizontal)
  // sqee = 1 for everything except the 0... compresses the x direction to create an ellipse
  float inkr = -0.05; // increment in radians
  float count = 0;

  do {
    drawTo(sqee * radius * cos(start + count) + bx,
           radius * sin(start + count) + by);
    count += inkr;
  }
  while ((start + count) > ende);

}

void bogenGZS(float bx, float by, float radius, int start, int ende, float sqee) {
  float inkr = 0.05;
  float count = 0;

  do {
    drawTo(sqee * radius * cos(start + count) + bx,
           radius * sin(start + count) + by);
    count += inkr;
  }
  while ((start + count) <= ende);
}


void drawTo(double pX, double pY) {
  double dx, dy, c;
  int i;

  // dx dy of new point
  dx = pX - lastX;
  dy = pY - lastY;
  //path lenght in mm, times 4 equals 4 steps per mm
  c = floor(4 * sqrt(dx * dx + dy * dy));

  if (c < 1) c = 1;

  for (i = 0; i <= c; i++) {
    // draw line point by point
    calculate_angles(lastX + (i * dx / c), lastY + (i * dy / c));

  }

  lastX = pX;
  lastY = pY;
}

double return_angle(double a, double b, double c) {
  // cosine rule for angle between c and a
  return acos((a * a + c * c - b * b) / (2 * a * c));
}

void calculate_angles(double x, double y) {

  double p1x = 30;
  double p1y = -25;
  double p5x = 55;
  double p5y = -25;


  // calculate distance between P4 and P6 (aligned along x-axis)
  double dx6 = L3 * cos(0.621);
  double dy6 = L3 * sin(0.621);
  double length46 = sqrt((L2 + dx6) * (L2 + dx6) + dy6 * dy6);
  //mySUI.print("length46: ");
  //mySUI.println(length46);
  double theta346 = atan(dy6/(L2 + dx6));
  //mySUI.print("theta346: ");
  //mySUI.println(theta346);

  // calculate distance between P5 and P6
  double length56 = sqrt((x - p5x) * (x - p5x) + (y - p5y) * (y - p5y));
  //mySUI.print("length56: ");
  //mySUI.println(length56);

  // calculate angle between origin, P5, and P6
  double alpha5 = atan((y - p5y) / (p5x - x));
  //mySUI.print("alpha5: ");
  //mySUI.println(alpha5);

  //calculate angle between P6, P5, and P4
  double beta5 = return_angle(L1, length46, length56);
  //mySUI.print("beta5: ");
  //mySUI.println(beta5);

  //calculate angle between x axis, P5, and P4
  double theta5 = M_PI - alpha5 - beta5;
  if(x > p5x) { theta5 -= M_PI; }
  //mySUI.print("theta5: ");
  //mySUI.println(theta5);

  //calculate location of P4 (polar / vector from P5)
  double p4x = L1 * cos(theta5) + p5x;
  double p4y = L1 * sin(theta5) + p5y;

  //mySUI.print("p4: ");
  //mySUI.print(p4x);
  //mySUI.print(", ");
  //mySUI.println(p4y);

  //calculate angle between x-axis, P4, and P6
  double thetax46 = atan((y-p4y) / (p4x-x));
  //mySUI.print("thetax46: ");
  //mySUI.println(thetax46); 
  
  //calculate angle between x-axis, P4 and P3
  double theta043 = thetax46-theta346;
  //mySUI.print("theta043: ");
  //mySUI.println(theta043);

  //calculate position of P3 (polar / vector from P4)
  double p3x = p4x - L2 * cos(theta043);
  double p3y = p4y + L2 * sin(theta043);

  //mySUI.print("p3: ");
  //mySUI.print(p3x);
  //mySUI.print(", ");
  //mySUI.println(p3y);

  // calculate distance between P1 and P3
  double length13 = sqrt((p1x - p3x) * (p1x - p3x) + (p1y - p3y) * (p1y - p3y));
  //mySUI.print("length13: ");
  //mySUI.println(length13);

  // calculate angle between x-axis, P1, and P3
  double alpha1 = atan(((p3y - p1y) / (p1x - p3x)));
  //mySUI.print("alpha1: ");
  //mySUI.println(alpha1);

  // calculate angle between P2, P1, and P3
  double beta1 = return_angle(L1, L2, length13);
  //mySUI.println(beta1);

  //calculate angle between P1, x-axis, and P2
  double theta1 = alpha1 - beta1;
  if(p3x > p1x) { theta1 += M_PI; }
  //mySUI.print("theta1: ");
  //mySUI.println(theta1);

  //position of P2
  double p2x = p1x - L1 * cos(theta1);
  double p2y = p1y + L1 * sin(theta1);
  //mySUI.print("p2: ");
  //mySUI.print(p2x);
  //mySUI.print(", ");
  //mySUI.println(p2y);

  servo2.writeMicroseconds(1450-541*theta1);
  servo3.writeMicroseconds(1250+636*theta5);
}
