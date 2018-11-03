//
//    Copyright 2004, Thomas C. McDermott, N5EG
//    This file is part of VNAR - the Vector Network Analyzer program.
//
//    VNAR is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    VNAR is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with VNAR, if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#pragma once
// Class to encapsulate the EZUSB device interface

// Only the functions are disclosed here because .NET has a conflict
// with header files.
// #including the "objbase.h" file in the main .NET file causes it not to 
// compile due to Interface conflicts. But "objbase.h" is required to access
// DeviceIoControl and USB definitions. So it's in the USB_EZ_interface.cpp
// CPP file, not here in the header. This header gets #included in the main
// program. VNADevice had to be redesigned so that it does not expose any
// variables or functions (private or public) that require the objbase.h
// header to define. All those functions are localized inside the methods
// of VNADevice in the .cpp file only. Yuck.
// Slight improvement: all the messy variables are localized inside a Helper
// function, which only gets mentioned in the .H file via a forward reference
// (thus hiding it's internals).


#include "stdafx.h"
#include "DataDisplay.h"
#using <mscorlib.dll>

extern volatile float magSig;
extern volatile float phaseSig;
extern volatile float volSig;

#define DIR_TRANS	0
#define DIR_REFL	1

	/// VNA Device wrapper. Holds state and device handle
public ref class VNADevice
{
private:
	bool Result;					// DeviceIoControl result
	int state;						// -1=no device +1=device OK
	int mode;
	//int mp;
	long lastFreq;
	int lastDir;
	int cable_before;
	int cable_after;
	int resistance;
	int capacitance;
	int inductance;
	int dur;						// total duration of one side signal
	System::IO::Ports::SerialPort^  serialPort;

	//	long int IICErrorCount;			// Temporary IIC error counter

	/*__nogc*/ class Helper * d;				// holds the USB device state

	void GetHandle(void);
	void ReleaseHandle(void);
	bool ToggleReset(bool hold);

public:
	VNADevice(System::IO::Ports::SerialPort^  port);					// Constructor: open device, set state
	~VNADevice();					// Destructor: release __nogc objects and structs
	bool Init(void);				// Build descriptors, get pipes
	int State();				// -1 = no device  +1 = device OK
	bool Start();					// Release reset of the 8051 processor on VNA
	bool Stop();					// Halt the 8051 processor on VNA
	bool Sweep(long startF, long stepF, int numPoints, int duration, int power);
	void Sweep(long startF, long stepF, int numPoints, int duration);
	void SetFreq(long startF, int direction);
	bool Download(array<System::Byte>^ Codebuffer, int CodeSize, unsigned short Address );	// download code to 8051
	bool Read(VNA_RXBUFFER * readbuf);		// Read 64 bytes of data from BULK_IN_EP2 - it blocks the
									// calling thread, but the VNA always tries to have a buffer
									// ready so it should never actually block.
									// If 1st byte is 0xff or 0xfe, it's a dummy return,
									// If 1st byte is 0x01, it's a valid data block
									// If 2nd byte is not 0x00, there was an I2C error on the 8051
	bool Write(VNA_TXBUFFER * writebuf);		// Write 64 bytes of data to BULK_OUT_EP2 endpoint
	bool WriteRead(VNA_TXBUFFER * TxBuffer, VNA_RXBUFFER * RxBuffer, int direction);	// Write data buffer then retrieve Rx buffer
	void SetMode(int m);
	void SetBefore(int l);
	void SetAfter(int l);
	void SetResistance(int v);
	void SetCapacitance(int v);
	void SetInductance(int v);
};
