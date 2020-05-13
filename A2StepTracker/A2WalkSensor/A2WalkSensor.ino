#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Used for LIS3DH hardware & software SPI
#define LIS3DH_CS 10
Adafruit_LIS3DH lis = Adafruit_LIS3DH();
 
const int DELAY_LOOP_MS = 0; // change to slow down how often to read and graph value

// battery level
const int MAX_ANALOG_VAL = 4095;
const float MAX_BATTERY_VOLTAGE = 4.2;
int currentBattery = 100;

unsigned long _totalFrameCount = 0;
unsigned long _startTimeStamp = 0;

// These are for the sensor values:
int NUM_SAMPLES = 50;
int averageMag = 0;
int steps = 0;
int previousSteps = 0;
int stepThreshold = 2000;
boolean inPeak = false;

int GOAL_CHUNK = 10; 
int GOAL_STEPS = 100;
int circlesCompleted = 1;
int centerX = display.width() / 2;
int centerY = 3 + display.height() / 2;

void setup() {
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0, 0);
  display.println("Screen initialized!");
  display.display();
  delay(250);

  display.println("Initializing accelerometer...");
  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldn't start");
    while (1) yield();
  }
  Serial.println("LIS3DH found!");

  lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

  Serial.print("Range = ");
  Serial.print(2 << lis.getRange());
  Serial.println("G");

  // Calibrate the average magnitude by taking an average over the number of samples
  int sum = 0;
  for (int i = 0 ; i < NUM_SAMPLES ; i++) {
    lis.read();
    long x = lis.x;
    long y = lis.y;
    long z = lis.z;
    double mag = sqrt(x * x + y * y + z * z);  
    sum = sum + mag ;
  }
  averageMag = sum/NUM_SAMPLES;
}

// This applies a mean filter over 10 samples that are read in 
// Its set to 10 samples because the sample data showed that we took an average of 5.36 sample/second and
// we need to have a <= 2 sec latency for step tracking
void stepCounter() {
  double sum = 0 ;
  double magnitudes[NUM_SAMPLES] = {0};
  double averagePeaks[NUM_SAMPLES] = {0};
  
  // Take samples and save the sum and magnitudes in an array
  for (int i = 0 ; i < NUM_SAMPLES ; i++) {
    lis.read();
    long x = lis.x;
    long y = lis.y;
    long z = lis.z;
    
    // Subtract out the average so that gravity force is excluded
    magnitudes[i] = sqrt(x * x + y * y + z * z) - averageMag;  
    Serial.print("Mag: ");
    Serial.println(magnitudes[i]);

    averagePeaks[i] = (magnitudes[i] + magnitudes[i-1]) / 2;
    Serial.print("Average peaks: ");
    Serial.println(averagePeaks[i]);

    if (averagePeaks[i] > stepThreshold  && inPeak == false) {
      steps = steps + 1;
      inPeak = true;
    }

    if (averagePeaks[i] < stepThreshold && inPeak == true) {
      inPeak = false;
    }

    if (steps < 0) {
      steps = 0;
    }
  }
}

void loop() {
  if(_startTimeStamp == 0){
    _startTimeStamp = millis();
  }

  display.clearDisplay();
  
  // Draw the battery level
  drawBatteryLevel();

  stepCounter();

  // Draw the steps
  drawSteps();

  // Render buffer to screen
  display.display();
  
  _totalFrameCount++;

  if(DELAY_LOOP_MS > 0){
    delay(DELAY_LOOP_MS);
  }
}

void drawSteps() {
  display.setCursor(0, 0);
  display.print("Steps:");
  display.print(steps);
  Serial.print("Steps: ");
  Serial.println(steps);
  
  if (previousSteps != steps && steps % GOAL_CHUNK == 0 && circlesCompleted <= 10) {
    circlesCompleted = circlesCompleted + 1;
  }
  
  //Number of circles indicates how close you are to the number of goal steps 
  // Radius / goal chunk i.e. 20/10 = 2 each step
  // num of circles we create is how far are we 
  // every 10 steps, we need to draw another circle, so -2 from whats left
  if (steps < GOAL_STEPS) {
    for (int i = 0; i < circlesCompleted * 2 ; i += 2) {
      if (steps > 0) {
        if (i == 20) {
          // draw pixel
          display.drawPixel(centerX, centerY, WHITE);
        } else {
          display.drawCircle(centerX, centerY, 20 - i, WHITE);
        }
      }
    }
  }
  previousSteps = steps;
  
  if (steps >= GOAL_STEPS) {
    for (int i = 0; i < 30 ; i += 2) {
        if (i == 0) {
          // draw pixel
          display.drawPixel(centerX, centerY, WHITE);
        } else {
          display.drawCircle(centerX, centerY, i, WHITE);
        }
        display.display();
    }
  }
}

void drawBatteryLevel() {
  int rawBattery = analogRead(A13);
  float voltageLevel = (rawBattery / 4095.0) * 2 * 1.1 * 3.3; // calculate voltage level
  float batteryFraction = (voltageLevel / MAX_BATTERY_VOLTAGE) * 100;
  if (batteryFraction < currentBattery) {
    currentBattery = (int) batteryFraction;
  }

  int16_t  x1, y1;
  uint16_t w, h;
  display.setTextSize(1);
  display.getTextBounds("XX%", 0, 0, &x1, &y1, &w, &h);
  
  // Giving 3 pixels of space between % and battery icon so 17 = battery width + 3 pixels
  display.setCursor(display.width() - w - 17, 0);
  display.print(currentBattery);
  display.print("%");
  //x, y, width, height, radius, colour
  display.drawRoundRect(display.width() - w + x1 + 3, 0, 14, h, 1, WHITE);
  
  // Draw in the filled based on the amount of current battery
  // Fill in width will be 14(battery width) * currentBattery/100
  uint16_t fillInWidth = 14 * currentBattery/100;
  display.fillRoundRect(display.width() - w + x1 + 3, 0, fillInWidth, h, 1, WHITE);
}
