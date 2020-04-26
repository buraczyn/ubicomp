#include "src/RGBConverter/RGBConverter.h"

const int INPUT_MODE_1 = 2;
const int INPUT_MODE_2 = 3;
const int INPUT_MODE_3 = 4;
const int INPUT_MODE_4 = 5;
const int INPUT_PHOTOCELL = A0;

const int OUTPUT_RGB_RED_LED = 6;
const int OUTPUT_RGB_GREEN_LED = 7;
const int OUTPUT_RGB_BLUE_LED = 8;
const int OUTPUT_LED_PIN = LED_BUILTIN;

const int DELAY_MS = 30;
const int MAX_COLOUR_VALUE = 255;
const int FADE_STEP = 5;
const int MIN_PHOTOCELL_VALUE = 200;
const int MAX_PHOTOCELL_VALUE = 800;

enum RGB{
  RED,
  GREEN,
  BLUE
};

int currentMode = 0;
int rgbValues[] = {MAX_COLOUR_VALUE, 0, 0};
enum RGB fadeUpColour = GREEN;
enum RGB fadeDownColour = RED;

void setup() {
  // put your setup code here, to run once:
  pinMode(INPUT_MODE_1, INPUT_PULLUP);
  pinMode(INPUT_MODE_2, INPUT_PULLUP);
  pinMode(INPUT_MODE_3, INPUT_PULLUP);
  pinMode(INPUT_MODE_4, INPUT_PULLUP);
  
  pinMode(OUTPUT_LED_PIN, OUTPUT);
  pinMode(OUTPUT_RGB_RED_LED, OUTPUT);
  pinMode(OUTPUT_RGB_GREEN_LED, OUTPUT);
  pinMode(OUTPUT_RGB_BLUE_LED, OUTPUT);

  setRGBColour(rgbValues[RED], rgbValues[GREEN], rgbValues[BLUE]);
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
  // Crossfade led
  crossFadeLEDs();

  // Change brightness according to photocell
  
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

void setRGBColour(int red, int green, int blue) {
  Serial.println("setting output");
  analogWrite(OUTPUT_RGB_RED_LED, red);
  analogWrite(OUTPUT_RGB_GREEN_LED, green);
  analogWrite(OUTPUT_RGB_BLUE_LED, blue);  
}

void crossFadeLEDs() {
  Serial.println("In the crossfade");
  rgbValues[fadeUpColour] += FADE_STEP;
  rgbValues[fadeDownColour]-= FADE_STEP;
  
  Serial.println(fadeUpColour);
  Serial.println(fadeDownColour);

  // Fade code based partially on https://gist.github.com/jamesotron/766994 and tutorials
  if (rgbValues[fadeUpColour] > MAX_COLOUR_VALUE) {
    Serial.println("changing the fadeup");
    rgbValues[fadeUpColour] = MAX_COLOUR_VALUE;
    fadeUpColour = (RGB)((int) fadeUpColour + 1);

    if (fadeUpColour > (int) BLUE) {
      fadeUpColour = RED;
    }
  }

  if (rgbValues[fadeDownColour] < 0) {
    Serial.println("changing the fade down");
    rgbValues[fadeDownColour] = 0;
    fadeDownColour = (RGB)((int) fadeDownColour + 1);

    if (fadeDownColour > (int) BLUE) {
      fadeDownColour = RED;
    }
  }

  // Set the photocell brightness
  int photocellValue = analogRead(INPUT_PHOTOCELL);
  int mappedPhotocellValue = map(photocellValue, MIN_PHOTOCELL_VALUE, MAX_PHOTOCELL_VALUE, 0, 255);
  int constrainedPhotocellValue= constrain(mappedPhotocellValue, 0, 255);
  int invertedValue = 255 - constrainedPhotocellValue;
  Serial.print(photocellValue);
  Serial.print(", ");
  Serial.println(invertedValue);
  
  setRGBColour(rgbValues[RED], rgbValues[GREEN], rgbValues[BLUE]);
  delay(DELAY_MS);
}
