#include <WinsonLib.h>
#define TxPin    2   // WCM's TxPin
#define RxPin    3   // WCM's RxPin
#define RstPin   4   // WCM's RstPin

#define PodCast_Address 0x00

WCM WCM( TxPin, RxPin, RstPin, Modbus, PodCast_Address);

// Global Variables
double data  = 0;
bool Pass = false;

void setup() {
  // Serial Monitor
  Serial.begin(9600);
  
  // WCM Initialized
  WCM.Init();
  
  // Power-on Reset
  Serial.print("Reset: ");
  //Pass = WCM.Reset(); 
  Pass = WCM.Reset( PodCast_Address);    
  if(Pass) Serial.println("Finish!!");
  else Serial.println("Fail!!");
}

void loop() {
  for(int i = 1; i < 4; i++)
  {
    // Display Address
    Serial.print("Address: 0x0" + String(i) + ", ");      
    
    // Read Current(mA)
    Serial.print("Current(mA): ");  
    data = WCM.mA(i);
    Serial.print(String(data,0) + " mA, ");
    
    // Read Current(A)
    data = WCM.A(i);
    Serial.print(String(data,3) + " A, ");
    
   // Read Temperature(oC) 
    Serial.print("Temperature(oC): ");
    data = WCM.oC(i);
    Serial.print(String(data,1) + " oC, ");
    
    // Read Temperature(oF) 
    data = WCM.oF(i);
    Serial.println(String(data,1) + " oF");
  }
  Serial.println("====================================================================================");
}
