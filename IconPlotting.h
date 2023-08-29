/* IconPlotting.h */
#include <Files.h>
#ifndef __ICONPLOTTING__
#define __ICONPLOTTING__

struct DTPBptrAndHandle{
DTPBPtr DTPBptr;
Handle MaskData;
Handle PixelMapData;
short run_count;
};
typedef struct DTPBptrAndHandle DTPBptrAndHandle;

pascal Handle IconGetter(ResType iconType, Ptr data);

pascal OSErr MakeIconCache (Handle *theCache, ProcPtr GetAnIcon, Ptr
        yourDataPtr ) = {0x303C, 0x0604, 0xABC9};

pascal OSErr LoadIconCache (const Rect *theRect, short alignment, long
        transform, Handle theSuite ) = {0x303C, 0x0606, 0xABC9};

pascal OSErr PlotIconSuite (const Rect *theRect, short alignment,
        short transform, Handle theSuite) = {0x303C, 0x0501, 0xABC9};

pascal OSErr DisposeIconSuite (Handle theSuite, Boolean disposeData)
        = {0x303C, 0x0302, 0xABC9};

pascal OSErr PlotIconMethod (const Rect *r, short alignment, short transform, ProcPtr method, Ptr data)
        = {0x303C, 0x0805, 0xABC9};
        
pascal OSErr PlotIconID (const Rect *theRect, short alignment,
        short transform, short theResID) = {0x303C, 0x0500, 0xABC9};

#endif
