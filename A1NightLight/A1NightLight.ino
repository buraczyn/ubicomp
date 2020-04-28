#include "src/RGBConverter/RGBConverter.h"

const int INPUT_MODE_1 = 2;
const int INPUT_MODE_2 = 3;
const int INPUT_MODE_3 = 4;
const int INPUT_MODE_4 = 5;
const int INPUT_PHOTOCELL = A0;
const int INPUT_MICROPHONE = A1; 
const int INPUT_ANALOG_1 = 11;

const int INPUT_FORCE = A2;

const int OUTPUT_GREEN = 13;

const int OUTPUT_RGB_RED_LED = 6;
const int OUTPUT_RGB_GREEN_LED = 9;
const int OUTPUT_RGB_BLUE_LED = 10;
const int OUTPUT_LED_PIN = LED_BUILTIN;

const int DELAY_MS = 30;
const int MAX_COLOUR_VALUE = 255;
const int FADE_STEP = 5;

const int MAX_ANALOG_IN = 676; //1024 with 5V, 676 with 3.3V on Arduino Uno
const int MAX_ANALOG_OUT = 255;

RGBConverter rgbConverter;
float hue = 0.0f;
float fade_step = 0.001f;

int touchedCutoff = .5;

float lastForce = 0.0;
float peakForce = 0.0;
float forceThreshold = 0.70;

int currentMode = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(INPUT_MODE_1, INPUT_PULLUP);
  pinMode(INPUT_MODE_2, INPUT_PULLUP);
  pinMode(INPUT_MODE_3, INPUT_PULLUP);
  pinMode(INPUT_MODE_4, INPUT_PULLUP);
  pinMode(INPUT_PHOTOCELL, INPUT);
  pinMode(INPUT_MICROPHONE, INPUT);
  pinMode(INPUT_ANALOG_1, INPUT);

  pinMode(INPUT_FORCE, INPUT);
  
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
  crossFadeLEDs(getLightness());  
}

// Respond to sound
void mode2() {
  digitalWrite(OUTPUT_LED_PIN, HIGH);
  Serial.println("Mode 2");
  respondToSound();
}

// Configuring colour with lofi input
void mode3() {
  Serial.println("Mode 3");

    // If the capacitive sensor reads above a certain threshold,
  //  turn on the LED
  Serial.println(readCapacitivePin(INPUT_ANALOG_1));
  if (readCapacitivePin(INPUT_ANALOG_1) > touchedCutoff) {
    Serial.println("In the or");
    digitalWrite(OUTPUT_GREEN, HIGH);
  }
  else {
    Serial.println("not in there");
    digitalWrite(OUTPUT_GREEN, LOW);
  }

}

// Creative feature
void mode4() {
  float forceMapped = mapFloat((float) analogRead(INPUT_FORCE), 0.0, 1023.0, 0.0, 1.0);
  float force = constrain(forceMapped, 0.0, 1.0);
  
  if (force > peakForce) {
    Serial.print("Force: " );
    Serial.println(force);
    Serial.println("Greater than peak force!");
    peakForce = force;
  }
  if (force <= forceThreshold) {
    if (peakForce > forceThreshold) {
      Serial.print("Peak force: " );
      Serial.println(peakForce);
      peakForce = 0.0;
    }
  }
  crossFadeLEDs(peakForce);
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

void crossFadeLEDs(float lightValue) {
  // Some parts of this are from the tutorial on RGB LED outputs
  byte rgb[3];
  rgbConverter.hslToRgb(hue, 1, lightValue, rgb);
  setRGBColour(rgb[0], rgb[1], rgb[2]); 

  // update hue based on step size
  hue += fade_step;

  // hue ranges between 0 - 1, so if > 1, reset to 0
  if (hue > 1.0){
    hue = 0.0;
  }

  delay(DELAY_MS);
}

void respondToSound() {
  int sound = analogRead(INPUT_MICROPHONE);

  // Since microphone works peak-to-peak from 0V to the reference voltage (either 3.3V or 5V)
  // we only look at half of the amplitude from MAX_ANALOG_IN/2 to MAX_ANALOG_IN
  int soundValue = map(sound, MAX_ANALOG_IN/2, MAX_ANALOG_IN, 0, MAX_ANALOG_OUT);
  soundValue = constrain(soundValue, 0, MAX_ANALOG_OUT);
  
  float lightness = mapFloat((float) soundValue, 0, MAX_ANALOG_OUT, 0.0f, 1.0f);
  lightness = constrain(lightness, 0.0f, 1.0f);
  crossFadeLEDs(lightness);
}

// map() for floats
float mapFloat(float x, float xMin, float xMax, float yMin, float yMax) {
  return (x - xMin) * (yMax - yMin) / (xMax - xMin) + yMin;
}

// readCapacitivePin
//  Input: Arduino pin number
//  Output: A number, from 0 to 17 expressing
//          how much capacitance is on the pin
//  When you touch the pin, or whatever you have
//  attached to it, the number will get higher
//  In order for this to work now,
// The pin should have a resistor pulling
//  it up to +5v.
// Taken from: https://sites.google.com/a/mtholyoke.edu/idesign-studio-workshop/labs-tutorials/workshop-5-capacitive-touch
uint8_t readCapacitivePin(int pinToMeasure){
  // This is how you declare a variable which
  //  will hold the PORT, PIN, and DDR registers
  //  on an AVR
  volatile uint8_t* port;
  volatile uint8_t* ddr;
  volatile uint8_t* pin;
  // Here we translate the input pin number from
  //  Arduino pin number to the AVR PORT, PIN, DDR,
  //  and which bit of those registers we care about.
  byte bitmask;
  if ((pinToMeasure >= 0) && (pinToMeasure <= 7)){
    port = &PORTD;
    ddr = &DDRD;
    bitmask = 1 << pinToMeasure;
    pin = &PIND;
  }
  if ((pinToMeasure > 7) && (pinToMeasure <= 13)){
    port = &PORTB;
    ddr = &DDRB;
    bitmask = 1 << (pinToMeasure - 8);
    pin = &PINB;
  }
  if ((pinToMeasure > 13) && (pinToMeasure <= 19)){
    port = &PORTC;
    ddr = &DDRC;
    bitmask = 1 << (pinToMeasure - 13);
    pin = &PINC;
  }
  // Discharge the pin first by setting it low and output
  *port &= ~(bitmask);
  *ddr  |= bitmask;
  delay(1);
  // Make the pin an input WITHOUT the internal pull-up on
  *ddr &= ~(bitmask);
  // Now see how long the pin to get pulled up
  int cycles = 16000;
  for(int i = 0; i < cycles; i++){
    if (*pin & bitmask){
      cycles = i;
      break;
    }
  }
  // Discharge the pin again by setting it low and output
  //  It's important to leave the pins low if you want to 
  //  be able to touch more than 1 sensor at a time - if
  //  the sensor is left pulled high, when you touch
  //  two sensors, your body will transfer the charge between
  //  sensors.
  *port &= ~(bitmask);
  *ddr  |= bitmask;
  
  return cycles;
}
