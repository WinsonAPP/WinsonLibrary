#include <WinsonLib.h>
#define TxPin 2   // DWCS's TxPin
#define RxPin 3   // DWCS's RxPin

#define SlaveAddress 0x01 // Key in SlaveAddress
#define PodCast_Address 0x00

DWCS DWCS( TxPin , RxPin , Modbus , SlaveAddress);

// Global Variables
double data  = 0;
bool Pass = false;

void setup() {
  // Serial Monitor
  Serial.begin(9600);
  
  // DWCS Initialized
  DWCS.Init();
  
  // Set Address
  Serial.print("Set Address("); 
  Serial.print( String(SlaveAddress) + "): "); 
  Pass = DWCS.SetAddress( PodCast_Address, SlaveAddress); 
  if( Pass) Serial.println("Finish!!");
  else Serial.println("Fail!!");
  
  // Mode Selection
  // DC(): Set Measure Mode to DC. 
  // AC(): Set Measure Mode to AC.
  // DC(SlaveAddress): Set DC Mode to Specific SlaveAddress.  ex. DWCS.DC(0X01);
  // AC(SlaveAddress): Set AC Mode to Specific SlaveAddress.  ex. DWCS.AC(0X01);
  
  #if 1
  Serial.print("DC Mode : ");
  Pass = DWCS.DC();
  //Pass = DWCS.DC( SlaveAddress); 
  #else 
  Serial.print("AC Mode : ");
  Pass = DWCS.AC();
  //Pass = DWCS.AC( SlaveAddress); 
  #endif
  
  if( Pass) Serial.println("Finish!!");
  else Serial.println("Fail!!");
  
  // Power-on Reset
  Serial.print("Reset: "); 
  Pass = DWCS.Reset();   
  //Pass = DWCS.Reset( SlaveAddress);   
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
  
  // Read Temperature(oF) 
  data = DWCS.oF();
  Serial.println( String(data,1) + " oF");
}
