#include <Servo.h>
#include "HX711.h" //load cell library

#define DAT 13
#define CLK 9
HX711 scale;
float calibration_factor = 103.5;
bool calibrationRun = false;       //set to false if you're doing the actual test. Set to true if you're calibrating 100g
char endCalibration;

Servo motor;
float dist = 0;
int rotations = 0;
int turn = 0;
int pinAprev;
int pinAval;
unsigned long lastInterrupt = 0;

void setup() {
  motor.attach(10);
  motor.write(90);
  pinMode(5, INPUT_PULLUP);
  pinMode(11,INPUT_PULLUP);
  pinMode(12,INPUT);
  pinMode(6,INPUT);

  //Set up the load cell
  scale.begin(DAT, CLK);
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
  Serial.println("Scale Initialized and Tared");
  scale.set_scale(calibration_factor);

  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(12), encoderInterrupt, CHANGE);
}

void loop() {

  if(calibrationRun == true)
  {
      scale.set_scale(calibration_factor); //Adjust to this calibration factor

    while(endCalibration != '0')
    {
      if(Serial.available())
      {
        endCalibration = Serial.read();
        if(endCalibration == '+' || endCalibration == 'a')
          calibration_factor += 1;
        else if(endCalibration == '-' || endCalibration == 'z')
          calibration_factor -= 1;
      }

      Serial.print("Reading: ");
      Serial.print(scale.get_units(), 6);
      Serial.print(" g"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
      Serial.print(" calibration_factor: ");
      Serial.print(calibration_factor);
      Serial.println();
    }
  }

  motorFunc();
}

void motorFunc(){
  int buttonState1 = digitalRead(5);
  int buttonState2 = digitalRead(11);
  if (buttonState1 == LOW){
     motor.write(60);
  }
  else if (buttonState2 == LOW){
    motor.write(120);
  }
  else{
    motor.write(90);
  }
}

void encoderInterrupt(){
  unsigned long interrupt = millis();
  if (interrupt - lastInterrupt > 0.1){
    pinAval = digitalRead(12);
    if (pinAval != pinAprev){
      if (digitalRead(6)!=pinAval){
        rotations += 1;
        turn = int(rotations/33)+1;
        dist += ((64-turn)*3.1415)/33;
        Serial.print(dist/10);
        Serial.print(" ");
        Serial.println(scale.get_units(), 6);     //load cell reading
      }
      else{
        rotations -= 1;
        turn = int(rotations/33)+1;
        dist -= ((64-turn)*3.1415)/33;
        Serial.print(dist/10);
        Serial.print(" ");
        Serial.println(scale.get_units(), 6);     //load cell reading
      }
    }
    pinAprev = pinAval;
    lastInterrupt = interrupt;
  }
}