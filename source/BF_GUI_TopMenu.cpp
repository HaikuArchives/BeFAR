#include <stdio.h>

#include "BF_Dict.h"
#include "BF_GUI_TopMenu.h"
#include "BF_GUI_WinMain.h"
#include "BF_GUI_Func.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "TopMenu"

/////////////////////////////////////////////////////////////////////
BF_GUI_TopMenu* poTopMenu = NULL;
/////////////////////////////////////////////////////////////////////

BF_GUI_TopMenu_HItem::BF_GUI_TopMenu_HItem(
	const char *pc_Title,
	BBitmap *po_SIcon)
:BF_GUI_ViewMenu_Item(pc_Title,"",po_SIcon)
{

}

void
BF_GUI_TopMenu_HItem::AddHItem(
	const char *pc_Title,
	int32	i_MessageWhat,
	BView   *po_Owner,
	const char *pc_KeyLabel)
{
	BMessage *poMessage = new BMessage(i_MessageWhat);
	poMessage->AddPointer("bf_view",po_Owner);
	AddHItem(pc_Title,poMessage,pc_KeyLabel);
}

void					
BF_GUI_TopMenu_HItem::AddHItem(
	const char *pc_Title,
	BMessage *po_Message,
	const char *pc_KeyLabel)
{
	BF_GUI_TopMenu_VItem *po = new BF_GUI_TopMenu_VItem(pc_Title,po_Message,pc_KeyLabel);
	loVItem.AddItem(po);
}
/////////////////////////////////////////////////////////////////////
BF_GUI_TopMenu_VItem::BF_GUI_TopMenu_VItem(
	const char *pc_Title,
	BMessage *po_Message, 
	const char *pc_KeyLabel
):BF_GUI_ViewMenu_Item(pc_Title,"",NULL)
{
	sKeyLabel = pc_KeyLabel;
	poMessage = po_Message;
}

BF_GUI_TopMenu_VItem::~BF_GUI_TopMenu_VItem()
{
	if(poMessage) DELETE(poMessage);
}

float	
BF_GUI_TopMenu_VItem::CalcWidth(BF_GUI_Setup_Font * po_Font)
{	
	BF_GUI_ViewMenu_Item::CalcWidth(po_Font);
	fWidth += 20;
	fKeyWidth = po_Font->oFont.StringWidth(sKeyLabel.String());	
	fWidth += fKeyWidth;
	
	return fWidth;
}
/////////////////////////////////////////////////////////////////////
BF_GUI_TopVMenu::BF_GUI_TopVMenu(BRect & o_Rect,BL_List *plo_Menu,BF_GUI_TopMenu *po_HMenu)
:BF_GUI_DlgView_VMenu(o_Rect,"",B_FOLLOW_ALL,plo_Menu,BF_GUI_DLGVIEW_VMENU_DEFAULT)
{
	bDestroyMenu = false;
	ASSERT(po_HMenu && plo_Menu);
	poHMenu = po_HMenu;
}

void
BF_GUI_TopVMenu::NavEnter()
{
	BF_GUI_ViewMenu_Item *poItem = (BF_GUI_ViewMenu_Item*)ploMenu->ItemAt(iNavCursorIndex);
	if(!poItem) return;
	//

	BMessenger 	oMessenger(poHMenu);
	BMessage 	oMessage(BF_MSG_TOPMENU_HCLOSE);
	oMessage.AddBool("bf_bOk",true);
	oMessage.AddString("bf_Code",poItem->sCode.String());
	oMessenger.SendMessage(&oMessage);		
}

void
BF_GUI_TopVMenu::NavEscape()
{	
	BMessenger 	oMessenger(poHMenu);
	BMessage 	oMessage(BF_MSG_TOPMENU_HCLOSE);
	oMessage.AddBool("bf_bOk",false);
	oMessenger.SendMessage(&oMessage);		
}

void					
BF_GUI_TopVMenu::DrawItem(BView *po_Render,int32 i_Index,bool b_ReqDrawBack)
{
	BF_GUI_DlgView_VMenu::DrawItem(po_Render,i_Index,b_ReqDrawBack);
	////
	BF_GUI_TopMenu_VItem *po;
	po = (BF_GUI_TopMenu_VItem*)ploMenu->ItemAt(i_Index);
	ASSERT(po);

	if(po->sTitle!="" && po->sKeyLabel!=""){
		BPoint 	oPoint(0,0);		
		BRect 	oRect(po_Render->Bounds());
		oPoint.y += ((float)(i_Index-iFirstIndex)) * Height();		
		oPoint.x = oRect.Width()-po->fKeyWidth;
		oPoint.y += poFont->fAscent;
		po_Render->DrawString(po->sKeyLabel.String(),oPoint);	
	}
}

void
BF_GUI_TopVMenu::KeyDown(const char *bytes, int32 numBytes)
{
	/* handle keys */
	if(numBytes==1 && (bytes[0]==B_RIGHT_ARROW || bytes[0]==B_LEFT_ARROW)){
		BMessage oMsg(BF_MSG_TOPMENU_HCLOSE);
		oMsg.AddInt8("key",(const int8)bytes[0]);
		BMessenger oMessenger(poHMenu);
		oMessenger.SendMessage(&oMsg);
	}else
		BF_GUI_DlgView_VMenu::KeyDown(bytes,numBytes);
}											
/////////////////////////////////////////////////////////////////////

BF_GUI_TopMenu::BF_GUI_TopMenu(
	const BRect &o_Rect,
	BL_List 	*plo_HItem	)
:BF_GUI_DlgView_HMenu(o_Rect,"top_menu",BG_GUI_DIALOG_WINRESIZE_RESIZE_ALL,plo_HItem)
{
	poTopMenu = this;	
	poParent = NULL;
	poVMenu = NULL;
	bItemsInCenter = false;	
	bHideCursorOnDefocus = false;
	iHCursor = -1;
}

void
BF_GUI_TopMenu::Invoke(bool b_Ok)
{
	if(poVMenu) return;
	//
	BF_GUI_Func_PanelsEnable(true);
	//
	poParent->MakeFocus();
	//
	if(b_Ok){
		BF_GUI_TopMenu_HItem *poHItem = (BF_GUI_TopMenu_HItem*)ploMenu->ItemAt(iNavCursorIndex);
		ASSERT(poHItem);
		BF_GUI_TopMenu_VItem *poVItem = (BF_GUI_TopMenu_VItem*)poHItem->loVItem.ItemAt(iHCursor);
		ASSERT(poVItem);
		//	
		if(poVItem->poMessage){
			BView *poView = NULL;	
			ASSERT(B_OK==poVItem->poMessage->FindPointer("bf_view",(void**)&poView) && poView);
			BMessenger 	oMessenger(poView);	
			oMessenger.SendMessage(poVItem->poMessage);
		}
	}	
	Parent()->RemoveChild(this);				
	delete this;	
}

void
BF_GUI_TopMenu::NavEnter()
{	
	Invoke(true);
}

void
BF_GUI_TopMenu::NavEscape()
{	
	Invoke(false);
}

void
BF_GUI_TopMenu::MakeFocus(bool b_Focused)
{
	if(!b_Focused && !poVMenu){
		Hide();
	}
	BF_GUI_DlgView_HMenu::MakeFocus(b_Focused);
}

void
BF_GUI_TopMenu::NavChangeCursor(int32 i_NewCursor,int i_DirectMove,bool b_Redraw)
{	
	CloseVMenu();
	BF_GUI_DlgView_HMenu::NavChangeCursor(i_NewCursor,i_DirectMove,b_Redraw);
	if(IsHidden()) return;
	OpenVMenu();
}

void
BF_GUI_TopMenu::Show(void)
{	
	BF_GUI_DlgView_HMenu::Show();
	OpenVMenu();	
}

void
BF_GUI_TopMenu::OpenVMenu(bool b_FocusHMenu)
{	
	if(poVMenu) return;
	printf("OpenVMenu()\n");
	//
	BF_GUI_TopMenu_HItem *poHItem = (BF_GUI_TopMenu_HItem*)ploMenu->ItemAt( iNavCursorIndex );
	if(!poHItem) return;
	//
	BView *poParent = Parent();
	ASSERT(poParent);
	//
	BRect oRect(CalcItemXPos(iNavCursorIndex),Frame().bottom+1,0,0);
	//
	poVMenuPanel = new BF_GUI_DlgPanel(oRect,"","dlg",B_FOLLOW_NONE,0,false);
	poVMenuPanel->SetSizeBy(
		BF_GUI_ViewMenu_CalcMaxWidth(&poHItem->loVItem,&poSysSetup->oFontToolView,false),
		5+poSysSetup->oFontToolView.fHeight*(poHItem->loVItem.CountItems() ) 
	);
	poVMenuPanel->LocalBounds(oRect);
	//
	poVMenu = new BF_GUI_TopVMenu(oRect,&poHItem->loVItem,this);	
	//
	poVMenuPanel->AddChild(poVMenu);
	//
	poParent->AddChild(poVMenuPanel);
	if(b_FocusHMenu) poVMenu->MakeFocus();
}

void
BF_GUI_TopMenu::CloseVMenu()
{
	if(!poVMenu) return;
	poVMenuPanel->Hide();
	Parent()->RemoveChild(poVMenuPanel);				
	DELETE(poVMenuPanel);
	poVMenuPanel = NULL;
	poVMenu = NULL;
}

void
BF_GUI_TopMenu::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what){
	case BF_MSG_TOPMENU_HCLOSE:
		MakeFocus(true);
		ASSERT(poVMenu);
		iHCursor = poVMenu->iNavCursorIndex;
		CloseVMenu();
		
		bool bExit;
		if(B_OK==po_Message->FindBool("bf_bOk",&bExit)){
			if(bExit) NavEnter(); else NavEscape();
		}else{		
			char cKey;
			ASSERT(B_OK == po_Message->FindInt8("key",(int8*)&cKey));
			switch(cKey){
			case B_RIGHT_ARROW:
				NavChangeCursor(iNavCursorIndex+1,BF_GUI_TOOLMENU_MOVE_DOWN);
				break;
			case B_LEFT_ARROW:
				NavChangeCursor(iNavCursorIndex-1,BF_GUI_TOOLMENU_MOVE_UP);
				break;
			default:
				break;
			}
		}		
		break;
	default:
		BF_GUI_DlgView_HMenu::MessageReceived(po_Message);
	}
}
/////////////////////////////////////////////////////////////////////

void 
BF_GUI_TopMenu_Run()
{
	BL_List *poList = new BL_List();	
	BF_GUI_TopMenu_HItem	*poHItem;
	BF_GUI_Panel			*poPanel;
	
	//////	
	poPanel = poWinView->PanelOnTop(true);
	if(poPanel){		
		poHItem = new BF_GUI_TopMenu_HItem(B_TRANSLATE(BF_DICT_TOPMENU_LEFT));	
		poPanel->PrepareTopMenuItem(poHItem);
		poList->AddItem(poHItem);
	}
	/////
	poHItem = new BF_GUI_TopMenu_HItem(B_TRANSLATE(BF_DICT_TOPMENU_COMMON));		
	poHItem->AddHItem(B_TRANSLATE(BF_DICT_TOPMENU_COMMON_MAINSETUP),BF_MSG_MAINVIEW_MAINSETUP_0,poWinView,"");	
	poHItem->AddHItem(B_TRANSLATE(BF_DICT_TOPMENU_COMMON_EMPTY_TRASH),BF_MSG_MAINVIEW_EMPTY_TRASH,poWinView,"");	
	poHItem->AddHItem(B_TRANSLATE(BF_DICT_TOPMENU_COMMON_COMPARE),BF_MSG_MAINVIEW_COMPART_PANELS,poWinView,"");		
	poHItem->AddHItem("",0,poWinView,"");		
	poHItem->AddHItem(B_TRANSLATE(BF_DICT_TOPMENU_COMMON_SAVE_SETUP),BF_MSG_MAINVIEW_MAINSETUP_SAVE,poWinView,"");
	poHItem->AddHItem(B_TRANSLATE(BF_DICT_TOPMENU_COMMON_LOAD_SETUP),BF_MSG_MAINVIEW_MAINSETUP_LOAD,poWinView,"");	
	poHItem->AddHItem(B_TRANSLATE(BF_DICT_TOPMENU_COMMON_DEFAULT_SETUP),BF_MSG_MAINVIEW_MAINSETUP_SETDEFAULT,poWinView,"");				
	poHItem->AddHItem(B_TRANSLATE(BF_DICT_TOPMENU_COMMON_SAVE_PALETTE),BF_MSG_MAINVIEW_SAVE_PALLETE,poWinView,"");			
	poHItem->AddHItem(B_TRANSLATE(BF_DICT_TOPMENU_COMMON_LOAD_PALETTE),BF_MSG_MAINVIEW_LOAD_PALLETE,poWinView,"");
	poHItem->AddHItem("",0,poWinView,"");
	poHItem->AddHItem(B_TRANSLATE(BF_DICT_TOPMENU_COMMON_SELECTDICT),BF_MSG_MAINVIEW_SELECTDICT,poWinView,"");	
	poHItem->AddHItem("",0,poWinView,"");			
	poHItem->AddHItem(B_TRANSLATE(BF_DICT_TOPMENU_COMMON_ABOUT),BF_MSG_MAINVIEW_ABOUT,poWinView,"");				
	poHItem->AddHItem("",0,poWinView,"");		
	poHItem->AddHItem(B_TRANSLATE(BF_DICT_TOPMENU_COMMON_QUIT),BF_MSG_MAINVIEW_ASK_QUIT,poWinView,"Alt-W,Alt+Q");				
	poList->AddItem(poHItem);	
	///
	poPanel = poWinView->PanelOnTop(false);	
	if(poPanel){		
		poHItem = new BF_GUI_TopMenu_HItem(B_TRANSLATE(BF_DICT_TOPMENU_RIGHT));	
		poPanel->PrepareTopMenuItem(poHItem);
		poList->AddItem(poHItem);
	}			
	/////////////////////////////////////////////

	BRect oRect(poWinView->Bounds());
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	BF_GUI_TopMenu *poMenu = new BF_GUI_TopMenu(oRect,poList);			
	poMenu->poParent = poWin->CurrentFocus();
	
	BF_GUI_Func_PanelsEnable(false);
	
	BF_GUI_Func_AddChildToMainView ( poMenu );
	//poMenu->MakeFocus();	
	
}
//////////////////////////////////////////////////////////////////////////
