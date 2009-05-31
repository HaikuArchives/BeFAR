#include <stdio.h>

#include "BF_Msg.h" 
#include "BF_GUI_Func.h" 
#include "BF_GUI_DlgViews.h"  
#include "BF_GUI_WinMain.h"  

#include <Clipboard.h>

/*====================================================================*/
BF_GUI_ViewEdit::BF_GUI_ViewEdit(
	const  BRect & o_Rect,
	const char *pc_Name,
	const char *pc_Value,
	uint32	i_FollowMode,
	uint32	i_Flags
)
:BF_GUI_DialogView(o_Rect,pc_Name,i_FollowMode,i_Flags|B_WILL_DRAW)
{
	if(pc_Value) sValue = pc_Value;
	//
	idThreadPulse = 0;
	iNavIndex = sValue.LengthUTF8();
	iNavFirstChar = 0;
	bSelected = true;
	bSysKeysHandle = true;
	bCursorActive = true;
	//
	SetViewColor(B_TRANSPARENT_COLOR);	
	bPulseCursorShow = true;
	//	
	bParentCall = true;
	bAlwaysCursor = false;	
	oColBack = SYS_COLOR(BF_COLOR_DIALOG_BUTTON_FOCUS);
	oColSel = SYS_COLOR(BF_COLOR_DIALOG_BUTTON_TEXT);
	//
	iDisableCount = 0;
}	

BF_GUI_ViewEdit::~BF_GUI_ViewEdit()
{
	while(B_OK!=kill_thread(idThreadPulse));
	printf("BF_GUI_ViewEdit::~BF_GUI_ViewEdit()\n");
}

void 					
BF_GUI_ViewEdit::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what){
	case B_COPY:{
		if(be_clipboard->Lock()){					
			be_clipboard->Clear();			
			BMessage *po = be_clipboard->Data();
			po->MakeEmpty();
			po->AddData("text/plain", B_MIME_TYPE, sValue.String(),sValue.Length());		
			be_clipboard->Commit();
			be_clipboard->Unlock();
		}			
		break;}
	case B_PASTE:{		
		if(!be_clipboard->Lock()) return;
		BMessage 	*po = be_clipboard->Data();
		const char	*pcData=NULL;
		int32		iTextLen;		
		if(!po || B_OK!=po->FindData("text/plain",B_MIME_TYPE,(const void**)&pcData,&iTextLen) || !pcData) return;
		if(iTextLen<=0) return;
		
		// set end of line
		char pcBuffer[iTextLen+1];
		memcpy(pcBuffer,pcData,iTextLen);
		pcBuffer[iTextLen] = '\0';		
		// update screen		
		SetText(pcBuffer,true);
		be_clipboard->Unlock();
		break;}
	case BF_MSG_PULSE:
		Pulse();
		break;
	default:
		BView::MessageReceived(po_Message);
	}
}

int32 
BF_GUI_ViewEdit_PulseThread(void *data)
{
	ASSERT(data);
	BView *poView = (BView*)data;
	BMessage oMessage(BF_MSG_PULSE);
	BMessenger oMessenger(poView);
	
	while(true){
		snooze(200000);
		oMessenger.SendMessage(&oMessage);
	}
}


void		
BF_GUI_ViewEdit::AttachedToWindow(void)
{
	BView::AttachedToWindow();
	//Window()->SetPulseRate(100);	
	
	idThreadPulse = spawn_thread(BF_GUI_ViewEdit_PulseThread,"BeFar:edit_pulse",B_THREAD_SUSPENDED,(void*)this);	
	ASSERT(idThreadPulse>0,"can`t start thread BeFar:edit_pulse\n");	
	ASSERT(B_OK==resume_thread(idThreadPulse));		
	set_thread_priority(idThreadPulse,1);
}

void		
BF_GUI_ViewEdit::Pulse(void)
{
	bPulseCursorShow = !bPulseCursorShow;	
	if(bCursorActive) DrawCursor(bPulseCursorShow && (Window()->IsActive() || bAlwaysCursor));	
}

void					
BF_GUI_ViewEdit::Draw(BRect o_Rect)
{
	/* draw body */
	DrawBody(o_Rect);
 	/* draw cursor */
 	DrawCursor(bPulseCursorShow); 
}

void
BF_GUI_ViewEdit::DrawBody(BRect o_Rect)
{
	char *pcText=(char*)sValue.String()+ PCharCharPosInBytes_UTF8(sValue.String(),iNavFirstChar)  ;
	BRect oRect = Bounds();
	/* draw background */
	SetHighColor(oColBack);
	FillRect(oRect);
	/* draw selected background */ 
	if(bSelected){
		BRect oRect1(oRect);
		oRect1.right = oRect1.left+1+StringWidth(pcText);
		SetHighColor(oColSel);
		FillRect(oRect1);
	}
	/* draw text */

	if(bSelected){
		SetHighColor(oColBack);
		SetLowColor(oColSel); 
	}else{
		SetHighColor(oColSel);
		SetLowColor(oColBack); 
	}
	MovePenTo(1,oRect.Height()-poFont->fDescent);
 	DrawString(pcText);
}

void					
BF_GUI_ViewEdit::SetText(const char *pc_NewText,bool b_GoEnd)
{		
	ASSERT(pc_NewText);
	iNavIndex = 0;
	iNavFirstChar = 0;
	sValue = pc_NewText;			
	Invalidate(Bounds());
	
	if(b_GoEnd) NavGo(sValue.Length());
}

void 					
BF_GUI_ViewEdit::MakeFocus(bool focused)
{	
	BView::MakeFocus(focused);
	if(focused){
		resume_thread(idThreadPulse);		
		DrawCursor(TRUE);
	}else{
		suspend_thread(idThreadPulse);
		if(!bAlwaysCursor) DrawCursor(FALSE);
	}
}

void 
BF_GUI_ViewEdit::DrawCursor(bool b_Show)
{
	printf("----start --------------------------------\n");
	
	if(!bAlwaysCursor && !IsFocus()) b_Show = false;
	//
	float fWidth = 0;
	BRect oRect(Bounds());
	bool bReqFullDrawing=false;
	//
	if(iNavIndex<iNavFirstChar){
		iNavFirstChar = iNavIndex;
		bReqFullDrawing = true;
	}
	//
	if(iNavIndex>0){
		BL_String s;						
		while(TRUE){				
			s = sValue;
			printf("pre  RemoveUTF8 1\n");
			s.RemoveUTF8(iNavIndex,s.LengthUTF8()-iNavIndex);
			printf("post RemoveUTF8 1 iNavIndex=%i LengthBytes=%i LengthChars=%i new LengthBytes=%i new LengthChars=%i \n",
				iNavIndex,sValue.Length(),sValue.LengthUTF8(),s.Length(),s.LengthUTF8());
			s.RemoveUTF8(0,iNavFirstChar);
			printf("RemoveUTF8 2 iNavFirstChar=%i new length=%i\n",iNavFirstChar,s.Length());
			fWidth = poFont->oFont.StringWidth(s.String());	
			if((fWidth+1)>=oRect.Width()){
				iNavFirstChar++;				
				bReqFullDrawing = true;
				continue;
			}
			break;			
		}				
	}	
	
	if(bReqFullDrawing){
		DrawBody(Bounds());					
		return;
	}
	
	
	if(b_Show) 	SetHighColor(oColSel); 
	else 		SetHighColor(oColBack);	
	BPoint o1(1+fWidth,0),o2(o1.x,oRect.Height());
	StrokeLine(o1,o2);

	if(!b_Show){
		SetHighColor(oColSel);
		SetLowColor(oColBack);	
		
		BL_String s;
		s.AppendUTF8( sValue.StringUTF8(iNavIndex),1);
		//o1.x;
		o1.y = oRect.Height()-poFont->fDescent;
		
		DrawString(s.String(),o1);
		
	}
	printf("----end --------------------------------\n");
}

void					
BF_GUI_ViewEdit::SaveToMessage(BMessage *po_Message)
{
	po_Message->AddString(Name(),sValue.String());	
}

void
BF_GUI_ViewEdit::Enable(bool b_Enable)
{
	if(b_Enable && iDisableCount==1){
		SetFlags(B_WILL_DRAW|B_FRAME_EVENTS|B_NAVIGABLE);		
	}else
	if(!b_Enable && iDisableCount==0){
		SetFlags(B_WILL_DRAW|B_FRAME_EVENTS);
	}
	
	iDisableCount += !b_Enable?+1:-1;
	ASSERT(iDisableCount>=0);
	
	if(iDisableCount==0 && b_Enable) OnEnable(true);
	if(iDisableCount==1 && !b_Enable) OnEnable(false);	
}

bool					
BF_GUI_ViewEdit::Enabled()
{
	return iDisableCount==0 ;
}

void
BF_GUI_ViewEdit::OnEnable(bool b_Enable)
{
}

#include <UTF8.h>

bool
BF_GUI_ViewEdit::OnKeyDown(const char *bytes, int32 numBytes)
{
	printf(" chars : %i,%i  count:%i\n",(unsigned)bytes[0],bytes[1],numBytes);
	
	if(bSysKeysHandle && numBytes==1 && bytes[0]==B_RIGHT_ARROW){
		NavGo(iNavIndex+1);
	}else	
	if(bSysKeysHandle && numBytes==1 && bytes[0]==B_LEFT_ARROW){
		NavGo(iNavIndex-1);
	}else		
	if(bSysKeysHandle && numBytes==1 && bytes[0]==B_HOME){
		NavGo(0);
	}else		
	if(bSysKeysHandle && numBytes==1 && bytes[0]==B_END){
		NavGo(sValue.LengthUTF8());
	}else			
	if(numBytes==1 && bytes[0]==B_BACKSPACE){	
		ClearSelecting(true);
		if(iNavIndex<=0) return false;
		sValue.RemoveUTF8(iNavIndex-1,1);
		NavGo(iNavIndex-1,true);
	}else
	if(numBytes==1 && bytes[0]==B_DELETE){	
		if(sValue=="" || !bSysKeysHandle) return false;
		ClearSelecting(true);
		if(iNavIndex == sValue.Length()) return true;
		sValue.RemoveUTF8(iNavIndex,1);		
		Draw(Bounds());
	}else
	if(bParentCall && KeyDownExt(bytes,numBytes)) return true ;else		
	if((1==numBytes && bytes[0]>31) || (2==numBytes)){
		ClearSelecting(true);
		sValue.InsertUTF8(bytes,1,iNavIndex);
		NavGo(iNavIndex+1,true);
	}else
		return false;
	return true;
}

void					
BF_GUI_ViewEdit::KeyDown(const char *bytes, int32 numBytes)
{	
	if(!OnKeyDown(bytes,numBytes)) BView::KeyDown(bytes,numBytes);
}

void
BF_GUI_ViewEdit::ClearSelecting(bool b_Remove)
{
	if(!bSelected) return;
	if(b_Remove){
		sValue = "";
		iNavIndex = 0;
		iNavFirstChar = 0;
	}
	bSelected = false;
	Draw(Bounds());
	
}

void
BF_GUI_ViewEdit::NavGo(int32 i_NewCursor,bool b_FullRedraw)
{
	ClearSelecting();
	//
	if(i_NewCursor>sValue.LengthUTF8()) i_NewCursor  = sValue.LengthUTF8();
	if(i_NewCursor<0) i_NewCursor = 0;
	//
	if(i_NewCursor==iNavIndex) return;
	//
	if(b_FullRedraw){
		iNavIndex = i_NewCursor;
		Draw(Bounds());
	}else{
		DrawCursor(false);
		iNavIndex = i_NewCursor;
		DrawCursor(true);
	}
}
