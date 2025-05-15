#include <LiquidCrystal_I2C.h>
#include "Servo.h"

#define SERVO_OPEN    1
#define SERVO_CLOSE   0

#define PowerRX_Exit_Pin    A2
#define PowerRX_Enter_Pin   2   
#define PowerRX1_Pin        3
#define PowerRX2_Pin        4
#define PowerRX3_Pin        5

#define Inlet_motor         6
#define Exit_motor          7

#define LaserRX4_Enter_Pin  A0
#define LaserRX5_Exit_Pin   A1

#define LaserRX1_Pin        8
#define LaserTX1_Pin        9

#define LaserRX2_Pin       10
#define LaserTX2_Pin       11

#define LaserRX3_Pin       12
#define LaserTX3_Pin       13

Servo Inlet_ServoControl;
Servo Exit_ServoControl;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void Inlet_Servo_State(char state) {
  if (state == SERVO_OPEN) {
    Inlet_ServoControl.write(0);
    delay(500);
    Inlet_ServoControl.write(90);
  } else if (state == SERVO_CLOSE) {
    Inlet_ServoControl.write(90);
    delay(500);
    Inlet_ServoControl.write(0);
  }
}

void Exit_Servo_State(char state) {
  if (state == SERVO_OPEN) {
    Exit_ServoControl.write(0);
    delay(500);
  } else if (state == SERVO_CLOSE) {
    Exit_ServoControl.write(90);
    delay(500);
  }
}

void Emergency_State()
{
  digitalWrite(LaserTX1_Pin,HIGH);
  digitalWrite(LaserTX2_Pin,HIGH);
  digitalWrite(LaserTX3_Pin,HIGH);
  delay(500);
  digitalWrite(LaserTX1_Pin,LOW);
  digitalWrite(LaserTX2_Pin,LOW);
  digitalWrite(LaserTX3_Pin,LOW);
  delay(500);
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" Optical Garage");
  lcd.setCursor(1, 1);
  lcd.print("P1:N P2:N P3:N");

  pinMode(LaserRX4_Enter_Pin, INPUT_PULLUP);
  pinMode(LaserRX5_Exit_Pin, INPUT_PULLUP);

  pinMode(LaserTX1_Pin, OUTPUT);
  pinMode(LaserRX1_Pin, INPUT_PULLUP);

  pinMode(LaserTX2_Pin, OUTPUT);
  pinMode(LaserRX2_Pin, INPUT_PULLUP);

  pinMode(LaserTX3_Pin, OUTPUT);
  pinMode(LaserRX3_Pin, INPUT_PULLUP);

  pinMode(PowerRX1_Pin, OUTPUT);
  pinMode(PowerRX2_Pin, OUTPUT);
  pinMode(PowerRX3_Pin, OUTPUT);
  pinMode(PowerRX_Enter_Pin, OUTPUT);
  pinMode(PowerRX_Exit_Pin, OUTPUT);

  digitalWrite(PowerRX_Enter_Pin, HIGH);
  digitalWrite(PowerRX_Exit_Pin, HIGH);
  digitalWrite(PowerRX1_Pin, HIGH);
  digitalWrite(PowerRX2_Pin, HIGH);
  digitalWrite(PowerRX3_Pin, HIGH);

  Inlet_ServoControl.attach(Inlet_motor);
  Exit_ServoControl.attach(Exit_motor);

  Exit_Servo_State(SERVO_CLOSE);
  Inlet_Servo_State(SERVO_CLOSE);
}

void loop() {
  static char num_of_cars = 0;
  static bool garage_is_full = false;
  static int ExitRX_lightCounter = 0;
  static bool ExitRX_lastLaserState = HIGH;
  bool ExitRX_currentLaserState = digitalRead(LaserRX5_Exit_Pin);

  static int InletRX_lightCounter = 0;
  static bool InletRX_lastLaserState = HIGH;
  bool InletRX_currentLaserState = digitalRead(LaserRX4_Enter_Pin);

  static int LaserRX1_lightCounter = 0;
  static bool LaserRX1_lastLaserState = HIGH;
  bool LaserRX1_currentLaserState = digitalRead(LaserRX1_Pin);
  static bool LaserRX1_isOccupied = false;

  static int LaserRX2_lightCounter = 0;
  static bool LaserRX2_lastLaserState = HIGH;
  bool LaserRX2_currentLaserState = digitalRead(LaserRX2_Pin);
  static bool LaserRX2_isOccupied = false;

  static int LaserRX3_lightCounter = 0;
  static bool LaserRX3_lastLaserState = HIGH;
  bool LaserRX3_currentLaserState = digitalRead(LaserRX3_Pin);
  static bool LaserRX3_isOccupied = false;

    if (ExitRX_lastLaserState == HIGH && ExitRX_currentLaserState == LOW) {
    ExitRX_lightCounter++;
    Serial.print("Exit Light Detect Total = ");
    Serial.println(ExitRX_lightCounter);
    delay(50);
  }
  ExitRX_lastLaserState = ExitRX_currentLaserState;

  if (InletRX_lastLaserState == HIGH && InletRX_currentLaserState == LOW) {
    InletRX_lightCounter++;
    Serial.print("Inlet Light Detect Total = ");
    Serial.println(InletRX_lightCounter);
    delay(50);
  }
  InletRX_lastLaserState = InletRX_currentLaserState;

  if (ExitRX_lightCounter == 2) 
  {
    num_of_cars--;
    Exit_Servo_State(SERVO_OPEN);
    ExitRX_lightCounter = 0;
    delay(1000);
    Exit_Servo_State(SERVO_CLOSE);
  }

  if (InletRX_lightCounter == 2 && num_of_cars <3) {
    
    Inlet_Servo_State(SERVO_OPEN);
    InletRX_lightCounter = 0;
    delay(1000);
    Inlet_Servo_State(SERVO_CLOSE);
  }

  // --- Slot 1 Logic ---
  if (LaserRX1_lastLaserState == HIGH && LaserRX1_currentLaserState == LOW)
  {
    delay(50); // Debounce
    LaserRX1_lightCounter++;
    Serial.print("LaserRX1 Light Detect Total = ");
    Serial.println(LaserRX1_lightCounter);

    if (!LaserRX1_isOccupied && LaserRX1_lightCounter >= 3) 
    {
      lcd.setCursor(4, 1); // Position for P1
      lcd.print("F");
      LaserRX1_isOccupied = true;
      LaserRX1_lightCounter = 0;
      num_of_cars++;
    } 
    else if (LaserRX1_isOccupied && LaserRX1_lightCounter >= 4) 
    {
      lcd.setCursor(4, 1);
      lcd.print("N");
      LaserRX1_isOccupied = false;
      LaserRX1_lightCounter = 0;
      num_of_cars--;
    }
  }
  LaserRX1_lastLaserState = LaserRX1_currentLaserState;

  // --- Slot 2 Logic ---
  if (LaserRX2_lastLaserState == HIGH && LaserRX2_currentLaserState == LOW) 
  {
    delay(50);
    LaserRX2_lightCounter++;
    Serial.print("LaserRX2 Light Detect Total = ");
    Serial.println(LaserRX2_lightCounter);

    if (!LaserRX2_isOccupied && LaserRX2_lightCounter >= 3) 
    {
      lcd.setCursor(9, 1); // Position for P2
      lcd.print("F");
      LaserRX2_isOccupied = true;
      LaserRX2_lightCounter = 0;
      num_of_cars++;
    } 
    else if (LaserRX2_isOccupied && LaserRX2_lightCounter >= 4) 
    {
      lcd.setCursor(9, 1);
      lcd.print("N");
      LaserRX2_isOccupied = false;
      LaserRX2_lightCounter = 0;
      num_of_cars--;
    }
  }
  LaserRX2_lastLaserState = LaserRX2_currentLaserState;

  // --- Slot 3 Logic ---
  if (LaserRX3_lastLaserState == HIGH && LaserRX3_currentLaserState == LOW) 
  {
    delay(50);
    LaserRX3_lightCounter++;
    Serial.print("LaserRX3 Light Detect Total = ");
    Serial.println(LaserRX3_lightCounter);

    if (!LaserRX3_isOccupied && LaserRX3_lightCounter >= 3) {
      lcd.setCursor(14, 1); // Position for P3
      lcd.print("F");
      LaserRX3_isOccupied = true;
      LaserRX3_lightCounter = 0;
      num_of_cars++;
      } 
    else if (LaserRX3_isOccupied && LaserRX3_lightCounter >= 4) 
    {
      lcd.setCursor(14, 1);
      lcd.print("N");
      LaserRX3_isOccupied = false;
      LaserRX3_lightCounter = 0;
      num_of_cars--;
    }
  }
  LaserRX3_lastLaserState = LaserRX3_currentLaserState;

}
