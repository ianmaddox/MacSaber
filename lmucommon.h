// lmucommon.h

#ifndef LMUCOMMON_H
#define LMUCOMMON_H

enum {
	kGetSensorReadingID   = 0,  // getSensorReading(int *, int *)
	
	kGetLEDBrightnessID   = 1,  // getLEDBrightness(int, int *)
	kSetLEDBrightnessID   = 2,  // setLEDBrightness(int, int, int *)
	
	kSetLEDFadeID         = 3,  // setLEDFade(int, int, int, int *)
	
	// other firmware-related functions
	// verifyFirmwareID     = 4,  // verifyFirmware(int *)
	// getFirmwareVersionID = 5,  // getFirmwareVersion(int *)
	
	// other flashing-related functions
	// ...
};

#endif