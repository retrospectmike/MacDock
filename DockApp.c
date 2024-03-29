#include"GestaltEqu.h"
#include<Values.h>
#include<AppleEvents.h>
#include "DockCoreUpdate.h"
#include "Files.h" 
#include "IconPlotting.h"

#define kBaseResID 128 	// base id for all resources
#define kAboutWinID 129	// Window rsrc ID
#define kMoveToFront (WindowPtr)-1L 
#define mApple	kBaseResID
#define iAbout 1
#define mFile kBaseResID+1
#define iUpdateAppList	1
#define iQuit	3
#define mOptions mFile+1
#define iAutoHide 1
#define iShowSelf 2
#define osEvtMsgBitShift 24

#define kSleep 20L
#define kGestaltMask 1L //apple events checking


#define SetHighByte(longNum) (longNum |=0xFF000000)
#define ClearHighByte(longNum) (longNum &= 0x00FFFFFF)
#define UNLISTED 999

unsigned short gMacDockItemNum = UNLISTED;		/*Unreachable item number*/
DTPBptrAndHandle DtpbPtrNHandle; // global used in DockCoreUpdate but init'ed in main

Boolean gDone, gDockHidden, gShowSelf;
unsigned short gPixelBitDepth;
Boolean gStopNullEventUpdates = false;

void ToolBoxInit();
void WindowInit();
void MenuBarInit();
void EventLoop();
void DoEvent(EventRecord *eventPtr);
void HandleNull(EventRecord *eventPtr);
void HandleMouseDown(EventRecord *eventPtr);
void HandleMenuChoice(long menuChocie);
void HandleAppleChoice(short item);
void HandleFileChoice(short item);
void HandleOptionsChoice(short item);
void UpdateRunningApps(Boolean ForceRedraw);
void DoUpdate(EventRecord *eventPtr);
void HandleMouseMove(EventRecord *eventPtr);
void HandleMouseUp(EventRecord *eventPtr, PointPtr lastMouseDown);
void EventInit();
void HideDock();
void ShowDock();
void SystemIdentify();

//AppleEvent handlers:
pascal OSErr DoOpenApp(AppleEvent theAppleEvent, AppleEvent reply, long refCon);
pascal OSErr DoQuitApp(AppleEvent theAppleEvent, AppleEvent reply, long refCon);
pascal OSErr DoPrintDoc(AppleEvent theAppleEvent, AppleEvent reply, long refCon);
pascal OSErr DoOpenDoc(AppleEvent theAppleEvent, AppleEvent reply, long refCon);

Boolean gAutoHide = false;	//Default setting for auto hide Dock option

void main(){
	ToolBoxInit();
	WindowInit();
	MenuBarInit();
	gShowSelf = false;	
	SystemIdentify(); //for future release to ID color screens
	DtpbPtrNHandle.MaskData = NewHandle(kLargeIconSize);
	DtpbPtrNHandle.PixelMapData = NewHandle(kLarge8BitIconSize);
	
	EventInit();
	EventLoop();
}

/* SystemIdentify() - find out screen color */
void SystemIdentify()
{	
 GDHandle screen;
 screen = GetMainDevice();
 gPixelBitDepth = (**(**screen).gdPMap).pixelSize;//TestDeviceAttribute(screen,gdDevType);
 //if(1 < gPixelBitDepth) SysBeep(10);
}

/* ToolBoxInit() - standard init funcs caller */
void ToolBoxInit(){
	InitGraf(&thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(nil);
	InitCursor();
}

/**** MenuBarInit() - Standard menubar inits****/
void MenuBarInit(){
	Handle menuBar;
	MenuHandle menu;
	ControlHandle control;
	long feature;
	
	menuBar = GetNewMBar(kBaseResID);
	SetMenuBar(menuBar);
	menu = GetMHandle(mApple);
	AddResMenu(menu,'DRVR');
	DrawMenuBar();
}
/*	WindowInit()
 *	Reads a window resource and shows the Dock window
 *	Checks computer's screen size and positions
 *	Dock window in the bottom center.
 *	Also initializes the global gDockHidden to not hide
 */
void WindowInit(){
	WindowPtr window;
	GDHandle screen;
	short screen_res_x, screen_res_y;
	short dockWinHeight,dockWinWidth;
	window = GetNewCWindow(kBaseResID,nil,kMoveToFront);
	//MOVE WINDOW TO BOTTOM CENTER OF SCREEN AND
	//DYNAMICALLY ACCOUNT FOR CUR SCREEN SIZE:
	dockWinWidth = window->portRect.right-thePort->portRect.left;
	dockWinHeight = window->portRect.bottom-thePort->portRect.top;
	screen = GetMainDevice();
	if(window==nil){
		SysBeep(10);
		ExitToShell();}
	MoveWindow(window,
				(**screen).gdRect.right/2-dockWinWidth/2,
				(**screen).gdRect.bottom-dockWinHeight,
				true);
	ShowWindow(window);
	SetPort(window);
	gDockHidden = false;
}


/* EventLoop()
 * Top-level loop handling
 * Also handles Null events
 */
void EventLoop(){
	EventRecord event;
	static long lastNullUpdate;
	gDone = false;
	while(gDone == false)
	{
		//Future work: create the initial Region to be inside 
		//the dock to trigger osEvt
		//for mouse if it moves
		if(WaitNextEvent(everyEvent, &event, kSleep, nil))
			DoEvent(&event);
		else if(!gStopNullEventUpdates){
				if(lastNullUpdate - TickCount() > 30)
					UpdateRunningApps(false); //ForceDraw == false
			}
	}
}

/* HideDock
 * Tucks the Dock window out of the way 
 */
void HideDock(){
	WindowPtr window;
	GDHandle screen;
	short dockWinHeight,dockWinWidth;
	window = thePort; //make this smarter to support mult windows
	dockWinWidth = window->portRect.right-thePort->portRect.left;
	dockWinHeight = window->portRect.bottom-thePort->portRect.top;
	screen = GetMainDevice();
	if(window==nil){SysBeep(10);ExitToShell();}
	MoveWindow(window,
			(**screen).gdRect.right/2-dockWinWidth/2,
			(**screen).gdRect.bottom-dockWinHeight/2,
			true);
	gDockHidden = true;
}
/* ShowDock
 * Positions the Dock window to be in full view position
 */
void ShowDock(){
	WindowPtr window;
	GDHandle screen;
	short dockWinHeight,dockWinWidth;
	window = thePort; //make this smarter to support mult windows
	dockWinWidth = window->portRect.right-thePort->portRect.left;
	dockWinHeight = window->portRect.bottom-thePort->portRect.top;
	screen = GetMainDevice();
	if(window==nil){SysBeep(10);ExitToShell();}
	MoveWindow(window,
			(**screen).gdRect.right/2-dockWinWidth/2,
			(**screen).gdRect.bottom-dockWinHeight,
			true);
	gDockHidden = false;
}

/* HandleOsEvt
 * Handler for suspends/resumes mainly to support
 * 'clicking through' from another app -> into MacDock -> to the 
 * destination app all in one click.
 * As well as one of many functs that can Show the Dock if hidden
 */
void HandleOsEvt(EventRecord *eventPtr){
	if((eventPtr->message)>>osEvtMsgBitShift == suspendResumeMessage){
		//Suspend-Resume:;
		if((eventPtr->message)&0x0001){ //Resume
			GlobalToLocal(&eventPtr->where);
			if((eventPtr->where).v > 0 && (eventPtr->where).v < kDockIconHeight+2*kDockVSpace )
			{	//Resumed right into the Dock window
				if((eventPtr->modifiers) & cmdKey)//Cmd+Click
				{
					if(!RevealApp((eventPtr->where).h / (kDockHSpace+kDockIconWidth+kDockHSpace)))
					{
						if(gAutoHide && gDockHidden)
							ShowDock(); 
					}
				}
				else if(!LaunchApp((eventPtr->where).h / 
							  (kDockHSpace+kDockIconWidth+kDockHSpace)))
				{
					if(gAutoHide && gDockHidden)
						ShowDock(); 
				}
			}
			else{ //Resumed outside of the Dock 
				if(gAutoHide && gDockHidden)
					ShowDock();
			}
		}
		else{ //Suspend: Hide dock.
			if(gAutoHide && !gDockHidden)
				HideDock();
		}
	}
}
/* HandleMouseMove
 * Unused at the moment, meant to one day handle
 * any mouse-motion-activated feedback such
 * as highlighting an app icon
 */
void HandleMouseMove(EventRecord *eventPtr){
	WindowPtr whichWindow;
	GrafPtr oldPort;
	short thePart;
	thePart = FindWindow(eventPtr->where, &whichWindow);
	switch(thePart){
		case inContent:
			SetPort(whichWindow);
			GlobalToLocal(&eventPtr->where);
			//LaunchApp((eventPtr->where).h / (kDockHSpace+kDockIconWidth+kDockHSpace));
			//HighlightItem((eventPtr->where).h / (kDockHSpace+kDockIconWidth+kDockHSpace));
			break;
	}
}

/* DoEvent
 * Standard event handler
 */
void DoEvent(EventRecord *eventPtr){
  char theChar;
  static Point lastMouseDownPt;
  switch(eventPtr->what){
  	case kHighLevelEvent:
  		AEProcessAppleEvent(eventPtr);
  		break;
  	case osEvt:
  		HandleOsEvt(eventPtr);
  		//if(eventPtr->message & mouseMovedMessage;
  		break;
    case mouseDown:
    	HandleMouseDown(eventPtr);
    	lastMouseDownPt = eventPtr->where; //in local coordinates by now
    	break;
    case mouseUp:
    	//HandleMouseUp(eventPtr,&lastMouseDownPt);
    	break;
    case keyDown:
    case autoKey:
    	theChar = eventPtr->message & charCodeMask;
    	if((eventPtr->modifiers & cmdKey) != 0)
    		HandleMenuChoice(MenuKey(theChar));
    	break;
    case updateEvt:
    	DoUpdate(eventPtr);
    	break;
  }
}

/* HandleMouseUp
 * UNUSED at the moment but meant to handle
 * functionality such as "un-highlighting" an app
 * that was clicked down on (which could highlight it)
 */
void HandleMouseUp_NU(EventRecord *eventPtr, PointPtr lastMouseDown){
	WindowPtr whichWindow;
	GrafPtr oldPort;
	short thePart;
	ControlHandle control;
	short lastMouseDownItemNum, curMouseDownItemNum;
	
	thePart = FindWindow(eventPtr->where, &whichWindow);
	
	lastMouseDownItemNum = lastMouseDown->h / 
							(kDockHSpace+kDockIconWidth+kDockHSpace);
	GlobalToLocal(&eventPtr->where);
	curMouseDownItemNum = (eventPtr->where).h / 
							(kDockHSpace+kDockIconWidth+kDockHSpace);
	switch(thePart)
	{
		case inContent:
			SetPort(whichWindow);
			if(FindControl(eventPtr->where, whichWindow, &control))
			{
				//no controls in MacDock yet..
			}
			else
			{
				if(lastMouseDownItemNum == curMouseDownItemNum)
				{
					LaunchApp(curMouseDownItemNum);
				}
				else
				{
					InvertItem(lastMouseDownItemNum);
				}
			}
			break;
	}
}
/* HandleMouseDown
 * Watch for clicks in the Dock and launch 
 * that app if it happens.
 */
void HandleMouseDown(EventRecord *eventPtr){
	WindowPtr whichWindow;
	GrafPtr oldPort;
	short thePart;
	long menuChoice;
	ControlHandle control;
	short ignored;
	thePart = FindWindow(eventPtr->where, &whichWindow);
	switch(thePart){
		case inMenuBar:
			menuChoice = MenuSelect(eventPtr->where);
			HandleMenuChoice(menuChoice);
			break;
		case inSysWindow:
			SystemClick(eventPtr, whichWindow);
			break;
		case inContent:
			SetPort(whichWindow);
			GlobalToLocal(&eventPtr->where);
			if(FindControl(eventPtr->where, whichWindow, &control)){
				; //no controls in my program..
			}
			else
			{ //mouse down in content of Dock
				if((eventPtr->modifiers) & cmdKey)//Cmd+Click
				{
					if(!RevealApp((eventPtr->where).h / (kDockHSpace+kDockIconWidth+kDockHSpace)))
					{
						if(gAutoHide && gDockHidden)
							ShowDock(); 
					}
				}
				else if(!LaunchApp((eventPtr->where).h / (kDockHSpace+kDockIconWidth+kDockHSpace)))
				{
					if(gAutoHide && gDockHidden)
						ShowDock(); 
				}
			}
			break;
		case inDrag:
			//DragWindow(whichWindow, eventPtr->where,
			//			&screenBits.bounds);
			break;

	}
}

void HandleMenuChoice(long menuChoice)
{
	short menu;
	short item;
	if(menuChoice!=0)
	{
		menu = HiWord(menuChoice);
		item = LoWord(menuChoice);
		
		switch(menu)
		{
			case mApple:
				HandleAppleChoice(item);
				break;
			case mFile:
				HandleFileChoice(item);
				break;
			case mOptions:
				HandleOptionsChoice(item);
				break;
		}
		HiliteMenu(0);
	}
}

void HandleAppleChoice(short item)
{
	MenuHandle appleMenu;
	Str255 accName;
	short accNumber;
	DialogPtr dialog;
	Boolean dialogDone = false;
	short itemHit;
	switch(item){
		case iAbout:
			gStopNullEventUpdates = true;
			dialog = GetNewDialog(kBaseResID, nil, kMoveToFront);
			ShowWindow(dialog);
			while(! dialogDone){
				ModalDialog(nil, &itemHit);
				switch(itemHit){
					default:
						dialogDone = true;
				}
			DisposDialog(dialog);
			gStopNullEventUpdates = false;
			}
			break;
		default:
			appleMenu = GetMHandle(mApple);
			GetItem(appleMenu,item,accName);
			accNumber = OpenDeskAcc(accName);
			break;
	}
}

void HandleFileChoice(short item){
	switch(item){
		case iUpdateAppList:
			UpdateRunningApps(true);
			break;
		case iQuit:
			gDone = true;
			break;
	}
}

/* HandleOptionsChoice
 * Handles Optios menu -related choices.
 * Includes checking/unchecking toggle options
 * such as AutoHide and updating state var values.
 */
void HandleOptionsChoice(short item){
	MenuHandle menu;
	switch(item){
		case iAutoHide:
			menu = GetMenu(mOptions);
			if(!gAutoHide)
				CheckItem(menu, iAutoHide, true);
			else
				CheckItem(menu, iAutoHide, false);
			
			gAutoHide = !gAutoHide;
			break;
		case iShowSelf:
			menu = GetMenu(mOptions);
			if(!gShowSelf)
				CheckItem(menu, iShowSelf, true);
			else
				CheckItem(menu, iShowSelf, false);
			
			gShowSelf = !gShowSelf;
			UpdateRunningApps(true);
			break;
	}
}

/* DoUpdate
 * Handles window uncovering
 * update events.  For now this just
 * naively redraws the whole Dock.
 */
void DoUpdate(EventRecord *eventPtr)
{
	WindowPtr window;
	window = (WindowPtr) eventPtr->message;
	BeginUpdate(window);
	EndUpdate( window );
	
	UpdateRunningApps(true);/*Do this outside of the BeginUpdate / EndUpdate
							 * or else missing segments of icons occur*/
}

/* EventInit
 * Initializes for the basic 4 required
 * AppleEvents, otherwise does nothing.
 */
void EventInit(){
	OSErr err;
	long feature;
	
	err = Gestalt(gestaltAppleEventsAttr,&feature);
	if(err!=noErr){SysBeep(10);return;}
	else{
		if(!(feature & (kGestaltMask << gestaltAppleEventsPresent)))
		{SysBeep(10);return;}
	}
	err = AEInstallEventHandler( kCoreEventClass, kAEOpenApplication, DoOpenApp, 0L, false);
	if(err!=noErr){SysBeep(10);return;}
	err = AEInstallEventHandler( kCoreEventClass, kAEOpenDocuments, DoOpenDoc, 0L, false);
	if(err!=noErr){SysBeep(10);return;}
	err = AEInstallEventHandler( kCoreEventClass, kAEPrintDocuments, DoPrintDoc, 0L, false);
	if(err!=noErr){SysBeep(10);return;}
	err = AEInstallEventHandler( kCoreEventClass, kAEQuitApplication, DoQuitApp, 0L, false);
	if(err!=noErr){SysBeep(10);return;}
}

/* AppleEvent handler functions 
 * Currently do nothing but return noErr
 */
pascal OSErr DoOpenApp(AppleEvent theAppleEvent, AppleEvent reply, long refCon){
	UpdateRunningApps(true);
	return noErr;
}
pascal OSErr DoQuitApp(AppleEvent theAppleEvent, AppleEvent reply, long refCon){
	gDone = true;
	return noErr;
}
pascal OSErr DoOpenDoc(AppleEvent theAppleEvent, AppleEvent reply, long refCon){
	return noErr;
}
pascal OSErr DoPrintDoc(AppleEvent theAppleEvent, AppleEvent reply, long refCon){
	return noErr;
} 
