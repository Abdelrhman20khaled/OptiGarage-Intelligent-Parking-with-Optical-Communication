// Laser TX Pins
#define LaserTX_OUT          10
#define LaserTX_POWER        9
#define LaserTX_GND          8

// Laser RX Pins
#define LaserRX_POWER        11
#define LaserRX_OUT          12
#define LaserRX_GND          13

// IR Sensor Pins 
#define IR_GND               6      
#define IR_OUT               7


unsigned long lastPressTime = 0;
char pressCount = 0;
const unsigned long timeout = 500;  // 1 second to wait for second press

void setup() {
  Serial.begin(9600);

  pinMode(LaserTX_OUT, OUTPUT);
  pinMode(LaserTX_POWER, OUTPUT);
  pinMode(LaserTX_GND, OUTPUT);
  digitalWrite(LaserTX_POWER, HIGH);
  digitalWrite(LaserTX_GND, LOW);

  pinMode(LaserRX_POWER, OUTPUT);
  pinMode(LaserRX_OUT, OUTPUT);
  pinMode(LaserRX_GND, OUTPUT);
  digitalWrite(LaserRX_POWER, HIGH);
  digitalWrite(LaserRX_GND, LOW);

  pinMode(IR_GND, OUTPUT);
  pinMode(IR_OUT, INPUT_PULLUP);
  digitalWrite(IR_GND, LOW);
}

void sendLaserPulses(int numPulses) {
  for (int i = 0; i < numPulses; i++) {
    digitalWrite(LaserTX_OUT, HIGH);
    delay(150);
    digitalWrite(LaserTX_OUT, LOW);
    delay(150);
  }
}

void loop() {
  if (digitalRead(IR_OUT) == LOW) {
    delay(50);
    if (digitalRead(IR_OUT) == LOW) {
      pressCount++;
      lastPressTime = millis();
      while (digitalRead(IR_OUT) == LOW);  // wait for release
    }
  }

  // Check for timeout
  if (pressCount > 0 && (millis() - lastPressTime > timeout)) 
  {
    if (pressCount == 1) 
    {
      sendLaserPulses(2);  // 1 press → 2 pulses -> Open/Close the door
    } else if (pressCount == 2) 
    {
      sendLaserPulses(3);  // 2 presses → 3 pulses -> Car reserve the Slot
    }
    else if (pressCount == 3) 
    {
      sendLaserPulses(4);  // 2 presses → 4 pulses -> Car Leaves the Slot
    }
    else if (pressCount == 4)
     {
      sendLaserPulses(5);  // 2 presses → 4 pulses -> Slot has a problem
    }
    pressCount = 0;  // Reset
  }
}
