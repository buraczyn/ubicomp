const int INPUT_MODE_1 = 2;
const int INPUT_MODE_2 = 3;
const int INPUT_MODE_3 = 4;
const int INPUT_MODE_4 = 5;
const int OUTPUT_LED_PIN = LED_BUILTIN;

const int DELAY_MS = 30;

void readButton(int input) {
  int buttonState = digitalRead(input);
  digitalWrite(OUTPUT_LED_PIN, !buttonState);
  Serial.println(buttonState);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(INPUT_MODE_1, INPUT_PULLUP);
  pinMode(INPUT_MODE_2, INPUT_PULLUP);
  pinMode(INPUT_MODE_3, INPUT_PULLUP);
  pinMode(INPUT_MODE_4, INPUT_PULLUP);
  pinMode(OUTPUT_LED_PIN, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  readButton(INPUT_MODE_1);
  readButton(INPUT_MODE_2);
  readButton(INPUT_MODE_3);
  readButton(INPUT_MODE_4);



}
