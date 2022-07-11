// Setting Digital Pin : 
const uint8_t ledPin = 13;  // Built-in LED
const uint8_t WSHPin = 2;   // WSH Switch IC
const uint8_t WSHPin2 = 3;  // WSH Switch IC
// WSH231 need to connect external pull-high resistor

void setup() { 
  pinMode( WSHPin, INPUT);   // sets the digital pin 2 as Input  
  pinMode( WSHPin2, INPUT);  // sets the digital pin 3 as Input
  pinMode( ledPin, OUTPUT);  // sets the digital pin 13 as Output
}

void loop() {
  if( digitalRead( WSHPin) == 0 || digitalRead( WSHPin2) == 0)
    digitalWrite( ledPin, 1);
  else
    digitalWrite( ledPin, 0);
}
