#include <stdio.h>

#include "BF_Msg.h" 
#include "BF_GUI_Func.h" 
#include "BF_GUI_DlgViews.h"  
#include "BF_GUI_WinMain.h"  


/*================================================================*/
BF_GUI_ViewMenu_Item::BF_GUI_ViewMenu_Item(
	const char *pc_Title,
	const char *pc_Code,
	BBitmap *po_SIcon)
{
	sCode = pc_Code;	
	sTitle = pc_Title;	
	fWidth  = 0;	
	poSIcon = po_SIcon;
	bSelected = false;
}

BF_GUI_ViewMenu_Item::BF_GUI_ViewMenu_Item(BF_GUI_ViewMenu_Item *po_Src)
{
	Set(po_Src);	
}

void					
BF_GUI_ViewMenu_Item::Set(BF_GUI_ViewMenu_Item *po_Src)
{
	ASSERT(po_Src);
	sCode = po_Src->sCode;
	sTitle = po_Src->sTitle;
	fWidth = po_Src->fWidth;
	if(po_Src->poSIcon) poSIcon = new BBitmap(poSIcon); else poSIcon = NULL;
	bSelected = po_Src->bSelected;
}

BF_GUI_ViewMenu_Item::~BF_GUI_ViewMenu_Item()
{
	if(poSIcon) DELETE(poSIcon);
}

void
BF_GUI_ViewMenu_Item::Draw(BF_GUI_DlgView_Menu *po_Parent,BView *po_Render,float f_PosY)
{
}

void					
BF_GUI_ViewMenu_Item::MakeFocus(BF_GUI_DlgView_Menu *po_Parent)
{	
}

float
BF_GUI_ViewMenu_Item::CalcWidth(BF_GUI_Setup_Font * po_Font)
{
	ASSERT(po_Font);
	fWidth =  po_Font->oFont.StringWidth(sTitle.String());
	fWidth += 10; // for focus_pointer
	return fWidth;
}

float
BF_GUI_ViewMenu_CalcMaxWidth(BL_List *plo_Menu,BF_GUI_Setup_Font * po_Font,bool b_ShowIcons)
{
	ASSERT(po_Font && plo_Menu);
	/* calc max_item  width */
	float fMaxWidth=0;
	{
		BF_GUI_ViewMenu_Item *po;
		for(int i=0;i<plo_Menu->CountItems();i++){
			po = (BF_GUI_ViewMenu_Item*)plo_Menu->ItemAt(i);			
			ASSERT(po);
			po->CalcWidth(po_Font);
			if(po->fWidth>fMaxWidth) fMaxWidth = po->fWidth;
		}
		if(b_ShowIcons) fMaxWidth+=19;
	}		
	return fMaxWidth;
}

float
BF_GUI_ViewMenu_CalcTotalWidth(BL_List *plo_Menu,BF_GUI_Setup_Font * po_Font,bool b_ShowIcons)
{
	ASSERT(po_Font && plo_Menu);
	/* calc max_item  width */
	float fTotalWidth=0;
	{
		BF_GUI_ViewMenu_Item *po;
		for(int i=0;i<plo_Menu->CountItems();i++){
			po = (BF_GUI_ViewMenu_Item*)plo_Menu->ItemAt(i);			
			ASSERT(po);
			po->CalcWidth(po_Font);
			fTotalWidth += po->fWidth+10;			
		}
	}	
	return fTotalWidth;
}
/*================================================================*/
// plo_Menu will deleted with all items after close
BF_GUI_DlgView_Menu::BF_GUI_DlgView_Menu(
		const BRect 	&o_Rect,
		const char	*pc_Name,
		uint32	i_FollowType,
		BL_List *plo_Menu
		)
:BF_GUI_DialogView(o_Rect,pc_Name,i_FollowType,B_WILL_DRAW|B_NAVIGABLE)
{
	ASSERT(plo_Menu);
	ploMenu = plo_Menu;
	iNavCursorIndex = -1;	
	bDestroyMenu = true;
	bHideCursorOnDefocus = true;
	//
	SetViewColor(B_TRANSPARENT_COLOR);
	//	
}

BF_GUI_ViewMenu_Item*	
BF_GUI_DlgView_Menu::ItemByTitle(const char *pc_Title)
{
	ASSERT(pc_Title);
	BF_GUI_ViewMenu_Item* po;
	for(int i=0;i<ploMenu->CountItems();i++){
		po = ItemAt(i);
		if(po->sTitle==pc_Title) return po;
	}
	return NULL;
}

BF_GUI_ViewMenu_Item*   
BF_GUI_DlgView_Menu::ItemAt(int32 i_Index)
{
	if(i_Index>=ploMenu->CountItems()) return NULL;
	return (BF_GUI_ViewMenu_Item*)ploMenu->ItemAt(i_Index);
}

BF_GUI_ViewMenu_Item*   
BF_GUI_DlgView_Menu::FocusItem()
{
	if(iNavCursorIndex<0 || ploMenu->CountItems()==0) return NULL;
	return (BF_GUI_ViewMenu_Item*)ploMenu->ItemAt(iNavCursorIndex);
}

float
BF_GUI_DlgView_Menu::Height()
{
	return poFont->fHeight;
}


BF_GUI_DlgView_Menu::~BF_GUI_DlgView_Menu()
{
	printf("BF_GUI_DlgView_Menu::~BF_GUI_DlgView_Menu()\n");	
	/*---------*/
	if(bDestroyMenu) DELETE(ploMenu);
	printf("BF_GUI_DlgView_Menu::~BF_GUI_DlgView_Menu()\n");
}


int BF_GUI_DlgView_Menu_Sort_0(const void * p_1, const void *p_2)
{
	BF_GUI_ViewMenu_Item *po1 = *((BF_GUI_ViewMenu_Item**)p_1);
	BF_GUI_ViewMenu_Item *po2 = *((BF_GUI_ViewMenu_Item**)p_2);
	
	return po1->sTitle.Compare(po2->sTitle);
}

void					
BF_GUI_DlgView_Menu::SortList(BL_List *plo_List)
{
	ASSERT(plo_List);
	plo_List->SortItems(BF_GUI_DlgView_Menu_Sort_0);
}

void					
BF_GUI_DlgView_Menu::SetList(BL_List *plo_NewMenu)
{
	if(bDestroyMenu) DELETE(ploMenu) else ploMenu = NULL;
	ASSERT(plo_NewMenu);
	ploMenu = plo_NewMenu;
	iNavCursorIndex = ploMenu->CountItems()>0?0:-1;	
}

void					
BF_GUI_DlgView_Menu::DeleteItemAt(int i_Index)
{
	BF_GUI_ViewMenu_Item *poItem = ItemAt(i_Index);
	if(!poItem) return;
	ploMenu->RemoveItem(i_Index);
	DELETE(poItem);
	if(iNavCursorIndex>=ploMenu->CountItems()) iNavCursorIndex--;
	Draw(Bounds());
}

void
BF_GUI_DlgView_Menu::KeyDown(const char *bytes, int32 numBytes)
{
	/* is list empty */
	if(iNavCursorIndex<0){
		BView::KeyDown(bytes,numBytes);
		return;
	}
	/* handle keys */
	if(numBytes==1 && (bytes[0]==B_DOWN_ARROW || bytes[0]==B_RIGHT_ARROW)){		
		NavChangeCursor(iNavCursorIndex+1,BF_GUI_TOOLMENU_MOVE_DOWN);
	}else
	if(numBytes==1 && (bytes[0]==B_UP_ARROW  || bytes[0]==B_LEFT_ARROW)){		
		NavChangeCursor(iNavCursorIndex-1,BF_GUI_TOOLMENU_MOVE_UP);
	}else
	if(numBytes==1 && (bytes[0]==B_HOME || bytes[0]==B_PAGE_UP)){		
		NavChangeCursor(0,BF_GUI_TOOLMENU_MOVE_POS);
	}else
	if(numBytes==1 && (bytes[0]==B_END  || bytes[0]==B_PAGE_DOWN)){	
		NavChangeCursor(ploMenu->CountItems()-1,BF_GUI_TOOLMENU_MOVE_END);
	}else	
	if(numBytes==1 && bytes[0]==B_ENTER){		
		NavEnter();
	}else
	if(numBytes==1 && bytes[0]==B_ESCAPE){		
		NavEscape();
	}else{	
		
		if(BF_GUI_DialogView::KeyDownExt(bytes,numBytes)) return;else
		BView::KeyDown(bytes,numBytes);
	}	
};

void					
BF_GUI_DlgView_Menu::NavChangeCursor(int32 i_NewCursor,int i_DirectMove,bool b_Redraw)
{
	if(i_NewCursor<0){
		if(i_DirectMove!=BF_GUI_TOOLMENU_MOVE_UP)
			i_NewCursor = 0;
		else
			i_NewCursor = ploMenu->CountItems()-1;
	}
	if(i_NewCursor>=ploMenu->CountItems()){
		if(i_DirectMove!=BF_GUI_TOOLMENU_MOVE_DOWN)
			i_NewCursor = ploMenu->CountItems()-1;
		else
			i_NewCursor = 0;
	}
	//
	if(i_NewCursor==iNavCursorIndex) return;
	///
	if(i_NewCursor<0) return;
	// find next non-empty line //
	BF_GUI_ViewMenu_Item *po;
	while(TRUE){
		po = (BF_GUI_ViewMenu_Item*)ploMenu->ItemAt(i_NewCursor);
		if(po->sTitle!="") break;else
		switch(i_DirectMove){
		case BF_GUI_TOOLMENU_MOVE_END:			
			if(i_NewCursor-->=0) continue;else return;
			break;
		case BF_GUI_TOOLMENU_MOVE_DOWN:
		case BF_GUI_TOOLMENU_MOVE_PG_DOWN:		
			i_NewCursor++;			break;
		case BF_GUI_TOOLMENU_MOVE_PG_UP:					
		case BF_GUI_TOOLMENU_MOVE_UP:
			i_NewCursor--;			break;
		}
		if(i_NewCursor==ploMenu->CountItems()) return;
		if(i_NewCursor<0) return;
		break;
	}
	// draw items
	int32 iOldCursor = iNavCursorIndex;
	iNavCursorIndex = i_NewCursor;
	
	if(b_Redraw){
		if( iNavCursorIndex>=0 )	DrawItem(this,iOldCursor,true);			
		if(b_Redraw) DrawItem(this,iNavCursorIndex,true);
	}
		
	po->MakeFocus(this);
}

void
BF_GUI_DlgView_Menu::NavEscape()
{
	BMessage oMessage(BF_MSG_DIALOG_PRESSED_CANCEL);
	oMessage.AddPointer("bf_DlgView_Focus",this);
	BMessenger oMessenger(Parent());		
	oMessenger.SendMessage(&oMessage);			
}

void					
BF_GUI_DlgView_Menu::NavEnter()
{
	BF_GUI_ViewMenu_Item *po = (BF_GUI_ViewMenu_Item*)ploMenu->ItemAt(iNavCursorIndex);	
	if(po->Invoke(this)) return;
	BMessage oMessage(BF_MSG_DIALOG_PRESSED_OK);
	oMessage.AddPointer("bf_DlgView_Focus",this);
	BMessenger oMessenger(Parent());		
	oMessenger.SendMessage(&oMessage);		
}

void					
BF_GUI_DlgView_Menu::AddItem(BF_GUI_ViewMenu_Item* po_Item,bool b_SetCursorOnItem)
{	
	ASSERT(po_Item);
	ploMenu->AddItem(po_Item);
	
	DrawItem(this,ploMenu->CountItems()-1,true);
	
	if(iNavCursorIndex<0) NavChangeCursor(0,BF_GUI_TOOLMENU_MOVE_POS);else
	if(b_SetCursorOnItem) NavChangeCursor(ploMenu->CountItems()-1,BF_GUI_TOOLMENU_MOVE_POS);		
}

void					
BF_GUI_DlgView_Menu::SaveToMessage(BMessage *po_Message)
{	
	po_Message->AddInt32(Name(),iNavCursorIndex);	
	//
	BF_GUI_ViewMenu_Item *po = (BF_GUI_ViewMenu_Item*)ploMenu->ItemAt(iNavCursorIndex);	
	if(po){
		BString s;	
		s<<Name();	
		s<<"_code";
		po_Message->AddString(s.String(),po->sCode.String());	
	}
}

void					
BF_GUI_DlgView_Menu::Draw(BRect o_Rect)
{		
	/* create render */
	BRect 	oRect(Bounds());
	BBitmap oB(oRect,B_RGBA32,TRUE);
	BView	*pv;
	oB.AddChild(pv = new BView(oRect,"",B_FOLLOW_ALL,B_WILL_DRAW));	
	oB.Lock();	
	/* init render */
	pv->SetFont(&poFont->oFont);
	pv->SetViewColor(B_TRANSPARENT_COLOR);	
	
	/* draw background */			
	pv->SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BACK));
	pv->FillRect(Bounds());		
			
	DrawItems(pv);
	
	/* close render */	
	pv->Sync();
	oB.Unlock();
	DrawBitmap(&oB,oRect);
}

void	
BF_GUI_DlgView_Menu::DrawItems(BView *po_Render)
{
	/* draw items */	
	for(int i=0;i<ploMenu->CountItems();i++){
		DrawItem(po_Render,i,false);
	}
}

void	
BF_GUI_DlgView_Menu::Draw_FocusItemPointer(BPoint & o_PointStart,BView *po_Render)
{
	BPoint o1(o_PointStart),o2(o1),o3;		
	o1.y += poFont->fHeight/2-2;				
	o2.y += poFont->fHeight/2;
	o2.x += 3;		
	o3 = o1;
	o3.y+=4;
	po_Render->FillTriangle(o1,o2,o3);
}

void	
BF_GUI_DlgView_Menu::MakeFocus(bool	b_Focused)
{
	BView::MakeFocus(b_Focused);
	if(iNavCursorIndex<0 && ploMenu && ploMenu->CountItems()>0) iNavCursorIndex = 0;
	if(iNavCursorIndex>=0) DrawItem(this,iNavCursorIndex,true);
}

void 		
BF_GUI_DlgView_Menu::AttachedToWindow(void)
{	
	BF_GUI_DialogView::AttachedToWindow();
	NavChangeCursor(iNavCursorIndex<0?0:iNavCursorIndex,BF_GUI_TOOLMENU_MOVE_POS,false);	
}


void					
BF_GUI_DlgView_Menu::DrawItem(BView *po,int32 i_Index,bool b_ReqDrawBack)
{
}


void					
BF_GUI_DlgView_Menu::SetColor_Text(BView * po_Render,bool b_IsFocused,BF_GUI_ViewMenu_Item *po_Item)
{
	po_Render->SetHighColor((po_Item && po_Item->bSelected)?SYS_COLOR(BF_COLOR_DIALOG_MENU_ITEMSELECTED):SYS_COLOR(BF_COLOR_DIALOG_TEXT));	
	if((po_Item && po_Item->bSelected)){				
		po_Render->SetLowColor(SYS_COLOR(BF_COLOR_DIALOG_BUTTON_FOCUS));	
	}else{	
		po_Render->SetLowColor(SYS_COLOR(BF_COLOR_DIALOG_BACK));	
	}
}

void					
BF_GUI_DlgView_Menu::SetColor_Back(BView *po_Render,bool b_IsFocused)
{
	if(b_IsFocused){
		po_Render->SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BUTTON_FOCUS));	
	}else{
		po_Render->SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BACK));	
	}	
}
/*================================================================*/
BF_GUI_DlgView_VMenu::BF_GUI_DlgView_VMenu(
	const BRect	&o_Rect,
	const char	*pc_Name,
	uint32	i_FollowType,
	BL_List *plo_Menu,
	uint32	i_Style,
	float	f_NewItemHeight)
:BF_GUI_DlgView_Menu(o_Rect,pc_Name,i_FollowType,plo_Menu)
{
	iStyle = i_Style | BF_GUI_DLGVIEW_VMENU_SET_VSCROLLBAR;
	iFirstIndex = 0;
	fNewItemHeight = f_NewItemHeight;
	
	iPageSize = (int32)( Bounds().Height() / Height() );
	poVScroll = NULL;
	
	if(iStyle & BF_GUI_DLGVIEW_VMENU_SET_VSCROLLBAR) CreateVScroll();
}

void					
BF_GUI_DlgView_VMenu::AddItem(BF_GUI_ViewMenu_Item* po_Item,bool b_SetCursorOnItem)
{
	BF_GUI_DlgView_Menu::AddItem(po_Item,b_SetCursorOnItem);
	CreateVScroll();
}

void 					
BF_GUI_DlgView_VMenu::CreateVScroll()
{		
	if(!poVScroll){	
		if(iPageSize<ploMenu->CountItems()){
			
			ResizeTo(Bounds().Width()-14,Bounds().Height());
									
			BRect oRect(Frame());
			oRect.left = oRect.right+6; 
			oRect.right = oRect.left+8;
			poVScroll = new BF_GUI_VScrollBar(oRect,"scroll",B_FOLLOW_TOP_BOTTOM|B_FOLLOW_RIGHT);		
			
			if(Window()) Parent()->AddChild(poVScroll);
		}
	}
	
	if(poVScroll) poVScroll->SetLimits(ploMenu->CountItems(),iPageSize);
}


void 					
BF_GUI_DlgView_VMenu::AttachedToWindow(void)
{
	if(poVScroll){
		Parent()->AddChild(poVScroll);		
	}
	BF_GUI_DlgView_Menu::AttachedToWindow();
}

void					
BF_GUI_DlgView_VMenu::FrameResized(float width, float height)
{
	BF_GUI_DlgView_Menu::FrameResized(width,height);
	iPageSize = (int32)( Bounds().Height() / Height() );
}

float
BF_GUI_DlgView_VMenu::Height()
{
	if(fNewItemHeight>0) return fNewItemHeight;
	float f = poFont->fHeight;
	if((iStyle & BF_GUI_DLGVIEW_VMENU_SICON) && f<18) f=18;
	
	return f;
}
void					
BF_GUI_DlgView_VMenu::SetList(BL_List *plo_NewMenu)
{
	BF_GUI_DlgView_Menu::SetList(plo_NewMenu);
	iFirstIndex = 0;
	CreateVScroll();
}

void					
BF_GUI_DlgView_VMenu::SaveToMessage(BMessage *po_Message)
{
	if((iStyle & BF_GUI_DLGVIEW_VMENU_CAN_SELECTING) && ploMenu){
		BF_GUI_ViewMenu_Item *po;
		BL_String	s;
		
		for(int i=0;i<ploMenu->CountItems();i++){			
			po = ItemAt(i);
			if(po->bSelected){
				s=Name();
				s+="_sel";
				po_Message->AddString(s.String(),po->sCode.String());			
			}
		}
	}
	BF_GUI_DlgView_Menu::SaveToMessage(po_Message);
}

void
BF_GUI_DlgView_VMenu::KeyDown(const char *bytes, int32 numBytes)
{
	/* is list empty */
	if(iNavCursorIndex<0){
		BView::KeyDown(bytes,numBytes);
		return;
	}
	/* handle keys */
	if((iStyle & BF_GUI_DLGVIEW_VMENU_CAN_SELECTING) && numBytes==1 && bytes[0]==B_INSERT){
		NavSelect();
	}else
	if(numBytes==1 && bytes[0]==B_PAGE_UP){		
		NavChangeCursor(iNavCursorIndex - iPageSize,BF_GUI_TOOLMENU_MOVE_PG_UP);
	}else
	if(numBytes==1 && bytes[0]==B_PAGE_DOWN){	
		NavChangeCursor(iNavCursorIndex + iPageSize,BF_GUI_TOOLMENU_MOVE_PG_DOWN);
	}else	
		BF_GUI_DlgView_Menu::KeyDown(bytes,numBytes);
}

void
BF_GUI_DlgView_VMenu::NavSelect()
{
	BF_GUI_ViewMenu_Item*   poItem = FocusItem();	
	if(!poItem) return;
	
	poItem->bSelected = !poItem->bSelected;
	DrawItem(this,iNavCursorIndex,true);
	NavChangeCursor(iNavCursorIndex+1,BF_GUI_TOOLMENU_MOVE_DOWN,true);
}

void	
BF_GUI_DlgView_VMenu::DrawItems(BView *po_Render)
{
	/* draw items */	
	for(int i=iFirstIndex;i<iFirstIndex+iPageSize;i++){
		if(i>=ploMenu->CountItems()) break;
		DrawItem(po_Render,i,false);
	}
}

void
BF_GUI_DlgView_VMenu::DrawItemExt(BView *po_Render,int32 i_Index,bool b_ReqDrawBack)
{
	DrawItem(po_Render,i_Index,b_ReqDrawBack);
}

void					
BF_GUI_DlgView_VMenu::DrawItem(BView *po_Render,int32 i_Index,bool b_ReqDrawBack)
{	
	ASSERT(po_Render);
	if((i_Index - iFirstIndex) > iPageSize) return;

	BPoint oPointStart(0,0),oPoint;		
	oPointStart.y += ((float)(i_Index-iFirstIndex)) * Height();
	oPoint = oPointStart;
	//
	bool 	bListFocused = IsFocus();
	bool	bItemFocused = i_Index==iNavCursorIndex;
	// draw background 
	if(bListFocused || b_ReqDrawBack)
	{
		SetColor_Back(po_Render,bListFocused && bItemFocused);
		BRect oRect =po_Render->Bounds();
		oRect.top = oPoint.y;
		oRect.bottom = oRect.top+Height();
		po_Render->FillRect(oRect);
	}
	
	// check for empty index
	if(i_Index<0) return;
	
				
	BF_GUI_ViewMenu_Item *po;
	po = (BF_GUI_ViewMenu_Item*)ploMenu->ItemAt(i_Index);
	ASSERT(po);	//	
	SetColor_Text(po_Render,bItemFocused,po);
	
	if(po->sTitle!=""){
		oPoint.x = 10.0;
		if(iStyle & BF_GUI_DLGVIEW_VMENU_SICON){
			if(po->poSIcon){
				po_Render->SetDrawingMode(B_OP_ALPHA);				
				po_Render->DrawBitmap(po->poSIcon,oPoint);
				po_Render->SetDrawingMode(B_OP_COPY);
			}
			oPoint.x+=20;
		}
		oPoint.y += poFont->fAscent;
		po_Render->DrawString(po->sTitle.String(),oPoint);
	}else{
		oPoint.x = 10.0;
		oPoint.y+=Height()/2;
		BRect oRect(po_Render->Bounds());
		BPoint oPointEnd(oRect.right-4.0,oPoint.y);
		po_Render->StrokeLine(oPoint,oPointEnd);
	}
	//
	if(bItemFocused) Draw_FocusItemPointer(oPointStart,po_Render);	
	//
	po->Draw(this,po_Render,oPointStart.y);
}

//
void					
BF_GUI_DlgView_VMenu::MouseDown(BPoint point)
{	
	if(!(Flags() & B_NAVIGABLE)) return;
	/*  */
	int32 iNewCursor;
	/* */
	BPoint oPoint = point;
	iNewCursor = (int32)(oPoint.y/Height()) + iFirstIndex;
	if(iNewCursor!=iNavCursorIndex){
		NavChangeCursor(iNewCursor,BF_GUI_TOOLMENU_MOVE_POS);
		NavEnter();
	}else{
		NavEnter();
	}
}

void					
BF_GUI_DlgView_VMenu::NavChangeCursor(int32 i_NewCursor,int i_DirectMove,bool b_Redraw)
{
	int32 iOldCursor = iNavCursorIndex;
	BF_GUI_DlgView_Menu::NavChangeCursor(i_NewCursor,i_DirectMove,false);
	
	BRect oRect(Bounds());
	
	int iCount = (int)(oRect.Height()/Height());	
	int i1=iNavCursorIndex-iFirstIndex;
	
	if(i1>=iCount){
		iFirstIndex = iNavCursorIndex - iCount+1;						
		Draw(Bounds());					
	}else
	if(i1<0){
		iFirstIndex = iNavCursorIndex;
		Draw(Bounds());		
	}else{
		if(iOldCursor>=0) DrawItem(this,iOldCursor,true);
		DrawItem(this,iNavCursorIndex,true);		
	}
	if(iStyle & BF_GUI_DLGVIEW_VMENU_NAV_PARENTINFORM){
		BMessenger oMessenger(Parent());
		BMessage   oMessage(BF_MSG_DIALOG_VMENU_NAV);
		oMessage.AddString("bf_name",Name());
		oMessage.AddInt32("bf_index",iNavCursorIndex);
		oMessage.AddInt32("bf_oldindex",iOldCursor);
		oMessenger.SendMessage(&oMessage);
	}
	
	if(poVScroll) poVScroll->SetCursor(iNavCursorIndex);
}
/*================================================================*/
BF_GUI_DlgView_VCMenu_Column::BF_GUI_DlgView_VCMenu_Column(const char *pc_ColName,float f_Width)
{
	ASSERT(pc_ColName);
	sName = pc_ColName;
	fWidth = f_Width;
}

BF_GUI_DlgView_VCMenu::BF_GUI_DlgView_VCMenu(
	const BRect 	&o_Rect,
	const char	*pc_Name,
	uint32	i_FollowType,
	BL_List *plo_Menu,
	uint32	i_Style,
	BL_List	 *plo_Col)
:BF_GUI_DlgView_VMenu(o_Rect,pc_Name,i_FollowType,plo_Menu,i_Style)
{
	ASSERT(plo_Col);
	ploCol = plo_Col;
}

BF_GUI_DlgView_VCMenu::~BF_GUI_DlgView_VCMenu()
{
	DELETE(ploCol);
}


void					
BF_GUI_DlgView_VCMenu::DrawItem(BView *po_Render,int32 i_Index,bool b_ReqDrawBack)
{
	if(!po_Render) po_Render = this;	
	//
	bool	bListFocused = IsFocus();
	bool	bItemFocused = i_Index==iNavCursorIndex;
	BPoint 	oPoint(0,0);		
	oPoint.y += ((float)(i_Index-iFirstIndex)) * Height();
	//
	if(bListFocused || b_ReqDrawBack){
		SetColor_Back(po_Render,bListFocused && bItemFocused);
		BRect oRect =po_Render->Bounds();
		oRect.top = oPoint.y;
		oRect.bottom = oRect.top+poFont->fHeight;
		po_Render->FillRect(oRect);
	}			
	
	// check for empty index
	if(i_Index<0) return;
	
	// get item 
	BF_GUI_ViewMenu_Item *po;
	po = (BF_GUI_ViewMenu_Item*)ploMenu->ItemAt(i_Index);
	if(!po) return;
	
	SetColor_Text(po_Render,bItemFocused,po);
	if(po->sTitle!=""){
		if(iStyle & BF_GUI_DLGVIEW_VMENU_SICON){
			if(po->poSIcon){
				po_Render->SetDrawingMode(B_OP_ALPHA);				
				po_Render->DrawBitmap(po->poSIcon,oPoint);
				po_Render->SetDrawingMode(B_OP_COPY);
			}
			oPoint.x+=20;
		}		
		//
		BF_GUI_ViewMenu_Item *poColData;
		BF_GUI_DlgView_VCMenu_Column	*poColInfo;
		BPoint	o1,o2;
		o1.y=oPoint.y;
		o2.y=oPoint.y+Height();
		oPoint.y += poFont->fAscent;
		for(int32 i=-1;i<po->loColItem.CountItems();i++){		
			poColInfo = (BF_GUI_DlgView_VCMenu_Column*)ploCol->ItemAt(i+1);
			ASSERT(poColInfo);
			//
			if(i>=0){
				o1.x = oPoint.x;
				o2.x = oPoint.x;
				po_Render->StrokeLine(o1,o2);
			}
			oPoint.x+=3;
			//
			if(i<0) poColData = po;else poColData = (BF_GUI_ViewMenu_Item*)po->loColItem.ItemAt(i);
			if(poColData){
				int iLength = BF_GUI_GetTextLength(poColData->sTitle,poColInfo->fWidth-6,poFont->oFont);
				po_Render->DrawString(poColData->sTitle.String(),iLength,oPoint);
			}
			oPoint.x+=poColInfo->fWidth-3;			
		}
	}else{
		oPoint.x = 4.0;
		oPoint.y+=Height()/2;
		BRect oRect(po_Render->Bounds());
		BPoint oPointEnd(oRect.right-4.0,oPoint.y);
		po_Render->StrokeLine(oPoint,oPointEnd);
	}
	//
}

/*================================================================*/
BF_GUI_DlgView_HMenu::BF_GUI_DlgView_HMenu(
	const BRect	&o_Rect,
	const char	*pc_Name,
	uint32	i_FollowType,
	BL_List *plo_Menu)
:BF_GUI_DlgView_Menu(o_Rect,pc_Name,i_FollowType,plo_Menu)
{
	bItemsInCenter = true;
	// calc width for every item //
	BF_GUI_ViewMenu_Item *po;
	fItemsWidth=0;
	for(int i=0;i<ploMenu->CountItems();i++){
		po = (BF_GUI_ViewMenu_Item*)ploMenu->ItemAt(i);
		po->CalcWidth(poFont);
		fItemsWidth+=po->fWidth+10;
		if(i>0)	fItemsWidth+=10;
	}		
}
//
float
BF_GUI_DlgView_HMenu::CalcItemXPos(int32 i_Index)
{
	float fx=0;
	if(bItemsInCenter) fx += Bounds().Width()/2-fItemsWidth/2;
	//
	{// calc width of all prev.items
		BF_GUI_ViewMenu_Item *po;
		for(int i=0;i<i_Index;i++){
			po = (BF_GUI_ViewMenu_Item*)ploMenu->ItemAt(i);
			fx += po->fWidth+10;
		}
	}	
	return fx;
}

void					
BF_GUI_DlgView_HMenu::DrawItem(BView *po_Render,int32 i_Index,bool b_ReqDrawBack)
{
	BPoint 	oPoint(CalcItemXPos(i_Index),0);					
	bool	bListFocused = IsFocus();
	bool	bItemFocused = i_Index==iNavCursorIndex;
	//
	BF_GUI_ViewMenu_Item *po;
	po = (BF_GUI_ViewMenu_Item*)ploMenu->ItemAt(i_Index);
	if(!po) return;
	//
	if(bListFocused || b_ReqDrawBack){
		SetColor_Back(po_Render,bListFocused && bItemFocused);
		BRect oRect = po_Render->Bounds();
		oRect.left = oPoint.x;
		oRect.right = oRect.left+po->fWidth+10;
		po_Render->FillRect(oRect);				
	}
	//	
	SetColor_Text(po_Render,bItemFocused);	
	if(bItemFocused) Draw_FocusItemPointer(oPoint,po_Render);		
	
	oPoint.y += poFont->fAscent;
	oPoint.x += 10;
	po_Render->DrawString(po->sTitle.String(),oPoint);
	//	
}

void					
BF_GUI_DlgView_HMenu::MouseDown(BPoint point)
{	
	if(!(Flags() & B_NAVIGABLE)) return;
	
	BRect 	oRect(Bounds());
	BPoint 	oPointStart(bItemsInCenter?(oRect.Width()/2-fItemsWidth/2):0,0);			

	{// calc width of all prev.items
		BF_GUI_ViewMenu_Item *po;
		for(int i=0;i<ploMenu->CountItems();i++){
			po = (BF_GUI_ViewMenu_Item*)ploMenu->ItemAt(i);
			oPointStart.x += po->fWidth+10;
			if(oPointStart.x>=point.x){
				if(i!=iNavCursorIndex){
					NavChangeCursor(i,BF_GUI_TOOLMENU_MOVE_POS);
					NavEnter();
				}else{
					NavEnter();
				}
				break;
			}
		}
	}
}

