/*
 * This sketch contains a rough Skweezee for Arduino implementation.
 * 
 * 3 examples illustrate the basic functionality; toggle block comments
 * to switch examples. I connected an RGB LED to pin 3, 5, 6 (green, blue, red).
 * 
 * 
 * The Arduino implementation tries to stay as close as possible to the Processing version.
 * A few notable differences:
 * 
 *   - The vector contains the raw measurements (0–255);
 *     while in Processing all vector components are scaled to 0–1.
 *     This means here, raw[] and vector[] are the same.
 *     
 *   - Most output is directly scaled based on the reference point 'max'
 *     which felt more natural to do. This also means that most functions
 *     return a value from 0–255 (in Processing this is 0–1).
 *     
 *   - Most output is based on the smoothed signal (the moving average)
 *     instead of the direct magnitude
 */

// first define the functions of the digital I/O pins
// the following pins control the multiplexers (enable and select)
#define EN 13
#define S0_A 12
#define S1_A 11
#define S2_A 10
#define S0_B 9
#define S1_B 8
#define S2_B 7

int raw[28]; // vector
float _max = 0; 
float _mag = 0;
int dir[28];
float _mags[5];

void setup() {
  Serial.begin(9600); // baud rate 9600 bps
  pinMode(EN, OUTPUT);
  pinMode(S0_A, OUTPUT);
  pinMode(S1_A, OUTPUT);
  pinMode(S2_A, OUTPUT);
  pinMode(S0_B, OUTPUT);
  pinMode(S1_B, OUTPUT);
  pinMode(S2_B, OUTPUT);
}

void loop() {
  
  // Enable the multiplexer
  digitalWrite(EN, HIGH);
  Serial.write(0);

  // Loop through all the relevant combinations of the multiplexers
  int n = 0;
  for (int i=0; i<7; i++) { 
     for (int j=i+1; j<8; j++) {
        digitalWrite(S2_A, bitRead(i,2) );
        digitalWrite(S1_A, bitRead(i,1) );
        digitalWrite(S0_A, bitRead(i,0) );
        digitalWrite(S2_B, bitRead(j,2) );
        digitalWrite(S1_B, bitRead(j,1) );
        digitalWrite(S0_B, bitRead(j,0) );
        delayMicroseconds(30); // time needed to go from L to H
        raw[n] = analogRead(0)>>2;
        raw[n] = 255-raw[n];
        n++;
     } 
  }

  // Performs basic vector and time series analysis.
  analysis();


/* EXAMPLES */


  // EXAMPLE 1: DYNAMICS (SQUARE/ROOT
  // RGB LED turns red/blue depending on magnitude
  // red on lighter squeezes, blue on hard squeezes
  analogWrite(3, 0);  // green
  analogWrite(5, square());  // blue
  analogWrite(6, root());  // red


/*
  // EXAMPLE 2: STABILITY (STDEV)
  // RGB LED turns green on change, off when stable
  analogWrite(3, stdev()/5);  // green
  analogWrite(5, 0);  // bue
  analogWrite(6, 0);  // red
*/

/*
  // EXAMPLE 3: CHANGE (DIFF)
  // RGB LED turns red on squeeze, blue on release
  float d = diff();
  float inv = -1*diff();
  analogWrite(3, 0);  // green
  analogWrite(5, constrain(inv, 0, 255));  // blue
  analogWrite(6, constrain(d, 0, 255));  // red
*/

  // Serial.println(diff());

  // Now disable the multiplexer (not really necessary)
  digitalWrite(EN, LOW);
  delay(20);
  
}


/* Basic vector and time series analysis, 
 */
void analysis() {
  
  // calculate vector magnitude
  float m = 0;
  for(int j = 0; j < 28; j++) {
    m += raw[j];  
  }
  _mag = m/28;
  
  // compare & store max
  if(_mag > _max) {
    _max = _mag;  
  }
  
  // store 5 most recent magnitudes
  // in a sliding window (used for moving average)
  _mags[4] = _mags[3];
  _mags[3] = _mags[2];
  _mags[2] = _mags[1];
  _mags[1] = _mags[0];
  _mags[0] = _mag;
  
  // calculate vector direction (unit vector)
  float dir[28];
  for (int j = 0; j < 28; j++) {
    dir[j] = raw[j]/_mag;  
  }
  
}


/* Returns magnitude, relative to maximum
 * (similar to 'norm' in Processing library)
 */
int mag() {
  return 255*_mag/_max;
}


/* Returns moving average, relative to maximum.
 * This moving average is based on the sliding window
 * of the last 5 magnitudes.
 * 
 * (this is actually the 'norm' method in Processing library)
 * 
 * avg() delivers a smoothed, auto-calibrated value
 */
int avg() {
  int sum = 0;
  for (int i = 0; i<5; i++) {
    sum += _mags[i];
  }
  return 255*(sum/5)/_max;  
}


/* Returns moving standard deviation, which is based
 * on the sliding window of the last 5 magnitudes.
 * 
 * stdev() delivers a measure of stability.
 */
float stdev() {
  float a = avg();
  float var = 0;
  for(int i = 0; i < 5; i++) {
    float d = (float) 255.0*_mags[i]/_max - a;
    var += d*d;
  }
  return sqrt(var/5);
}


/* Returns an approximation of the first derivative.
 *  
 *  diff() delivers a measure of change (speed);
 *  a positive number indicates 'squeezing',
 *  a negative number indicates 'releasing'.
 */
int diff() {
  float sum = -1*_mags[0];
  sum += 8*_mags[1];
  sum += -8*_mags[3];
  sum += 1*_mags[4];
  return sum/12;
}


/* Inverse of avg()
 */
int inv() {
  return 255-avg();  
}


/* Returns the squared moving average.
 *  
 * square() can be used to emphasize or nuance hard squeezes.
 */
int square() {
  int a = avg();
  return a*a/255;
}


/* Returns the square root moving average.
 *  
 * root() can be used to emphasize or nuance light squeezes.
 */
int root() {
  float a = avg();
  return 255*sqrt(a/255);
}



