#include <TM1637.h>
#include <RunningMedian.h>

/*
 * Yardstick
 * 
 * Arduino Nano ultrasonic range finder 
 * 
 * D2: TM1637 CLK
 * D3: TM1637 DIO
 * D4: HC-SR04 Trig
 * D5: HC-SR04 Echo
 * 
 * GND: TM1637 GND & HC-SR04 Gnd
 * +5V: TM1637 VCC & HC-SR04 Vcc
 */

// TM1637 (7 segment display)
#define TM_CLK_PIN 2
#define TM_DIO_PIN 3

// HC-SR04 (ultrasonic sensor)
#define SR03_TRIGGER_PIN 4
#define SR04_ECHO_PIN 5

// Modes
#define METRIC 0
#define IMPERIAL 1

// Time to distance and unit conversion
const float MM_FACTOR = 0.343 / 2; // Speed of sound divided by 2
const float CM_FACTOR = MM_FACTOR * 0.1;
const float IN_FACTOR = MM_FACTOR * 1/25.4;

// Helper
const unsigned int POW_LOOKUP[] = { 1, 10, 100, 1000, 10000 };

// Configurable
const int MEDIAN_SAMPLES = 20; // Interpolation (number of samples taken into account) 1-255
unsigned int mode = METRIC; // Display mode METRIC (cm) or IMPERIAL (in) 
int brightness = 2; // display brightness 0-7

// Internal
TM1637 tm(TM_CLK_PIN,TM_DIO_PIN); // configure tm
RunningMedian durationSamples = RunningMedian(MEDIAN_SAMPLES);
long duration;
int distance;
bool startup;

void setup() {
  // tm initialization
  tm.init();
  tm.set(brightness);

  // sr initialization
  pinMode(SR03_TRIGGER_PIN, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(SR04_ECHO_PIN, INPUT); // Sets the echoPin as an INPUT
}

char digitAt (int val, int n) {
  return val / POW_LOOKUP[n] % 10 ; 
}

void loop() {
  // reset trigger pin
  digitalWrite(SR03_TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  // set trigger pin HIGH for 10 microseconds
  digitalWrite(SR03_TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(SR03_TRIGGER_PIN, LOW);
  // read echo pin
  duration = pulseIn(SR04_ECHO_PIN, HIGH); // travel time in microseconds
  
  durationSamples.add(duration);

  // distance from median duration
  if (mode == METRIC) distance = durationSamples.getMedian() * CM_FACTOR;
  else if (mode == IMPERIAL) distance = durationSamples.getMedian() * IN_FACTOR;
  
  // tm.display(position, character 0-15);
  tm.display(3,digitAt(distance, 0));
  tm.display(2,digitAt(distance, 1));
  tm.point(0);
  tm.display(1,digitAt(distance, 2));
  tm.display(0,digitAt(distance, 3));
}
