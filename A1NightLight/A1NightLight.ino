#include "src/RGBConverter/RGBConverter.h"

const int INPUT_MODE_1 = 2;
const int INPUT_MODE_2 = 3;
const int INPUT_MODE_3 = 4;
const int INPUT_MODE_4 = 5;
const int INPUT_PHOTOCELL = A0;

const int OUTPUT_RGB_RED_LED = 6;
const int OUTPUT_RGB_GREEN_LED = 9;
const int OUTPUT_RGB_BLUE_LED = 10;
const int OUTPUT_LED_PIN = LED_BUILTIN;

const int DELAY_MS = 50;
const int MAX_COLOUR_VALUE = 255;
const int FADE_STEP = 5;

RGBConverter rgbConverter;
float hue = 0.0f;
float fade_step = 0.001f;

int currentMode = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(INPUT_MODE_1, INPUT_PULLUP);
  pinMode(INPUT_MODE_2, INPUT_PULLUP);
  pinMode(INPUT_MODE_3, INPUT_PULLUP);
  pinMode(INPUT_MODE_4, INPUT_PULLUP);
  pinMode(INPUT_PHOTOCELL, INPUT);
  
  pinMode(OUTPUT_LED_PIN, OUTPUT);
  pinMode(OUTPUT_RGB_RED_LED, OUTPUT);
  pinMode(OUTPUT_RGB_GREEN_LED, OUTPUT);
  pinMode(OUTPUT_RGB_BLUE_LED, OUTPUT);

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
  Serial.println("Mode 1");
  // Crossfade led
  crossFadeLEDs();  
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
  analogWrite(OUTPUT_RGB_RED_LED, red);
  analogWrite(OUTPUT_RGB_GREEN_LED, green);
  analogWrite(OUTPUT_RGB_BLUE_LED, blue);   
}

float getLightness() {
  // Get the reading from the photocell
  float photocellValue = (float) analogRead(INPUT_PHOTOCELL);
  float mappedPhotocellValue = mapFloat(photocellValue, 200.0f, 800.0f, 0.0f, 1.0f);
  return constrain(mappedPhotocellValue, 0.0f, 1.0f);
}

void crossFadeLEDs() {
  // Some parts of this are from the tutorial on RGB LED outputs
  byte rgb[3];
  float lightness = getLightness();
  rgbConverter.hslToRgb(hue, 1, lightness, rgb);
  setRGBColour(rgb[0], rgb[1], rgb[2]); 

  // update hue based on step size
  hue += fade_step;

  // hue ranges between 0 - 1, so if > 1, reset to 0
  if (hue > 1.0){
    hue = 0.0;
  }

  delay(DELAY_MS);
}

// map() for floats
float mapFloat(float x, float xMin, float xMax, float yMin, float yMax) {
  return (x - xMin) * (yMax - yMin) / (xMax - xMin) + yMin;
}
