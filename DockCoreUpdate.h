#ifndef __COREUPDATE__#define __COREUPDATE__#include "QuickDraw.h"#include "ToolUtils.h"#include <GestaltEqu.h>#include <AppleEvents.h>#include "Processes.h" //for paramblockrec stuff and processLaunch#define kFinderICON_ID 128#define kGenAppICON_ID 129#define kDockIconWidth 32#define kDockIconHeight kDockIconWidth //square icons#define kDockHSpace 2#define kDockVSpace 2#define MAX_DOCK_ITEM_COUNT 8#define kBlinksOnClick 1 //number of blinks when an app is clicked#define kTicksBtwBlink 5 //60ths of a second between blinksvoid UpdateRunningApps();void OpenApp();Boolean LaunchApp(unsigned short);void FlashItem(unsigned short item_num);void InvertItem(unsigned short item_num);#endif