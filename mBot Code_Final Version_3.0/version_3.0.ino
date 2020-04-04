#include "MeMCore.h"

#define IRLeft A3    //set pin A3 to Left IR
#define IRRight A2   //set pin A2 to Right IR
#define lowSpeed 100 //lower running speed for wheel adjustment
#define HIGHFREQ A0  //Sound sensor for high freq
#define LOWFREQ A1   //Sound sensor for low freq

// For Victory Music
#define NOTE_B0 31
#define NOTE_C1 33
#define NOTE_CS1 35
#define NOTE_D1 37
#define NOTE_DS1 39
#define NOTE_E1 41
#define NOTE_F1 44
#define NOTE_FS1 46
#define NOTE_G1 49
#define NOTE_GS1 52
#define NOTE_A1 55
#define NOTE_AS1 58
#define NOTE_B1 62
#define NOTE_C2 65
#define NOTE_CS2 69
#define NOTE_D2 73
#define NOTE_DS2 78
#define NOTE_E2 82
#define NOTE_F2 87
#define NOTE_FS2 93
#define NOTE_G2 98
#define NOTE_GS2 104
#define NOTE_A2 110
#define NOTE_AS2 117
#define NOTE_B2 123
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_R1 111 //REST FOR NO. OF CROTCHETS
#define NOTE_R2 222
#define NOTE_R3 333
#define NOTE_R4 444
//---------------------------------------------------------------------------

MeDCMotor MotorL(M1);              //MOVE MBOT (LEFT)
MeDCMotor MotorR(M2);              //MOVE MBOT (RIGHT)
MeLineFollower lineFinder(PORT_2); //BLACK STRIP
MeRGBLed led(PORT_7);              //LEDRGB *PORT 3 affects the speed*
MeLightSensor lightSensor(PORT_6); //LIGHT SENSOR LDR
MeUltrasonicSensor ultraSensor(PORT_1); // ULTRASONIC SENSOR

//---------------------------------------------------------------------------
//MOVEMENT OF THE MBOT
int moveSpeed = 250;
void Forward()
{
  MotorL.run(-moveSpeed);
  MotorR.run(moveSpeed);
}
void TurnLeft()
{
  MotorL.run(moveSpeed * 0.65);
  MotorR.run(moveSpeed * 0.65);
  delay(450);
}
void TurnRight()
{
  MotorL.run(-moveSpeed * 0.65);
  MotorR.run(-moveSpeed * 0.65);
  delay(450);
}
void Turn180()
{
  Turn180IR();
}

void TurnLeft2()
{
  TurnLeft();
  while(ultraSensor.distanceCm() >9){
  Forward();
  }
  TurnLeft();
}

void TurnRight2()
{
  TurnRight();
  while(ultraSensor.distanceCm() >9){
  Forward();
  }
  TurnRight();
}
void Stop()
{
  MotorL.run(0);
  MotorR.run(0);
}
//END OF MOVEMENT OF MBOT
//---------------------------------------------------------------------------
// IR SENSOR FUNCTIONS
#define REFERENCELEFT 651  // set reference value for Left IR
#define REFERENCERIGHT 653 // set reference value for Right IR
double referenceLeft = 0;
double referenceRight = 0;
double inputLeft, inputRight;

// Calibration of IR (DONE BEFOREHAND)
void setupIRCalibrate(void)
{
  for (int i = 0; i < 10; i++)
  {
    inputLeft = analogRead(IRLeft);
    inputRight = analogRead(IRRight);
    referenceLeft += inputLeft;
    referenceRight += inputRight;
    delay(100);
  }
  referenceLeft /= 10;
  referenceRight /= 10;
  // Serial.println(referenceLeft);
  // Serial.println(referenceRight);
}

// Movement correction with the help of IR
void moveForward(void)
{
    inputLeft = analogRead(IRLeft);
    //Serial.println(inputLeft);
    inputRight = analogRead(IRRight);
    //Serial.println(inputRight);
    int a,a1, b, b1;
    //assume the mbot is even closer to the right
    if (inputRight < (REFERENCERIGHT-55)){
      b1=0;
      MotorL.run(-(lowSpeed-35));
      MotorR.run(moveSpeed);
      a1++;
    }
    //assume the mbot is even closer to the left
    if (inputLeft < (REFERENCELEFT-55)){
      a1=0;
      MotorR.run(-(lowSpeed-35));
      MotorL.run(moveSpeed);
      b1++;
    }
    if (inputRight < (REFERENCERIGHT-35)){ //correct towards left
      b = 0;
      MotorL.run(-lowSpeed);
      MotorR.run(moveSpeed);
      a++;
    }
    else if (inputLeft < (REFERENCELEFT-35)){ //correct towards right
      a = 0;
      MotorL.run(-moveSpeed);
      MotorR.run(lowSpeed);
      b++;
    }
    else if (a > 100){ //repay correction debt
      MotorL.run(-moveSpeed);
      MotorR.run(lowSpeed);
    }
    else if (a1 > 100){ //repay correction debt
      MotorL.run(-moveSpeed);
      MotorR.run(lowSpeed);
    }
    else if (b > 100){ //repay correction debt
      MotorL.run(-lowSpeed);
      MotorR.run(moveSpeed);
    }
    else if (b1 > 100){ //repay correction debt
      MotorL.run(-lowSpeed);
      MotorR.run(moveSpeed);
    }
    else{    
      MotorL.run(-moveSpeed);
      MotorR.run(moveSpeed);
    }
  }
// Take in IR values when mbot does 180 turn, to avoid crashing on the walls
void Turn180IR()
{
  int inputLeft2, inputRight2;
  inputLeft2 = analogRead(IRLeft);
  inputRight2 = analogRead(IRRight);
  // if mbot is too close to the right, it turns anticlockwise
  if (inputLeft2 > inputRight2){
    MotorL.run(moveSpeed);
    MotorR.run(moveSpeed);
    delay(555);
  }
  // if mbot is too close to the left, it turns clockwise
  else {
    MotorL.run(-moveSpeed);
    MotorR.run(-moveSpeed);
    delay(555);
  }
}
//---------------------------------------------------------------------------
//COLOUR TEST
// Define time delay before the next R
//RGB colour turns ON to allow LDR to stabilize
#define RGBWait 10 //in milliseconds
// Define time delay before taking another LDR reading
#define LDRWait 10 //in milliseconds

int ledRed[] = {250, 0, 0};
int ledGreen[] = {0, 250, 0};
int ledBlue[] = {0, 0, 250};

//floats to hold colour arrays
float colourArray[] = {0, 0, 0};
float whiteArray[] = {438, 367, 418};
float blackArray[] = {340, 276, 323};
float greyDiff[] = {98, 91, 95};

char colourStr[3][5] = {"R = ", "G = ", "B = "};

int colour_test()
{
  //turn on one colour at a time and LDR reads 5 times
  for (int c = 0; c <= 2; c++)
  {
    // Serial.print(colourStr[c]);
    turn_RGB_led(c); //turn ON the LED, red, green or blue, one colour at a time.
    delay(RGBWait);
    //get the average of 5 consecutive readings for the current colour and return an average
    colourArray[c] = getAvgReading(5);
    //the average reading returned minus the lowest value divided by the maximum possible range, multiplied by 255 will give a value between 0-255, representing the value for the current reflectivity (i.e. the colour LDR is exposed to)
    colourArray[c] = (colourArray[c] - blackArray[c]) / (greyDiff[c]) * 255;
    turn_OFF_led();
    // Serial.println(int(colourArray[c])); //show the value for the current colour LED, which corresponds to either the R, G or B of the RGB code
  }
  //check_colour
  if (colourArray[0] < 30 && colourArray[1] < 30 && colourArray[2] < 30){ //black
    Serial.print("Black");
    SoundChallenge();
  }
  else if (colourArray[0] < 90 && colourArray[1] < 144 && colourArray[2] < 112){ //green 50,83,59
    Serial.print("Green");
    TurnRight();
  }
  else if (colourArray[0] < 176 && colourArray[1] > 160 && colourArray[2] > 204){ //blue
    Serial.print("Blue");
    TurnRight2();
  }
  else if (colourArray[0] < 173 && colourArray[1] < 161 && colourArray[2] < 238) {//purple
    Serial.print("Purple");
    TurnLeft2();
  }
  else if (colourArray[0] < 218 && colourArray[1] < 77 && colourArray[2] < 85) {//red
    Serial.print("Red");
    TurnLeft();
  }
  else if (colourArray[0] > 234 && colourArray[1] > 144 && colourArray[2] > 94){ //yellow
    Serial.print("Yellow");
    Turn180();
  }
}

//turn on one colour at a time and LDR reads 5 times
void turn_RGB_led(int i)
{
  led.setColorAt(0, ledRed[i], ledGreen[i], ledBlue[i]); //turn ON the LED, red, green or blue, one colour at a time.
  led.setColorAt(1, ledRed[i], ledGreen[i], ledBlue[i]);
  led.show();
  delay(RGBWait);
}

void turn_OFF_led()
{
  led.setColorAt(0, 0, 0, 0); //turn ON the LED, red, green or blue, one colour at a time.
  led.setColorAt(1, 0, 0, 0);
  led.show();
  delay(RGBWait);
}

// Calibration Purposes
void setBalance()
{
  //set white balance
  // Serial.println("Put White Sample For Calibration ...");
  delay(5000);    //delay for five seconds for getting sample ready
                  // maybe can add buzzer after 5 seconds up
  turn_OFF_led(); //Check Indicator OFF during Calibration
                  //scan the white sample.
                  //go through one colour at a time, set the maximum reading for each colour -- red, green and blue to the white array
  for (int i = 0; i <= 2; i++)
  {
    turn_RGB_led(i);
    whiteArray[i] = getAvgReading(5);   //scan 5 times and return the average,
    // Serial.println(int(whiteArray[i])); //986,982,980
    turn_OFF_led();
  }
  //done scanning white, time for the black sample.
  //set black balance
  // Serial.println("Put Black Sample For Calibration ...");
  delay(5000); //delay for five seconds for getting sample ready
               //go through one colour at a time, set the minimum reading for red, green and blue to the black array
  for (int i = 0; i <= 2; i++)
  {
    turn_RGB_led(i);
    blackArray[i] = getAvgReading(5);
    // Serial.println(int(blackArray[i])); //366,302,340
    turn_OFF_led();
    //the differnce between the maximum and the minimum gives the range
    greyDiff[i] = whiteArray[i] - blackArray[i];
  }

  //delay another 5 seconds for getting ready colour objects
  // Serial.println("Colour Sensor Is Ready.");
  delay(5000);
}

int getAvgReading(int times)
{
  //find the average reading for the requested number of times of scanning LDR
  int reading;
  int total = 0;
  //take the reading as many times as requested and add them up
  for (int i = 0; i < times; i++)
  {
    reading = lightSensor.read();
    total = reading + total;
    delay(LDRWait);
  }
  //calculate the average and return it
  return total / times;
}
//---------------------------------------------------------------------------
// BUZZER - VICTORY MUSIC

MeBuzzer buzzer;

int melody[] = {NOTE_C3, NOTE_R3, NOTE_G4, NOTE_R2, NOTE_G4, NOTE_R2, NOTE_C4, NOTE_R3, NOTE_G4, NOTE_R2, NOTE_G4, NOTE_R1, NOTE_G3, //ONE LINE =  2SECS = 4BARS AT 120BPM
                NOTE_AS3, NOTE_R2, NOTE_C4, NOTE_R2, NOTE_G4, NOTE_R2, NOTE_G4, NOTE_R2, NOTE_R4, NOTE_G4, NOTE_R2, NOTE_G4, NOTE_R2,
                NOTE_F3, NOTE_R3, NOTE_F4, NOTE_R2, NOTE_F4, NOTE_R2, NOTE_F3, NOTE_R3, NOTE_F4, NOTE_R2, NOTE_F4, NOTE_R2,
                NOTE_R4, NOTE_F4, NOTE_R2, NOTE_F4, NOTE_G4, NOTE_R1, NOTE_F4, NOTE_R2, NOTE_DS4, NOTE_R2, NOTE_D4, NOTE_R1, NOTE_AS3, NOTE_R2};

void victory_tune()
{

  for (int note = 0; note <= 51; note++)
  {

    if (melody[note] == NOTE_R1)
    {
      delay(125);
    }
    else if (melody[note] == NOTE_R2)
    {
      delay(250);
    }
    else if (melody[note] == NOTE_R3)
    {
      delay(375);
    }
    else if (melody[note] == NOTE_R4)
    {
      delay(500);
    }
    else
    {
      buzzer.noTone(8);
      buzzer.tone(8, melody[note], 250); // tone(pin,freq,duration)
      delay(50);
    }
  }
}

//---------------------------------------------------------------------------
// FREQUENCY SOUND TEST

// Detect up to 20 values for each low and high frequency
// Low Frequency - 100 Hz to 300 Hz
// High Frequency - 3000 Hz or higher
// Sum the individual values and find its average respectively
// Find Ratio = Average High Frequency / Average Low Frequency

// Upon reading of 100 sets of Low Frequency and High Frequency
// The limit is observed as such:
// High Freq Music - has HIGHFREQ > 0 - Turn Right
// Low Freq Music - has LOWFREQ > 390 - Turn Left
// Taking the maximum of the 100 values, for condition to be met
// if neither conditions met - Terminate, play Victory Music
void SoundChallenge()
{
  int i;
  double read_Low, read_High;
  double low_reference, high_reference;
  read_Low = analogRead(LOWFREQ);
  low_reference = read_Low;
  read_High = analogRead(HIGHFREQ);
  high_reference = read_High;
  for (i = 0; i < 100; i++)
  {
    read_Low = analogRead(LOWFREQ);
    read_High = analogRead(HIGHFREQ);
    if (read_Low > low_reference)
    {
      low_reference = read_Low;
    }
    if (read_High > high_reference)
    {
      high_reference = read_High;
    }
  }
  /*for (i=0; i < 20; i ++)
  {
    read_Low = analogRead(LOWFREQ);
    // Serial.println(analogRead(LOWFREQ)); //Check values for Low Freq
    read_High = analogRead(HIGHFREQ);
    // Serial.println(analogRead(HIGHFREQ)); // Check values for High Freq
*/
  
  Serial.println(low_reference);
  Serial.println(high_reference);
  // If the max frequency for low is detected to be more than 390
  // if-statement will make the mbot turn left
  if (low_reference > 390)
  {
    // Serial.println("LOW FREQUENCY, TURN LEFT");
    TurnLeft();
  }
  // If the max frequency for high is detected to be more than 3
  // if-statement will make the mbot turn right
  else if (high_reference > 3) // Values to be calculated
  {
    // Serial.println("HIGH FREQUENCY, TURN RIGHT");
    TurnRight();
  }
  //If frequency recorded is out of those range, the mbot buzzer will play
  //its victory tune
  else
  {
    // Serial.println("END OF MAZE, Buzzer!");
    victory_tune();
    delay(300);
  }
}

//---------------------------------------------------------------------------
void setup()
{
  Serial.begin(9600);
}

void loop(){
  int sensorState = lineFinder.readSensors();
  if (sensorState == S1_IN_S2_IN) //Sensor 1 and 2 are inside of black line
  {
    Stop();
    colour_test();
  }
  else
  {
    moveForward(); //Sensor 1 and 2 are outside of black line (IR sensor included)
  }


}
