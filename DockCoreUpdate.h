#ifndef __COREUPDATE__#define __COREUPDATE__#include <QuickDraw.h>#include <ToolUtils.h>#include <GestaltEqu.h>#include <AppleEvents.h>#include <Processes.h> //for paramblockrec stuff and processLaunch#include <Files.h>#include <Folders.h>#include <Script.h>#include <Aliases.h>#define kFinderICON_ID 129#define kGenAppICON_ID 130#define kSmallIconHeight 16#define kLargeIconHeight 32#define kDockHBorderBig 6#define kDockHBorderSmall 3#define kDockHSpace 2#define kDockVSpace 2#define MAX_DOCK_ITEM_COUNT 16#define kBlinksOnClick 1 //number of blinks when an app is clicked#define kTicksBtwBlink 5 //60ths of a second between blinks#define kNumDockableProcessTypes 5#define kDockFaveALIS_baseID 128#define kMacDockAppSignature 'MCDK'struct DockRecord{  FSSpec fsspec; //including a valid vRefNum for PBDTRec creation  OSType ioFileCreator; //aka the process's processSignature  unsigned long ioFileType; //aka process's processType    Boolean isFavorited;  Boolean isRunning;};typedef struct DockRecord DockRecord;struct DockListNode{  struct DockRecord dockRec;  struct DockListNode* prev;  struct DockListNode* next;};typedef struct DockListNode DockListNode;typedef struct DockListNode* DockListNodePtr;DockListNode* MakeDockListNodeWithDummyItem();void DeleteDockListNode(DockListNode **dListPtr);Boolean DockToggleFavoriteForItem(unsigned short item_num);void InitDock();Boolean IsDockableProcess(ProcessInfoRec *procInfoRec);Boolean CheckForRunningAppChange(short   *num_apps, 								unsigned long *, 								unsigned long *);void UpdateRunningApps(Boolean ForceRedraw);void OpenApp();Boolean LaunchApp(unsigned short);void FlashItem(unsigned short item_num);void InvertItem(unsigned short item_num);void UpdateDockSize(unsigned short NumApps);unsigned short CalcSlotClicked(Point *pt);pascal Handle IconGetter(ResType iconType, Ptr data);pascal OSErr FindFolder(short vRefNum,OSType folderType,Boolean createFolder,    short *foundVRefNum,long *foundDirID)    = {0x7000,0xA823}; #endif