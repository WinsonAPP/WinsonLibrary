/*
 Name:		WinsonLib.cpp
 Created:	2022/6/30 上午 11:41:27
 Author:	RD
 Editor:	http://www.visualmicro.com
*/

#include "WinsonLib.h"

#pragma region WSerial
WSerial::WSerial(byte Rx, byte Tx) {
	this->_serial = SoftwareSerial(Rx, Tx);
}
void WSerial::begin() {
	this->_serial.end();
	this->_serial.begin(9600);
	delay(1000);
}
String WSerial::Write(String Text) {
	this->_serial.print(Text);
	//Wait until RX recieved data 
	ClearRxBuff();
	Listen(2000);
	//if Rx Recieved Completed
	if (_uif) {
		return GetRxBuff();
	}
	else return "Err";
}
int32_t WSerial::WriteModbus(byte SlaveAddress, byte FunctionCode, word DeviceAddress, word RegisterNum) {
	word crc = 0xffff;//CRC Initial  
	byte WriteData[8];//TX Data Buffer  
	//SlaveAddress : if use Podcast Address(0x00), DWCS will Not Response. 
	WriteData[0] = SlaveAddress;

	//FunctionCode[ Read(03H)\ Write(06H)]
	WriteData[1] = FunctionCode;

	//DeviceAddress 
	WriteData[2] = highByte(DeviceAddress);
	WriteData[3] = lowByte(DeviceAddress);

	//RegisterNumber
	WriteData[4] = highByte(RegisterNum);
	WriteData[5] = lowByte(RegisterNum);

	//CRC Code Calculation
	for (int i = 0; i < 6; i++) crc = CRC(crc, WriteData[i]);
	WriteData[6] = lowByte(crc);
	WriteData[7] = highByte(crc);

	//Send Command to DWCS
	for (int i = 0; i < 8; i++) _serial.write(WriteData[i]);

	//Recieve Data To RxBuff
	ClearRxBuff();
	Listen(1000);

	byte RxBuffCache[20];
	int CacheIndex = _index;
	//SaveDataToRxCache & Reset Rxindex
	for (int j = 0; j < CacheIndex; j++) RxBuffCache[j] = _ubuff[j];

	//Start Verify Recieved Data. 
	int i = 0;
	bool start = false;
	BytesArrayConverter Converter;

	//Search SlaveAddress
	for (i = 0; i < CacheIndex; i++) {
		if (RxBuffCache[i] == SlaveAddress) {
			crc = CRC(0xffff, RxBuffCache[i]);
			start = true;
			break;
		}
	}
	if (!start) return 0;

	// Check FunctionCode
	if (RxBuffCache[i + 1] == FunctionCode) {
		crc = CRC(crc, RxBuffCache[i + 1]);
	}
	else {
		//ErrCode
		return 0;
	}

	//Write 06H / Read 03H
	if (FunctionCode == 0x06) {
		for (int j = 0; j < 6; j++) {
			if (WriteData[2 + j] != RxBuffCache[i + 2 + j]) {
				return 0;
			}
		}
		return  RegisterNum;
	}
	else if (FunctionCode == 0x03) {
		if (RxBuffCache[i + 2] != 0x04) return 0;
		Converter.array[3] = RxBuffCache[i + 3];
		Converter.array[2] = RxBuffCache[i + 4];
		Converter.array[1] = RxBuffCache[i + 5];
		Converter.array[0] = RxBuffCache[i + 6];

		for (int j = 2; j < 7; j++) 	crc = CRC(crc, RxBuffCache[i + j]);

		if (lowByte(crc) != RxBuffCache[i + 7]) return 0;
		if (highByte(crc) != RxBuffCache[i + 8]) return 0;

		return Converter.ToInt32;

	}
	else return 0;
}
byte WSerial::Listen(int ms) {
	_index = 0;
	_uif = false;
	while (ms > 0) {
		while (this->_serial.available()) {
			// get the new byte:
			_ubuff[_index++] = this->_serial.read();
			if (_ubuff[_index-1] == '\n') {
				_uif = true;
				return _index;
			}
		}
		delay(1);
		ms--;
	}
	return _index;
}
word WSerial::CRC(word temp_crc, byte Buff)
{
	byte i;
	temp_crc = temp_crc ^ Buff; // XOR
	for (i = 0; i < 8; i++)
	{
		if ((temp_crc & 0x0001) == 1)
		{
			temp_crc >>= 1; // Shift
			temp_crc ^= 0xA001; // XOR
		}
		else
		{
			temp_crc >>= 1; // Shift
		}
	}
	return temp_crc;
}
String WSerial::GetRxBuff() {
	return String((char*)_ubuff).substring(0,_index);
}
bool WSerial::CheckComplete() {
	return _uif;
}
void WSerial::ClearRxBuff() {
	this->_serial.end();
	this->_serial.begin(9600);
}
#pragma endregion
#pragma region WCM
WCM::WCM(byte Tx, byte Rx, byte Rst, WType_t Type) {
	_type = Type;
	_serial = WSerial(Tx,Rx);
	_rst = Rst;
	_addr = 0x01;
}
WCM::WCM(byte Tx, byte Rx, byte Rst, WType_t Type,byte Address) {
	_type = Type;
	_serial = WSerial(Tx, Rx);
	_rst = Rst;
	if (Address == 0x00)_addr = 0x01;
	else _addr = Address;
}
void WCM::Init() {
	//Initial reset pin
	pinMode(_rst, OUTPUT);
	digitalWrite(_rst, HIGH);
	//InitialSerial
	_serial.begin();
}
byte WCM::addr() {
	return _addr;
}
double WCM::A() {
	return mA() / 1000;
}
double WCM::mA() {
	if (_type == Modbus) return mA(_addr);
	Wdata_t d = SignedCurrent();
	return d.Value;
}
Wdata_t WCM::SignedCurrent() {
	Wdata_t d;	
	byte c = 0;
	if (_type == Modbus) {
		d.Sign = NaN;
		d.Value = mA(_addr);
		return d;
	}
	_serial.ClearRxBuff();
	for (int i = 0; i < 5; i++) if (_serial.Listen(1000) == 8) break;

	if (_serial.CheckComplete()) {
		String Recieved = _serial.GetRxBuff();
		if (Recieved.indexOf("~") != -1) {
			Recieved.replace("~", "");
			d.Sign = WType_t::AC;
		}
		else if (Recieved.indexOf("+") != -1) {
			Recieved.replace("+", "");
			d.Sign = WType_t::DC;
		}
		else if (Recieved.indexOf("-") != -1) {
			d.Sign = WType_t::DC;
		}
		else return d;
		Recieved.replace("\r\n", "");
		d.Value = Recieved.toDouble();

		return d;
	}
	return d;
}
bool WCM::Reset() {
	if (_type == Modbus) return Reset(_addr);
	digitalWrite(_rst, LOW);
	delay(1000);
	digitalWrite(_rst, HIGH);
	return true;
}
//Modbus Mode
double WCM::mA(byte SlaveAddress) {
	int32_t d = _serial.WriteModbus(SlaveAddress, 0x03, 0x0002, 0x0002);
	return (double)d;
}
double WCM::A(byte SlaveAddress) {
	return  mA(SlaveAddress)/1000;
}
double WCM::oC() {
	return oC(_addr);
}
double WCM::oC(byte SlaveAddress) {
	int32_t d = _serial.WriteModbus(SlaveAddress, 0x03, 0x0004, 0x0002);
	return (double)d / 10;
}
double WCM::oF() {
	return oC() * 1.8 + 32;
}
double WCM::oF(byte SlaveAddress) {
	return oC(SlaveAddress) * 1.8 + 32;
}
bool WCM::Reset(byte SlaveAddress) {
	int32_t d = _serial.WriteModbus(SlaveAddress, 0x06, 0x0000, 0x0100);
	if (d == 256 || SlaveAddress == 0x00) return true;
	else return false;
}
bool WCM::SetAddress(byte NewAddress) {
	if (SetAddress(_addr, NewAddress)) {
		_addr = NewAddress;
		return true;
	}
	return false;	
}
bool WCM::SetAddress(byte OldAddress, byte NewAddress) {
	if (_type != Modbus || NewAddress == 0x00) return false;
	int32_t d = _serial.WriteModbus(OldAddress, 0x06, 0x0010, NewAddress);
	if (d == NewAddress || OldAddress == 0x00) return true;
	else return false;
}
bool WCM::FactoryReset() {
	return SetAddress(0x00,0x01);
}

#pragma endregion
#pragma region DWCS
DWCS::DWCS(byte TxPin, byte Rx_RstPin, WType_t Type) {
	_type = Type;
	_rx_rst = Rx_RstPin;
	_tx = TxPin;
	_addr = 0x01;
}
DWCS::DWCS(byte TxPin, byte Rx_RstPin, WType_t Type, byte SlaveAddress) {
	_type = Type;
	_rx_rst = Rx_RstPin;
	_tx = TxPin;
	_addr = SlaveAddress;
}
void DWCS::Init() {
	switch (this->_type)
	{
	case WType_t::DC:
	case  WType_t::AC:
		_serial = WSerial(_tx, -1);
		pinMode(_rx_rst, OUTPUT);
		digitalWrite(_rx_rst, HIGH);
		break;
	case  WType_t::AT:
	case  WType_t::Modbus:
		_serial = WSerial(_tx, _rx_rst);
		break;
	default:
		break;
	}
	_serial.begin();
}
byte DWCS::addr() {
	return _addr;
}
Wdata_t DWCS::read_normal() {
	Wdata_t d;
	d.Sign = NaN;
	d.Value = 0;

	_serial.ClearRxBuff();
	for (int i = 0; i < 5; i++) if (_serial.Listen(1000) == 8) break;
	if (_serial.CheckComplete()) {
		String Recieved = _serial.GetRxBuff();
		if (Recieved.indexOf("~") != -1) {
			Recieved.replace("~", "");
			d.Sign = WType_t::AC;
		}
		else d.Sign = WType_t::DC;

		if (Recieved.indexOf("+") != -1)Recieved.replace("+", "");
		Recieved.replace("\r\n", "");
		d.Value = Recieved.toDouble();

		return d;
	}
	return d;
}
Wdata_t DWCS::read_pro() {
	Wdata_t d;
	d.Sign = NaN;
	d.Value = 0;
	String Recieved = _serial.Write("AT+MEAS\r\n");

	if (Recieved.indexOf("~") != -1) {
		Recieved.replace("~", "");
		d.Sign = WType_t::AC;
	}
	else d.Sign = WType_t::DC;

	Recieved.replace("\r\n", "");
	d.Value = Recieved.toDouble();
	return d;
}
double DWCS::A() {
	switch (_type)
	{
		case WType_t::AC:
		case WType_t::DC:
			return 	read_normal().Value;
		case WType_t::AT:
			return 	read_pro().Value;
		case WType_t::Modbus:
			return A(_addr);
		default:
			return 0;
	}
}
double DWCS::mA() {
	return A() * 1000;
}
Wdata_t DWCS::SignedCurrent() {
	Wdata_t d;
	d.Sign = NaN;
	d.Value = 0;
	switch (_type)
	{
		case WType_t::AC:
		case WType_t::DC:
			return 	read_normal();
		case WType_t::AT:
			return 	read_pro();

			d.Sign = NaN;
			d.Value = A(_addr);
			return d;

		default:
			return d;
	}
}
bool DWCS::Reset() {
	switch (_type)
	{
		case WType_t::AC:
		case WType_t::DC:
			digitalWrite(_rx_rst, LOW);
			delay(1000);
			digitalWrite(_rx_rst, HIGH);

		case WType_t::AT:
			if (_serial.Write("AT+RST\r\n") == "OK\r\n") return true;
			else return false;
		case WType_t::Modbus:
			return Reset(_addr);
		default:
			return false;
	}
}
double DWCS::oC() {
	if (_type == Modbus) return oC(_addr);

	String Recieved = _serial.Write("AT+TEMP\r\n");
	Recieved.replace("\r\n", "");
	return Recieved.toDouble();
}
double DWCS::oF() {
	return oC() * 1.8 + 32;
}
bool DWCS::DC() {
	if (_type == Modbus) return DC(_addr);

	if (_serial.Write("AT+CURR,0\r\n") == "OK\r\n") return true;
	else return false;
}
bool DWCS::AC() {
	if (_type == Modbus)return AC(_addr);

	if (_serial.Write("AT+CURR,1\r\n") == "OK\r\n") return true;
	else return false;
}
//Modbus Mode
double DWCS::mA(byte SlaveAddress) {
	int32_t d = _serial.WriteModbus(SlaveAddress, 0x03, 0x0002, 0x0002);
	return (double)d;
}
double DWCS::A(byte SlaveAddress) {
	return  mA(SlaveAddress) / 1000;
}
double DWCS::oC(byte SlaveAddress) {
	int32_t d = _serial.WriteModbus(SlaveAddress, 0x03, 0x0004, 0x0002);
	return (double)d / 10;
}
double DWCS::oF(byte SlaveAddress) {
	return oC(SlaveAddress) * 1.8 + 32;
}
bool DWCS::Reset(byte SlaveAddress) {
	int32_t d = _serial.WriteModbus(SlaveAddress, 0x06, 0x0000, 0x0100);
	if (d == 256 || SlaveAddress == 0x00) return true;
	else return false;
}
bool DWCS::SetAddress(byte NewAddress) {
	if (SetAddress(_addr, NewAddress)) {
		_addr = NewAddress;
		return true;
	}
	return false;
}
bool DWCS::SetAddress(byte OldAddress, byte NewAddress) {
	if (_type != Modbus || NewAddress == 0x00) return false;
	int32_t d = _serial.WriteModbus(OldAddress, 0x06, 0x0010, NewAddress);
	if (d == NewAddress || OldAddress == 0x00) return true;
	else return false;
}
bool DWCS::FactoryReset() {
	return SetAddress(0x00, 0x01);
}
bool DWCS::DC(byte SlaveAddress) {
	int32_t d = _serial.WriteModbus(SlaveAddress, 0x06, 0x0020, 0x0000);
	if (d == 0 || SlaveAddress == 0x00) return true;
	else return false;
}
bool DWCS::AC(byte SlaveAddress) {
	int32_t d = _serial.WriteModbus(SlaveAddress, 0x06, 0x0020, 0x0001);
	if (d == 1 || SlaveAddress == 0x00) return true;
	else return false;
}


#pragma endregion
#pragma region WCS
// Setting:Single Output Current 
WCS::WCS(uint8_t analogPin, uint16_t mVperA)
{
	_Mode = SingleOutput;
	_AIpin = analogPin;
	_sensitivity = mVperA;
	_midPoint = 512;
}
// Setting:Differential Output Current
WCS::WCS(uint8_t analogPin, uint8_t analogPin2, uint16_t mVperA)
{
	_Mode = DifferentialOutput;
	_AIpin = analogPin;
	_AIpin2 = analogPin2;
	_sensitivity = mVperA;
	_midPoint = 0;
}
// Current Reset
void WCS::Reset()
{
	int32_t dataSum;

	ReadADCBuffer(_dataScaled);

	dataSum = 0;
	for (int i = 0; i < 120; i++)
	{
		dataSum += _dataScaled[i];
	}
	_midPoint = dataSum / 120;
}
// Measure DC Current
double WCS::A_DC()
{
	int16_t steps;
	if (_Mode == SingleOutput) // Single Mode
	{
		analogRead(_AIpin); //  read analog pin to stabilize the ADC  
		steps = analogRead(_AIpin) - _midPoint;
	}
	else if (_Mode == DifferentialOutput) // Differential Mode
	{
		analogRead(_AIpin); //  read analog pin to stabilize the ADC  
		analogRead(_AIpin2); //  read analog pin to stabilize the ADC 
		steps = analogRead(_AIpin) - analogRead(_AIpin2) - _midPoint;
	}
	return (double)steps * 5 / 1023 / ((double)_sensitivity * 0.001);
}
// Measure AC Current
double WCS::A_AC()
{
	int16_t steps;
	double steps2;
	double dataSum;

	ReadADCBuffer(_dataScaled);

	dataSum = 0.0;
	for (int i = 0; i < 120; i++)
	{
		steps = _dataScaled[i] - _midPoint;
		steps2 = ((double)steps * 5 / 1023) / ((double)_sensitivity * 0.001);
		dataSum += sq(steps2);
	}
	return sqrt(dataSum / 120);
}
// ADC Buffer
void WCS::ReadADCBuffer(int16_t* rawData)
{
	analogRead(_AIpin); //  read analog pin to stabilize the ADC 
	analogRead(_AIpin2); //  read analog pin to stabilize the ADC 

	if (_Mode == SingleOutput) // Single Mode
	{
		/*
		for (int i = 0; i < 120; i++) // Configure by Sampling(120 points) for 2 cycles of AC
		{
			rawData[i] = analogRead(_AIpin);
			delayMicroseconds(716); // Sample Rate = 1200Hz
		}
		*/
		//_start2 = micros();
		for (int i = 0; i < 120; i++)
		{
			_start = micros();
			rawData[i] = analogRead(_AIpin);
			while (micros() - _start < 829); // Sample Rate = 1200Hz
		}
		//_end2 = micros();
	}
	else if (_Mode == DifferentialOutput) // Differential Mode
	{
		//_start2 = micros();
		for (int i = 0; i < 120; i++)
		{
			_start = micros();
			rawData[i] = analogRead(_AIpin) - analogRead(_AIpin2);
			while (micros() - _start < 829); // Sample Rate = 1200Hz
		}
		//_end2 = micros();
	}
}
#pragma endregion

