#import "MacSaber.h"

#define TICKS 6
#define MAXBRIGHT 4079

@implementation MacSaber
-(id)init
{
	srandom(time(NULL));
	going = TRUE;
	error = FALSE;
	reportEnabled = TRUE;
	sStrike = 3;
	sIdle = 2;
	sHit = 5;
	sOff = 1;
	sOn = 1;
	sStart = 1;
	sSwing = 8;
	roll2 = rollD2 = tilt2 = tiltD2 = 0;
	timer = [[NSTimer scheduledTimerWithTimeInterval:0.1
															target:self
														 selector:@selector(getpos:)
														 userInfo:nil
														  repeats:YES] retain];
	idle = [[NSTimer scheduledTimerWithTimeInterval:0.01
														  target:self
														selector:@selector(idle:)
														userInfo:nil
														 repeats:YES] retain];
	
	NSString *macTypeName;
	Gestalt('mnam', &macTypeCName);
	macTypeName = [[[NSString alloc] initWithCString:macTypeCName] autorelease];
	
	structSize = 57;
	
	if([macTypeName rangeOfString:@"MacBook"].length != 0) {
		macType = MACBOOK;
		structSize = 37;
	} else if([macTypeName rangeOfString:@"PowerBook5,6"].length != 0) {
		macType = POWERBOOK;
	} else if([macTypeName rangeOfString:@"PowerBook5,7"].length != 0) {
		macType = POWERBOOK;
	} else if([macTypeName rangeOfString:@"PowerBook5,"].length != 0) {
		macType = HIRESPOWERBOOK;
	} else if([macTypeName rangeOfString:@"PowerBook"].length != 0) {
		macType = POWERBOOK;
	} else {
		macType = MACBOOK;
		structSize = 37;
	}
	
	NSLog(@"Specs: %i, %@",macType,macTypeName);
	
	
	[[NSSound soundNamed:@"start0"] play];		 
	setKbBrightness(0,500);
	return self;
}

- (IBAction)debug:(id)sender
{
	if ([debugPane isVisible]) 
	{
		NSLog(@"Debug off");
		[debugPane orderOut:sender];
	} else {
		NSLog(@"Debug on");		
		[debugPane orderFront:sender];
	}
}


- (IBAction)report:(id)sender
{
	if(reportEnabled == FALSE)
	{	
		NSLog(@"Reporting disabled");
		return;
	}
//	NSLog([debugBox stringValue]);

	NSURL * url;
	NSString * debugBoxValue = [debugBox stringValue];
	
	NSMutableString *urltext = [[NSMutableString alloc] initWithString:@"mailto:macsaber-reports@isnoop.net?subject=MacSaber%20Error%20Report&body="];
	[urltext appendString:[debugBoxValue stringByAddingPercentEscapesUsingEncoding:NSASCIIStringEncoding]];

	NSLog(urltext);
	url = [NSURL URLWithString:urltext];
	assert(url != nil);
	(void) [[NSWorkspace sharedWorkspace] openURL:url];
	
	[reportButton setTitle:@"Thank you!"];
	[reportButton setEnabled:FALSE];
	
	reportEnabled = FALSE;
}

- (IBAction)checkForUpdates:(id)sender
{
	[self setVersionURL:@"http://isnoop.net/apps/version.php?current="];
	[self checkVersion];
}

- (IBAction)check:(id)sender
{
	if(error == true)
		return;
	if(going == FALSE)	
	{	
		[[NSSound soundNamed:[NSString stringWithFormat:@"on%d",`()%sOn]] play];
		going = TRUE;
		timer = [[NSTimer scheduledTimerWithTimeInterval:0.1
																target:self
															 selector:@selector(getpos:)
															 userInfo:nil
															  repeats:YES] retain];
		idle = [[NSTimer scheduledTimerWithTimeInterval:0.01
															  target:self
															selector:@selector(idle:)
															userInfo:nil
															 repeats:YES] retain];
		
	} else {
		if([[NSSound soundNamed:@"start0"] isPlaying])
			[[NSSound soundNamed:@"start0"] stop];
//		if([[NSSound soundNamed:@"on0"] isPlaying])
//			[[NSSound soundNamed:@"on0"] stop];
		
		[[NSSound soundNamed:@"idle0"] stop];
		[[NSSound soundNamed:[NSString stringWithFormat:@"off%d",random()%sOff]] play];
		[idle invalidate];
		[idle release];
		[timer invalidate];
		[timer release];
		going = FALSE;
	}

}

- (void)idle:(NSTimer *)aTimer
{
	if(![[NSSound soundNamed:@"idle0"] isPlaying])
		[[NSSound soundNamed:@"idle0"] play];
}

- (IBAction)getpos:(NSTimer *)aTimer
{
	int hitThresh = 128;
	int strikeThresh = 90;
	int swingThresh = 20;

	int gyro[3];
	static BOOL firstRun = TRUE;

	//Difference
	int rollD, tiltD, deltaD;

	// Acceleration
	int rollA, tiltA, deltaA;
	int mult, result;
	
	// Keyboard brightness
	unsigned int bright = 0, fade;

	mult = macType == MACBOOK ? 1 : 2.3;
	
	NSMutableString *rollDir;
	
	result = getMotion(macType,structSize,&gyro);

	if(firstRun == TRUE)
	{
		NSBundle *app = [NSBundle mainBundle];
		NSString *ident = [app bundleIdentifier];
		NSDictionary *infoDict = [app infoDictionary];
		NSString *myVersion = (NSString *)[infoDict valueForKey:@"CFBundleVersion"];	

		if(result != 1)
		{
			
			[debugBox setStringValue:[NSString stringWithFormat:@"Version: %@\nSMS Error: %d\n%s\nClass: %d",myVersion,result,macTypeCName+1,macType]];
			NSLog(reportData);
			[idle invalidate];
			[idle release];
			[timer invalidate];
			[timer release];
			error = TRUE;	
			return;
			//		[self check];
		} else {
			[reportButton setTitle:@"Nothing to report"];
			[reportButton setEnabled:FALSE];			
			reportEnabled = FALSE;			
			[debugBox setStringValue:[NSString stringWithFormat:@"SYSTEM OK\nVersion: %@\n%s\nClass: %d",myVersion,macTypeCName+1,macType]];		
		}
	}
	tilt = gyro[2] * 2;
	roll = gyro[0];
	// Add a little dead zone around the 90 degree points
	// Not all motion sensors are created equal
	roll = abs(roll) <= 8 ? 0 : roll;
	roll = abs(roll) >= 120 ? 120 : roll;
		
	// Calculate difference
	rollD = roll - roll2;
	tiltD = tilt - tilt2;

	deltaD = abs(rollD) < abs(tiltD) ? rollD : tiltD;
	
	// Calculate momentary acceleration
	rollA = abs(rollD - rollD2);
	tiltA = abs(tiltD - tiltD2);

	deltaA = rollA < tiltA ? rollA : tiltA;
	
	if(deltaA * mult > hitThresh)
	{
		bright = 1;
		[[NSSound soundNamed:[NSString stringWithFormat:@"hit%d",random()%sHit]] play];
	} else if (deltaA > strikeThresh) {
		bright = 1;
		[[NSSound soundNamed:[NSString stringWithFormat:@"strike%d",random()%sStrike]] play];		
	}

	if(bright > 0)
	{
		bright = (deltaA - strikeThresh) * 50 + getKbBrightness();
		bright = bright > MAXBRIGHT ? MAXBRIGHT : bright; 
		fade = 150 + bright / 15;
//		printf("%d - %d\n	",bright, fade);
		setKbBrightness(bright,10);
		setKbBrightness(0,fade);
	} else {
		setKbBrightness(0,500);
	}
		
	if(abs(deltaD) * mult > swingThresh)
	{	
		[[NSSound soundNamed:[NSString stringWithFormat:@"swing%d",random()%sSwing]] play];		
	}

	if(gyro[1] == -1)
	{
		roll = gyro[0] > 0 ? gyro[0]-256 : gyro[0];
		rollDir = @"Right";
	}
	else{
		roll = gyro[0] * -1 > 0 ? gyro[0]+256 : gyro[0];
		rollDir = @"Left ";
//		[f1 setStringValue:[NSString stringWithFormat:@"Left %d",roll]];
	}
	
	
	[f1 setStringValue:[NSString stringWithFormat:@"%d",roll]];
	[f2 setStringValue:[NSString stringWithFormat:@"%d",tilt]];
	[f3 setStringValue:[NSString stringWithFormat:@"%d",deltaD]];
	[f3b setStringValue:[NSString stringWithFormat:@"%d",deltaA]];
	
	roll2 = roll;
	tilt2 = tilt;
	rollD2 = rollD;
	tiltD2 = tiltD;
	firstRun = FALSE;
}

- (void)awakeFromNib
{
	[f1 setStringValue:@""];
	[f2 setStringValue:@""];
	[f3 setStringValue:@""];
	[f1b setStringValue:@""];
	[f2b setStringValue:@""];
	[f3b setStringValue:@""];
}


- (void)setVersionURL:(NSMutableString *)value {
	[value retain];
	[versionURL release];
	versionURL = [[NSMutableString alloc] initWithString:value];
}

- (void)checkVersion {
	NSBundle *app = [NSBundle mainBundle];
	NSString *ident = [app bundleIdentifier];
	NSDictionary *infoDict = [app infoDictionary];
	NSString *myVersion = (NSString *)[infoDict valueForKey:@"CFBundleVersion"];	

	[versionURL appendString:[myVersion stringByAddingPercentEscapesUsingEncoding:NSASCIIStringEncoding]];
	NSDictionary *plist = [NSDictionary dictionaryWithContentsOfURL:[NSURL URLWithString:versionURL]];
	if (!plist) {
		NSLog(@"Couldn't access version info");
		NSLog(@"versionURL: %@", versionURL);
		return;
	}
	
	NSDictionary *versionDict = (NSDictionary *)[plist valueForKey:ident];
	NSString *currentVersion = (NSString *)[versionDict valueForKey:@"version"];
	NSString *downloadURL = (NSString *)[versionDict valueForKey:@"url"];
	int diff;
	diff = [myVersion compare:currentVersion options:NSCaseInsensitiveSearch];

	if (diff < 0) {
		NSAlert *alert = [NSAlert alertWithMessageText:@"Version Info"
													defaultButton:@"Ignore"
												 alternateButton:(downloadURL) ? @"Download" : @""
													  otherButton:nil
									informativeTextWithFormat:[NSString stringWithFormat:@"A newer version of this application is available (%@)", currentVersion]];
		int result = [alert runModal];
		if (result == NSAlertAlternateReturn) {
			[[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:downloadURL]];
		} else {
			NSBeep();
		}
		
	} else if(diff == 0) {
		
		NSAlert *alert = [NSAlert alertWithMessageText:@"Version Info"
													defaultButton:@"Hooray!"
												 alternateButton:nil
													  otherButton:nil
									informativeTextWithFormat:[NSString stringWithFormat:@"You have the latest version available (%@)", currentVersion]];
		[alert runModal];
	} else {
		
		NSAlert *alert = [NSAlert alertWithMessageText:@"Version Info"
													defaultButton:@"Ignore"
												 alternateButton:(downloadURL) ? @"Download" : @""
													  otherButton:nil
									informativeTextWithFormat:[NSString stringWithFormat:@"You are running %@.\nThe latest public release is %@.", myVersion, currentVersion]];
		int result = [alert runModal];
		if (result == NSAlertAlternateReturn) {
			[[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:downloadURL]];
		} else {
			NSBeep();
		}
		
	}
	NSLog([NSString stringWithFormat:@"ID: %@\nLatest: %@\nYours: %@\n", ident, currentVersion, myVersion]);
}

@end
