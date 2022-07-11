#include <WinsonLib.h>

// Setting Analog Pin & Sensitivity(mV/A)
WCS  WCS1 = WCS( 0, _WCS1700);

double data = 0;

void setup() {
  Serial.begin(9600);

  // Power-on Reset
  Serial.println("Reset");
  WCS1.Reset(); 
}

void loop() {
  // Measure AC Current
  data = WCS1.A_AC();
  Serial.print("Current(A) : ");
  Serial.println(String(data,3) + " A");
  
  delay(500);
}
