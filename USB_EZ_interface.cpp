//
//    Copyright 2004-2007, Thomas C. McDermott, N5EG
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
/// \author Thomas C. McDermott

#pragma once
// Encapsulate the EZUSBSYS device as a .NET managed object
//
// Updated: 4-4-03 to use pinned pointer for code download
//
// Updated: 7-8-03 for the .NET 2003 compiler to avoid exposing
// any methods or variables to the caller via the header file.
// It ends up being messy as a result...
//
// 2-22-04 MS VC++ posting says that using command switch
// /d1PrivateNativeTypes  will revert to 2002 behavior of not making
// the native types (unmanaged types) public. This might avoid the
// problem. KB article #822330
//
// Updated: 3-10-04 to use Helper class and forward reference in the .H
// file to sidestep the problem, pin the entire Helper object with one pointer
//


#include "stdafx.h"
#include "objbase.h"
#include "DataDisplay.h"
#include "Mockup.h"
#include "Constants.h"
#include "AudioInput.h"
#using <mscorlib.dll>
#include <math.h>
#include <cstdio>

using namespace System;
	using namespace System::Windows::Forms;
//volatile float magSig;
//volatile float phaseSig;


extern "C"
{
    // Declare USB constants and structures
	#include "winioctl.h"				/// IOCTL definitions
//	#include "c:\ntddk\inc\usb100.h"	/// general USB
#pragma warning(disable: 4200)			// Supress C4200 warning in Microsoft-supplied headers
//	#include "c:\ntddk\inc\usbdi.h"		// USB Device Interface
#pragma warning(default: 4200)
//	#include "c:\ntddk\inc\devioctl.h"	/// Device IOCTL definitions
//	#include "C:\Cypress\USB\Drivers\ezusbdrv\ezusbsys.h"  /// Ezusb IOCTL codes
}
#include "USB_EZ_interface.h"
#define USB_STRING			char[256]
#define USB_CONFIG_DESCR	unsigned short[1024]


#define DDSToFreq(X) (long)(((double)(X)  * 288000000.0 / (4294967296.0 * 65536.0 ) + 0.5 ) )

/// Holds the USB device state while hiding it from other code
/// Helper is not on the managed heap, and requires explicit lifetime control.
/// Neither it nor it's contents get moved in memory by the garbage collector
/*__nogc*/ class Helper				
#define 	BULK_TRANSFER_CONTROL int			
#define USB_DEVICE_DESCRIPTOR int
#define EZUSB_DRIVER_VERSION int
#define GET_STRING_DESCRIPTOR_IN char
#define VENDOR_REQUEST_IN int
{
public:
	BULK_TRANSFER_CONTROL * pInPipe;	///< USB Bulk Transfer Input Pipe
	BULK_TRANSFER_CONTROL * pOutPipe;	///< USB Bulk Transfer Output Pipe
	USB_DEVICE_DESCRIPTOR * pDevDescr;	///< USB Device Descriptor
	EZUSB_DRIVER_VERSION * pDrvVers;	///< EZUSB Driver Version
	GET_STRING_DESCRIPTOR_IN * pStrDescr;	///< VNA Device String Descriptor
	UCHAR * pInterfaceInfo;				///< Interface Information Structure
	unsigned short * pConfigDescr;		///< Configuration Descriptor
	char * pDevString;					///< USB Device Identifier, string version
	HANDLE DevDrvHandle;				///< Device Handle
	LPDWORD pBytesReturned;				///< Size of return buffer from EzUSB calls

	Helper()
	{
		pInPipe = new BULK_TRANSFER_CONTROL;
		pOutPipe = new BULK_TRANSFER_CONTROL;
		pDevDescr = new USB_DEVICE_DESCRIPTOR;
		pDrvVers = new EZUSB_DRIVER_VERSION;
		pStrDescr = new GET_STRING_DESCRIPTOR_IN;
		pInterfaceInfo = new UCHAR[1024];
		pDevString = new USB_STRING;
		pConfigDescr = new USB_CONFIG_DESCR;
		pBytesReturned = new DWORD;
	}
	~Helper()
	{
		delete pInPipe;
		delete pOutPipe;
		delete pDevDescr;
		delete pDrvVers;
		delete pStrDescr;
		delete pInterfaceInfo;
		delete pDevString;
		delete pConfigDescr;
		delete pBytesReturned;
	}
};

	/// Find USB device, acquire handle
void VNADevice::GetHandle(void)
	{
//		HANDLE hDevice;

//		char __pin * usbdev0 = "\\\\.\\ezusb-0";		// device 0
//		char __pin * usbdev1 = "\\\\.\\ezusb-1";		// device 1

		pin_ptr<char> usbdev0 = "\\\\.\\ezusb-0";		// device 0
		pin_ptr<char> usbdev1 = "\\\\.\\ezusb-1";		// device 1

#if 0
		hDevice = CreateFile(usbdev0,		// try device 0
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);

		if (hDevice==INVALID_HANDLE_VALUE)
		{
			hDevice = CreateFile(usbdev1,	// try device 1
				GENERIC_WRITE,
				FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				0,
				NULL);
		}

		if (hDevice==INVALID_HANDLE_VALUE)			
			state = -1;						// open failed
		else
#endif
			state = 1;						// open succeded

//		d->DevDrvHandle = hDevice;
};

	/// Release handle to USB device
void VNADevice::ReleaseHandle(void)
	{
#if 0
		if (state == 1)
			CloseHandle(d->DevDrvHandle);
#endif
};

	/// Set or Release RESET on the Ez-USB processor
bool VNADevice::ToggleReset(bool hold)
	{
		// use the vendor request type to set/release the reset register in the 8051

//		VENDOR_REQUEST_IN __pin * pRequest = new VENDOR_REQUEST_IN;

		pin_ptr<VENDOR_REQUEST_IN> pRequest = new VENDOR_REQUEST_IN;
#if 0
		pRequest->bRequest = 0xA0;			// Anchorchips Vendor Request Type
		pRequest->wValue = CPUCS_REG_EZUSB;		// 8051 Control / Status Register
		pRequest->wIndex = 0x00;
		pRequest->wLength = 0x01;
		pRequest->bData = (hold) ? 1 : 0;	// 1 holds 8051 in reset, 0 starts 8051 (at 0x0000)
		pRequest->direction = 0x00;

		GetHandle();

		Result = (DeviceIoControl((HANDLE)d->DevDrvHandle,
			IOCTL_Ezusb_VENDOR_REQUEST,
			pRequest,
			sizeof(VENDOR_REQUEST_IN),
			NULL,
			0,
			d->pBytesReturned,
			NULL) !=0);	// casts BOOL (which is typedef'd to int) to bool

		ReleaseHandle();
#endif
//		return(Result);
	return(true);
};

	/// Construct the VNADevice
VNADevice::VNADevice(System::IO::Ports::SerialPort^  port)							
{
	d = new Helper;					// Allocate a Helper to the VNADevice
	GetHandle();
	serialPort = port;
	ReleaseHandle();
	mode = 0;
	lastFreq = 0;
	lastDir = -1;
	hardware = HW_MOCKUP;
	hasScanCommand = false;
	minHWFreq = 500000;
	maxHWFreq = 900000000;
	audioRefLevel = 0.0;
	S11Real = gcnew array<Double>(1200);
	S11Imag = gcnew array<Double>(1200);
	S21Real = gcnew array<Double>(1200);
	S21Imag = gcnew array<Double>(1200);
};
	/// Destructor for VNADevice
VNADevice::~VNADevice()
{
	delete d;						// since d is on the unmanaged heap
}
	/// Build Device Descriptors and Pipes for USB device
bool VNADevice::Init(void)						
{
	GetHandle();

//	this->SetFreq(50000000L, true);

// Code between these two markers is new experimental code

	//Result = DeviceIoControl((HANDLE)d->DevDrvHandle,
	//	IOCTL_Ezusb_GET_DEVICE_DESCRIPTOR,
	//	NULL,
	//	0,
	//	d->pDevDescr,
	//	sizeof(USB_DEVICE_DESCRIPTOR),
	//	d->pBytesReturned,
	//	NULL);

	//Result = DeviceIoControl((HANDLE)d->DevDrvHandle,
	//	IOCTL_EZUSB_GET_DRIVER_VERSION,
	//	NULL,
	//	0,
	//	d->pDrvVers,
	//	sizeof(EZUSB_DRIVER_VERSION),
	//	d->pBytesReturned,
	//	NULL);

	//Result = DeviceIoControl((HANDLE)d->DevDrvHandle,
	//	IOCTL_Ezusb_GET_CONFIGURATION_DESCRIPTOR,
	//	NULL,
	//	0,
	//	d->pConfigDescr,
	//	sizeof(USB_CONFIG_DESCR),
	//	d->pBytesReturned,
	//	NULL);


	//d->pStrDescr->Index = 1;
	//d->pStrDescr->LanguageId = 27;

	//Result = DeviceIoControl((HANDLE)d->DevDrvHandle,	// gets a UNICODE string
	//	IOCTL_Ezusb_GET_STRING_DESCRIPTOR,
	//	d->pStrDescr,
	//	sizeof(GET_STRING_DESCRIPTOR_IN),
	//	d->pDevString,
	//	sizeof(USB_STRING),
	//	d->pBytesReturned,
	//	NULL);

	//Result = DeviceIoControl((HANDLE)d->DevDrvHandle,
	//	IOCTL_Ezusb_GET_PIPE_INFO,
	//	NULL,
	//	0,
	//	d->pInterfaceInfo,
	//	sizeof(d->pInterfaceInfo),
	//	d->pBytesReturned,
	//	NULL);

	// BUGBUG Something before this point corrupts memory and crashes Windows - thus it's removed

	//// The following code has not been modified to the pinned object style of coding...
	////pPipeInfo = (PUSBD_INTERFACE_INFORMATION) pInterfaceInfo;

	////pInPipe->pipeNum = 0;			// most likely
	////pOutPipe->pipeNum = 1;		// most likely

	////for (int i=0; i<static_cast<int>(pPipeInfo->NumberOfPipes); i++)		// Identify pipes we need
	////{
	////	if (pPipeInfo->Pipes[i].EndpointAddress == 0x02) //EP2-Out
	////		pOutPipe->pipeNum = i;
	////	if (pPipeInfo->Pipes[i].EndpointAddress == 0x82) //EP2-In
	////		pInPipe->pipeNum = i;
	////}

// End experimental code marker

	ReleaseHandle();

	//return(i!=0);					// success if #pipes > zero
	return(true); // guarantee success
};
	/// State of USB device
int VNADevice::State() {return state;};

	/// Release reset on the 8051 processor
bool VNADevice::Start() { return(ToggleReset(0)); };		
	/// Halt the 8051 processor
bool VNADevice::Stop()	 { return(ToggleReset(1)); };		
	/// Download code to 8051 at Address
bool VNADevice::Download(array<System::Byte> ^Codebuffer, int CodeSize, unsigned short Address )	
{
//	ANCHOR_DOWNLOAD_CONTROL __pin * pDownAddr = new ANCHOR_DOWNLOAD_CONTROL;


//	unsigned char __pin *pB = &Codebuffer[0];	// pB is a pinned pointer (doesn't get moved in memory).
												// It's needed when passing a pointer to an unmanaged external DLL.
												// Codebuffer gets unpinned when pB goes out of scope, or
												// gets assigned a value of zero.

#if 0	
	pin_ptr<ANCHOR_DOWNLOAD_CONTROL> pDownAddr = new ANCHOR_DOWNLOAD_CONTROL;
	pin_ptr<System::Byte> pB = &Codebuffer[0];

	pDownAddr->Offset = Address;

	GetHandle();

	Result = (DeviceIoControl((HANDLE)d->DevDrvHandle,
		IOCTL_EZUSB_ANCHOR_DOWNLOAD,
		pDownAddr,
		sizeof(ANCHOR_DOWNLOAD_CONTROL),
		(LPVOID)pB,								
		CodeSize,										
		d->pBytesReturned,
		NULL) !=0);	// casts BOOL (which is typedef'd to int) to bool

	ReleaseHandle();
#endif
	return(true);
//	return(Result);
};
	/// Read data from BULK endpoint
bool VNADevice::Read(VNA_RXBUFFER * readbuf)				
{
//	void __pin * rb = readbuf;			// pin the readbuf in memory
#if 0	
	pin_ptr<VNA_RXBUFFER> rb = readbuf;			// pin the readbuf in memory

	d->pInPipe->pipeNum = 0;			// most likely

	GetHandle();

	Result = (DeviceIoControl((HANDLE)d->DevDrvHandle,
		IOCTL_EZUSB_BULK_READ,
		d->pInPipe,
		sizeof(BULK_TRANSFER_CONTROL),
		rb,								// readbuf
		64,
		d->pBytesReturned,
		NULL) !=0);	// casts BOOL (which is typedef'd to int) to bool

	ReleaseHandle();
#endif
//	return(Result);
	return(true);
};
	/// Write data to BULK endpoint
bool VNADevice::Write(VNA_TXBUFFER * writebuf)					
{
//	void __pin * wb = writebuf;				// pin the writebuf in memory

#if 0	
	pin_ptr<VNA_TXBUFFER> wb = writebuf;	// pin the writebuf in memory

	d->pOutPipe->pipeNum = 1;				// most likely

	GetHandle();

	Result = (DeviceIoControl((HANDLE)d->DevDrvHandle,
		IOCTL_EZUSB_BULK_WRITE,
		d->pOutPipe,
		sizeof(BULK_TRANSFER_CONTROL),
		wb,									// writebuf
		64,
		d->pBytesReturned,
		NULL) !=0);	// casts BOOL (which is typedef'd to int) to bool

	ReleaseHandle();
#endif
//	return(Result);
	return(true);
};

void VNADevice::Sweep(__int64 startF, __int64 stepF, int numPoints, int duration)
{
	Sweep(startF, stepF, numPoints, duration, false);
}

bool VNADevice::Sweep(__int64 startF, __int64 stepF, int numPoints, int duration, int power)
{
	String ^s;
	array<String ^>^ sa, ^ss;
//	String ^ t;
	if (hardware != HW_NANOVNA) {
		SetAudioPower(power);
		ArmAudio(numPoints,serialPort);
	}
	dur = duration;

	if (hardware == HW_MOCKUP) {
		//		if (!power) dur += 2; // add 2 duration for lead in and out
		StartAudioSimulation(mode, numPoints + 10, dur, startF, stepF, cable_before, cable_after, 0, resistance, capacitance, sourcecapacitance, inductance, noise);
	} else {
		try {
			//		t = String::Format("0 {0} {1} {2} ", startF, numPoints+5, stepF);
			//		serialPort->WriteLine(String::Format("0 1000000 1 0 ", startF, numPoints, stepF));
			//		Sleep(200);
			if (!serialPort->IsOpen) { 
				if (!FindVNA())
					return false;
			}
			if (hardware == HW_NANOVNA) {
				//				serialPort->WriteLine(String::Format("resume"));
				//				Sleep(20);
				//				s = serialPort->ReadLine();
//				s = serialPort->ReadExisting();
				if (hasScanCommand) {
//					Perform(String::Format("scan {0} {1} {2}",startF, stepF, numPoints));
#if 1
//					sa = Perform(String::Format("offset {0}",IFREQ));
					s = serialPort->ReadExisting();		// In case previous scan was aborted

					serialPort->WriteLine(String::Format("scanraw 2 {0} {1} {2}",startF, stepF, numPoints, duration));
//					serialPort->WriteLine(String::Format("scanraw {0} {1} {2} {3}",startF, stepF, numPoints, duration));

					s = serialPort->ReadLine();
//					Sleep(20);
//					s = serialPort->ReadExisting();
#endif
				} else {
					int base = 0;
					while (numPoints > 0) {
						int startIndex;
//						sa = Perform(String::Format("freq {0}",startF));	// Stop sweeping
						sa = Perform(String::Format("sweep {0} {1}",startF, startF+100*stepF, 101));
//						sa = Perform("resume");
						Sleep(800);
//						sa = Perform("pause");
					again_1:
						sa = Perform("data 0");
						startIndex = 1;
						if (sa->Length == 102) {
							startIndex = 0;
						} else if (sa->Length != 103)
							goto again_1;
						for (int i = 0; i < sa->Length-1-startIndex; i++) {
							s = sa[i+startIndex];
							ss = s->Split(' ');
							S11Real[i+base] = Convert::ToDouble(ss[0])*10.0;
							S11Imag[i+base] = Convert::ToDouble(ss[1])*10.0;
						}
						if (S11Real[base] > 10000.0) {
							S11Real[base] = S11Real[base+1];
							S11Imag[base] = S11Imag[base+1];
						}
					again_2:
						sa = Perform("data 1");
						startIndex = 1;
						if (sa->Length == 102) {
							startIndex = 0;
						} else if (sa->Length != 103)
							goto again_2;
						for (int i = 0; i < sa->Length-1-startIndex; i++) {
							s = sa[i+startIndex];
							ss = s->Split(' ');
							S21Real[i+base] = Convert::ToDouble(ss[0])*3.0;
							S21Imag[i+base] = Convert::ToDouble(ss[1])*3.0;
						}
						if (S21Real[base] > 10000.0) {
							S21Real[base] = S21Real[base+1];
							S21Imag[base] = S21Imag[base+1];
						}

						base += 101;
						numPoints -= 101;
						startF += 101*stepF;
					}
				}
				index = 0;
			} else {
				//serialPort->ReadExisting();
				if (power)
					serialPort->WriteLine(String::Format("F2 {0} {1} {2} {3} {4} {5}", startF, numPoints + 10, stepF, dur, IFREQ, hardware-2));
				else
					serialPort->WriteLine(String::Format("F0 {0} {1} {2} {3} {4} {5}", startF, numPoints + 10, stepF, dur+2, IFREQ, hardware-2)); // add 2 duration for lead in and out
				Sleep(20);
			}
		}
		catch (System::Exception^ e) {
			MessageBox::Show(e->Message, "Error");
			return(false);
		}

	}
	return(true);
	// mp = 0;
}

void VNADevice::SetFreq(__int64 startF, int direction)
{
	if (hardware != HW_MOCKUP){
		try {
			if (!serialPort->IsOpen) {
				if (!FindVNA())
					return;
			}
			if (serialPort->IsOpen) {
				if (hardware == HW_NANOVNA) {
//					Perform(String::Format("freq {0}", startF));
#if 0
					String	^s = serialPort->ReadExisting();
					serialPort->WriteLine(String::Format("freq {0}", startF));
					s = serialPort->ReadLine();
//					s = serialPort->ReadExisting();
#endif
				} else {
					SetAudioPower((direction == 2?true:false));
					serialPort->WriteLine(String::Format("F{1} {0} 1 0 5 {2} {3}", startF, direction, IFREQ, hardware-2));
				}
			}
		}	
		catch (System::Exception^ e) {
			     MessageBox::Show(e->Message, "Error");
//				serialPort->Close();
//				Sleep(1000);
//				serialPort->Open();
//				Sleep(1000);
				//serialPort->ReadExisting();

				 return;
		}
	} else {
		StartAudioSimulation(mode, 1, 5, startF, 0, cable_before, cable_after, direction, resistance, capacitance, sourcecapacitance, inductance, noise);
	}
}

array<String ^>^VNADevice::Perform(String ^command)
{
	array<String ^>^ sa;
	String ^s = serialPort->ReadExisting(); // Discard what is available
	System::Threading::Thread::Sleep(10);
	serialPort->WriteLine(command);
	System::Threading::Thread::Sleep(100);
	s = serialPort->ReadExisting();
	int count = 0;
	while (!s->Contains("ch>")) {			// Wait for all output
		System::Threading::Thread::Sleep(100);
		s = s->Concat(serialPort->ReadExisting());
		count++;
		if (count > 30)	{ // longest command takes 2 seconds
			MessageBox::Show("Timeout when communicating with NanoVNA", "Error");
			throw;
		}
	}
	sa = s->Split('\n');
	return(sa);
}

bool VNADevice::FindVNA()
		 {
			 int step=0;
			 try 
			 {
				 if (hardware == HW_MOCKUP)
					 return true;

				 //					       serialPort->PortName = SetPortName(_serialPort->PortName);
				 //        serialPort->BaudRate = SetPortBaudRate(_serialPort->BaudRate);
				 serialPort->Parity =(System::IO::Ports::Parity) 0;
				 //serialPort->DataBits = (System::IO::Ports::DataBits)8;
				 serialPort->StopBits = (System::IO::Ports::StopBits)1;
				 serialPort->Handshake = (System::IO::Ports::Handshake)0;

				 // Set the read/write timeouts
				 serialPort->ReadTimeout = 2000;
				 serialPort->WriteTimeout = 1000;

		  		 if (!serialPort->IsOpen) {
					 serialPort->ReadBufferSize = 40000;
					 serialPort->Open();
					 Sleep(200);

				 }
				 if (hardware == HW_NANOVNA) {
					 String ^s;
				again1:
					 s = serialPort->ReadExisting();
					 if (s->Length != 0) {
						Sleep(100);
						goto again1;
					 }
					 serialPort->NewLine = "\r\n";
					array<String ^>^ sa;
					sa = Perform("scanraw");
					 step=1;
						 if (sa->Length > 2 && sa[1]->StartsWith("usage: scanraw {channel(0|1|2)}")) {
							 hasScanCommand = true;
							 Perform("pause");
						 } else {
							 hasScanCommand = false;
							 Perform("resume");
							 Perform("cal off");
						 }

						 //							serialPort->WriteLine("cal off");
						 //							 s = serialPort->ReadLine(); // cal off
						 //							 s = serialPort->ReadExisting(); // ch>
						 //						    System::Threading::Thread::Sleep(100);
						 //							serialPort->WriteLine("pause");
						 //						    s = serialPort->ReadLine();
						 //						    s = serialPort->ReadExisting();
						 return true;
				 } else { 
					 String ^s;
				again2:
					 s = serialPort->ReadExisting();	// Throw away whatever was send before
					 if (s->Length != 0) {
						Sleep(100);
						goto again2;
					 }
					 serialPort->Write("f3");
					 System::Threading::Thread::Sleep(200);
					 s = serialPort->ReadLine();
					 if (s->StartsWith("TAPR VNA v4"))
						 return true;
				 }
			 }
			 catch( Exception^ /* e */ )	
			 {	
				 return false;
			 }
			 return false;
}


#define PhaseToQ(X) (short)( sin((X) * DEGR2RAD) * 1800 + 1850 )
#define PhaseToI(X) (short)( cos((X) * DEGR2RAD) * 1800 + 1850 )
#define Noise(X, Y) (short)( X * (1.0 +  Y * ((1.0 * rand() / RAND_MAX) - 0.5) ));
#define NoiseLevel 0.005
#define MaxLevel +0
#define MinLevel -57
#define MaxDAC	3500
#define MagDac(X)	((short)(MaxDAC * (( X ) - MinLevel) / (MaxLevel - MinLevel) ) )

#define MAXAVERAGE	300

static float sumreflphase[MAXAVERAGE];
static float sumtranphase[MAXAVERAGE];
static float sumreflmag[MAXAVERAGE];
static float sumtranmag[MAXAVERAGE];
static float sumreflevel[MAXAVERAGE];


static float Median(float *data, int size)
{
	int i, j;
	float swap;

	// bubble sort the elements
	for (i=size-1; i>=0; i--) {
        for (j=1; j<=i; j++) {
            if (data[j-1] > data[j])
			{
				swap = data[j-1];
				data[j-1] = data[j];
				data[j] = swap;
			}
		}
	}
	return(data[size/2]);	// the median value
}


	/// Write TxBuffer to VNA (command),  readback the result to RxBuffer (response)
bool VNADevice::WriteRead(VNA_TXBUFFER * TxBuffer, VNA_RXBUFFER * RxBuffer, int direction)
{
	// modifications to test Proto8 target changes
	bool rxsuccess = true;
//	long freq;
	float reflphase,tranphase;
	float reflmag,tranmag;
	float reflevel;
	unsigned long freq;
	int i;
	if (hardware == HW_NANOVNA) {
		array<String^>^ sa;
		String ^s;
		double x;
		double y;
		reflevel = 0.0; // Always 0.0 for NanoVNA
		try {
			if (hasScanCommand) {
again:
				s = serialPort->ReadLine();
//				if (s->StartsWith("done"))
//					return false;
//				if (s->StartsWith("start"))
//					goto again;
				if (s->StartsWith("scanraw"))
					goto again;
				if (s->StartsWith("ch>"))
					return false;
				if (s->StartsWith("0 "))
					goto again;
				if (s->StartsWith("1 "))
					goto again;
				sa = s->Split('\t');

				freq = Convert::ToInt32(sa[0]);
				x =  Convert::ToDouble(sa[1]);
				y = Convert::ToDouble(sa[2]);
				reflphase = (float) (atan2(y, x) * RAD2DEGR);
				reflmag = (float)todb(sqrt(x*x + y*y));

				x = Convert::ToDouble(sa[3]);
				y = Convert::ToDouble(sa[4]);
				tranphase = (float) (atan2(y, x) * RAD2DEGR);
				tranmag = (float)todb(sqrt(x*x + y*y));
			} else {
				x = S11Real[index];
				y = S11Imag[index];
				reflphase = (float) (atan2(y, x) * RAD2DEGR);
				reflmag = (float)todb(sqrt(x*x + y*y));

				x = S21Real[index];
				y = S21Imag[index];
				tranphase = (float) (atan2(y, x) * RAD2DEGR);
				tranmag = (float)todb(sqrt(x*x + y*y));

			}
			index++;
		}
		catch( Exception^ /* e */ )	
		{	
			return false;
		}

	} else {

		//unsigned int level;
		int availableSamples = ((dur+2) * SAMPPERMS - 2);
		//	int reply = TxBuffer->ReplyType;
		int retries=0;
		//	return true;
#define MAX_RETRIES	100		
		//	i = 0;
		for (i=0; i<availableSamples; i++) 
		{
			while (!RetreiveData((int)TxBuffer->TxAccum, dur, sumreflmag[i], sumreflphase[i], sumtranmag[i], sumtranphase[i], sumreflevel[i], freq, availableSamples) && retries < MAX_RETRIES) {
				Sleep(2);
				retries++;
			}
			if (retries >= MAX_RETRIES)
				return (false);
		}
#if 1
		reflmag = Median(sumreflmag,availableSamples);
		tranmag = Median(sumtranmag,availableSamples);
		reflphase = Median(sumreflphase,availableSamples);
		tranphase = Median(sumtranphase,availableSamples);
		reflevel = Median(sumreflevel,availableSamples);
#else
		reflmag = sumreflmag[0];
		tranmag = sumtranmag[0];
		reflphase = sumreflphase[0];
		tranphase = sumtranphase[0];
		reflevel = sumreflevel[0];
#endif
		// mp++;
	}
	RxBuffer->Vref1 = DB2SHORT(reflevel);
	//	RxBuffer->Vref2 = level;
	NormalizePhase(reflphase);
	RxBuffer->ReflPQ = PHASE2SHORT(reflphase) ;
	RxBuffer->ReflMQ = DB2SHORT(reflmag);
	NormalizePhase(tranphase);
	RxBuffer->TranPQ = PHASE2SHORT(tranphase) ;
	RxBuffer->TranMQ = DB2SHORT(tranmag);
	RxBuffer->Freq = freq;
	return(rxsuccess);
}


void VNADevice::SetMode(int m) {
	mode = m;
}

void VNADevice::SetBefore(int l) {
	cable_before = l;
}

void VNADevice::SetAfter(int l) {
	cable_after = l;
}

void VNADevice::SetResistance(int l) {
	resistance = l;
}

void VNADevice::SetCapacitance(int l) {
	capacitance = l;
}

void VNADevice::SetSourceCapacitance(int l) {
	sourcecapacitance = l;
}

void VNADevice::SetInductance(int l) {
	inductance = l;
}

void VNADevice::SetNoise(float c) {
	noise = c;
}

void VNADevice::SetMinFreq(__int64 f) {
	minHWFreq = f;
}

void VNADevice::SelectHardware(int h) {
	hardware = h;
}

int VNADevice::GetHardware() {
	return(hardware);
}

void VNADevice::SetMaxFreq(__int64 f) {
	maxHWFreq = f;
}

__int64 VNADevice::GetMaxFreq() {
	return(maxHWFreq);
}

__int64 VNADevice::GetMinFreq() {
	return(minHWFreq);
}

void VNADevice::SetAudioRefLevel(double r) {
	audioRefLevel = r;
}

double VNADevice::GetAudioRefLevel() {
	return(audioRefLevel);
}

void VNADevice::SetIF(int r) {
	IFREQ = r;
	OpenAudio();
}

int VNADevice::GetIF() {
	return(IFREQ);
}
