#include "module2.h"

void m2init(void)
{
    pinMode(13,OUTPUT);
}

void m2app(void)
{
    digitalWrite(13,HIGH);
    delay(1000);
    digitalWrite(13,LOW);
    delay(1000);
}


int fconfigWeight(int *set_weight, int *config_weight) {
  const int ENC_CLK = 2;
  const int ENC_DT  = 3;
  const int ENC_SW  = 4;
  const int LONG_PRESS_MS = 1000;
  const int WEIGHT_STEP   = 1;   // was 0.1, now smallest int step

  static bool initialized = false;
  static int  lastClkState;
  static bool isPressed = false;
  static unsigned long pressStartTime = 0;

  if (!initialized) {
    pinMode(ENC_CLK, INPUT);
    pinMode(ENC_DT,  INPUT);
    pinMode(ENC_SW,  INPUT_PULLUP);

    *config_weight = *set_weight;
    lastClkState = digitalRead(ENC_CLK);
    initialized = true;
  }

  int clkState = digitalRead(ENC_CLK);
  if (clkState != lastClkState) {
    if (digitalRead(ENC_DT) != clkState) {
      *config_weight += WEIGHT_STEP;
    } else {
      *config_weight -= WEIGHT_STEP;
    }
  }
  lastClkState = clkState;

  if (digitalRead(ENC_SW) == LOW) {
    if (!isPressed) {
      isPressed = true;
      pressStartTime = millis();
    } else if (millis() - pressStartTime > LONG_PRESS_MS) {
      *set_weight = *config_weight;
      isPressed = false;
      initialized = false;
    }
  } else {
    isPressed = false;
  }
}





// ---------------- Globally initialised shared pointers ----------------
int real_weight_val = 0;
int set_weight_val  = 500;      // default target, another module can overwrite this

int *real_weight = &real_weight_val;
int *set_weight  = &set_weight_val;

// ---------------- Emergency stop ----------------
volatile bool emergencyStop = false;

// Required by attachInterrupt() — must stay a standalone function
void emergencyStopISR() {
  emergencyStop = true;
}

int fcheckWeightAndMove(int *real_weight, int *set_weight) {
  const int SERVO_PIN        = 9;
  const int START_BUTTON_PIN = 7;
  const int STOP_BUTTON_PIN  = 2;    // must be pin 2 or 3 on Nano (interrupt capable)
  const int INITIAL_ANGLE    = 0;
  const int OPEN_ANGLE       = 90;
  const int RS232_RX         = 10;   // Nano RX  <- scale TX
  const int RS232_TX         = 11;   // Nano TX  -> scale RX
  const long RS232_BAUD      = 9600;

  static Servo weightServo;
  static SoftwareSerial rs232Serial(RS232_RX, RS232_TX);
  static bool initialized    = false;
  static bool dispensing     = false;
  static int  current_weight = 0;

  // one-time setup, runs only on the very first call
  if (!initialized) {
    weightServo.attach(SERVO_PIN);
    weightServo.write(INITIAL_ANGLE);

    pinMode(START_BUTTON_PIN, INPUT_PULLUP);
    pinMode(STOP_BUTTON_PIN, INPUT_PULLUP);

    rs232Serial.begin(RS232_BAUD);
    attachInterrupt(digitalPinToInterrupt(STOP_BUTTON_PIN), emergencyStopISR, FALLING);

    initialized = true;
  }

  // emergency stop always wins, checked first
  if (emergencyStop) {
    weightServo.write(INITIAL_ANGLE);
    dispensing = false;
    return &current_weight;
  }

  // start button pressed -> begin dispensing
  if (!dispensing && digitalRead(START_BUTTON_PIN) == LOW) {
    weightServo.write(OPEN_ANGLE);
    dispensing = true;
    *real_weight = 0;
  }

  // while dispensing, keep pulling weight from the RS-232 scale
  if (dispensing) {
    if (rs232Serial.available()) {
      *real_weight = rs232Serial.parseInt();
    }

    current_weight = *real_weight;   // store live reading

    if (*real_weight >= *set_weight) {
      weightServo.write(INITIAL_ANGLE);
      dispensing = false;
    }
  }

  return &current_weight;
}




void fcheckWeight(int *real_weight, int *set_weight, int *complete_weight) {
  const int SERVO_PIN     = 9;
  const int INITIAL_ANGLE = 0;
  const int OPEN_ANGLE    = 120;
  const int RS232_RX      = 10;   // Nano RX  <- scale TX
  const int RS232_TX      = 11;   // Nano TX  -> scale RX
  const long RS232_BAUD   = 9600;

  static Servo weightServo;
  static SoftwareSerial rs232Serial(RS232_RX, RS232_TX);
  static bool initialized = false;
  static bool dispensing  = false;

  if (!initialized) {
    weightServo.attach(SERVO_PIN);
    weightServo.write(INITIAL_ANGLE);
    rs232Serial.begin(RS232_BAUD);
    initialized = true;
  }

  // first call after a reset -> rotate servo to start dispensing
  if (!dispensing) {
    weightServo.write(OPEN_ANGLE);
    dispensing = true;
  }

  // take one reading each time this function is called
  if (rs232Serial.available()) {
    *real_weight = rs232Serial.parseInt();
  }

  // check if target reached
  if (*real_weight == *set_weight) {
    weightServo.write(INITIAL_ANGLE);
    *complete_weight = *real_weight;
    dispensing = false;   // ready for the next cycle
  }
}