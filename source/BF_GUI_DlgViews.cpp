#include <stdio.h>

#include "BF_Dict.h" 
#include "BF_Msg.h" 
#include "BF_GUI_Func.h" 
#include "BF_GUI_DlgViews.h"  
#include "BF_GUI_WinMain.h"  

#include <Clipboard.h>

//////////////////////////////////////////////
uint32
BF_GUI_GetTextLength(BString & s_Text,float f_Width,BFont & o_Font)
{
	if(o_Font.StringWidth(s_Text.String())<f_Width) return s_Text.Length();	 	 

	int iLength = s_Text.Length();	 
	while(iLength>0 && o_Font.StringWidth(s_Text.String(),iLength)>=f_Width) iLength--;
	return iLength;		 
}	 

//////////////////////////////////////////////


BF_GUI_DialogView::BF_GUI_DialogView(
	const BRect oRect,
	const char *pc_Name,
	uint32	i_FollowMode,
	uint32	i_Flags)
:BView(oRect,pc_Name,i_FollowMode,i_Flags)
{
	poFont = &poSysSetup->oFontToolView;
	SetFont(&poFont->oFont);
}

void
BF_GUI_DialogView::SetSetupFont(BF_GUI_Setup_Font *po_Font)
{
	ASSERT(po_Font);
	poFont = po_Font;
	SetFont(&poFont->oFont);
}

void					
BF_GUI_DialogView::MouseDown(BPoint point)
{
	if(!IsFocus() && (Flags() & B_NAVIGABLE)) MakeFocus();
}

void
BF_GUI_DialogView::SetupUpdated()
{
	Draw(Bounds());
}

void 					
BF_GUI_DialogView::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what){
	case BF_MSG_DIALOG_ENABLE:{
		bool bEnable;
		ASSERT(B_OK==po_Message->FindBool("bf_bEnable",&bEnable));
		if(bEnable)		SetFlags(Flags()|B_NAVIGABLE);
		else			SetFlags(Flags() & (!B_NAVIGABLE));
		break;}
	case BF_MSG_SETUP_UPDATED:
		SetupUpdated();
		break;
	default:
		BView::MessageReceived(po_Message);
	}
}

/* handle keys for set focus on next or prev view */
bool
BF_GUI_DialogView::KeyDownExt(const char *bytes, int32 numBytes)
{
	if(numBytes==1 && bytes[0]==B_ENTER){
		BMessage oMessage(BF_MSG_DIALOG_PRESSED_OK);
		oMessage.AddPointer("bf_DlgView_Focus",this);
		BMessenger oMessenger(Parent());		
		oMessenger.SendMessage(&oMessage);		
	}else
	if(numBytes==1 && bytes[0]==B_ESCAPE){
		BMessage oMessage(BF_MSG_DIALOG_PRESSED_CANCEL);
		oMessage.AddPointer("bf_DlgView_Focus",this);
		BMessenger oMessenger(Parent());		
		oMessenger.SendMessage(&oMessage);		
	}else	
	if(numBytes==1 && (bytes[0]==B_DOWN_ARROW || bytes[0]==B_TAB)){
		BView *poView = this;
		while(TRUE){
			poView = poView->NextSibling();
			if(!poView) poView =  Parent()->ChildAt(0);	
			if(!(poView->Flags() & B_NAVIGABLE)) continue;
			break;
		}
		poView->MakeFocus(true);
	}else
	if(numBytes==1 && bytes[0]==B_UP_ARROW){
		BView *poView=this;
		while(TRUE){
			poView = poView->PreviousSibling();
			if(!poView) poView =  Parent()->ChildAt(Parent()->CountChildren()-1);
			if(!(poView->Flags() & B_NAVIGABLE)) continue;
			break;
		}		
		poView->MakeFocus(true);
	}else	
		return false;
	return true;
}
///////////////////////////////////////////////////////////////////////

BF_GUI_VScrollBar::BF_GUI_VScrollBar(const BRect o_Rect,const char* pc_Name,uint32	i_FollowMode)
:BF_GUI_DialogView(o_Rect,pc_Name,i_FollowMode,B_WILL_DRAW)
{
	iLimit = 0;
	iVisibleLimit = 0;
	iCursor = 0;
	//
	SetViewColor(B_TRANSPARENT_COLOR);	
}

void					
BF_GUI_VScrollBar::SetLimits(int i_Limit,int i_VisibleLimit)
{
	iLimit = i_Limit;
	iVisibleLimit = i_VisibleLimit;
	iCursor = 0;
	Draw(Bounds());
}

void					
BF_GUI_VScrollBar::SetCursor(int i_Cursor)
{
	iCursor = i_Cursor;
	Draw(Bounds());
}

void					
BF_GUI_VScrollBar::Draw(BRect o_Rect)
{
	// create render 
	BRect 	oRect(Bounds());
	BBitmap oB(oRect,B_RGBA32,TRUE);
	BView	*pv;
	oB.AddChild(pv = new BView(oRect,"",B_FOLLOW_ALL,B_WILL_DRAW));	
	oB.Lock();	
	// init render 
	pv->SetFont(&poFont->oFont);
	pv->SetViewColor(B_TRANSPARENT_COLOR);

	// draw background
	pv->SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BACK));		
	pv->FillRect(Bounds());	
	
	//
	float fVDelta = iLimit!=0?(iVisibleLimit / iLimit):0;
	float fSDelta = iLimit!=0?((pv->Bounds().Height()-11) / iLimit):0;
	float fBarHeight = 	iVisibleLimit*fSDelta;
	fSDelta = iLimit!=0?((pv->Bounds().Height()-fBarHeight-11) / iLimit):0;		
	
	// draw center_line
	pv->SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_PROGRESS_FILLED));
	oRect = pv->Bounds();
	pv->StrokeLine(BPoint(4,oRect.top),BPoint(4,oRect.bottom));
	
	//  draw bar
	oRect=pv->Bounds();
	oRect.top = iCursor * fSDelta+7;
	oRect.bottom = oRect.top + fBarHeight;
	oRect.left+=2;
	oRect.right-=2;
		
	pv->SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_PROGRESS_FILLED));		
	pv->FillRect(oRect);	
		
	{// draw bottom figure 
		oRect = pv->Bounds();
		BPoint o1(2,oRect.bottom-5),o2(6,o1.y),o3;		
		o3.Set(4,oRect.bottom);
		pv->SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_PROGRESS_FILLED));
		pv->FillTriangle(o1,o2,o3);
	}
	
	{// draw top figure 	
		oRect = pv->Bounds();
		BPoint o1(2,oRect.top+5),o2(6,o1.y),o3;		
		o3.Set(4,oRect.top);
		pv->SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_PROGRESS_FILLED));
		pv->FillTriangle(o1,o2,o3);
	}
	
	// close render 
	pv->Sync();
	oB.Unlock();
	DrawBitmap(&oB,oRect);
}

///////////////////////////////////////////////////////////////////////
BF_GUI_ViewFrame::BF_GUI_ViewFrame(BRect & o_Rect,int32 i_FollowMode)
:BF_GUI_DialogView(o_Rect,"frame",i_FollowMode,B_WILL_DRAW)
{
}
void
BF_GUI_ViewFrame::Draw(BRect o_Rect)
{
	SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BACK));	
	FillRect(Bounds());
	
	SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_VIEWBORDER_UNFOCUSED));
	StrokeRect(Bounds());
}

////////////////////////////////////////////////////////////////////////
BF_GUI_DlgPanel::BF_GUI_DlgPanel(
	const BRect & o_Rect,
	const char *pc_Title,		
	const char *pc_Name,
	int i_FollowMode,
	int i_Styles,
	bool b_DoubleBorder)
:BView(o_Rect,pc_Name,i_FollowMode,i_Styles | B_WILL_DRAW)
{
	sTitle = pc_Title;	
	/*===*/
	SetFont(&poSysSetup->oFontToolView.oFont);
	SetViewColor(B_TRANSPARENT_COLOR);		
	//
	fShadowX = 10.0;
	fShadowY = 9.0;
	fBorderH = 10;	
	fBorderTop = 10;
	if(fBorderTop<poSysSetup->oFontToolView.fHeight) fBorderTop = poSysSetup->oFontToolView.fHeight;
	fBorderBottom = 10;		
	if(b_DoubleBorder){
		fBorderH += fBorderH;
		fBorderTop += fBorderTop;
		fBorderBottom += fBorderBottom;
	}
}

void
BF_GUI_DlgPanel::MouseDown(BPoint point)
{
	BView *poView;
	BRect oChildFrame;
	for(int iChild=CountChildren()-1;iChild>=0;iChild--){
		poView = ChildAt(iChild);
		
		oChildFrame = poView->Frame();		
		if(oChildFrame.Contains(point)){
			BPoint oPoint(point);
			oPoint.x -= oChildFrame.left;
			oPoint.y -= oChildFrame.top;			
			poView->MouseDown(oPoint);
			return;
		}		
	}
}
void
BF_GUI_DlgPanel::AddOkCancelMenu(BRect & o_Rect,bool b_RectInited)
{
	// prepare items for menu
	BL_List *ploMenu = new BL_List();
	ploMenu = new BL_List();
	ploMenu->AddItem(new BF_GUI_ViewMenu_Item(BF_DictAt(BF_DICT_OK),"ok"));
	ploMenu->AddItem(new BF_GUI_ViewMenu_Item(BF_DictAt(BF_DICT_CANCEL),"cancel"));
	
	AddMenu(o_Rect,ploMenu,b_RectInited);
}

void
BF_GUI_DlgPanel::AddMenu(BRect & o_Rect,BL_List *plo_Menu,bool b_RectInited)
{
	ASSERT(plo_Menu);	
	/* insert menu */		
	if(!b_RectInited){
		/*LocalBounds(o_Rect);
		o_Rect.top = o_Rect.bottom - poSysSetup->oFontToolView.fHeight;*/		
		o_Rect.top = o_Rect.bottom+5;
		o_Rect.bottom = o_Rect.top + poSysSetup->oFontToolView.fHeight;
	}	
		
	BF_GUI_DlgView_HMenu *poMenu = new BF_GUI_DlgView_HMenu(o_Rect,"menu",B_FOLLOW_BOTTOM,plo_Menu);	
	AddChild(poMenu);	
}

void
BF_GUI_DlgPanel::SetSizeBy(float f_X,float f_Y)
{
	ResizeTo( fBorderH + fBorderH +fShadowX + f_X,
		fBorderTop + fBorderBottom + fShadowY + f_Y );
}

void
BF_GUI_DlgPanel::SetHeightFromLastChild()
{
	BView *poChild = ChildAt( CountChildren()-1 );
	ASSERT(poChild);
	BRect oRect(Bounds());
	BRect oRectChild(poChild->Frame());
	ResizeTo( oRect.Width(),
		fBorderBottom + fShadowY + oRectChild.bottom );
}

void					
BF_GUI_DlgPanel::LocalBounds(BRect & o_Rect)
{
	o_Rect = Bounds();
	o_Rect.top += fBorderTop+1;
	o_Rect.left += fBorderH+1;
	o_Rect.right -= fShadowX+fBorderH+1;
	o_Rect.bottom -= fShadowY+fBorderBottom+1;
}

void 					
BF_GUI_DlgPanel::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what){
	case BF_MSG_SETUP_UPDATED:
		Draw(Bounds());
		break;
	default:
		BView::MessageReceived(po_Message);
	};
}


void 					
BF_GUI_DlgPanel::MoveToCenter(BView *po_View)
{
	ASSERT(po_View);
	/* move window to center of screen*/
	BRect oRect;
	{
		BPoint	oPoint;
		BRect	oRect1;
		oRect = po_View->Frame();
		oRect1 = Frame();
		oPoint.x = oRect.Width()/2 - oRect1.Width()/2;
		oPoint.y = oRect.Height()/2 - oRect1.Height()/2;
		MoveTo(oPoint);		
	}
}

void					
BF_GUI_DlgPanel::Draw(const BRect o_Rect)
{
	///
	BRect 	oRect;
	BPoint	oPoint;
	rgb_color oBackColor;	
	SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BACK));
	oRect = Bounds();
	oRect.right -= fShadowX;
	oRect.bottom -= fShadowY;	
	FillRect(oRect);
	/* draw border */
	SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_TEXT));	
	oRect = Bounds();
	oRect.left += ((int)fBorderH)/2-1;
	oRect.right -= fShadowX + ((int)fBorderH)/2-1;
	oRect.top += ((int)fBorderTop)/2-1;
	oRect.bottom -= fShadowY + ((int)fBorderBottom)/2-1;
	//
	StrokeRect(oRect);
	oRect.bottom-=2;
	oRect.left+=2;
	oRect.right-=2;
	oRect.top+=2;
	StrokeRect(oRect);			
	/* draw shadow */
	SetDrawingMode(B_OP_ALPHA);
	//SetBlendingMode(B_CONSTANT_ALPHA, B_ALPHA_OVERLAY);
	oRect = Bounds();
	oRect.left = oRect.right-fShadowX+1;
	oRect.top = fShadowY;	
	rgb_color oCol;
	RGBA_SET(oCol,100,100,100,170);
	SetHighColor(oCol);
	FillRect(oRect);	
	oRect = Bounds();
	oRect.top = oRect.bottom-fShadowY+1;	
	oRect.left+=fShadowX;
	oRect.right-=fShadowX;
	FillRect(oRect);		
	SetDrawingMode(B_OP_COPY);
	/* draw title */
	if(sTitle!=""){
		oRect=Bounds();
		oRect.bottom = fBorderTop;
		oRect.right -= fShadowX;
		oPoint.y = fBorderTop/2 - poSysSetup->oFontToolView.fHeight/2 + poSysSetup->oFontToolView.fAscent;
		float fTextWidth = poSysSetup->oFontToolView.oFont.StringWidth(sTitle.String());
		oPoint.x=oRect.Width()/2-fTextWidth/2;
		oRect.left = oPoint.x-5;
		oRect.right = oRect.left + fTextWidth + 10;		
		SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BACK));	
		FillRect(oRect);
		SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_TEXT));	
		SetLowColor(SYS_COLOR(BF_COLOR_DIALOG_BACK));	
		DrawString(sTitle.String(),oPoint);
	}
	/**/		
	BView::Draw(o_Rect);
	/**/	
	SetHighColor(oBackColor);
};

////////////////////////////////////////////////////////////////////////
BF_GUI_Dialog::BF_GUI_Dialog(
	const BRect & o_Rect,
	const char *pc_Title,
	const char *pc_Name,
	const BMessage &o_Message,
	uint32	i_WinResizeType,
	bool b_DoubleBorder
)
:BF_GUI_DlgPanel(o_Rect,pc_Title,pc_Name?pc_Name:"tool_view",
	i_WinResizeType!=BG_GUI_DIALOG_WINRESIZE_RESIZE_ALL?B_FOLLOW_NONE:B_FOLLOW_ALL,	
	B_WILL_DRAW|B_FULL_UPDATE_ON_RESIZE|B_FRAME_EVENTS,b_DoubleBorder)
{
	iWinResizeType = i_WinResizeType;
	oMessage = o_Message;
}

void					
BF_GUI_Dialog::MouseDown(BPoint point)
{
	BView *poView;
	BRect oChildFrame;
	for(int iChild=CountChildren()-1;iChild>=0;iChild--){
		poView = ChildAt(iChild);
		
		oChildFrame = poView->Frame();		
		if(oChildFrame.Contains(point)){
			BPoint oPoint(point);
			oPoint.x -= oChildFrame.left;
			oPoint.y -= oChildFrame.top;			
			poView->MouseDown(oPoint);
			return;
		}		
	}
}

void
BF_GUI_Dialog::FocusFirstView()
{
	/* set focus to first B_NAVIGABLE child */
	BView* poView;
	for(int i=0;i<CountChildren();i++){
		poView = ChildAt(i);
		if(poView->Flags() & B_NAVIGABLE){
			poView->MakeFocus();
			break;
		}
	}
}

void 					
BF_GUI_Dialog::AttachedToWindow(void)
{	
	FocusFirstView();
	BView::AttachedToWindow();
}

void BF_GUI_Dialod_CalcCenter(BRect & o_Rect)
{
	/* move window to center of screen*/
	BRect oRect;
			
	oRect = poWinView->Frame();	
	o_Rect.left = oRect.Width()/2 - o_Rect.Width()/2;
	o_Rect.right+=o_Rect.left;
	o_Rect.top = oRect.Height()/2 - o_Rect.Height()/2;
	o_Rect.bottom+=o_Rect.top;
	
}



void
BF_GUI_Dialog::EnableDialog(bool b_Enable)
{
	BView *poView;
	BMessage oMessage(BF_MSG_DIALOG_ENABLE);
	oMessage.AddBool("bf_bEnable",b_Enable);
	//
	for(int i=0;i<CountChildren();i++){
		poView = (BView*)ChildAt(i);
		poView->MessageReceived(&oMessage);
	}			
}
BMessage*				
BF_GUI_Dialog::Message()
{
	return &oMessage;
}

void 					
BF_GUI_Dialog::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what){
	/////////////////////////////////	
	case BF_MSG_PANEL_FOCUS_AND_ENABLE:{
		FocusFirstView();
		break;};	
	/////////////////////////////////	
	case BF_MSG_VIEW_MAINWIN_RESIZED:{
		switch(iWinResizeType){
		case BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER:
			MoveToCenter(Parent());
			break;
		case BG_GUI_DIALOG_WINRESIZE_MOVE_RCENTER:{
			if(!Parent()) break;
			BRect oRect = Parent()->Bounds();
			oRect.left = oRect.Width()/2+30;
			MoveTo(oRect.left,Frame().top);
			break;}
		}
		break;}
	/////////////////////////////////
	case BF_MSG_DIALOG_FOCUS:
		EnableDialog(true);	
		FocusFirstView();
		break;
	case BF_MSG_DIALOG_ENABLE:{	
		bool bEnable;
		ASSERT(B_OK==po_Message->FindBool("bf_bEnable",&bEnable));
		EnableDialog(bEnable);	
		break;}
	case BF_MSG_DIALOG_PRESSED_OK:		
	case BF_MSG_DIALOG_PRESSED_CANCEL:{
	
		if(!ReadyForClose()) return;
	
		if(po_Message->what==BF_MSG_DIALOG_PRESSED_OK)	Save(oMessage);
		BView *poView=NULL;
		ASSERT(B_OK==oMessage.FindPointer("bf_focus",(void**)&poView));	
		BMessenger oMessenger(poView);
		oMessage.AddBool(BF_GUI_DIALOG_MESSAGE_DATA_OK,
						BF_MSG_DIALOG_PRESSED_OK==po_Message->what);
		oMessenger.SendMessage(&oMessage);

		RemoveSelf();
		delete this;
		break;}
	case BF_MSG_REDRAWDIALOGS:{
		Draw(Bounds());
		BView *poView;
		for(int i=0;i<CountChildren();i++){
			poView = ChildAt(i);
			poView->Draw(poView->Bounds());
		}
		break;}
	case BF_MSG_DIALOG_CLOSE_NOW:
		RemoveSelf();
		delete this;
		break;
	default:
		BView::MessageReceived(po_Message);
	}	
}

void					
BF_GUI_Dialog::Save(BMessage& o_Message)
{
	BF_GUI_DialogView *po;
	for(int i=0;i<CountChildren();i++){
		po = (BF_GUI_DialogView*)ChildAt(i);
		po->SaveToMessage(&o_Message);
	}
}



///////////////////////

BF_GUI_ViewText::BF_GUI_ViewText(
	const BRect o_Rect,
	const char *pc_Name,
	const char *pc_Title,
	uint32	i_FollowMode,
	uint32	i_Flags,	
	bool b_AlignCenter
):BF_GUI_DialogView(o_Rect,pc_Name,i_FollowMode,i_Flags|B_WILL_DRAW)
{
	sTitle = pc_Title;
	SetViewColor(B_TRANSPARENT_COLOR);		
	bAlignCenter = b_AlignCenter;
}

void					
BF_GUI_ViewText::Draw(BRect o_Rect)
{
	BRect oRect;
	/* draw back */
	oRect = Bounds();
	SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BACK));
	FillRect(oRect);
	/* draw text */
	SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BUTTON_TEXT));
	SetLowColor(SYS_COLOR(BF_COLOR_DIALOG_BACK));
	BPoint oPoint;
	if(bAlignCenter){
		float fWidth = poFont->oFont.StringWidth(sTitle.String());
		oPoint.Set(oRect.Width()/2-fWidth/2,poFont->fAscent);
	}else
		oPoint.Set(0,poFont->fAscent);
	DrawString(sTitle.String(),oPoint);	
}
BF_GUI_ViewEdit*
BF_GUI_ViewEdit_Create(	const BRect &o_Rect,
						const char*pc_Comment,
						BView *po_Parent,
						const char*pc_Name,
						const char *pc_Value,
						uint32	i_FollowMode,
						uint32	i_Flags)
{
	ASSERT(po_Parent && pc_Comment && pc_Name && pc_Value);
	float fCommentWidth = poSysSetup->oFontToolView.oFont.StringWidth(pc_Comment);
	////////
	BRect oRect(o_Rect);
	BL_String s(pc_Name);
	s<<"_comment";
	oRect.right = oRect.left+fCommentWidth;
	BF_GUI_ViewText *poText = new BF_GUI_ViewText(oRect,s.String(),pc_Comment,B_FOLLOW_LEFT|B_FOLLOW_TOP,0);
	po_Parent->AddChild(poText);
	///////
	oRect = o_Rect;
	oRect.left+=fCommentWidth+5;
	BF_GUI_ViewEdit* poEdit = new BF_GUI_ViewEdit(oRect,pc_Name,pc_Value,i_FollowMode,i_Flags);
	po_Parent->AddChild(poEdit);
	return poEdit;
}						

/*========================================================*/
BF_GUI_ViewCheck::BF_GUI_ViewCheck(
	const BRect	o_Rect,
	const char	*pc_Name,
	const char	*pc_Title,
	bool		b_Value,
	uint32		i_FollowMode,
	uint32		i_Flags
):BF_GUI_DialogView(o_Rect,pc_Name,i_FollowMode,i_Flags|B_WILL_DRAW)
{
	sTitle = pc_Title;
	bValue = b_Value;
	SetViewColor(B_TRANSPARENT_COLOR);	
	bPulseCursorShow = true;
	idThreadPulse = 0;
}

BF_GUI_ViewCheck::~BF_GUI_ViewCheck()
{
	while(B_OK!=kill_thread(idThreadPulse));
}

int32 
BF_GUI_ViewCheck_PulseThread(void *data)
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
BF_GUI_ViewCheck::AttachedToWindow(void)
{
	BView::AttachedToWindow();
	//Window()->SetPulseRate(100);	
	
	idThreadPulse = spawn_thread(BF_GUI_ViewCheck_PulseThread,"BeFar:check_pulse",B_THREAD_SUSPENDED,(void*)this);
	ASSERT(idThreadPulse>0,"can`t start thread BeFar:check_pulse\n");	
	ASSERT(B_OK==resume_thread(idThreadPulse));		
	set_thread_priority(idThreadPulse,1);
}


void 					
BF_GUI_ViewCheck::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what){
	case BF_MSG_PULSE:
		Pulse();
		break;
	default:
		BView::MessageReceived(po_Message);
	}
}
void					
BF_GUI_ViewCheck::Pulse(void)
{
	if(!IsFocus()) return;
	bPulseCursorShow = !bPulseCursorShow;	
	DrawCursor(bPulseCursorShow);
}

void					
BF_GUI_ViewCheck::KeyDown(const char *bytes, int32 numBytes)
{
	if(numBytes==1 && bytes[0]==B_SPACE){
		bValue = !bValue;
		Draw(Bounds());
	}else
	if(KeyDownExt(bytes,numBytes)) return;else	
		BF_GUI_DialogView::KeyDown(bytes,numBytes);
}

void 					
BF_GUI_ViewCheck::MakeFocus(bool focused)
{	
	BView::MakeFocus(focused);
	if(focused){
		resume_thread(idThreadPulse);		
		DrawCursor(TRUE);
	}else{
		suspend_thread(idThreadPulse);
		DrawCursor(FALSE);
	}
}

void
BF_GUI_ViewCheck::MouseDown(BPoint point)
{
	bValue = !bValue;
	Draw(Bounds());	
	
	BF_GUI_DialogView::MouseDown(point);  
}

void					
BF_GUI_ViewCheck::SaveToMessage(BMessage *po_Message)
{
	po_Message->AddBool(Name(),bValue);	
}


void					
BF_GUI_ViewCheck::DrawCursor(bool b_Show)
{
	/* draw value */	
	if(b_Show)	SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BUTTON_TEXT));
	else		SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BACK));		
	BPoint 	oPoint(StringWidth("[")+1,poFont->fHeight);		
	BPoint  oPoint1(oPoint.x+StringWidth("X")-2,oPoint.y);
	StrokeLine(oPoint,oPoint1);
	//	
}

void					
BF_GUI_ViewCheck::Draw(BRect o_Rect)
{
	BRect oRect;
	/* draw back */
	oRect = Bounds();
	SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BACK));
	FillRect(oRect);
	/* draw */
	SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BUTTON_TEXT));
	SetLowColor(SYS_COLOR(BF_COLOR_DIALOG_BACK));	
	BPoint 	oPoint(0,poFont->fAscent);			
	/* draw value */
	DrawString("[",oPoint);			
	oPoint.x+=StringWidth("[");
	DrawString(bValue?"X":" ",oPoint);	
	oPoint.x+=StringWidth("X");
	DrawString("]",oPoint);	
	/* draw text */		
	oPoint.x+=StringWidth("]")+2;
	DrawString(sTitle.String(),oPoint);	
	/* draw  */	
}
/*========================================================*/
BF_GUI_ViewCheck_Style::BF_GUI_ViewCheck_Style(
	const BRect 		o_Rect,
	const char	*pc_Name,
	const char	*pc_Title,
	uint32		*pi_SrcStyles,
	uint32		i_SrcStyle,
	uint32		i_FollowMode,
	uint32		i_Flags)
:BF_GUI_ViewCheck(o_Rect,pc_Name,pc_Title,(*pi_SrcStyles) & i_SrcStyle,i_FollowMode,i_Flags)
{
	piSrcStyles = pi_SrcStyles;
	iSrcStyle = i_SrcStyle;
}

void
BF_GUI_ViewCheck_Style::SaveToMessage(BMessage *po_Message)
{
	BF_GUI_ViewCheck::SaveToMessage(po_Message);
	if(!bValue)
		*piSrcStyles = (*piSrcStyles) & (~iSrcStyle);
	else
		*piSrcStyles = (*piSrcStyles) | iSrcStyle;
}
/*========================================================*/


BF_GUI_ViewProgress::BF_GUI_ViewProgress(
	const BRect o_Rect,
	const char *pc_Name,
	const char *pc_Title,
	uint32	i_FollowMode,
	uint32	i_Flags,
	int64 *pi_Count,
	int64 *pi_Index
):BF_GUI_DialogView(o_Rect,pc_Name,i_FollowMode,i_Flags)
{
	sTitle = pc_Title;
	SetViewColor(B_TRANSPARENT_COLOR);		
	//
	iCount = 0;
	iIndex = 0;
	fDelta = 0;
	//
	piCount = pi_Count;
	piIndex = pi_Index;
	//
}

BF_GUI_ViewProgress::~BF_GUI_ViewProgress()
{
/*	if(piCount){
		(*piCount)=-2;
		(*piIndex)=-2;
		piCount=NULL;
		piIndex = NULL;
	}*/
}


void					
BF_GUI_ViewProgress::SetControls(int64 *pi_Count,int64 *pi_Index)
{
	piCount = pi_Count;
	piIndex = pi_Index;
	BWindow *poWind = Window();
	if(poWind){
		poWind->Lock();
		poWind->SetPulseRate(100);
		poWind->Unlock();		
	}
	//Draw(Bounds());
}

void 					
BF_GUI_ViewProgress::AttachedToWindow(void)
{
	BView::AttachedToWindow();
	//
	Window()->SetPulseRate(100);
	//
}

void
BF_GUI_ViewProgress::Pulse(void)
{
	/* signal from owner */
	if(piCount && ((*piCount)==-1)){
		(*piCount)=-2;
		(*piIndex)=-2;
		piCount=NULL;
		piIndex = NULL;
	}
	if(!piCount || !piIndex) return;
	
	bool bRedraw=false;
	if(iCount!=(*piCount)){
		SetLimit(*piCount);
		bRedraw=true;
	}
	if(iIndex!=(*piIndex)){
		iIndex=*piIndex;
		if(iIndex>iCount) iIndex = iCount;
		bRedraw=true;		
	}
	if(bRedraw) Draw(Bounds());
}

void					
BF_GUI_ViewProgress::Draw(BRect o_Rect)
{
	BRect oRect;
	/* draw back */
	oRect = Bounds();
	oRect.bottom = poFont->fHeight;
	SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BACK));
	FillRect(oRect);
	/* draw text */
	BL_String s,s1;
	s<<sTitle;
	s<<" ";
	//
	s1=iIndex;
	s1.SetDigits();
	s<<s1;
	s<<" of ";
	//
	s1=iCount;
	s1.SetDigits();
	s<<s1;
	float fWidth = poFont->oFont.StringWidth(s.String());
	BPoint oPoint(oRect.Width()/2-fWidth/2,
		poFont->fAscent);
	SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BUTTON_TEXT));
	SetLowColor(SYS_COLOR(BF_COLOR_DIALOG_BACK));
	DrawString(s.String(),oPoint);	
	///
	oRect = Bounds();
	oRect.top += poFont->fHeight+1;
	oRect.right = oRect.left + (float)iIndex * fDelta;
	SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BUTTON_TEXT));
	FillRect(oRect,B_MIXED_COLORS);
	//
	oRect.left = oRect.right+1;
	oRect.right = Bounds().right;
	SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_PROGRESS_EMPTY));
	FillRect(oRect);
	
	///
}

void
BF_GUI_ViewProgress::SetLimit(int64 i_Count)
{
	iCount = i_Count;
	iIndex = 0;
	fDelta = Bounds().Width() / (float)iCount ;
	Draw(Bounds());
}

void
BF_GUI_ViewProgress::Step(int64 i_Delta)
{
	iIndex+=i_Delta;
	if(iIndex>iCount) iIndex = iCount;
	Draw(Bounds());
}

void 	
BF_GUI_ViewProgress::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what){
	case BF_MSG_PROGRESS_SETLIMIT:{
		int64 iNewLimit;
		ASSERT(B_OK==po_Message->FindInt64("bf_iLimit",&iNewLimit),"BF_GUI_ViewProgress::MessageReceived_BF_MSG_PROGRESS_SETLIMIT");
		SetLimit(iNewLimit);
		break;}
	case BF_MSG_PROGRESS_STEP:{
		int64 iDelta=1;
		if(B_OK!= po_Message->FindInt64("bf_iDelta",&iDelta)) iDelta=1;
		Step(iDelta);
		break;}
	default:
		BView::MessageReceived(po_Message);
	}
}


class BF_GUI_Dialog_Alert:public BF_GUI_Dialog{
public:
								BF_GUI_Dialog_Alert(
										const BRect & o_Rect,
										const char *pc_Title,
										const char *pc_Text,
										const char *pc_Name,
										int32* pi_Result,
										BL_List *po_List,
										const char *pc_Text1=NULL);
virtual void 					MessageReceived(BMessage* po_Message);												
private:
		int32* 					piResult;
		BF_GUI_DlgView_HMenu 		*poMenu;
};

BF_GUI_Dialog_Alert::BF_GUI_Dialog_Alert(
	const BRect & o_Rect,
	const char *pc_Title,		
	const char *pc_Text,
	const char *pc_Name,
	int32* pi_Result,
	BL_List *po_List,
	const char *pc_Text1
):BF_GUI_Dialog(o_Rect,pc_Title,pc_Name,BMessage(),BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER)
{
	ASSERT(pi_Result);
	piResult = pi_Result;	
	/* make childs */
	BRect oRect;
	/* make text */	
	LocalBounds(oRect);
	oRect.bottom=oRect.top+poSysSetup->oFontToolView.fHeight;
	BF_GUI_ViewText *poText =new BF_GUI_ViewText(oRect,"text",pc_Text,BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER,0);
	AddChild(poText);
	if(pc_Text1){
		/* make text1 */	
		//LocalBounds(oRect);
		oRect.top=oRect.bottom+5;
		oRect.bottom=oRect.top+poSysSetup->oFontToolView.fHeight;
		BF_GUI_ViewText *poText1 =new BF_GUI_ViewText(oRect,"text1",pc_Text1,B_FOLLOW_TOP|B_FOLLOW_LEFT_RIGHT,0);
		AddChild(poText1);
	}
	/* make menu */
	if(po_List){
		if(po_List->CountItems()>0){
			LocalBounds(oRect);
			oRect.top=oRect.bottom - poSysSetup->oFontToolView.fHeight;	
			BMessage oMessage;
			poMenu = new BF_GUI_DlgView_HMenu(oRect,"menu",B_FOLLOW_BOTTOM|B_FOLLOW_LEFT_RIGHT,po_List);			
			AddChild(poMenu);	
		}else{
			DELETE(po_List);
		}
	}
}

void 					
BF_GUI_Dialog_Alert::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what){
	case BF_MSG_DIALOG_PRESSED_OK:
		*piResult = poMenu->iNavCursorIndex;
		RemoveSelf();
		delete this;
		break;
	case BF_MSG_DIALOG_PRESSED_CANCEL:{
		*piResult = -2;				
		RemoveSelf();
		delete this;		
		break;}
	default:
		BF_GUI_Dialog::MessageReceived(po_Message);
	}
}

int32
BF_Dialog_Alert(const char *pc_Title,const char *pc_Text,
	BL_List *plo_MenuItem,
	const char *pc_Text1)
{
	ASSERT(pc_Text && pc_Title);
	int32 iResult = -1;

	if(!plo_MenuItem){
		plo_MenuItem = new BL_List();		
		plo_MenuItem->AddItem(new BF_GUI_ViewMenu_Item(BF_DictAt(BF_DICT_OK),"ok"));
	}
	
	BRect oRect(poWin->Bounds());
	
	float fWidth = poSysSetup->oFontToolView.oFont.StringWidth(pc_Text);
	if(fWidth<100) fWidth=100;	
	
	float fMenuWidth = BF_GUI_ViewMenu_CalcTotalWidth(plo_MenuItem,&poSysSetup->oFontToolView,false);
	if(fWidth<fMenuWidth) fWidth = fMenuWidth;
	
	if((fWidth+120)>=oRect.Width()) fWidth=oRect.Width()-120;	
	
	oRect.Set(0,0,fWidth+100,100+(pc_Text1?(poSysSetup->oFontToolView.fHeight+2):0));
	{
		LOCK_WIN();
		BF_GUI_Dialod_CalcCenter(oRect);
	}
	BF_GUI_Dialog_Alert *poAlert = new BF_GUI_Dialog_Alert(oRect,pc_Title,
		pc_Text,"dialog_alert",&iResult,plo_MenuItem,pc_Text1);		
	
	BF_GUI_Func_AddChildToMainView ( poAlert);
	
	while(iResult==-1){
		snooze(1000);
	}	
	return iResult;
}


int32 BF_Dialog_Alert_Sep_Thread(void *pu_Data)
{
	ASSERT(pu_Data);
	BMessage *poMessage = (BMessage*)pu_Data;
	
	char 	*pcText,*pcText1,*pcTitle;
	BView	*poView;
	BL_List *ploMenuItem=NULL;
	
	ASSERT(B_OK==poMessage->FindString("bf_cTitle",(const char**)&pcTitle) && pcTitle);
	ASSERT(B_OK==poMessage->FindString("bf_cText",(const char**)&pcText) && pcText);
	if(B_OK!=poMessage->FindString("bf_cText1",(const char**)&pcText1)) pcText1 = NULL;
	ASSERT(B_OK==poMessage->FindPointer("bf_focus",(void**)&poView) && poView);
	ASSERT(B_OK==poMessage->FindPointer("bf_menu",(void**)&ploMenuItem));
	
	int32 iResult = BF_Dialog_Alert(pcTitle,pcText,ploMenuItem,pcText1);		
		
	BMessage 	oFinalMessage(poMessage->what);	
	BMessenger	oMessenger(poView);	
	DELETE(poMessage);
		
	oFinalMessage.AddInt32("menu",iResult);
	oMessenger.SendMessage(&oFinalMessage);		

	return 0;
}

void
BF_Dialog_Alert_Sep_Error(status_t uRes,BView *po_ViewOwner)
{
	BL_String s;
	BL_System_TranslError(uRes,s);	
	BF_Dialog_Alert_Sep("Error",s.String(),NULL,po_ViewOwner);
}

void
BF_Dialog_Alert_Sep(
	const char *pc_Title,
	const char *pc_Text,
	BL_List *plo_MenuItem,	
	BView *po_ViewOwner,
	const char *pc_Text1,
	int32	i_MessageWhat)
{
	ASSERT(po_ViewOwner);
	
	BF_GUI_Func_PanelsEnable(false);	
	/* prepare message */
	BMessage *poMessage = new BMessage(i_MessageWhat);
	poMessage->AddString("bf_cTitle",pc_Title);	
	poMessage->AddString("bf_cText",pc_Text);		
	poMessage->AddPointer("bf_focus",po_ViewOwner);
	poMessage->AddPointer("bf_menu",plo_MenuItem);
	if(pc_Text1) 	poMessage->AddString("bf_cText1",pc_Text1);	
	
	
	/* init thread */
	thread_id idThread = 
		spawn_thread(BF_Dialog_Alert_Sep_Thread,"BeFar:AlertWindow_Thread",
			B_THREAD_SUSPENDED,(void*)poMessage);	
	/* start thread */
	ASSERT(idThread>0,"can`t BeFar:AlertWindow_Thread\n");
	ASSERT(B_OK==resume_thread(idThread));						
}