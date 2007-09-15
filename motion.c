/* motion.c
 *
 * a little program to display the coords returned by
 * the powerbook motion sensor
 *
 * A fine piece of c0de, brought to you by
 *
 *               ---===---
 * *** teenage mutant ninja hero coders ***
 *               ---===---
 *
 * All of the software included is copyrighted by Christian Klein <chris@5711.org>.
 *
 * Copyright 2005 Christian Klein. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Christian Klein.
 * 4. The name of the author must not be used to endorse or promote
 *    products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Modified for iBook compatibility by Pall Thayer <http://www.this.is/pallit>
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <IOKit/IOKitLib.h>
#include "defines.c"


int getMotion(int macType, int structSize, int *values)
{
	struct data {
		char x;
		char y;
		char z;
		char pad[structSize];
	};	
	
	int kernFunc;
	char *servMatch;
	int iflag = 0;
	kern_return_t result;
	mach_port_t masterPort;
	io_iterator_t iterator;
	io_object_t aDevice;
	io_connect_t  dataPort;
	
	IOItemCount structureInputSize;
	IOByteCount structureOutputSize;
	
	struct data inputStructure;
	struct data outputStructure;
	
	result = IOMasterPort(MACH_PORT_NULL, &masterPort);
	switch(macType)
	{
		case MACBOOK:
			iflag = 1;
			kernFunc = 5;
			servMatch ="SMCMotionSensor";
			break;
		case POWERBOOK:
			kernFunc = 21;
			servMatch = "IOI2CMotionSensor";
			break;
		case HIRESPOWERBOOK:
			kernFunc = 21;
			servMatch = "PMUMotionSensor";
			break;
		default:
			kernFunc = 21;
			servMatch = "PMUMotionSensor";
	}
	
	CFMutableDictionaryRef matchingDictionary = IOServiceMatching(servMatch);
	
	result = IOServiceGetMatchingServices(masterPort, matchingDictionary, &iterator);
	
	if (result != KERN_SUCCESS)
	{
		fputs(sprintf("IO Server '%s' returned error.\n",servMatch), stderr);
		return -1;
	}
	
	aDevice = IOIteratorNext(iterator);
	IOObjectRelease(iterator);
	
	if(aDevice == 0)
	{
		fputs("No motion sensor available\n", stderr);
		return -2;
	}
	
	result = IOServiceOpen(aDevice, mach_task_self(), 0, &dataPort);
	IOObjectRelease(aDevice);
	
	if(result != KERN_SUCCESS)
	{
		fputs("Could not open motion sensor device\n", stderr);
		return -3;
	}
	
	structureInputSize = sizeof(struct data);
	structureOutputSize = sizeof(struct data);

	memset(&inputStructure, iflag, structureInputSize);
	memset(&outputStructure, 0, structureOutputSize);
	
	result = IOConnectMethodStructureIStructureO(
																dataPort,
																kernFunc,				
																structureInputSize,
																&structureOutputSize,
																&inputStructure,
																&outputStructure
																);	
	if(result != KERN_SUCCESS)
	{
		puts("no coords");
		return -4;
	} else {
		values[0] = outputStructure.x;
		values[1] = outputStructure.y;
		values[2] = outputStructure.z;
	}
	
	IOServiceClose(dataPort);
	return 1;
}
