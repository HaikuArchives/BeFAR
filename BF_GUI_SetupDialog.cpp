/*
===============================================
Project:	BeFar
File:		BF_GUI_SetupDialiog.cpp
Desc:		Visual editing of main_setup
Author:		Baza
Created:	20.11.99
Modified:	20.11.99
===============================================
*/

#include <Mime.h>
#include <stdio.h>
#include "BF_Dict.h"
#include "BF_GUI_SetupDialog.h"
#include "BF_GUI_Func.h"
#include "BF_GUI_WinMain.h"

///////////////////////////////////////////////////////////////////  
BF_GUI_FontView::BF_GUI_FontView(
	BRect &o_Rect,
	int i_FollowMode
):BF_GUI_DialogView(o_Rect,"test",i_FollowMode,B_WILL_DRAW)
{
	SetViewColor(B_TRANSPARENT_COLOR);
	oFont = poSysSetup->oFontToolView;
	SetFont(&oFont.oFont);		
}

void
BF_GUI_FontView::Draw(BRect o_Rect)
{
	// draw background
	SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BACK));
	FillRect(Bounds());
	// draw text 
	SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BUTTON_TEXT));
	SetLowColor(SYS_COLOR(BF_COLOR_DIALOG_BACK));
	DrawString("Test !",BPoint(0,oFont.fAscent));
}

void	
BF_GUI_FontView::UpdateFont()
{
	if(!Parent()) return;
	BF_GUI_DlgView_VMenu *poStyles = (BF_GUI_DlgView_VMenu*)Parent()->FindView("styles");
	BF_GUI_DlgView_VMenu *poFamilies = (BF_GUI_DlgView_VMenu*)Parent()->FindView("families");
	BF_GUI_ViewEdit		 *poSize = (BF_GUI_ViewEdit*)Parent()->FindView("size");
	ASSERT(poStyles && poFamilies && poSize,"BF_GUI_FontView::UpdateFont()");
	//
	BF_GUI_ViewMenu_Item *poFamily = poFamilies->FocusItem();
	//BF_GUI_ViewMenu_Item *poStyle = poStyles->FocusItem();
	//
	oFont.Set(poFamily->sTitle.String(),B_BOLD_FACE,B_FORCE_ANTIALIASING,poSize->sValue.Float());
	SetFont(&oFont.oFont);	
	//	
	Draw(Bounds());
}


BF_GUI_SetupDialog_FontFamilyItem::BF_GUI_SetupDialog_FontFamilyItem(
const char *pc_Title,BL_List *plo_StyleItem
):BF_GUI_ViewMenu_Item(pc_Title,pc_Title)
{
	ASSERT(plo_StyleItem,"BF_GUI_SetupDialog_FontFamilyItem::BF_GUI_SetupDialog_FontFamilyItem");
	ploStyleItem = plo_StyleItem;
}
	
BF_GUI_SetupDialog_FontFamilyItem::~BF_GUI_SetupDialog_FontFamilyItem()
{
	DELETE(ploStyleItem);
}

void					
BF_GUI_SetupDialog_FontFamilyItem::MakeFocus(BF_GUI_DlgView_Menu *po_Parent)
{		
	BF_GUI_ViewMenu_Item::MakeFocus(po_Parent);
	if(!po_Parent->Parent() || !ploStyleItem) return;
	
	BF_GUI_DlgView_VMenu *poStyles = (BF_GUI_DlgView_VMenu*)po_Parent->Parent()->FindView("styles");
	if(!poStyles) return;
	// prepare new list //
	BL_List *ploStyle=new BL_List();
	for(int i=0;i<ploStyleItem->CountItems();i++) ploStyle->AddItem(new BF_GUI_ViewMenu_Item( (BF_GUI_ViewMenu_Item*) ploStyleItem->ItemAt(i) ));
	//poStyles->bDestroyMenu = false;
	poStyles->SetList(ploStyle);	
	poStyles->Draw(poStyles->Bounds());
	//
	BF_GUI_FontView *poTest = (BF_GUI_FontView*)po_Parent->Parent()->FindView("test");
	if(!poTest) return;
	poTest->UpdateFont();
}

///////////////////////////////////////////////////////////////////  
BF_GUI_SetupDialog_FontItem::BF_GUI_SetupDialog_FontItem(
	const char *pc_Title,
	const char *pc_Code,
	BF_GUI_Setup_Font *po_Font,
	BBitmap *po_SIcon
):BF_GUI_ViewMenu_Item(pc_Title,pc_Code,po_SIcon)	
{
	ASSERT(po_Font,"BF_GUI_SetupDialog_FontItem::BF_GUI_SetupDialog_FontItem");
	poFont = po_Font;	
}	

BF_GUI_SetupDialog_FontDialog::BF_GUI_SetupDialog_FontDialog(
	BF_GUI_SetupDialog_FontItem *po_FontItem,
	const BRect & o_Rect,
	const char *pc_Title,		
	const char *pc_Name,
	const BMessage &o_Message,
	uint32	i_WinResizeType, // BG_GUI_DIALOG_WINRESIZE_NONE
	bool b_DoubleBorder
):BF_GUI_Dialog(o_Rect,pc_Title,pc_Name,o_Message,i_WinResizeType,b_DoubleBorder)
{	
	ASSERT(po_FontItem,"BF_GUI_SetupDialog_FontDialog::BF_GUI_SetupDialog_FontDialog");
	poFontItem = po_FontItem;
}

void					
BF_GUI_SetupDialog_FontDialog::Save(BMessage& o_Message)
{
	BF_GUI_Dialog::Save(o_Message);	
	
	
	int32 iIndex;
	ASSERT(B_OK==o_Message.FindInt32("styles",&iIndex));			
	
	const char *pcStyle=NULL;
	ASSERT(B_OK==o_Message.FindString("styles_code",&pcStyle));			
	const char *pcFamily=NULL;
	ASSERT(B_OK==o_Message.FindString("families_code",&pcFamily));			
	const char *pcSize=NULL;
	ASSERT(B_OK==o_Message.FindString("size",&pcSize) && pcSize);				
	bool bAntialising=false;
	ASSERT(B_OK==o_Message.FindBool("anti_aliasing",&bAntialising));
	
	BL_String s(pcSize);	
	float fSize = s.Float();	

	int32 iStyles=0;	
	s = pcStyle;
	if(s.FindFirst("Bold")>=0) iStyles = iStyles | B_BOLD_FACE;
	if(s.FindFirst("Regular")>=0) iStyles = iStyles | B_REGULAR_FACE;
	if(s.FindFirst("Italic")>=0) iStyles = iStyles | B_ITALIC_FACE;
	
	int32 iFlags = 0;
	if(bAntialising)  
		iFlags = iFlags | B_FORCE_ANTIALIASING;
	else
		iFlags = iFlags | B_DISABLE_ANTIALIASING;
	
	poFontItem->poFont->Set(pcFamily,iStyles,iFlags,fSize);
}

void 					
BF_GUI_SetupDialog_FontDialog::AttachedToWindow(void)
{
	BF_GUI_Dialog::AttachedToWindow();

	BF_GUI_DlgView_Menu *poMenu = (BF_GUI_DlgView_Menu*)FindView("families");
	if(!poMenu) return;
	// try to search current font 
	BF_GUI_SetupDialog_FontFamilyItem *poItem =NULL;
	poItem = (BF_GUI_SetupDialog_FontFamilyItem*)poMenu->ItemByTitle(poFontItem->poFont->sName.String());
	if(poItem){
		poMenu->NavChangeCursor(poMenu->ploMenu->IndexOf(poItem),BF_GUI_TOOLMENU_MOVE_POS,false);
	}else{
		poItem = (BF_GUI_SetupDialog_FontFamilyItem*)poMenu->ItemAt(0);
		if(!poItem) return;
	}
}

bool					
BF_GUI_SetupDialog_FontItem::Invoke(BF_GUI_DlgView_Menu *po_Parent)
{
	BF_GUI_Func_EnableDialog( po_Parent->Parent()->Name(),false );
	/* prepare message */
	BMessage oMessage(BF_MSG_DIALOG_FOCUS/*BF_MSG_MAINVIEW_MAINSETUP_FONT_UPDATED*/);
	oMessage.AddPointer("bf_focus",po_Parent->Parent());
	/* make dialog */	
	printf("BF_GUI_SetupDialog_FontItem::Invoke this=%i\n",this);
	BF_GUI_SetupDialog_FontDialog *poDialog = new BF_GUI_SetupDialog_FontDialog(this,BRect(0,0,440,0),
		sTitle.String(),"color_dialog",oMessage,BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER,true);	
	/* resize dialog */	
	BRect oRect;	
	
	// make fonts_menu //	
	BL_List *poList = new BL_List();	
	int32 iCountFam = count_font_families();
	for(int32 iFam=0;iFam<iCountFam;iFam++){
		font_family uFam;
		uint32 iFlags;
		if( B_OK != get_font_family(iFam,&uFam,&iFlags)) continue;
	
		// load styles
		BL_List *plsStyle = new BL_List();
		int32 iCountStyles = count_font_styles(uFam);
		for(int iStyle=0;iStyle<iCountStyles;iStyle++){
			font_style uStyle;
			if(B_OK != get_font_style(uFam,iStyle,&uStyle,&iFlags)) continue;
			plsStyle->AddItem( new BF_GUI_ViewMenu_Item(uStyle,uStyle));
		}			
		// make store item
		BF_GUI_SetupDialog_FontFamilyItem *poItem = new BF_GUI_SetupDialog_FontFamilyItem(uFam,plsStyle);
		poList->AddItem(poItem);										
	}			
	poDialog->LocalBounds(oRect);		
	oRect.bottom = oRect.top + 300;
	oRect.right = oRect.left + 200;
	poDialog->AddChild( new BF_GUI_DlgView_VMenu(oRect,"families",B_FOLLOW_ALL,poList,BF_GUI_DLGVIEW_VMENU_NAV_PARENTINFORM) );
	
	// make styles //
	poDialog->LocalBounds(oRect);
	oRect.bottom = oRect.top + 300;
	oRect.left += 210;	
	poDialog->AddChild( new BF_GUI_DlgView_VMenu(oRect,"styles",B_FOLLOW_ALL,new BL_List(),BF_GUI_DLGVIEW_VMENU_NAV_PARENTINFORM) );
	
	// make anti_aliasing
	{
		poDialog->LocalBounds(oRect);
		oRect.top += 310;
		oRect.bottom = oRect.top + poSysSetup->oFontToolView.fHeight*2;
		oRect.right = oRect.left + 140;
		BF_GUI_ViewCheck *poCheck = new BF_GUI_ViewCheck(oRect,"anti_aliasing",BF_DictAt(BF_DICT_FONTSETUP_ANTI)
			,poFont->iFlags & B_FORCE_ANTIALIASING
			,B_FOLLOW_LEFT|B_FOLLOW_TOP,B_WILL_DRAW|B_NAVIGABLE);
		poDialog->AddChild(poCheck);
	}
	
	
	// make test 
	poDialog->LocalBounds(oRect);
	oRect.top += 310;
	oRect.bottom = oRect.top + poSysSetup->oFontToolView.fHeight*3;
	oRect.left += 160;
	poDialog->AddChild( new BF_GUI_FontView(oRect,B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM) );	
	
	// make size 
	poDialog->LocalBounds(oRect);
	oRect.top += 310+poSysSetup->oFontToolView.fHeight*2;
	oRect.bottom = oRect.top + poSysSetup->oFontToolView.fHeight;
	oRect.right = oRect.left + 150;
	BL_String s;
	s<<poFont->fSize;
	BF_GUI_ViewEdit_Create(oRect,BF_DictAt(BF_DICT_SIZE),poDialog,"size",s.String()
		,B_FOLLOW_LEFT|B_FOLLOW_BOTTOM,B_NAVIGABLE);
							
	///////////////////////////////		
	/* make bottom menu */
	BRect oRect1;
	poDialog->LocalBounds(oRect1);
	oRect.left = oRect1.left;
	oRect.right = oRect1.right;	
	oRect.top = oRect.bottom+10;
	oRect.bottom = oRect.top + poSysSetup->oFontToolView.fHeight;
	BL_List *ploMenu = new BL_List();
	ploMenu->AddItem(new BF_GUI_ViewMenu_Item(BF_DictAt(BF_DICT_OK),"ok"));
	poDialog->AddMenu(oRect,ploMenu,true);		
	
	/* finish */
	poDialog->SetHeightFromLastChild();
	poDialog->MoveToCenter( poWinView );					
	BF_GUI_Func_AddChildToMainView( poDialog );	
	
	return true;
}

///////////////////////////////////////////////////////////////////  

BF_GUI_ColorsView::BF_GUI_ColorsView(BRect &o_Rect,
	int i_FollowMode,
	rgb_color *pu_Color)
:BF_GUI_DialogView(o_Rect,"color_rgb",i_FollowMode,B_WILL_DRAW)
{
	puColor = pu_Color;	
	SetViewColor(B_TRANSPARENT_COLOR);
}

void
BF_GUI_ColorsView::Draw(BRect o_Rect)
{
	// fill back //
	SetHighColor(*puColor);
	BRect oRect(Bounds());	
	FillRect(oRect);
}
 
///////////////////////////////////////////////////////////////////  
BF_GUI_ColorView::BF_GUI_ColorView(
	BRect & o_Rect,
	int32 i_FollowMode,
	uint8 *pi_Color,
	int8   i_ColorIndex,
	BF_GUI_ColorsView *po_Unit
):BF_GUI_DialogView(o_Rect,"colors",i_FollowMode,B_WILL_DRAW|B_NAVIGABLE)
{
	ASSERT(po_Unit,"BF_GUI_ColorView::BF_GUI_ColorView");
	poUnit = po_Unit;
	piColor = pi_Color;
	iColorIndex = i_ColorIndex;
	SetFont(&poSysSetup->oFontToolView.oFont);
	SetViewColor(B_TRANSPARENT_COLOR);
}

void					
BF_GUI_ColorView::MouseDown(BPoint point)
{
	if(point.x>255) return;
	DrawCursor(false);
	*piColor = (uint8)point.x;	
	DrawCursor(true);
	
	poUnit->Draw(poUnit->Bounds());
}

void
BF_GUI_ColorView::DrawCursor(bool b_Show)
{
	// draw positon //
	if(b_Show)
		SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_TEXT));
	else
		SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BACK));

	BPoint o1(*piColor-1,1),o2;
	o2.Set(o1.x+3,1);
	StrokeLine(o1,o2);
	o1.x++;
	o1.y++;
	StrokeLine(o1,o2);
	
	o1.Set(*piColor-1,Bounds().Height()-1);
	o2.Set(o1.x+2,o1.y);
	StrokeLine(o1,o2);
	o1.x++;
	o1.y--;
	StrokeLine(o1,o2);
	
	if(b_Show){
		// fill rect with current color //
		BRect oRect(Bounds());
		oRect.left+=259;
		oRect.top+=3;
		oRect.bottom-=3;
		rgb_color oCol={0,0,0};
		if(iColorIndex==0) oCol.red = *piColor;else
		if(iColorIndex==1) oCol.green  = *piColor;else
		if(iColorIndex==2) oCol.blue  = *piColor;
		
		SetHighColor(oCol);
		FillRect(oRect);
	}
}

void
BF_GUI_ColorView::Draw(BRect o_Rect)
{
	// fill back //
	SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BACK));
	BRect oRect(Bounds());	
	FillRect(oRect);		
	
	// draw lines //
	BPoint oPointTop(0,4),oPointBottom(0,oRect.Height()-4);
	rgb_color oCol;
	RGB_SET(oCol,0,0,0);
	for(int32 i=0;i<256;i++){	
		SetHighColor(oCol);
		
		StrokeLine(oPointTop,oPointBottom);
		
		oPointTop.x++;
		oPointBottom.x++;

		if(iColorIndex==0) oCol.red++;else
		if(iColorIndex==1) oCol.green++;else
		if(iColorIndex==2) oCol.blue++;
	}
	DrawCursor(true);
}

///////////////////////////////////////////////////////////////////  

BF_GUI_SetupDialog_ColorItem::BF_GUI_SetupDialog_ColorItem(
	const char *pc_Title,
	const char *pc_Code,					
	rgb_color *pu_Color
):BF_GUI_ViewMenu_Item(pc_Title,pc_Code)
{
	puColor = pu_Color;
}

bool					
BF_GUI_SetupDialog_ColorItem::Invoke(BF_GUI_DlgView_Menu *po_Parent)
{
	//////////////////////////////
	BF_GUI_Func_EnableDialog( po_Parent->Parent()->Name(),false );
	/* prepare message */
	BMessage oMessage(BF_MSG_DIALOG_FOCUS);
	oMessage.AddPointer("bf_focus",po_Parent->Parent());
	/* make dialog */	
	BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(BRect(0,0,400,0),
		sTitle.String(),"color_dialog",oMessage,BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER);	
	/* resize dialog */	
	BRect oRect;	
	/* unit */
	poDialog->LocalBounds(oRect);		
	oRect.left = oRect.right - 40;
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight*6+5*2;
	BF_GUI_ColorsView *poUnit = new BF_GUI_ColorsView(oRect,B_FOLLOW_RIGHT|B_FOLLOW_TOP,puColor);
	poDialog->AddChild(poUnit);
	/* compoments */
	poDialog->LocalBounds(oRect);	
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight*2;	
	oRect.right-=50;
	poDialog->AddChild(new BF_GUI_ColorView(oRect,B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,&puColor->red,0,poUnit));
	oRect.top = oRect.bottom+5;
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight*2;	
	poDialog->AddChild(new BF_GUI_ColorView(oRect,B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,&puColor->green,1,poUnit));
	oRect.top = oRect.bottom+5;
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight*2;	
	poDialog->AddChild(new BF_GUI_ColorView(oRect,B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,&puColor->blue,2,poUnit));
	///////////////////////////////		
	/* menu */
	BRect oRect1;
	poDialog->LocalBounds(oRect1);
	oRect.right = oRect1.right;
	poDialog->AddOkCancelMenu(oRect);		
	/* finish */
	poDialog->SetHeightFromLastChild();
	poDialog->MoveToCenter( poWinView );					
	BF_GUI_Func_AddChildToMainView ( poDialog );	
	
	return true;
}

void					
BF_GUI_SetupDialog_ColorItem::Draw(BF_GUI_DlgView_Menu *po_Parent,BView *po_Render,float f_PosY)
{
	BF_GUI_ViewMenu_Item::Draw(po_Parent,po_Render,f_PosY);
	ASSERT(po_Render,"BF_GUI_SetupDialog_ColorItem::Draw");
	
	// draw color
	BRect oRect(po_Render->Bounds());
	oRect.top += f_PosY+3;
	oRect.bottom = oRect.top + po_Parent->Height()-6;
	
	oRect.left = oRect.right-50;
	po_Render->SetHighColor(*puColor);
	po_Render->FillRect(oRect);
	
	// draw rgb_color
	if(!poSIcon){
		rgb_color 	oCol;
		BRect 		oRect(po_Render->Bounds());
		oRect.left+=10;
		oRect.top += f_PosY+3;
		oRect.bottom = oRect.top + po_Parent->Height()-6;		
		
		RGB_SET(oCol,255,0,0);
		oRect.left+=2;			
		oRect.right = oRect.left+3;
		po_Render->SetHighColor(oCol);
		po_Render->FillRect(oRect);
		
		RGB_SET(oCol,0,255,0);
		oRect.left+=2;			
		oRect.right = oRect.left+3;
		po_Render->SetHighColor(oCol);
		po_Render->FillRect(oRect);
		
		RGB_SET(oCol,0,0,255);
		oRect.left+=2;			
		oRect.right = oRect.left+3;
		po_Render->SetHighColor(oCol);
		po_Render->FillRect(oRect);
		
	}
}
///////////////////////////////////////////////////////////////////
  
BF_GUI_SetupDialog::BF_GUI_SetupDialog(BRect & o_Rect,BView *po_View)
:BF_GUI_Dialog(o_Rect,BF_DictAt(BF_DICT_MAINSETUP),"",BMessage(),BG_GUI_DIALOG_WINRESIZE_RESIZE_ALL)
{
	poSysSetup->CopyTo(oSetup);
	//
	oMessage.what = BF_MSG_MAINVIEW_MAINSETUP_CLOSE;
	oMessage.AddPointer("bf_focus",po_View);		
	/* make left menu*/		
	BL_List *ploLeftList = new BL_List();
	ploLeftList->AddItem(new BF_GUI_ViewMenu_Item(BF_DictAt(BF_DICT_MAINSETUP_COLSFONTS),"colors_fonts"));
	ploLeftList->AddItem(new BF_GUI_ViewMenu_Item(BF_DictAt(BF_DICT_MAINSETUP),"flags_main"));
	ploLeftList->AddItem(new BF_GUI_ViewMenu_Item(BF_DictAt(BF_DICT_MAINSETUP_FILES),"filetasks_copy"));
	ploLeftList->AddItem(new BF_GUI_ViewMenu_Item(BF_DictAt(BF_DICT_MAINSETUP_DEBUG),"flags_debug"));

	BRect oRect;	
	LocalBounds(oRect);	
	oRect.bottom -= oSetup.oFontToolView.fHeight*2;
	poMenu = new BF_GUI_DlgView_VMenu(oRect,"main_menu",
		B_FOLLOW_LEFT|B_FOLLOW_TOP_BOTTOM,ploLeftList,0);
	AddChild(poMenu);
	
	/* make bottom menu */
	LocalBounds(oRect);
	oRect.top = oRect.bottom - oSetup.oFontToolView.fHeight;
	BL_List *ploMenu = new BL_List();
	ploMenu->AddItem(new BF_GUI_ViewMenu_Item(BF_DictAt(BF_DICT_OK),"ok"));
	AddMenu(oRect,ploMenu,true);		

	// load icons //	
	poFontSIcon = BL_Load_SIconFromMIME(BL_MIME_FONT);
}

BF_GUI_SetupDialog::~BF_GUI_SetupDialog()
{
	if(poFontSIcon) DELETE(poFontSIcon);
}
 
void
BF_GUI_SetupDialog::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what){
	case BF_MSG_MAINVIEW_MAINSETUP_STYLES_UPDATED:{
		EnableDialog(true);		
		poMenu->MakeFocus();
		//
		int32 iMenuRes=-1;
		if(B_OK==po_Message->FindInt32("menu",&iMenuRes)) oSetup.SetMainStyle(iSetupMainStyle);
		//
		break;}
	case BF_MSG_MAINVIEW_MAINSETUP_0:{
		EnableDialog(true);		
		poMenu->MakeFocus();
		break;}
	case BF_MSG_DIALOG_PRESSED_OK:{
		BView 	*po;
		const char 	*pc;
		ASSERT(B_OK==po_Message->FindPointer("bf_DlgView_Focus",(void**)&po),"BF_GUI_SetupDialog::MessageReceived");
		pc = po->Name();
		if(strcmp(pc,"main_menu")==0){			
			InvokeMenu(-1,poMenu->iNavCursorIndex);
		}else{			
			//
			poSysSetup->UpdateFrom(oSetup);			
			if(poSysSetup->MainStyle() & BF_SETUP_AUTOSAVE) poSysSetup->Save();
			//
			BF_GUI_Dialog::MessageReceived(po_Message);
			//
			BMessenger oMessenger(poWin);
			BMessage   oMessage(BF_MSG_SETUP_UPDATED);
			oMessenger.SendMessage(&oMessage);			
		}
		break;}
	default:
		BF_GUI_Dialog::MessageReceived(po_Message);
	};
} 

void
BF_GUI_SetupDialog::Insert_MainStyle_Check(const char *pc_Title,int i_Style,BRect & o_Rect,BView *po_Dialog,bool b_FirstView)
{
	if(!b_FirstView){
		o_Rect.top = o_Rect.bottom+5;
		o_Rect.bottom = o_Rect.top+poSysSetup->oFontToolView.fHeight;
	}else{
		o_Rect.bottom = o_Rect.top+poSysSetup->oFontToolView.fHeight;
	}
	BF_GUI_ViewCheck_Style *poCheck = new BF_GUI_ViewCheck_Style(o_Rect,pc_Title,pc_Title,
		&iSetupMainStyle,i_Style,B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);	
	po_Dialog->AddChild(poCheck);
}

 
void
BF_GUI_SetupDialog::InvokeMenu(int32 i_OldCursor,int32 i_NewCursor)
{
	switch(i_NewCursor){
	case 0:{ // colors_fonts 
		Invoke_ColorsSetup();
		break;}
	case 1:{ // main_flags
		Invoke_FlagsSetup(BF_GUI_SETUPDIALOG_FLAGS_MAIN);	
		break;}		
	case 2:{ // filetasks_flags
		Invoke_FlagsSetup(BF_GUI_SETUPDIALOG_FLAGS_FILETASKS);	
		break;}		
	case 3:{ // debug_flags
		Invoke_FlagsSetup(BF_GUI_SETUPDIALOG_FLAGS_DEBUG);	
		break;}
	}
	/*
	//////////////////////////// set new right views/////////
	switch(i_NewCursor){
	case 0:{ // colors 		
		
		loRightView.AddItem(poColorsMenu);
		break;}
	}*/
}

void
BF_GUI_SetupDialog::Invoke_ColorsSetup()
{
	EnableDialog(false);
	/* prepare message */
	BMessage oMessage(BF_MSG_MAINVIEW_MAINSETUP_0);
	oMessage.AddPointer("bf_focus",this);
	/* make dialog */	
	BRect oRect(poWinView->Bounds());
	oRect.left=0;
	oRect.right=400;
	oRect.top+=70;
	oRect.bottom-=70;
	/* make dialog */	
	BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(oRect,BF_DictAt(BF_DICT_MAINSETUP_COLSFONTS),"colors_dialog"
		,oMessage,BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER);

	/* insert colors */		
	BL_List *ploItem = new BL_List();	
	for(int iColor=0;iColor<BF_COLOR_END;iColor++){
		ploItem->AddItem(new BF_GUI_SetupDialog_ColorItem(
			BF_DictAt(poSysSetup->aoColor[iColor].iDictCode),"",&oSetup.aoColor[iColor].rgb));
	}
	
	ploItem->AddItem(new BF_GUI_ViewMenu_Item("",""));
	/* insert fonts */		
	ploItem->AddItem(new BF_GUI_SetupDialog_FontItem(BF_DictAt(BF_DICT_FONTSETUP_FONTPANEL),"back",&oSetup.oFontNode,poFontSIcon?new BBitmap(poFontSIcon):NULL));
	ploItem->AddItem(new BF_GUI_SetupDialog_FontItem(BF_DictAt(BF_DICT_FONTSETUP_FONTDIALOG),"frame",&oSetup.oFontToolView,poFontSIcon?new BBitmap(poFontSIcon):NULL));
	ploItem->AddItem(new BF_GUI_SetupDialog_FontItem(BF_DictAt(BF_DICT_FONTSETUP_FONTMONO),"dir",&oSetup.oFontMono,poFontSIcon?new BBitmap(poFontSIcon):NULL));
	ploItem->AddItem(new BF_GUI_SetupDialog_FontItem(BF_DictAt(BF_DICT_FONTSETUP_FONTCOMMAND),"dir",&oSetup.oFontCMDLine,poFontSIcon?new BBitmap(poFontSIcon):NULL));
	/*-------*/
	poDialog->LocalBounds(oRect);	
	oRect.bottom -= +oSetup.oFontToolView.fHeight*2;
	

	BF_GUI_DlgView_VMenu* poColorsMenu = new BF_GUI_DlgView_VMenu(oRect,"colors_menu",
		B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP_BOTTOM,ploItem,BF_GUI_DLGVIEW_VMENU_SICON|BF_GUI_DLGVIEW_VMENU_SET_VSCROLLBAR);		
	poDialog->AddChild(poColorsMenu);	
	
	/* menu ok */	
	BL_List *ploMenu = new BL_List();
	ploMenu->AddItem(new BF_GUI_ViewMenu_Item(BF_DictAt(BF_DICT_OK),"ok"));
	poDialog->AddMenu(oRect,ploMenu);		
	
	/*
	// add scroll_bar
	BF_GUI_VScrollBar *poBar = new BF_GUI_VScrollBar(poColorsMenu,"scroll",B_FOLLOW_TOP_BOTTOM|B_FOLLOW_RIGHT);
	poColorsMenu->SetVScroll(poBar);
	poDialog->AddChild(poBar);			
	*/
	
	
	/* finish */
	//poDialog->SetHeightFromLastChild();
	poDialog->MoveToCenter( poWinView );					
	BF_GUI_Func_AddChildToMainView ( poDialog );	
}

void
BF_GUI_SetupDialog::Invoke_FlagsSetup(int i_Type)
{
	iSetupMainStyle = oSetup.MainStyle();

	EnableDialog(false);
	/* prepare message */
	BMessage oMessage(BF_MSG_MAINVIEW_MAINSETUP_STYLES_UPDATED);
	oMessage.AddPointer("bf_focus",this);
	/* make dialog */	
	BRect oRect(poWinView->Bounds());
	oRect.left=0;
	oRect.right=350;
	oRect.top+=30;
	oRect.bottom-=30;
	/* make dialog */	
	BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(oRect,BF_DictAt(BF_DICT_FONTSETUP_FLAGS),"flags_dialog",
		oMessage,BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER);
			
	/* flags */
	poDialog->LocalBounds(oRect);	
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	////
	switch(i_Type){
	case BF_GUI_SETUPDIALOG_FLAGS_MAIN:{			
		Insert_MainStyle_Check(BF_DictAt(BF_DICT_MAINSETUP_ASKEXIT),BF_SETUP_MAIN_ASK_EXIT,oRect,poDialog,true);
		Insert_MainStyle_Check(BF_DictAt(BF_DICT_MAINSETUP_VOLSHOWHOME),BF_SETUP_MAIN_SELECTVOL_SHOWSPEC,oRect,poDialog,false);
		Insert_MainStyle_Check(BF_DictAt(BF_DICT_MAINSETUP_VOLSTACK),BF_SETUP_MAIN_SELECTVOL_SHOWSTACK,oRect,poDialog,false);
		Insert_MainStyle_Check(BF_DictAt(BF_DICT_MAINSETUP_VOLICONS),BF_SETUP_MAIN_SELECTVOL_SHOWSICON,oRect,poDialog,false);
		Insert_MainStyle_Check(BF_DictAt(BF_DICT_MAINSETUP_PATHICON),BF_SETUP_MAIN_FP_HEADERICON,oRect,poDialog,false);	
		Insert_MainStyle_Check(BF_DictAt(BF_DICT_MAINSETUP_PANBACK),BF_SETUP_NODEPANEL_USE_BACKSPACE,oRect,poDialog,false);	
		Insert_MainStyle_Check(BF_DictAt(BF_DICT_MAINSETUP_PANRET),BF_SETUP_MAIN_RETURN_REMEMBER,oRect,poDialog,false);		 		
		Insert_MainStyle_Check(BF_DictAt(BF_DICT_MAINSETUP_CMDPATH),BF_SETUP_CMDLINE_SHOWPATH,oRect,poDialog,false);	
		Insert_MainStyle_Check(BF_DictAt(BF_DICT_MAINSETUP_TRACKADDICONS),BF_SETUP_FILESPANEL_TRACKERADDONS_SHOWICONS,oRect,poDialog,false);		
		Insert_MainStyle_Check(BF_DictAt(BF_DICT_MAINSETUP_SEARCHVOLICONS),BF_SETUP_SEARCH_SETUP_VOLICONS,oRect,poDialog,false);		
		Insert_MainStyle_Check(BF_DictAt(BF_DICT_MAINSETUP_AUTOSAVE),BF_SETUP_AUTOSAVE,oRect,poDialog,false);			
		break;}
	case BF_GUI_SETUPDIALOG_FLAGS_FILETASKS:{
		Insert_MainStyle_Check(BF_DictAt(BF_DICT_SETUPFILES_SHOWDLG),BF_SETUP_OPERROSTER_DEFDIALOG,oRect,poDialog,true);
		Insert_MainStyle_Check(BF_DictAt(BF_DICT_SETUPFILES_COPYDES),BF_SETUP_NODEPANEL_DESELECT_AFTER_COPY,oRect,poDialog,false);
		Insert_MainStyle_Check(BF_DictAt(BF_DICT_SETUPFILES_MOVEDES),BF_SETUP_NODEPANEL_DESELECT_AFTER_MOVE,oRect,poDialog,false);
		Insert_MainStyle_Check(BF_DictAt(BF_DICT_SETUPFILES_ASKDEL),BF_SETUP_MAIN_ASK_DELETE,oRect,poDialog,false);
		Insert_MainStyle_Check(BF_DictAt(BF_DICT_SETUPFILES_ASKCOPY),BF_SETUP_MAIN_ASK_COPY,oRect,poDialog,false);	
		break;}		
	case BF_GUI_SETUPDIALOG_FLAGS_DEBUG:{
		Insert_MainStyle_Check("Debug : disable node_monitoring",BF_SETUP_DEB_NOMONITORING,oRect,poDialog,true);
		Insert_MainStyle_Check("Debug : disable filelist_sorting",BF_SETUP_DEB_NOSORTING,oRect,poDialog,false);					
		break;}
	}
	/* menu ok */	
	BL_List *ploMenu = new BL_List();
	ploMenu->AddItem(new BF_GUI_ViewMenu_Item(BF_DictAt(BF_DICT_OK),"ok"));
	poDialog->AddMenu(oRect,ploMenu);		
	
	/* finish */
	//poDialog->SetHeightFromLastChild();
	poDialog->MoveToCenter( poWinView );					
	BF_GUI_Func_AddChildToMainView ( poDialog );
}
///////////////////////////////////////////////////////////////////
void
BF_GUI_SetupDialog_Run(BView *po_View)
{
	/* */
	BF_GUI_Func_PanelsEnable(false);
	/* make dialog */	
	BRect oRect(poWinView->Bounds());
	oRect.left=0;
	oRect.right=200;
	oRect.top+=40;
	oRect.bottom-=40;
	
	BF_GUI_SetupDialog *poDialog = new BF_GUI_SetupDialog(oRect,po_View);
	/* finish */
	//poDialog->SetHeightFromLastChild();
	poDialog->MoveToCenter( poWinView );					
	BF_GUI_Func_AddChildToMainView ( poDialog );	
}