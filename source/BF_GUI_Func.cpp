#include <Autolock.h>

#include "BF_Msg.h"
#include "BF_GUI_Setup.h"	
#include "BF_GUI_Func.h"
#include "BF_GUI_KeysMenu.h"
#include "BF_GUI_CmdLine.h"
#include "BF_GUI_WinMain.h"
#include "BF_GUI_OperRoster.h"

BF_GUI_Panel::BF_GUI_Panel(
	const BRect& 	o_Rect,		
	const char*		pc_FirstName,							
	bool			b_WinPos_OnLeft)
:BView(o_Rect,"_panel",B_FOLLOW_NONE,B_WILL_DRAW|B_NAVIGABLE|B_FRAME_EVENTS) 	
{
	bWinPos_OnLeft = b_WinPos_OnLeft;
	SetViewColor(B_TRANSPARENT_COLOR);
	iDisableCount = 0;
	bFixedFocus = false;
	poHeaderSIcon = NULL;
	/* set name */
	{
		ASSERT(pc_FirstName);
		BL_String s;
		s<<pc_FirstName;
		s<<"_panel";
		s<<"_";
		s<<(b_WinPos_OnLeft?"_left":"_right");
		SetName(s.String());
	}
	/**/
	poRender = this;
	poRenderBitmap = NULL;
	iRenderDeep = 0;
	bNeedUpdateAfterEnable = false;
}

BF_GUI_Panel::~BF_GUI_Panel()
{
	if(poHeaderSIcon) delete poHeaderSIcon;
}

void
BF_GUI_Panel::MouseUp(BPoint point)
{
	OnMouseUp(point);
}

void					
BF_GUI_Panel::MouseDown(BPoint point)
{
	OnMouseDown(point);
}

bool 					
BF_GUI_Panel::OnMouseDown(BPoint & o_Point)
{
	/* check for disable */
	if(!Enabled()){
		BRect 	oFrame = Frame();
		BPoint	oPoint;
		oPoint.x = oFrame.left + o_Point.x;
		oPoint.y = oFrame.top + o_Point.y;
		
		BMessenger 	oMessenger(poWinView);
		BMessage 	oMessage(BF_MSG_MAINVIEW_MOUSE_DOWN);
		oMessage.AddPoint("bf_point",oPoint);
		oMessage.AddPointer("bf_view",(void*)this);		
		oMessenger.SendMessage(&oMessage);
		return true;	
	}
	/* check for pressing in path_line */
	if(o_Point.y<=HeaderHeight()){
		NavPressInHeader(o_Point);
		return true;
	}	
	return false;
}
bool
BF_GUI_Panel::OnMouseUp(BPoint & o_Point)
{
	return false;
}


void		
BF_GUI_Panel::KeyDown(const char *bytes, int32 numBytes)
{
	if(OnBeforeKeyDown(bytes,numBytes)) return;
	if(poCmdLine && poCmdLine->OnKeyDown(bytes,numBytes)) return;
	if(poSysKeysMenu && poSysKeysMenu->OnKeyDown(bytes,numBytes)) return;	
	OnKeyDown(bytes,numBytes);
}

bool
BF_GUI_Panel::OnBeforeKeyDown(const char *bytes, int32 numBytes)
{
	return false;
}

void					
BF_GUI_Panel::PrepareCmdLine()
{
	if(!poCmdLine) return;
	poCmdLine->SetValues("","");
}

bool
BF_GUI_Panel::Close()
{
	if(!OnClose()) return false;
	
	poWinView->RemovePanel(this);
	
	return true;
}

bool
BF_GUI_Panel::OnKeyDown(const char *bytes, int32 numBytes)
{
	uint32 	iKeysModifiers = modifiers();

	if(numBytes==1 && bytes[0]==B_ESCAPE){
		Close();
		return true;
	}

	if(bytes[0] == B_FUNCTION_KEY){
		int32 iKey;
		BMessage *poMsg = Window()->CurrentMessage();
		if(!poMsg) return false;
		poMsg->FindInt32("key", &iKey);
		if(iKeysModifiers & B_RIGHT_OPTION_KEY){		
		}else{
			/*switch(iKey){
			case B_F9_KEY:{
				Run_TopMenu();
				break;}
			case B_F10_KEY:{	
				BMessage	oMessage(BF_MSG_MAINVIEW_ASK_QUIT);
				BMessenger 	oMessenger(poWinView);
				oMessenger.SendMessage(&oMessage);
				return true;
				break;}
			}*/
		}
	}		
	BView::KeyDown(bytes,numBytes);		
	return false;
}

void
BF_GUI_Panel::Draw_OpenTransaction()
{
	iRenderDeep++;
	/* open transaction */
	if(	iRenderDeep == 1){
		BRect oRect(Bounds());
		poRenderBitmap = new BBitmap(oRect,B_RGB16,true);		
		poRenderBitmap->AddChild( poRender = new BView(oRect,"",B_FOLLOW_ALL,B_WILL_DRAW) );
		poRenderBitmap->Lock();
		poRender->SetViewColor(B_TRANSPARENT_COLOR);
		poRender->SetFont(&poSysSetup->oFontNode.oFont,B_FONT_ALL);
	}
}

void
BF_GUI_Panel::Draw_CloseTransaction(BRect * po_RectDest)
{	
	iRenderDeep--;
	/* close transaction */
	if(iRenderDeep==0){	
		LOCK_WIN();
		poRender->Sync();		
		// poRenderBitmap->Unlock();
		SetDrawingMode(B_OP_COPY);
		if(po_RectDest)
			DrawBitmap(poRenderBitmap,*po_RectDest,*po_RectDest);
		else
			DrawBitmap(poRenderBitmap,Bounds());
		if(poRender->RemoveSelf()) 	DELETE(poRender);
		poRenderBitmap->Unlock();//
		DELETE(poRenderBitmap);		
		poRender = this;					
	}
}

void 
BF_GUI_Panel::DrawBackground(const BRect & o_Rect) const
{
	poRender->SetHighColor(SYS_COLOR(BF_COLOR_BACK));		
	poRender->FillRect(o_Rect);
}

void
BF_GUI_Panel::DrawPanel(BRect & o_Rect)
{
	BRect 	oRect;
	BPoint	o1,o2;	
	float	fHeight = poSysSetup->oFontNode.fHeight;	
	
	oRect = poRender->Bounds();
	
	// draw background 
	DrawBackground(oRect);	
	// draw left_frame 
	poRender->SetHighColor(SYS_COLOR(BF_COLOR_FRAME));	
	oRect = poRender->Bounds();
	o1.Set(1,HeaderHeight()/2-1);
	o2.Set(o1.x,oRect.bottom-fHeight/2+1);
	poRender->StrokeLine(o1,o2);
	o1.x+=2;o2.x=o1.x;
	o1.y+=2;o2.y-=2;
	poRender->StrokeLine(o1,o2);
	/* draw right_frame */
	poRender->SetHighColor(SYS_COLOR(BF_COLOR_FRAME));	
	o1.Set(oRect.right-1,HeaderHeight()/2-1);
	o2.Set(o1.x,oRect.bottom-fHeight/2+1);
	poRender->StrokeLine(o1,o2);
	o1.x-=2;o2.x=o1.x;
	o1.y+=2;o2.y-=2;
	poRender->StrokeLine(o1,o2);
	
	/* draw path */	
	DrawHeader(IsFocus());		
}

const BL_String
BF_GUI_Panel::Path() const
{
	BL_String sResult("//");
	return sResult;
}

void					
BF_GUI_Panel::Draw(BRect o_Rect)
{
	Draw_OpenTransaction();	
	DrawPanel(o_Rect);
	Draw_CloseTransaction();
}

void
BF_GUI_Panel::SetHeader(const char *pc_Header,BBitmap * po_SIcon)
{
	sHeader=pc_Header?pc_Header:"";
	if(po_SIcon){
		if(poHeaderSIcon) DELETE(poHeaderSIcon);
		poHeaderSIcon = new BBitmap(po_SIcon);
	}
	
	if(Parent()) DrawHeader(IsFocus());
}

float
BF_GUI_Panel::HeaderHeight()
{
	float 	fNodeHeight = poSysSetup->oFontNode.fHeight;
	if(poSysSetup->MainStyle() & BF_SETUP_MAIN_FP_HEADERICON)
	if(fNodeHeight<18) fNodeHeight = 18;
	return fNodeHeight;
}

void
BF_GUI_Panel::DrawStringCheck(const char *pc_Text,float f_Width,BPoint & o_Point,BFont *po_Font)
{
	ASSERT(pc_Text);
	
	if(!po_Font) po_Font = &poSysSetup->oFontNode.oFont;	
	///
	BRect oRect;
	oRect.top = o_Point.y-1000;
	oRect.bottom = o_Point.y+1000;
	oRect.left = o_Point.x;
	oRect.right = o_Point.x + f_Width;	

	BRegion oReg;
	oReg.Set(oRect);
	poRender->ConstrainClippingRegion(&oReg);
	poRender->DrawString(pc_Text,o_Point);
	poRender->ConstrainClippingRegion(NULL);
	///
			
	//poRender->DrawString(s_Text.String(),BF_GUI_GetTextLength(s_Text,f_Width,*po_Font),o_Point);
}

void
BF_GUI_Panel::DrawHeader(bool b_IsFocus)
{
	BString s;
	BRect 	oRect(poRender->Bounds());
	float	fPathWidth;	
	float 	fNodeHeight = HeaderHeight();
	float	fIconWidth = ((poSysSetup->MainStyle() & BF_SETUP_MAIN_FP_HEADERICON) && poHeaderSIcon)?18.0:0.0;
	/* prepare path */	
	{
		float fMax;
		s = sHeader;
		fMax = oRect.Width()-40-fIconWidth;
		while(TRUE){
			fPathWidth = poSysSetup->oFontNode.oFont.StringWidth(s.String());
			if(fPathWidth<=fMax) break;
			s.Remove(0,1);
		}
	}	
	
	float fWidth = poSysSetup->oFontNode.oFont.StringWidth(s.String()) + fIconWidth;
	float fLeft = (oRect.Width() - fWidth)/2;	

	/* fill blue backround */	
	oRect = poRender->Bounds();
	oRect.left+=5;
	oRect.right-=5;
	oRect.bottom = fNodeHeight;
	poRender->SetHighColor(SYS_COLOR(BF_COLOR_PATH_BACK));	
	poRender->FillRect(oRect);
		
	/* draw path */	
	oRect = poRender->Bounds();
	oRect.left=fLeft-2;
	oRect.right=fLeft+fWidth+2;
	oRect.bottom = fNodeHeight;
	if(b_IsFocus)
		poRender->SetHighColor(SYS_COLOR(BF_COLOR_PATH_FOCUSBACK));	
	else
		poRender->SetHighColor(SYS_COLOR(BF_COLOR_PATH_BACK));	
	poRender->FillRect(oRect);
	/* title */    	 
	oRect = Bounds(); 
	/* draw lines */
	BPoint o1,o2;
	/* left line */
	o1.Set(1,fNodeHeight/2-1);
	o2.Set(fLeft-3,o1.y);
	poRender->SetHighColor(SYS_COLOR(BF_COLOR_FRAME));
	poRender->StrokeLine(o1,o2);	
	o1.x+=2;
	o1.y+=2;o2.y=o1.y;
	poRender->StrokeLine(o1,o2);	
	/* right line */
	o1.Set(fLeft+fWidth+3,fNodeHeight/2-1);
	o2.Set(oRect.right-1,o1.y);
	poRender->SetHighColor(SYS_COLOR(BF_COLOR_FRAME));
	poRender->StrokeLine(o1,o2);	
	o2.x-=2;
	o1.y+=2;o2.y=o1.y;
	poRender->StrokeLine(o1,o2);
	if(poSysSetup->MainStyle() & BF_SETUP_MAIN_FP_HEADERICON){
		if(poHeaderSIcon){
			BPoint o(fLeft,0);
			poRender->SetDrawingMode(B_OP_ALPHA);		
			poRender->DrawBitmap(poHeaderSIcon,o);
			poRender->SetDrawingMode(B_OP_COPY);			
		}
	}			
	/* print header */
	o1.Set(fLeft+fIconWidth,poSysSetup->oFontNode.fAscent);	
	if(b_IsFocus){
		poRender->SetHighColor(SYS_COLOR(BF_COLOR_PATH_FOCUSTEXT));
		poRender->SetLowColor(SYS_COLOR(BF_COLOR_PATH_FOCUSBACK));
	}else{
		poRender->SetHighColor(SYS_COLOR(BF_COLOR_PATH_TEXT));
		poRender->SetLowColor(SYS_COLOR(BF_COLOR_PATH_BACK));
	}
	poRender->SetFont(&poSysSetup->oFontNode.oFont);
	poRender->DrawString(s.String(),o1);
}


float
BF_GUI_Panel::BottomHeight()
{
	return poSysSetup->oFontNode.fHeight;
}


BRect 
BF_GUI_Panel::BottomRect()
{
	BRect oRect;
	oRect = poRender->Bounds();
	oRect.left+=4;
	oRect.right-=4;
	oRect.top = oRect.bottom-BottomHeight();
	
	return oRect;
}

BRect
BF_GUI_Panel::ClientRect()
{
	BRect oRect(Bounds());
	oRect.left = 6;
	oRect.right -= 6;	
	oRect.top += HeaderHeight();
	oRect.bottom -= BottomHeight();
	
	return oRect;
}

void 
BF_GUI_Panel::DrawBottom(const char *pc_Text,bool b_DrawBack)
{
	BRect oRect;
	float fNodeHeight = poSysSetup->oFontNode.fHeight;
	/* erase background  */
	if(b_DrawBack){
		oRect = BottomRect();
		poRender->SetHighColor(SYS_COLOR(BF_COLOR_BACK));
		poRender->FillRect(oRect);
	}
	/* print */	
	if(pc_Text && pc_Text[0]!='\0'){
		BL_String s(pc_Text);	
	    float fWidth = poSysSetup->oFontNode.oFont.StringWidth(s.String());
		oRect = Bounds();
		float fLeft = (oRect.Width() - fWidth)/2;
		/* draw lines */
		BPoint o1,o2;
		/* left line */
		o1.Set(1,oRect.bottom-fNodeHeight/2+1);
		o2.Set(fLeft-(pc_Text?3.0:0.0),o1.y);
		poRender->SetHighColor(SYS_COLOR(BF_COLOR_FRAME));
		poRender->StrokeLine(o1,o2);	
		o1.x+=2;
		o1.y-=2;o2.y=o1.y;
		poRender->StrokeLine(o1,o2);	
		/* right line */
		o1.Set(fLeft+fWidth+3.0,oRect.bottom-fNodeHeight/2+1);
		o2.Set(oRect.right-1,o1.y);
		poRender->SetHighColor(SYS_COLOR(BF_COLOR_FRAME));
		poRender->StrokeLine(o1,o2);	
		o2.x-=2;
		o1.y-=2;o2.y=o1.y;
		poRender->StrokeLine(o1,o2);		
		/* draw info */
		o1.Set(fLeft,oRect.bottom-poSysSetup->oFontNode.fDescent);
		poRender->SetHighColor(SYS_COLOR(BF_COLOR_PANEL_INFO_TEXT));
		poRender->SetLowColor(SYS_COLOR(BF_COLOR_BACK));
		poRender->DrawString(s.String(),o1);
	}else{
		/* draw lines */
		BPoint o1,o2;
		oRect = poRender->Bounds();
		/**/
		o1.Set(1,oRect.bottom-fNodeHeight/2+1);
		o2.Set(oRect.right-1,o1.y);
		poRender->SetHighColor(SYS_COLOR(BF_COLOR_FRAME));
		poRender->StrokeLine(o1,o2);	
		o1.x+=2;
		o2.x-=2;
		o1.y-=2;o2.y=o1.y;
		poRender->StrokeLine(o1,o2);	
	}
}

void					
BF_GUI_Panel::ActionCopyFrom_Run(BL_String & s_Path,BF_NodeCollection & lo_Node,bool b_Move)
{
}

void
BF_GUI_Panel::PrepareTopMenuItem(BF_GUI_TopMenu_HItem *po_HItem)
{
	
}

void
BF_GUI_Panel::PrepareKeysMenu()
{
	poSysKeysMenu->Clear();		
	uint32 	iKeysModifiers = modifiers();
	
	if(iKeysModifiers & B_OPTION_KEY){
		poSysKeysMenu->SetText(9,"SaveSetup");
		poSysKeysMenu->SetMessage(9,new BMessage(BF_MSG_MAINVIEW_MAINSETUP_SAVE),poWinView);
	}else{					
		poSysKeysMenu->SetText(1,"LeftVol");
		poSysKeysMenu->SetMessage(1,new BMessage(BF_MSG_MAINVIEW_SELVOL_LEFT),poWinView);
		
		poSysKeysMenu->SetText(2,"RightVol");
		poSysKeysMenu->SetMessage(2,new BMessage(BF_MSG_MAINVIEW_SELVOL_RIGHT),poWinView);
	
		poSysKeysMenu->SetText(9,"Menu");
		poSysKeysMenu->SetMessage(9,new BMessage(BF_MSG_PANEL_TOPMENU),this);
		
		poSysKeysMenu->SetText(10,"Exit");
		poSysKeysMenu->SetMessage(10,new BMessage(BF_MSG_MAINVIEW_ASK_QUIT),poWinView);
	}	
}

void
BF_GUI_Panel::SetupUpdated()
{
	SetFont(&poSysSetup->oFontNode.oFont);
	Draw(Bounds());
}

void
BF_GUI_Panel::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what) {	
	///// set path /////
	case BF_MSG_TO_PANEL_SET_PATH:{
		BF_GUI_Panel *poPrevPanel = poWinView->PanelByIndex(bWinPos_OnLeft,-1);
		if(!Close()) break;		
		if(!poPrevPanel) break;
		BMessenger oMessenger(poPrevPanel);
		oMessenger.SendMessage(po_Message);
		break;}
	// cursor	
	case BF_MSG_TO_PANEL_FRIEND_NEWCURSOR:{
		const char *pcNodePath;
		if(B_OK!=po_Message->FindString("bf_pcNodePath",&pcNodePath) || !pcNodePath) return;
		const char *pcNodeName;
		if(B_OK!=po_Message->FindString("bf_pcNodeName",&pcNodeName) || !pcNodeName) return;
		int32 iNodeType = po_Message->FindInt32("bf_iNodeType");
		
		Action_Friend_NewCursor(pcNodePath,pcNodeName,iNodeType);
		
		break;}
	case BF_MSG_MAKEFOCUS:
		MakeFocus();
		break;
	case BF_MSG_SETUP_UPDATED:
		SetupUpdated();
		break;
	/* buffer messages */
	case B_COPY:{
		Action_CopyToBuffer();		
		break;}		
	case B_PASTE:{
		Action_PasteFromBuffer();		
		break;}				
	/* keyboard modifiers changed */
	case 1599425348:
	case 1599425365:
		if(poSysKeysMenu)	PrepareKeysMenu();
		break;
	/* main window  receive focus */
	case BF_MSG_TOTAL_WIN_RECEIVE_FOCUS:
		if(poSysKeysMenu)	PrepareKeysMenu();
		break;		
	case BF_MSG_PANEL_FOCUS_AND_ENABLE:
		BF_GUI_Func_PanelsEnable(true);
		MakeFocus();
		break;
	case BF_MSG_MAINVIEW_MAINSETUP_CLOSE:
		BF_GUI_Func_PanelsEnable(true);
		MakeFocus();
		break;
	case BF_MSG_PANEL_TOPMENU:{
		Run_TopMenu();
		break;}
	case BF_MSG_PANEL_TOPMENU_INVOKED:{
		MakeFocus();
		break;}
	/*=======================*/	
	case BF_MSG_VIEW_MAINWIN_RESIZED:{
		if(!Parent()) break;
				
		BRect oParRect = Parent()->Bounds();
		float fBotHeight = poSysSetup->oFontNode.fHeight + poSysSetup->oFontCMDLine.fHeight;
		fBotHeight += poOperRoster->fHeightInWinMain();

		if(bWinPos_OnLeft){
			ResizeTo(oParRect.Width()/2-1,oParRect.Height()- fBotHeight );
		}else{
			MoveTo(oParRect.Width()/2,0);
			ResizeTo(oParRect.Width()/2,oParRect.Height()- fBotHeight);
		}
		
		break;}
	default:{
		BView::MessageReceived(po_Message);
		break;}  			
	}
};

void
BF_GUI_Panel::Run_TopMenu()
{
	BF_GUI_TopMenu_Run();
}

void					
BF_GUI_Panel::FrameResized(float width, float height)
{
	Invalidate();
	BView::FrameResized(width,height);
}
void					
BF_GUI_Panel::NavPressInHeader(const BPoint & o_Point)
{
	BMessage oMessage(bWinPos_OnLeft?BF_MSG_MAINVIEW_SELVOL_LEFT:BF_MSG_MAINVIEW_SELVOL_RIGHT);
	BMessenger oMessenger(poWinView);
	oMessenger.SendMessage(&oMessage);
}

void
BF_GUI_Panel::Enable(bool b_Enable)
{
	if(b_Enable && iDisableCount==1){
		bFixedFocus = false;
		SetFlags(B_WILL_DRAW|B_FRAME_EVENTS|B_NAVIGABLE);		
	}else
	if(!b_Enable && iDisableCount==0){
		//if(IsFocus())	bFixedFocus = true;		
		SetFlags(B_WILL_DRAW|B_FRAME_EVENTS);			
	}
	
	iDisableCount += !b_Enable?+1:-1;
	ASSERT(iDisableCount>=0);
	
	if(iDisableCount==0 && b_Enable) OnEnable(true);
	if(iDisableCount==1 && !b_Enable) OnEnable(false);	
}

bool					
BF_GUI_Panel::Enabled()
{
	return iDisableCount==0 ;
}

void
BF_GUI_Panel::OnEnable(bool b_Enable)
{
	if(b_Enable && bNeedUpdateAfterEnable){
		Draw(Bounds());
		bNeedUpdateAfterEnable = false;
	}
}

void
BF_GUI_Panel::MakeFocus(bool focused)
{
	BView::MakeFocus(focused);
	OnFocus(focused);
}
void
BF_GUI_Panel::OnFocus(bool b_Focused)
{
	/* prepare keys_menu */
	if(b_Focused){
		if(poSysKeysMenu) 	PrepareKeysMenu();
		if(poCmdLine)		PrepareCmdLine();
	}else{
		if(poSysKeysMenu) 	poSysKeysMenu->Clear();
	 }		
	
	/* redraw path */
	DrawHeader(bFixedFocus || b_Focused);
}

bool					
BF_GUI_Panel::OnClose(){
	return true;
}

void
BF_GUI_Panel::SayFriend_About_CursorChanged(
	const char *pc_NodePath,
	const char *pc_NodeName,
	int i_NodeType)
{

	BF_GUI_Panel *po = poWinView->PanelOnTop(!bWinPos_OnLeft);
	if(!po) return;
	
	BMessenger oMessenger(po);
	BMessage   oMessage(BF_MSG_TO_PANEL_FRIEND_NEWCURSOR);
	if(pc_NodePath) oMessage.AddString("bf_pcNodePath",pc_NodePath);
	if(pc_NodeName) oMessage.AddString("bf_pcNodeName",pc_NodeName);
	oMessage.AddInt32("bf_iNodeType",i_NodeType);
	oMessenger.SendMessage(&oMessage);
}

void
BF_GUI_Panel::Nodes_GetAll(BF_NodeList& lo_ResultNode,bool b_ReqIcons)
{
	lo_ResultNode.DeleteItems();	
}

void
BF_GUI_Panel::Nodes_SetSelectingByName( BF_NodeList* plo_ReqNode,uint i_SelAction,bool b_ReqUpdate)
{
}

void					
BF_GUI_Panel::StoreVars(BF_GUI_Setup_Vars_Panel & o_Setup) const
{	
}
/////////////////////////////////////////////////
void
BF_GUI_Func_PanelsEnable(bool b_Enable,bool b_OperRosterAlso)
{
	BMessenger 	oMessenger(poWinView);
	BMessage	oMessage(BF_MSG_ENABLEPANELS);
	oMessage.AddBool("bf_bEnable",b_Enable);
	oMessage.AddBool("bf_bOperRoster",b_OperRosterAlso);
	oMessenger.SendMessage(&oMessage);
}

void
BF_GUI_Func_PanelsShow(bool b_Show)
{
	BMessenger 	oMessenger(poWinView);
	BMessage	oMessage(BF_MSG_SHOWPANELS);
	oMessage.AddBool("bf_bShow",b_Show);
	oMessenger.SendMessage(&oMessage);
}

void
BF_GUI_Func_RedrawDialogs()
{		
	BMessage	oMessage(BF_MSG_REDRAWDIALOGS);
	
	if(poWinView->CountChildren()<5) return;
	
	for(int i=0;i<poWinView->CountChildren();i++){
		BMessenger 	oMessenger(poWinView->ChildAt(i));
		oMessenger.SendMessage(&oMessage);
	}		
}

/*=========================================================*/
bool
BF_GUI_Func_EnableDialog(const char *pc_Name,bool b_Enable)
{
	ASSERT(pc_Name);
	BView *poView = poWin->FindView(pc_Name);
	if(!poView) return false;
	  
	BMessage	oMessage(BF_MSG_DIALOG_ENABLE);
	BMessenger 	oMessenger(poView);
	oMessage.AddBool("bf_bEnable",b_Enable);
	oMessenger.SendMessage(&oMessage);
	
	return true;
}
/*=========================================================*/
void
BF_GUI_Func_CloseDialog(BView *po_Dialog)
{
	BMessage	oMessage(BF_MSG_DIALOG_CLOSE_NOW);
	BMessenger 	oMessenger(po_Dialog);
	oMessenger.SendMessage(&oMessage);		
}
/*=========================================================*/
bool
BF_GUI_Func_AddChildToMainView(BView *po_View)
{	
	ASSERT(po_View);
	LOCK_WIN();	
	
	poWinView->AddChild(po_View);	
	return true;
}
////////////////////////////////////////////////////////
void
BF_GUI_MakeFocusTo(BView *po_View)
{
	ASSERT(po_View);
	BMessage	oMessage(BF_MSG_MAKEFOCUS);
	BMessenger 	oMessenger(po_View);
	oMessenger.SendMessage(&oMessage);
}
//////////////////////////////////////////////////////////