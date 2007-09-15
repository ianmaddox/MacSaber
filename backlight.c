// lmutracker.c
//
// gcc -o lmutracker lmutracker.c -framework IOKit -framework CoreFoundation

#include <mach/mach.h>

#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>
#include "lmucommon.h"

static double updateInterval = 0.1;
static io_connect_t dataPort = 0;
static isInit = 0;
static isValid = 0;

int initKbBrightness(void)
{
	kern_return_t     kr;
	io_service_t      serviceObject;
	
	if(isInit == 0)
	{
		isInit++;
	} else {
		return isValid;
	}
	
	// Look up a registered IOService object whose class is AppleLMUController
	serviceObject = IOServiceGetMatchingService(kIOMasterPortDefault,
															  IOServiceMatching("AppleLMUController"));
	if (!serviceObject) {
		mach_error("failed to find ambient light sensor",kr);
		return 0;
	}	

	// Create a connection to the IOService object
	kr = IOServiceOpen(serviceObject, mach_task_self(), 0, &dataPort);
	
	IOObjectRelease(serviceObject);
	if (kr != KERN_SUCCESS) {		
		mach_error("IOServiceOpen:", kr);
		return 0;
	}
	isValid = 1;
	return isValid;
}	

int getKbBrightness(void)
{
   kern_return_t kr;
   IOItemCount   scalarInputCount = 1;
   IOItemCount   scalarOutputCount = 1;
	
   SInt32        left = 0, right = 0;
   SInt32        in_unknown = 0, out_brightness;
	
	if(!initKbBrightness())
		return false;
	
   kr = IOConnectMethodScalarIScalarO(dataPort, kGetLEDBrightnessID,
                                      scalarInputCount, scalarOutputCount, in_unknown, &out_brightness);
	
	if (kr == KERN_SUCCESS) {
		return out_brightness;
	} else if (kr == kIOReturnBusy) {
		return 0;
	} else {
		mach_error("I/O Kit error:", kr);
		return -1;
	}	
	// out_brightness is the brightness value; you might want to calibrate
}

int setKbBrightness(SInt32 in_brightness, SInt32 in_time_ms)
{
	kern_return_t kr;
	IOItemCount   scalarInputCount = 3;
	IOItemCount   scalarOutputCount = 1;
	
	if(!initKbBrightness())
		return false;
	
//	SInt32        in_unknown = 0, in_brightness, in_time_ms, out_brightness;
	SInt32        in_unknown = 0, out_brightness;
	
//	in_brightness = 0xf00; // some particular target value
//	in_time_ms    = 15000; // time in ms
	
	if (in_time_ms > 0) {
		kr = IOConnectMethodScalarIScalarO(dataPort, kSetLEDFadeID,
													  scalarInputCount, scalarOutputCount, in_unknown, in_brightness,
													  in_time_ms, &out_brightness);	
	} else {
		kr = IOConnectMethodScalarIScalarO(dataPort, kSetLEDBrightnessID,
													  scalarInputCount, scalarOutputCount, in_unknown, in_brightness,
													  &out_brightness);			
	}	
	if (kr == KERN_SUCCESS) {
		return 1;
	} else if (kr == kIOReturnBusy) {
		return 0;
	} else {
		mach_error("I/O Kit error:", kr);
		return -1;
		
	}
}

