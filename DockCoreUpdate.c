#include "DockCoreUpdate.h"
#include "Aliases.h"
#include "IconPlotting.h"
        

/// Globals ///

FSSpec gFSSpec_arr[MAX_DOCK_ITEM_COUNT];	/*FSSpec for running apps*/
unsigned short gFSSpec_cnt = 0;				/*Count of running apps*/
extern Boolean gShowSelf;
extern unsigned short gMacDockItemNum;
extern DTPBptrAndHandle DtpbPtrNHandle;
#define kInitPsnCsum -627
unsigned long gPsnHiLast = kInitPsnCsum;
unsigned long gPsnLowLast = kInitPsnCsum;

/* FlashItem
 * Flashes a Dock item e.g. as UI feedback.
 * Number of flashes and pauses between flashes
 * are dictated by constants kBlinksOnClick and
 * constant kTicksBtwBlink.  See .h file.
 */
void FlashItem(unsigned short item_num){
	WindowPtr dockWin= thePort; //should be a better handle on the dock window
	Rect itemRect;
	long startTicks; //ticks = 1/60 of a second
	short blinkCnt = 0;
	
	itemRect.left = (kDockHSpace+kDockIconWidth+kDockHSpace)*item_num;
	itemRect.right = itemRect.left + (kDockHSpace+kDockIconWidth+kDockHSpace);
	itemRect.top = kDockVSpace;
	itemRect.bottom = itemRect.top + kDockIconHeight;
	while(blinkCnt < kBlinksOnClick){
		InvertRect(&itemRect);
		startTicks = TickCount();
		while((TickCount()-startTicks) < kTicksBtwBlink) ;
		InvertRect(&itemRect);
		startTicks = TickCount();
		while((TickCount()-startTicks) < kTicksBtwBlink) ;
		blinkCnt = blinkCnt+1;
	}
}

/* Invertitem
 * Inverts the section corresponding to 
 * the item number specified by (item_num)
 * This can be used for UI feedback to user
 */
void InvertItem(unsigned short item_num){
	WindowPtr dockWin= thePort; //should be a better handle on the dock window
	Rect itemRect;
	
	itemRect.left = (kDockHSpace+kDockIconWidth+kDockHSpace)*item_num;
	itemRect.right = itemRect.left + (kDockHSpace+kDockIconWidth+kDockHSpace);
	itemRect.top = kDockVSpace;
	itemRect.bottom = itemRect.top + kDockIconHeight;
	InvertRect(&itemRect);
}

/* LaunchApp 
 * Tries to launch the app in slot (item_num).
 * Returns true if there was a non-MacDock app
 * in that slot or else false 
 */ 
Boolean LaunchApp(unsigned short item_num){
	//do gestalt check gestaltOSAttr to see if sys supports the modern Launch records first...
	LaunchParamBlockRec launchPBR;
	if(gFSSpec_cnt <= item_num) return false;
	if(item_num == gMacDockItemNum)
	{	//Flash self icon and fail to launch
		FlashItem(item_num);
		return false;	//which should trigger upstream un-hide if nec.
	}
	launchPBR.launchBlockID = extendedBlock; //const indicating you're launching
	launchPBR.launchEPBLength = extendedBlockLen; //const specifying size, also from Processes.h
	//no need to set the  .launchFileFlags because using extendedBlock
	launchPBR.launchControlFlags = launchContinue;
	launchPBR.launchAppSpec = &(gFSSpec_arr[item_num]);
	FlashItem(item_num);
	LaunchApplication(&launchPBR);
	return true;
}

/* RevealApp 
 * Tries to reveal the app in slot (item_num).
 * Returns true if successful, false otherwise (or if clicked on invalid slot)
 */ 
Boolean RevealApp(unsigned short item_num){
	AEDesc fssDesc = {0};
	AEDesc aeDesc = {0};
	OSErr err;
	long appSig;
	AppleEvent appleEvent, reply;
	LaunchParamBlockRec launchPBR;
	//do gestalt check gestaltOSAttr to see if sys supports AppleScript w extensions...
	if(gFSSpec_cnt <= item_num) return false;
	
	
	FlashItem(item_num);
	appSig = 'MACS'; //Finder's app signature
	//Next line creates a descriptor with the app signature of Finder:
	err = AECreateDesc(typeApplSignature, (Ptr)(&appSig),
				(Size)sizeof(appSig), &aeDesc);
				
	//Create an AppleEvent directed at the Finder, where 'sope' is the Open keyword
	//err = AECreateAppleEvent('aevt', 'odoc', //this is for opening an app, but we want to reveal:
	err = AECreateAppleEvent('misc', 'mvis', 
							&aeDesc,
							kAutoGenerateReturnID, 1L,
							&appleEvent);
							
	err = AECreateDesc(typeFSS, (Ptr)(&(gFSSpec_arr[item_num])), (Size)sizeof(FSSpec), &fssDesc);
	
	//Add the list as the parameter for appleEvent:
	err = AEPutParamDesc(&appleEvent,keyDirectObject,&fssDesc);
	
	//Send the AppleEvent
	err = AESend( &appleEvent, &reply, 
				kAENoReply+kAECanInteract+kAECanSwitchLayer,
				kAENormalPriority,
				kAEDefaultTimeout, 
				nil, nil);
				
	//Dispose descriptors
	AEDisposeDesc(&aeDesc);	
	AEDisposeDesc(&fssDesc);
	//And launch the finder w/o flashing icon:
	launchPBR.launchBlockID = extendedBlock; //const indicating you're launching
	launchPBR.launchEPBLength = extendedBlockLen; //const specifying size, also from Processes.h
	//no need to set the  .launchFileFlags because using extendedBlock
	launchPBR.launchControlFlags = launchContinue;
	launchPBR.launchAppSpec = &(gFSSpec_arr[0]);
	LaunchApplication(&launchPBR);
}

/** Trial function to use AppleEvents to open a file
 ** Later will be modified to open the parent directory of an app (e.g.)
 **/
void OpenApp(){
	OSErr err;
	AEDesc aeDesc = {typeNull, NULL};
	long appSig, aliasKW;
	AppleEvent appleEvent, reply;
	
	AliasHandle myAlias;
	AEDescList myList, aliasDesc;
	Str63 aliasInfo;
	
	appSig = 'MACS';
	err = AECreateDesc(typeApplSignature, (Ptr)(&appSig),
				(Size)sizeof(appSig), &aeDesc);
	if(err != noErr) {SysBeep(10); ExitToShell();}
	
	err = AECreateAppleEvent('FNDR', 'sope', 
							&aeDesc,
							kAutoGenerateReturnID, 1L,
							&appleEvent);
	if(err != noErr) {SysBeep(10); ExitToShell();}
	
	err = NewAlias(nil,&(gFSSpec_arr[2]), &myAlias);
	if(err != noErr) {SysBeep(10); ExitToShell();}
	
	err = GetAliasInfo(myAlias, asiVolumeName, aliasInfo);
	err = GetAliasInfo(myAlias, asiParentName, aliasInfo);
	err = GetAliasInfo(myAlias, asiAliasName, aliasInfo); 
	// NOTE the above 3 AliasInfo responses all check out good
	
	
	
	err = AECreateList(nil, 0, false, &myList);
	if(err != noErr) {SysBeep(10); ExitToShell();}
	
	err = AECreateDesc(typeAlias, (Ptr)(&myAlias), (Size)sizeof(myAlias), &aliasDesc);
	if(err != noErr) {SysBeep(10); ExitToShell();}
	
	err = AEPutDesc(&myList, 1, &aliasDesc);
	if(err != noErr) {SysBeep(10); ExitToShell();}
	
	    err = AEPutParamDesc(&appleEvent,'fsel',(&myList));
	if(err != noErr) {SysBeep(10); ExitToShell();}

	                        
	err = AESend( &appleEvent, &reply, 
				kAENoReply+kAECanInteract+kAECanSwitchLayer,
				kAENormalPriority,
				kAEDefaultTimeout, 
				nil, nil);
	if(err != noErr) {SysBeep(10); ExitToShell();}
	
	AEDisposeDesc(&aeDesc);
	AEDisposeDesc(&aliasDesc);
}


/* UpdateRunningApps
 * This is the core function to update the Dock contents
 * with the current running apps.  
 * It does so by repeated queries to GetNextProcess
 * Accounts for the Finder not technically being an APPL type
 * Relies on constants kDockHSpace and kDockIconWidth 
 * and kDockIconHeight to determine the spacing.  See .h file.
 */ 
void UpdateRunningApps(Boolean ForceRedraw){
	ProcessInfoRec procInfoRec;
	Str255 procName;
	FSSpec appFileLoc; //includes member .vRefNum the vol ref number
	OSErr osErr;
	ProcessSerialNumber PSN;
	DTPBRec DTPBRecord;
	short num_apps = 0;
	Rect dock_bounds;
	Handle handle;
	unsigned long PsnHiCheckSum, PsnLowCheckSum;
	
	dock_bounds.top=kDockVSpace; 
	dock_bounds.left=kDockHSpace;
	dock_bounds.bottom=dock_bounds.top+kDockIconHeight; 
	dock_bounds.right = dock_bounds.left+kDockIconWidth;
	procInfoRec.processAppSpec = &appFileLoc;
	procInfoRec.processName = (StringPtr)(&procName);
	PSN.highLongOfPSN = 0; //Init PSN//
	PSN.lowLongOfPSN = kNoProcess;
	/**** Setup ParamBlock ****/
	//PBRecord.ioCompletion = nil;
	/**** Setup DTPBRecord ****/
	DTPBRecord.ioCompletion = nil;
	DTPBRecord.ioNamePtr = nil;
	DTPBRecord.ioDTRefNum = 0; //needs desktop db ref #!!!
	DTPBRecord.ioTagInfo = 0; //must be 0, RESERVED
	DtpbPtrNHandle.run_count = 0;
	DtpbPtrNHandle.DTPBptr = &DTPBRecord;
	DTPBRecord.ioDTBuffer = nil;	
	
	//Future: check that this desktop has a desktop database else quit
	
	PsnHiCheckSum = 0;
	PsnLowCheckSum = 0;
	//	Check if there has been a change in running apps
	while(GetNextProcess(&PSN) == noErr)
	{
		PsnHiCheckSum += PSN.highLongOfPSN;
		PsnLowCheckSum += PSN.lowLongOfPSN;
	}
	//	If there's been a change or the force flag, then redraw the dock
	if(gPsnHiLast != PsnHiCheckSum ||
		gPsnLowLast != PsnLowCheckSum ||	//if check indicates changes in apps
		ForceRedraw ||
		0 == gFSSpec_cnt) //if zero apps reported (assume an error occured in GetNextProcess()
	{
		gPsnHiLast = PsnHiCheckSum;
		gPsnLowLast = PsnLowCheckSum;
		gFSSpec_cnt = 0; //reset app count
		PSN.highLongOfPSN = 0;
		PSN.lowLongOfPSN = kNoProcess;
		
		EraseRect(&(thePort->portRect));
		while(GetNextProcess(&PSN) == noErr)
		{
			osErr = GetProcessInformation(&PSN,(ProcessInfoRecPtr)(&procInfoRec));
			if(procInfoRec.processType == 'APPL'|| procInfoRec.processType == 'FNDR')
			if(procInfoRec.processSignature != 'MCDK'||gShowSelf)
			{
				//look up its icon and get ready to update the dock
				DTPBRecord.ioFileCreator = procInfoRec.processSignature; //get the creator from the signature
				DTPBRecord.ioFileType = procInfoRec.processType;//get the app icon
				DTPBRecord.ioVRefNum = procInfoRec.processAppSpec->vRefNum;
				PBDTGetPath((DTPBPtr)(&DTPBRecord));
				if(noErr != DTPBRecord.ioResult)
				{
					SysBeep(10);
				 	ExitToShell();
				}
				
				//save the FSSPec and update the count:
				gFSSpec_arr[gFSSpec_cnt] = *(procInfoRec.processAppSpec);
				gFSSpec_cnt = gFSSpec_cnt+1;
								
				if(procInfoRec.processSignature == 'MCDK') //save my location in the dock
					gMacDockItemNum = gFSSpec_cnt-1;
				
				PlotIconMethod(&dock_bounds,0,0, IconGetter,(Ptr)(&DtpbPtrNHandle));//allocates handle inside...
				if(DtpbPtrNHandle.run_count >= 1);
					//DisposeHandle(DtpbPtrNHandle.MaskData);//release handle memory
				if(DtpbPtrNHandle.run_count >= 2);
					//DisposeHandle(DtpbPtrNHandle.PixelMapData);//release handle memory
				//DtpbPtrNHandle.run_count = 0;
				//DtpbPtrNHandle.handle = nil;
				
				if(DTPBRecord.ioResult != noErr)
				{ 
					EraseRect(&dock_bounds);
					if(procInfoRec.processSignature == 'MACS')
					{//	draw Finder icon, stored internally
						EraseRect(&dock_bounds);
						PlotIconID(&dock_bounds,0,0,kFinderICON_ID);
					}
					else if(DtpbPtrNHandle.run_count >= 2)
					{//	if got the b&w icon but didn't find color, plot b&w:
						PlotIcon(&dock_bounds,DtpbPtrNHandle.MaskData);
					}
					else
					{//plot a generic app icon
						PlotIconID(&dock_bounds,0,0,kGenAppICON_ID);
					}
				}
				DtpbPtrNHandle.run_count = 0;
				num_apps++;
				dock_bounds.left+=kDockHSpace+kDockIconWidth+kDockHSpace;
				dock_bounds.right+=kDockHSpace+kDockIconWidth+kDockHSpace;
			}
		}
		//And Erase the rest of the dock (re-use dock_bounds var):
		dock_bounds.bottom=(thePort->portRect).bottom;
		dock_bounds.bottom=thePort->portRect.right;
		//EraseRect(&dock_bounds);
	}
}
