#include "IconPlotting.h"

pascal Handle IconGetter(ResType iconType, Ptr data){
	short s, k;
	Handle h;
	DTPBRec* DTPBRecPtr;
	DTPBptrAndHandle* DtpbPtrNHandlePtr;
	OSErr err;
	DtpbPtrNHandlePtr = (DTPBptrAndHandle*)data;
	DTPBRecPtr = DtpbPtrNHandlePtr->DTPBptr;
    switch (iconType)
    {
        case 'ICN#': s = kLargeIconSize;     k = kLargeIcon;     break;
        case 'icl4': s = kLarge4BitIconSize; k = kLarge4BitIcon; break;
        case 'icl8': s = kLarge8BitIconSize; k = kLarge8BitIcon; break;
        case 'ics#': s = kSmallIconSize;     k = kSmallIcon;     break;
        case 'ics4': s = kSmall4BitIconSize; k = kSmall4BitIcon; break;
        case 'ics8': s = kSmall8BitIconSize; k = kSmall8BitIcon; break;

        default: SysBeep(10);ExitToShell();DebugStr("\punknown icon type");
    }
	DTPBRecPtr->ioDTReqCount = s;
	DTPBRecPtr->ioIconType = k;
	if(DtpbPtrNHandlePtr->run_count ==0){
		//DtpbPtrNHandlePtr->MaskData = NewHandle(s);
		h = DtpbPtrNHandlePtr->MaskData;
		DtpbPtrNHandlePtr->run_count = 1;
	}
	else{
		//DtpbPtrNHandlePtr->PixelMapData = NewHandle(s);
		h = DtpbPtrNHandlePtr->PixelMapData;
		DtpbPtrNHandlePtr->run_count = 2;
	}
	HLock(h);
	DTPBRecPtr->ioDTBuffer = *(h);
	err=PBDTGetIcon(DTPBRecPtr,false); //err also gets recorded in DTPBRecPtr!
	return h;
}
