/*
 Name:		WinsonLib.h
 Created:	2022/6/30 上午 11:41:27
 Author:	RD
 Editor:	http://www.visualmicro.com
*/

#ifndef _WinsonLib_h
#define _WinsonLib_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <SoftwareSerial.h>

// WCS Sensitivity (mV per Ampere)
// Built-in Conductor
//(Single)
#define _WCS38A25 7000
#define _WCS37A50 3500
#define _WCS2801  2000
#define _WCS2702  1000
#define _WCS2705  260
#define _WCS2810  135
#define _WCS2720  65
#define _WCS2750  32
#define _WCS3740  32
//(Differential)
#define _WCS2201  4200
#define _WCS2202  1120
#define _WCS2210  280

// Through Hole
//(Single)
#define _WCS2800  70
#define _WCS6800  65
#define _WCS1800  66
#define _WCS1700  33
#define _WCS1600  22
#define _WCS1500  11
//(Differential)
#define _WCS2200  140

enum WType_t {
	AC,
	DC,
	AT,
	ACDC,
	Modbus,
	NaN
};

union BytesArrayConverter {
	byte array[4];
	int32_t ToInt32;
};

struct Wdata_t
{
	WType_t Sign = NaN;
	double Value = 0;
};

enum _mode
{
	SingleOutput,
	DifferentialOutput
};

class WSerial {
	public:
		WSerial(byte,byte);
		void begin();
		String Write(String);
		byte Listen(int);
		String GetRxBuff();
		bool CheckComplete();
		int32_t WriteModbus(byte,byte,word,word);
		void ClearRxBuff();
	private:
		SoftwareSerial	_serial = SoftwareSerial(-1,-1);
		bool			_uif;
		byte			_ubuff[20];
		int				_index;
		word CRC(word,byte);
};
class WCM {
	public:
		WCM(byte,byte,byte,WType_t);
		WCM(byte, byte,byte, WType_t,byte);
		void Init();
		//Continious Function
		Wdata_t SignedCurrent();
		double mA();
		double A();
		bool Reset();
		//ModbusFunction
		double mA(byte);
		double A(byte);

		double oC(byte);
		double oC();

		double oF();
		double oF(byte);

		bool Reset(byte);

		bool SetAddress(byte);
		bool SetAddress(byte, byte);
		bool FactoryReset();

		byte addr();
	private:
		WSerial _serial = WSerial(-1,-1);
		WType_t _type;
		byte _rst;
		byte _addr;
};
class DWCS {
public:
	DWCS(byte, byte, WType_t);
	DWCS(byte, byte, WType_t, byte);

	void Init();
	Wdata_t SignedCurrent();
	double mA();
	double mA(byte);

	double A();
	double A(byte);

	double oC();
	double oC(byte);

	double oF();
	double oF(byte);

	bool Reset();
	bool Reset(byte);

	bool DC();
	bool DC(byte);

	bool AC();
	bool AC(byte);

	bool SetAddress(byte);
	bool SetAddress(byte, byte);
	bool FactoryReset();

	byte addr();
private:
	WSerial _serial = WSerial(-1, -1);
	WType_t _type;
	byte _rx_rst;
	byte _tx;
	byte _addr;
	Wdata_t read_normal();
	Wdata_t read_pro();
};
class WCS
{
public:
	WCS(uint8_t analogPin, uint16_t mVperA);
	WCS(uint8_t analogPin, uint8_t analogPin2, uint16_t mVperA);
	void Reset();
	double A_DC();
	double A_AC();

	//uint32_t _start2;
	//uint32_t _end2;
private:
	uint32_t _start;

	_mode   _Mode;
	uint8_t _AIpin;
	uint8_t _AIpin2;
	int16_t _midPoint;
	uint16_t _sensitivity;
	int16_t _dataScaled[120];

	void ReadADCBuffer(int16_t* rawData);
};

#endif



