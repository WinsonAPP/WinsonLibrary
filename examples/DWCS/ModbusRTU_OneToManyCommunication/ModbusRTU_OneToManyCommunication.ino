#include <WinsonLib.h>
#define TxPin 2   // DWCS's TxPin
#define RxPin 3   // DWCS's RxPin

#define PodCast_Address 0x00

DWCS DWCS(TxPin ,RxPin ,Modbus ,PodCast_Address);

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
  // DC(SlaveAddress): Set DC Mode to Specific SlaveAddress.  ex. DWCS.DC(0X01);
  // AC(SlaveAddress): Set AC Mode to Specific SlaveAddress.  ex. DWCS.AC(0X01);

  #if 1
  Serial.print("DC Mode: ");
  //Pass = DWCS.DC(); 
  Pass = DWCS.DC( PodCast_Address); 
  #else 
  Serial.print("AC Mode: ");
  //Pass = DWCS.AC(); 
  Pass = DWCS.AC( PodCast_Address); 
  #endif
  
  if( Pass) Serial.println("Finish!!");
  else Serial.println("Fail!!");
  
  // Power-on Reset
  Serial.print("Reset: ");
  Pass = DWCS.Reset(); 
  //Pass = DWCS.Reset( PodCast_Address);  
  if( Pass) Serial.println("Finish!!");
  else Serial.println("Fail!!");
}

void loop() { 
  for(int i = 1; i < 4 ;i++)
  {
    // Display Address
    Serial.print("Address: 0x0" + String(i) + ", ");    
    
    // Read Current(mA)
    Serial.print("Current(mA): ");    
    data = DWCS.mA(i);
    Serial.print(String(data,0) + " mA, ");
    
    // Read Current(A)
    data = DWCS.A(i);
    Serial.print(String(data,3) + " A, ");
    
    // Read Temperature(oC) 
    Serial.print("Temperature(oC): ");
    data = DWCS.oC(i);
    Serial.print(String(data,1) + " oC, ");
    
    // Read Temperature(oF) 
    data = DWCS.oF(i);
    Serial.println(String(data,1) + " oF");
  }
  Serial.println("====================================================================================");
}
