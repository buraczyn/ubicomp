const int INPUT_MODE_1 = 2;
const int INPUT_MODE_2 = 3;
const int INPUT_MODE_3 = 4;
const int INPUT_MODE_4 = 5;
const int OUTPUT_LED_PIN = LED_BUILTIN;
const int DELAY_MS = 30;

int currentMode = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(INPUT_MODE_1, INPUT_PULLUP);
  pinMode(INPUT_MODE_2, INPUT_PULLUP);
  pinMode(INPUT_MODE_3, INPUT_PULLUP);
  pinMode(INPUT_MODE_4, INPUT_PULLUP);
  pinMode(OUTPUT_LED_PIN, OUTPUT);

  Serial.begin(9600);
}

int getCurrentMode() {
  int mode1State = digitalRead(INPUT_MODE_1);
  int mode2State = digitalRead(INPUT_MODE_2);
  int mode3State = digitalRead(INPUT_MODE_3);
  int mode4State = digitalRead(INPUT_MODE_4);

  if (!mode1State == HIGH) {
    return 1;
  } else if (!mode2State == HIGH) {
    return 2;
  } else if (!mode3State == HIGH) {
    return 3;
  } else if (!mode4State == HIGH) {
    return 4;
  } else {
    return currentMode;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  currentMode = getCurrentMode();

  switch (currentMode) {
    case 1:
      mode1();
      break;
    case 2:
      mode2();
      break;
    case 3:
      mode3();
      break;
    case 4:
      mode4();
      break;
    case 0: 
      break;
  }
}

// Crossfading RGB LED 
void mode1() {
  digitalWrite(OUTPUT_LED_PIN, HIGH);
  Serial.println("Mode 1");
}

// Respond to sound
void mode2() {
  digitalWrite(OUTPUT_LED_PIN, HIGH);
  Serial.println("Mode 2");

}

// Configuring colour with lofi input
void mode3() {
  digitalWrite(OUTPUT_LED_PIN, HIGH);
  Serial.println("Mode 3");

}

// Creative feature
void mode4() {
  digitalWrite(OUTPUT_LED_PIN, HIGH);
  Serial.println("Mode 4");
  
}
