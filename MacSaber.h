/* MacSaber */

#import <Cocoa/Cocoa.h>
#import "motion.h"
#import "backlight.h"
#import "defines.c"
/*
@interface MacSaber : NSObject
{
	IBOutlet id debugPane;
}

@end
*/
@interface MacSaber : NSObject
{
    IBOutlet id f1;
	 IBOutlet id f1b;
    IBOutlet id f2;
    IBOutlet id f2b;
    IBOutlet id f3;
    IBOutlet id f3b;
    IBOutlet id lbl1;
    IBOutlet id lbl2;
    IBOutlet id lbl3;
	 IBOutlet id presButan;
	 IBOutlet id reportButton;
	 IBOutlet id debugBox;
	 IBOutlet id debugPane;
	 
	 BOOL going;
	 BOOL error;
	 BOOL reportEnabled;
	 NSTimer *timer, *idle;
	 
	 int macType;
	 int structSize;
	 int sStrike, sIdle, sHit, sOff, sOn, sStart, sSwing;
	 int roll, roll2, rollD2, tilt, tilt2, tiltD2;
	 NSMutableString *versionURL;
	 NSString *myVersion;
	 NSString *reportData;
	 char *macTypeCName;
 
}

- (void)setVersionURL:(NSString *)value;

- (IBAction)check:(id)sender;
- (IBAction)checkForUpdates:(id)sender;
- (IBAction)report:(id)sender;
- (IBAction)debug:(id)sender;

@end
