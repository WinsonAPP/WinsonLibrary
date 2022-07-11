#include <WinsonLib.h>
#define TxPin 2   // DWCS's TxPin
#define RstPin 3  // DWCS's Rst Pin

// DWCS( TxPin, RxPin, [AC/DC/AT])

//DWCS DWCS( TxPin, RstPin, AC);
DWCS DWCS( TxPin, RstPin, DC);

// Global Variables
double data  = 0;
bool Pass = false;

void setup() {
  // Serial Monitor
  Serial.begin( 9600);
  
  // DWCS Initialized
  DWCS.Init();
  
  // Power-on Reset
  Serial.println("Reset");
  DWCS.Reset(); 
}

void loop() {
  // Read Current(mA)
  Serial.print("Current(mA): ");
  data = DWCS.mA();
  Serial.print( String(data,0) + " mA, ");
  
  // Read Current(A)
  data = DWCS.A();
  Serial.println( String(data,3) + " A");
  
  delay(500);
}
