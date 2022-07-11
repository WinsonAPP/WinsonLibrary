#include <WinsonLib.h>
#define TxPin   2   // DWCS's TxPin
#define RxPin   3   // DWCS's RxPin

// DWCS( TxPin, RxPin, [AC/DC/AT])

DWCS DWCS( TxPin, RxPin, AT);

// Global Variables
double data  = 0;
bool Pass = false;

void setup() {
  // Serial Monitor
  Serial.begin(9600);
  
  // DWCS Initialized
  DWCS.Init();
 
  // Mode Selection
  // DC(): Set Measure Mode to DC. 
  // AC(): Set Measure Mode to AC.
  DWCS.Reset(); // Read RX Buffer to Clear the Buffer  
  #if 1
  Serial.print("DC Mode : ");
  Pass = DWCS.DC();
  #else
  Serial.print("AC Mode : ");
  Pass = DWCS.AC();
  #endif 
  
  if( Pass) Serial.println("Finish!!");
  else Serial.println("Fail!!");
   
  // Power-on Reset
  Serial.print("Reset: ");
  Pass = DWCS.Reset();  
  if( Pass) Serial.println("Finish!!");
  else Serial.println("Fail!!");
}

void loop() {
  // Read Current(mA)
  Serial.print("Current(mA): ");
  data = DWCS.mA();
  Serial.print( String(data,0) + " mA, ");
  
  // Read Current(A)
  data = DWCS.A();
  Serial.print( String(data,3) + " A, ");
  
  // Read Temperature(oC) 
  Serial.print("Temperature(oC): ");
  data = DWCS.oC();
  Serial.print( String(data,1) + " oC, ");
  
  //Read Temperature(oF) 
  data = DWCS.oF();
  Serial.println( String(data,1) + " oF");

  delay(500);
}
