#include <stdio.h>
#include "BF_GUI_CmdLine.h"
#include "BF_Dict.h"
#include "BF_Roster.h"
#include "BF_GUI_Func.h"
#include "BF_GUI_WinMain.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "CmdLine"

BF_GUI_CmdLine *poCmdLine = NULL;

BF_GUI_CmdLine::BF_GUI_CmdLine(const BRect & o_Rect)
:BView(o_Rect,"cmd_line",B_FOLLOW_LEFT_RIGHT|B_FOLLOW_BOTTOM,B_WILL_DRAW|B_FULL_UPDATE_ON_RESIZE|B_FRAME_EVENTS)
{	
	iDisableCount = 0;
	//
	SetViewColor(B_TRANSPARENT_COLOR);	
	SetFont(&poSysSetup->oFontCMDLine.oFont);
	//
	poEdit = new BF_GUI_ViewEdit(Bounds(),"cmd_edit","",B_FOLLOW_LEFT_RIGHT|B_FOLLOW_BOTTOM,B_WILL_DRAW|B_FULL_UPDATE_ON_RESIZE|B_FRAME_EVENTS);
	poEdit->bParentCall = false;
	poEdit->oColBack = SYS_COLOR(BF_COLOR_CMDLINE_BACK);
	poEdit->oColSel = SYS_COLOR(BF_COLOR_CMDLINE_TEXT);
	poEdit->Enable(false);
	poEdit->bAlwaysCursor = true;
	poEdit->bCursorActive = true;
	poEdit->bSelected = false;
	poEdit->bSysKeysHandle = false;
	poEdit->SetSetupFont(&poSysSetup->oFontCMDLine);
	AddChild(poEdit);
	
	poCmdLine = this;
	SetValues("","");	
		
}

void					
BF_GUI_CmdLine::SetValues(const char *pc_Path,const char *pc_Cmd,bool b_Redraw)
{
	if(pc_Path){
		sPath = pc_Path;
		if(poSysSetup->MainStyle() & BF_SETUP_CMDLINE_SHOWPATH) sVisPath = sPath; else sVisPath="";
		sVisPath<<">";
		if(pc_Cmd) sCmd = pc_Cmd;
		
		// resize edit //
		fWidth = poSysSetup->oFontCMDLine.oFont.StringWidth(sVisPath.String())+2;
		BRect oRect(Bounds());
		oRect.left = fWidth;
		poEdit->ResizeTo(oRect.Width(),oRect.Height());
		poEdit->MoveTo(oRect.left,0);	
		
		if(b_Redraw) Invalidate(Bounds());
	}
	if(pc_Cmd){
		poEdit->SetText(pc_Cmd,true);
	}
}
void					
BF_GUI_CmdLine::Draw(BRect o_Rect)
{
	BRect oRect;

	
	/* draw back */
	oRect = Bounds();
	oRect.right = fWidth-1;
	SetHighColor(SYS_COLOR(BF_COLOR_CMDLINE_BACK));
	FillRect(oRect);
	/* draw text */	
	SetHighColor(SYS_COLOR(BF_COLOR_CMDLINE_TEXT));
	SetLowColor(SYS_COLOR(BF_COLOR_CMDLINE_BACK));
	MovePenTo(0,oRect.Height()-poSysSetup->oFontCMDLine.fDescent);
	DrawString(sVisPath.String());			
	
}


bool					
BF_GUI_CmdLine::OnKeyDown(const char *bytes, int32 numBytes)
{
	if(numBytes==1 && bytes[0]==B_ENTER){
		if(poEdit->sValue=="") return false;
		Action_Enter();
		return true;
	}else
	if(numBytes==1 && bytes[0]==B_ESCAPE){
		if(poEdit->sValue=="") return false;
		poEdit->SetText("");		
		return true;
	}else
	if(numBytes==1 && bytes[0]==B_UP_ARROW){
		uint32 	iKeysModifiers = modifiers();
		if(iKeysModifiers & B_OPTION_KEY){			
			if(lsCommand.CountItems()==0) return true;
			BL_String *ps = (BL_String*)lsCommand.ItemAt(lsCommand.CountItems()-1);
			if(!ps) return true;
			poEdit->SetText(ps->String(),true);
			return true;
		}		
	}else
	if(numBytes==1 && bytes[0]==B_DOWN_ARROW){
		uint32 	iKeysModifiers = modifiers();
		if(iKeysModifiers & B_OPTION_KEY){									
			ShowHistory();
			return true;
		}		
	}else
	{}
	///
	return poEdit->OnKeyDown(bytes,numBytes);
}

#define DLG_TITLE "Commands"

void 					
BF_GUI_CmdLine::ShowHistory()
{
	if(lsCommand.CountItems()==0){
		BF_Dialog_Alert_Sep(B_TRANSLATE(BF_DICT_WARNING),B_TRANSLATE(BF_DICT_PATH_EMPTY),NULL,poWin->CurrentFocus());
		return;
	}
	
	BL_List *ploMenu = new BL_List();
	BF_GUI_ViewMenu_Item *po=NULL;
	
	
	BF_GUI_Func_PanelsEnable(false);
	
	// make menu //
	BL_String *ps=NULL;
	for(int iItem=0;iItem<lsCommand.CountItems();iItem++){
		ps = (BL_String*)lsCommand.ItemAt(iItem);
		po = new BF_GUI_ViewMenu_Item(ps->String(),ps->String());
		ploMenu->AddItem(po);
	}
	
	/// prepare message for dialog //
	BMessage oMessage(BF_MSG_CMDLINE_HISTORY_SELECTED);			
	oMessage.AddPointer("bf_focus",(void*)this);
	oMessage.AddPointer("bf_panel_focus",(void*)poWin->CurrentFocus());
	
	
	/* calc max_item  width */
	float fMaxWidth=0;
	{
		BF_GUI_ViewMenu_Item *po;
		float f;
		for(int i=0;i<ploMenu->CountItems();i++){
			po = (BF_GUI_ViewMenu_Item*)ploMenu->ItemAt(i);			
			ASSERT(po);
			f = poSysSetup->oFontToolView.oFont.StringWidth(po->sTitle.String());
			if(f>fMaxWidth) fMaxWidth = f;
		}
	}	
	/* check width */
	{
		float fTitleWidth = poSysSetup->oFontToolView.oFont.StringWidth(DLG_TITLE)+40;
		if(fTitleWidth>fMaxWidth) fMaxWidth = fTitleWidth;
	}
	/* prepare rect for dialog */
	BRect oDlgRect(0,0,300,0);	
	/* make dialog */	
	BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(oDlgRect,DLG_TITLE,"dialog",oMessage,
		BG_GUI_DIALOG_WINRESIZE_RESIZE_ALL,true);	
	/**/
	float fHeight = poSysSetup->oFontToolView.fHeight;
	
	BRect oMenuRect(0,0,0,0);
	poDialog->LocalBounds(oMenuRect);
	oMenuRect.right = oMenuRect.left + fMaxWidth;
	oMenuRect.bottom = oMenuRect.top + 5 + fHeight*ploMenu->CountItems();
		
	/* make menu */	
	BView* poMenu = new BF_GUI_DlgView_VMenu(oMenuRect,"menu",	B_FOLLOW_ALL,ploMenu,0);	
	/* finish */
	poDialog->AddChild(poMenu);	
	poDialog->SetSizeBy(oMenuRect.Width(),oMenuRect.Height());
	poDialog->MoveToCenter(poWinView);
	BF_GUI_Func_AddChildToMainView(poDialog);		
	/* */	
	return;			
}

#undef DLG_TITLE

void 					
BF_GUI_CmdLine::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what){
	case BF_MSG_CMDLINE_HISTORY_SELECTED:{
		BView *poView=NULL;
		ASSERT(B_OK==po_Message->FindPointer("bf_panel_focus",(void**)&poView) && poView);
		
		BF_GUI_Func_PanelsEnable(true);
		poView->MakeFocus();
		
		const char *pcCommand=NULL;
		if(B_OK!=po_Message->FindString("menu_code",&pcCommand) || !pcCommand) return;
		
		poEdit->SetText(pcCommand,true);
				
		
		break;}
	case BF_MSG_SETUP_UPDATED:
			
		if(!(poSysSetup->MainStyle() & BF_SETUP_CMDLINE_SHOWPATH)) sVisPath = ">";
	
		poEdit->oColBack = SYS_COLOR(BF_COLOR_CMDLINE_BACK);
		poEdit->oColSel = SYS_COLOR(BF_COLOR_CMDLINE_TEXT);
		poEdit->Draw(poEdit->Bounds());
		Draw(Bounds());				
		break;
	default:
		BView::MessageReceived(po_Message);
	}	
}

void
BF_GUI_CmdLine::Enable(bool b_Enable)
{	
	iDisableCount += !b_Enable?+1:-1;

	if(iDisableCount==0) poEdit->bCursorActive = true;
	if(iDisableCount==1) poEdit->bCursorActive = false;	
}

void					
BF_GUI_CmdLine::Action_Enter()
{
	printf("BF_GUI_CmdLine::Action_Enter()\n");	
	
	// store this command //
	if(lsCommand.CountItems()==poSysSetup->iCmdLineHistorySize) lsCommand.DeleteItemAt(0);
	lsCommand.AddItem(new BL_String(poEdit->sValue));					
		
	// try to run //
	BL_String sCommand(sPath);
	sCommand<<"/";
	sCommand<<poEdit->sValue;
	if(BF_Roster_RunCommand(sCommand)==0){			
	}else
	if(BF_Roster_RunCommand(poEdit->sValue)==0){
	}else
	if(B_OK==BF_Roster_RunEntry(BEntry(sCommand.String()))){
	}else
	{
		BF_Dialog_Alert_Sep(B_TRANSLATE(BF_DICT_ERROR),B_TRANSLATE(BF_DICT_PATH_CANNOTRUN),NULL,poWin->CurrentFocus());
		SetValues(NULL,"");
	}
	//
		
	sCmd = "";
	poEdit->SetText("",true);
}
