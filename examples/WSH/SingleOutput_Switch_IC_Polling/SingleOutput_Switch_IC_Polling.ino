// Setting Digital Pin : 
const uint8_t ledPin = 13;  // Built-in LED
const uint8_t WSHPin = 2;   // WSH Switch IC
// WSH130, WSH130NL, WSH131, WSH134 need to connect external pull-high resistor
 
void setup() {
  // put your setup code here, to run once:  
  pinMode( WSHPin, INPUT);    // sets the digital pin 2 as Input & Pull-high  
  pinMode( ledPin, OUTPUT);   // sets the digital pin 13 as Output
}

void loop() {
  if( digitalRead( WSHPin) == 0)
    digitalWrite( ledPin, 1);
  else
    digitalWrite( ledPin, 0);
}
