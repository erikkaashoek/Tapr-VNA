//
//    Copyright 2005 Thomas C. McDermott, N5EG
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


// TestFFT.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#pragma once

#include "stdafx.h"
#include <math.h>

#define PI	3.1415926535897932384626433832795


// Fast Fourier Transform. length must be exactly 2^n.
// Inverse = true computes InverseFFT
// Inverse = false computes FFT.
// Overwrites the real and imaginary arrays in-place

void FFT(double real[], double imag[], int length, bool Inverse = false)
{

	double wreal, wpreal, wimag, wpimag, theta;
	double tempreal, tempimag, tempwreal, direction;

	int Addr, Position, Mask, BitRevAddr, PairAddr;
	int m, k;


	direction = -1.0;		// direction of rotating phasor for FFT  

	if(Inverse)				
		direction = 1.0;	// direction of rotating phasor for IFFT 

	//  bit-reverse the addresses of both the real and imaginary arrays
	//  real[0..length-1] and imag[0..length-1] are the paired complex numbers

	for (Addr=0; Addr<length; Addr++)
	{
		// Derive Bit-Reversed Address
		BitRevAddr = 0;
		Position = length >> 1;
		Mask = Addr;
		while (Mask)
		{
			if(Mask & 1)
				BitRevAddr += Position;
			Mask >>= 1;
			Position >>= 1;
		}

		if (BitRevAddr > Addr)				// Swap 
		{
			double s;
			s = real[BitRevAddr];			// real part
			real[BitRevAddr] = real[Addr];
			real[Addr] = s;
			s = imag[BitRevAddr];			// imaginary part
			imag[BitRevAddr] = imag[Addr];
			imag[Addr] = s;
		}
	}

	// FFT, IFFT Kernel

	for (k=1; k < length; k <<= 1)
	{
		theta = direction * PI / (double)k;
		wpimag = sin(theta);
		wpreal = cos(theta);
		wreal = 1.0;
		wimag = 0.0;

		for (m=0; m < k; m++)
		{
			for (Addr = m; Addr < length; Addr += (k*2))
			{
				PairAddr = Addr + k;

				tempreal = wreal * real[PairAddr] - wimag * imag[PairAddr];
				tempimag = wreal * imag[PairAddr] + wimag * real[PairAddr];
				real[PairAddr] = real[Addr] - tempreal;
				imag[PairAddr] = imag[Addr] - tempimag;
				real[Addr] += tempreal;
				imag[Addr] += tempimag;
			}
			tempwreal = wreal;
			wreal = wreal * wpreal - wimag * wpimag;
			wimag = wimag * wpreal + tempwreal * wpimag;
		}
	} 

	if(Inverse)							// Normalize the IFFT coefficients
		for(int i=0; i<length; i++)
		{
			real[i] /= (double)length;
		    imag[i] /= (double)length;
		}

}
