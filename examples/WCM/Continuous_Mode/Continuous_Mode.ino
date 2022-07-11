#include <WinsonLib.h>
#define TxPin 2   // WCM's TxPin
#define RxPin 3   // WCM's RxPin
#define RstPin 4   // WCM's Rst Pin

// WCM( TxPin, RxPin, RstPin, [AC/DC/ACDC/Modbus])
WCM WCM( TxPin, RxPin, RstPin, ACDC);

// Global Variables
double data  = 0;
bool Pass = false;

void setup() {
  // Serial Monitor
  Serial.begin(9600);
  
  // DWCS Initialized
  WCM.Init();
  
  // Power-on Reset
  Serial.println("Reset");
  WCM.Reset();  
}

void loop() {
  // Read Current(mA)
  Serial.print("Current(mA): ");
  data = WCM.mA();
  Serial.print(String(data,0) + " mA, ");
  
  // Read Current(A)
  data = WCM.A();
  Serial.println(String(data,3) + " A");
  
  delay(500);
}
