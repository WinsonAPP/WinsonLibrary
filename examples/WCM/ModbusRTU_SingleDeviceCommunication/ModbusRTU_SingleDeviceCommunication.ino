#include <WinsonLib.h>
#define TxPin    2   // WCM's TxPin
#define RxPin    3   // WCM's RxPin
#define RstPin   4   // WCM's RstPin

#define SlaveAddress 0x01 // Key in SlaveAddress
#define PodCast_Address 0x00

WCM WCM( TxPin, RxPin, RstPin, Modbus, SlaveAddress);

// Global Variables
double data  = 0;
bool Pass = false;

void setup() {
  // Serial Monitor
  Serial.begin(9600);
  
  // WCM Initialized
  WCM.Init();
  
  // Set Address
  Serial.print("Set Address("); 
  Serial.print( String(SlaveAddress) + "): "); 
  Pass = WCM.SetAddress( PodCast_Address, SlaveAddress); 
  if( Pass) Serial.println("Finish!!");
  else Serial.println("Fail!!");

  // Power-on Reset
  Serial.print("Reset: "); 
  Pass = WCM.Reset();   
  //Pass = WCM.Reset( SlaveAddress);
  if( Pass) Serial.println("Finish!!");
  else Serial.println("Fail!!"); 
}

void loop() {
  // Read Current(mA)
  Serial.print("Current(mA): ");   
  data = WCM.mA();
  Serial.print( String(data,0) + " mA, ");
   
  // Read Current(A)
  data = WCM.A();
  Serial.print( String(data,3) + " A, ");
  
  // Read Temperature(oC) 
  Serial.print("Temperature(oC): ");
  data = WCM.oC();
  Serial.print( String(data,1) + " oC, ");
  
  // Read Temperature(oF) 
  data = WCM.oF();
  Serial.println(String(data,1) + " oF");
}
