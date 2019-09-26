/*
   Reads the status of all electrode pairs and sends this information
   to the serial port.
   The resistance between two electrode pairs is measured through
   a voltage divider and read in on pin A0
   Serial communication works in its simplest way.
   Arduino keeps on sending data.  The PC reads out.
   The hardware includes multiplexers 74HCT4051
   and a buffer with op-amp MCP602.
   The Arduino board needs to generate the Enable
   signal (active low) and the select inputs for
   the multiplexer.
*/

float sum = 0; // variable to store the value read
float magl = 0;
float magr = 0;
float maxl = 0;
float maxr = 0;

void setup() {
  Serial.begin(9600); // baud rate 9600 bps

  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
}

void loop() {
  // Enable the multiplexer

   
   
   /*int x = analogRead(3);

   mag = x;

   if(max < mag) max = mag;
   float norm = 1 - (mag / max);
   float square = norm*norm;

   Serial.println(analogRead(3));

  analogWrite(3, 255.0*square);
  analogWrite(5, 255.0*(1-norm));*/

  sum = 0;
  int x = analogRead(0);
  sum = x*x;

  x = analogRead(1);
  sum += x*x;

  magl = sqrt(sum);

  sum = 0;
  x = analogRead(2);
  sum = x*x;

  x = analogRead(3);
  sum += x*x;

  magr = sqrt(sum);

  if(maxl < magl) maxl = magl;
  if(maxr < magr) maxr = magr;

  //analogWrite(3, 10*(((magl / maxl))+((magr / maxr))));
  analogWrite(5, 255*(1-(magl / maxl)));
  analogWrite(6, 255*(1-(magr / maxr)));
  
  delay(20);
  
}
