// lmutracker.c
//
// gcc -o lmutracker lmutracker.c -framework IOKit -framework CoreFoundation

#include <mach/mach.h>

#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>
#include "lmucommon.h"

static double updateInterval = 0.1;
static io_connect_t dataPort = 0;

void updateTimerCallBack(CFRunLoopTimerRef timer, void *info) 
{
	kern_return_t kr;
	IOItemCount   scalarInputCount  = 0;
	IOItemCount   scalarOutputCount = 2;
	
	SInt32        left = 0, right = 0;
	
	kr = IOConnectMethodScalarIScalarO(dataPort, kGetSensorReadingID,
												  scalarInputCount, scalarOutputCount, &left, &right);
	
	if (kr == KERN_SUCCESS) {
		
		printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b%8ld %8ld", left, right);
		return;
		
	}
	
	if (kr == kIOReturnBusy)
		return;
	
	mach_error("I/O Kit error:", kr);
	exit(kr);
	
}

int getLightMeterValues(void)
{
	kern_return_t     kr;
	io_service_t      serviceObject;
	
	CFRunLoopTimerRef updateTimer;
	
	// Look up a registered IOService object whose class is AppleLMUController
	serviceObject = IOServiceGetMatchingService(kIOMasterPortDefault,
															  
															  IOServiceMatching("AppleLMUController"));
	if (!serviceObject) {
		
		fprintf(stderr, "failed to find ambient light sensor\n");
		exit(1);
		
	}
	
	// Create a connection to the IOService object
	kr = IOServiceOpen(serviceObject, mach_task_self(), 0, &dataPort);
	
	IOObjectRelease(serviceObject);
	if (kr != KERN_SUCCESS) {
		
		mach_error("IOServiceOpen:", kr);
		exit(kr);
		
	}
	
	setbuf(stdout, NULL);
	printf("%8ld %8ld", 0L, 0L);
	
	// Set up the loop and let it run...
	updateTimer = CFRunLoopTimerCreate(kCFAllocatorDefault,
												  CFAbsoluteTimeGetCurrent() + updateInterval,
												  
												  updateInterval, 0, 0, updateTimerCallBack, NULL);
	
	CFRunLoopAddTimer(CFRunLoopGetCurrent(), updateTimer, kCFRunLoopDefaultMode);
	CFRunLoopRun();
	
	exit(0);
}


