#include "BF_Dict.h"
#include "BF_GUI_WinMain.h"
#include "BF_Roster.h"
#include "BF_GUI_DlgViews.h"
#include "BF_GUI_FilesPanel_Tasks.h"

////////////////////////////////////////////////////////////////////////
void
BF_GUI_ViewMain::Action_MountVolumes()
{
///////////////////////////////
	/* prepare message */
	BMessage oMessage(BF_MSG_MAKEFOCUS);
	oMessage.AddPointer("bf_focus",poWin->CurrentFocus());
	/* */
	BF_GUI_Func_PanelsEnable(false);
	/* make dialog */	
	BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(BRect(0,0,300,0),
		"Mount/unmount volems","dialog",oMessage,BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER); //dd
	/* resize dialog */	
	BRect oRect;
	poDialog->LocalBounds(oRect);	
		
	/* comment */	
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	poDialog->AddChild(new BF_GUI_ViewText(oRect,"comment","Existing volumes",B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_WILL_DRAW,false));//dd
	oRect.top = oRect.bottom+5;	
	/* volumes */
	{
		BL_List 				*ploItem = new BL_List();
		BF_GUI_ViewMenu_Item 	*poItem = NULL;
		
		/* prepare volume_list */
		bool    bShowIcons = poSysSetup->MainStyle() & BF_SETUP_SEARCH_SETUP_VOLICONS;
		
		BF_NodeList loNodeVol;
		BF_Roster_VolumeList(loNodeVol,
			( (bShowIcons)?BF_ROSTER_LOADVOLS_SICON:0 )		
		);			
		BF_Volume *poNode;
					
		for(int i=0;i<loNodeVol.CountItems();i++){
			poNode = (BF_Volume*)loNodeVol.ItemAt(i);
			///
			if(poNode->bBootVolume) poNode->sName="boot";
			///
			poItem = new BF_GUI_ViewMenu_Item(poNode->sName.String(),poNode->sName.String() );
			ploItem->AddItem(poItem);
			if(poNode->poSIcon && bShowIcons)	poItem->poSIcon = new BBitmap( poNode->poSIcon );
			//
			poItem->bSelected = poNode->bBootVolume;
		}			
		oRect.bottom = oRect.top+70;
		BF_GUI_DlgView_VMenu *poMenu = new BF_GUI_DlgView_VMenu(oRect,"volumes",B_FOLLOW_ALL,
			ploItem,BF_GUI_DLGVIEW_VMENU_SICON|BF_GUI_DLGVIEW_VMENU_CAN_SELECTING);
		poDialog->AddChild(poMenu);
		oRect.top = oRect.bottom+5;
	}		
	/* divider */						
	oRect.bottom = oRect.top;
	poDialog->AddChild(new BF_GUI_ViewFrame(oRect,B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP));
	oRect.top = oRect.bottom + 5;	
	///////////////////////////////		
	/* menu */
	poDialog->AddOkCancelMenu(oRect);		
	/* finish */
	poDialog->SetHeightFromLastChild();
	poDialog->MoveToCenter( poWinView );
	BF_GUI_Func_AddChildToMainView ( poDialog );		
}
////////////////////////////////////////////////////////////////////////
void
BF_GUI_ViewMain::Action_Compare_Panels()
{
	BF_NodeList loLNode,loRNode;
	BF_GUI_Panel *poLPanel = PanelOnTop(true);
	BF_GUI_Panel *poRPanel = PanelOnTop(false);
	if(!poLPanel || !poRPanel) return;
	/////////////
	poLPanel->Nodes_GetAll(loLNode,false);
	poRPanel->Nodes_GetAll(loRNode,false);
	poLPanel->Nodes_SetSelectingByName(NULL,BF_NODE_SELECT_ON,false);
	poRPanel->Nodes_SetSelectingByName(NULL,BF_NODE_SELECT_ON,false);
	poLPanel->Nodes_SetSelectingByName(&loRNode,BF_NODE_SELECT_OFF);
	poRPanel->Nodes_SetSelectingByName(&loLNode,BF_NODE_SELECT_OFF);	
}
////////////////////////////////////////////////////////////////////////
void
BF_GUI_ViewMain::Action_SelVolume_Start(bool b_LeftPos)
{
	/* disable panels */
	BF_GUI_Func_PanelsEnable(false);
	/* prepare volume_list */
	bool    	bShowIcons = poSysSetup->MainStyle() & BF_SETUP_MAIN_SELECTVOL_SHOWSICON;
	BF_NodeList loNodeVol;
	BF_Roster_VolumeList(loNodeVol,	
		( (poSysSetup->MainStyle() & BF_SETUP_MAIN_SELECTVOL_SHOWSPEC)?BF_ROSTER_LOADVOLS_SPEC:0) |
		( (bShowIcons)?BF_ROSTER_LOADVOLS_SICON:0 )		
	);	
	BL_List *ploMenu = new BL_List();
	BF_GUI_ViewMenu_Item *po;
	BF_Node *poNode;
		
	
	for(int i=0;i<loNodeVol.CountItems();i++){
		poNode = (BF_Node*)loNodeVol.ItemAt(i);
		///
		po = new BF_GUI_ViewMenu_Item(poNode->sName.String(),poNode->sName.String() );
		ploMenu->AddItem(po);
		if(poNode->poSIcon && bShowIcons)	po->poSIcon = new BBitmap( poNode->poSIcon );
	}	
	
	// add ftp_volume //
	{
		po = new BF_GUI_ViewMenu_Item("ftp",":ftp" );
		ploMenu->AddItem(po);
	}
		
	/* try to make folder_stack */
	while(poSysSetup->MainStyle() & BF_SETUP_MAIN_SELECTVOL_SHOWSTACK){
		BF_GUI_Panel *poPanel = PanelOnTop(b_LeftPos);
		if(!poPanel) break;
		BF_Path oPath;
		oPath = poPanel->Path();
		if(!oPath.IsStorageKit()) break;
		
		bool 		bFirst=false;
		BL_String 	s;
		
		while(oPath.GoParent()){
			if(!bFirst){
				ploMenu->AddItem( new BF_GUI_ViewMenu_Item("",""));
				bFirst = true;
			}
			s = oPath.PathWFS();
			po = new BF_GUI_ViewMenu_Item(s==""?oPath.Path():s.String(),s.String() );	
			if(bShowIcons) po->poSIcon = BL_Load_SIconFromMIME( BL_MIME_FOLDER );
			ploMenu->AddItem(po);
		}
		/////////
		break;		
	}	
	
	/* prepare message for dialog */
	BMessage oMessage(BF_MSG_MAINVIEW_SELVOL_FINISH);			
	oMessage.AddPointer("bf_focus",(void*)this);
	oMessage.AddPointer("bf_panel_focus",(void*)poWin->CurrentFocus());
	oMessage.AddBool("bf_bPosLeft",b_LeftPos);
	/* calc max_item  width */
	float fMaxWidth=0;
	{
		BF_GUI_ViewMenu_Item *po;
		float f;
		for(int i=0;i<ploMenu->CountItems();i++){
			po = (BF_GUI_ViewMenu_Item*)ploMenu->ItemAt(i);			
			ASSERT(po);
			f = po->CalcWidth(&poSysSetup->oFontToolView);
			//f = poSysSetup->oFontToolView.oFont.StringWidth(po->sTitle.String());
			if(f>fMaxWidth) fMaxWidth = f;
		}
		if(bShowIcons) fMaxWidth+=19;	
	}	
	/* check width */
	const char *pcWinTitle = BF_DictAt(BF_DICT_VOLUMEDIALOG_TITLE);
	{
		float fTitleWidth = poSysSetup->oFontToolView.oFont.StringWidth(pcWinTitle)+40;
		if(fTitleWidth>fMaxWidth) fMaxWidth = fTitleWidth;
	}
	/* prepare rect for dialog */
	BRect oDlgRect(30,30,0,0);	
	if(!b_LeftPos){
		BRect oRect(Bounds());
		oDlgRect.left+=oRect.Width()/2;		
	}

	/* make dialog */	
	BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(oDlgRect,pcWinTitle,"dialog",oMessage,
		b_LeftPos?BG_GUI_DIALOG_WINRESIZE_NONE:BG_GUI_DIALOG_WINRESIZE_MOVE_RCENTER,false);	
	/**/
	float fHeight = poSysSetup->oFontToolView.fHeight;
	if(fHeight<19 && bShowIcons) fHeight = 19;
	
	BRect oMenuRect(0,0,0,0);
	poDialog->LocalBounds(oMenuRect);
	oMenuRect.right = oMenuRect.left + fMaxWidth;
	oMenuRect.bottom = oMenuRect.top + 5 + fHeight*ploMenu->CountItems();

	// check max_y_size
	{
		float fy = Bounds().Height()-10;
		if((oMenuRect.top + oMenuRect.Height())>fy) oMenuRect.bottom = fy - oMenuRect.top -10;
	}
	

		
	/* make menu */	
	BView* poMenu = new BF_GUI_DlgView_VMenu(oMenuRect,"menu",	B_FOLLOW_ALL,ploMenu,
		bShowIcons?BF_GUI_DLGVIEW_VMENU_SICON:0);	
	/* finish */
	poDialog->AddChild(poMenu);	
	poDialog->SetSizeBy(oMenuRect.Width(),oMenuRect.Height());
	BF_GUI_Func_AddChildToMainView ( poDialog );		
	/* */	
	return;
}


void
BF_GUI_ViewMain::Action_EmptyTrash()
{			
	BF_FilesPath	oTrashPath(poSysSetup->oPathSysTrash.Path());
	BF_NodeList		loNode;
	BF_Roster_LoadNodeList(oTrashPath,loNode,BF_ROSTER_LOAD_NODE_ALL_REAL);
				
	BF_GUI_OperRoster_AddTask( 
		new BF_GUI_FilesPanel_DeleteTask(oTrashPath,loNode,BF_FILESPANEL_DELETE_TO_VOID));	
}

void					
BF_GUI_ViewMain::Action_LoadSavePalete(bool b_Load)
{
	/* */
	BF_GUI_Func_PanelsEnable(false);
	/* make dialog */	
	BMessage oMessage(b_Load?BF_MSG_MAINVIEW_LOAD_PALLETE_1:BF_MSG_MAINVIEW_SAVE_PALLETE_1);
	oMessage.AddPointer("bf_focus",this);
	oMessage.AddPointer("bf_oldfocus",poWin->CurrentFocus());
			
	BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(BRect(0,0,300,0),
		BF_DictAt(b_Load?BF_DICT_PAL_LOAD:BF_DICT_PAL_SAVE),"dialog",oMessage,BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER);
	BRect oRect;	
	/* insert edit */
	poDialog->LocalBounds(oRect);	
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	BF_GUI_ViewEdit_Create(oRect,BF_DictAt(BF_DICT_PAL_FILENAME),poDialog,"bf_cName",
					"untitled",	B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
	/* menu */								
	poDialog->AddOkCancelMenu(oRect);	
	/* finish */	
	poDialog->SetHeightFromLastChild();
	poDialog->MoveToCenter( poWinView );	
	BF_GUI_Func_AddChildToMainView ( poDialog );		
}

void					
BF_GUI_ViewMain::Action_SavePalete_1(BMessage * po_Message)
{
	BF_GUI_Func_PanelsEnable(true);
	BView *poOldFocus=NULL;
	ASSERT(B_OK==po_Message->FindPointer("bf_oldfocus",(void**)&poOldFocus));
	poOldFocus->MakeFocus();
	
	const char *pcName=NULL;	
	if(B_OK!=po_Message->FindString("bf_cName",&pcName) && !pcName) return;
	
	poSysSetup->SavePalete(pcName);
}

void					
BF_GUI_ViewMain::Action_LoadPalete_1(BMessage * po_Message)
{
	BF_GUI_Func_PanelsEnable(true);
	BView *poOldFocus=NULL;
	ASSERT(B_OK==po_Message->FindPointer("bf_oldfocus",(void**)&poOldFocus));
	poOldFocus->MakeFocus();
	
	const char *pcName=NULL;	
	if(B_OK!=po_Message->FindString("bf_cName",&pcName) && !pcName) return;
	
	poSysSetup->LoadPalete(pcName);
	
	BMessenger oMessenger(poWin);
	BMessage   oMessage(BF_MSG_SETUP_UPDATED);
	oMessenger.SendMessage(&oMessage);		
}

#include "BF_GUI_FtpPanel.h"

void
BF_GUI_ViewMain::Action_Open_FtpPanel(bool b_PosLeft)
{
	BF_GUI_FtpPanel *po;
	po = new 	BF_GUI_FtpPanel(poWinView->PanelRect(b_PosLeft),b_PosLeft);			
	poWinView->AddPanel(po);	
	
	//BView *poCurrentFocus = Window()->CurrentFocus();
	//if(!poCurrentFocus)	BF_GUI_MakeFocusTo(po);
}

#include "BF_Node.h"
#include "BF_Roster.h"

void	
BF_GUI_ViewMain::Action_SelectDict()
{
	//  get app_path //
	BL_String sAppPath;
	if(!BF_Path::GetPathForCurrentApp(sAppPath)) return;

	// load nodes //
	BF_Path 	oPath(sAppPath.String());
	BF_NodeList loNode;
	if(!BF_Roster_LoadNodeList(oPath,loNode,BF_ROSTER_LOAD_NODE_FILE|BF_ROSTER_LOAD_SICON)) return;
	
	// make list for menu //
	BL_List *ploMenu=new BL_List();
	for(int i=0;i<loNode.CountItems();i++){	
		BF_Node *poNode =  loNode.NodeAt(i);
		if(!poNode->CheckByMask("*.dict")) continue;
		ploMenu->AddItem(new BF_GUI_ViewMenu_Item(poNode->sName.String(),poNode->sName.String(),new BBitmap(poNode->poSIcon)));
	}
	
	// make dialog //
	/* */
	BF_GUI_Func_PanelsEnable(false);
	/* make dialog */	
	BMessage oMessage(BF_MSG_MAINVIEW_SELECTDICT_1);
	oMessage.AddPointer("bf_focus",this);
	oMessage.AddPointer("bf_panel_focus",(void*)poWin->CurrentFocus());
			
	BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(BRect(0,0,300,0),
		BF_DictAt(BF_DICT_SELDICT_TITLE),"dialog",oMessage,BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER);	
	BRect oRect;	
	/* insert edit */
	poDialog->LocalBounds(oRect);	
	oRect.bottom = oRect.top+300;
	
	poDialog->AddChild(new BF_GUI_DlgView_VMenu(oRect,"bf_lFile",B_FOLLOW_ALL,ploMenu,BF_GUI_DLGVIEW_VMENU_SICON));
	/* menu */								
	poDialog->AddOkCancelMenu(oRect);	
	/* finish */	
	poDialog->SetHeightFromLastChild();
	poDialog->MoveToCenter( poWinView );	
	BF_GUI_Func_AddChildToMainView ( poDialog );		
}


void				
BF_GUI_ViewMain::Action_SelectDict_1(BMessage *po_Message)
{
	ASSERT(po_Message);
	BView *poView=NULL;
	ASSERT(B_OK==po_Message->FindPointer("bf_panel_focus",(void**)&poView) && poView);
	//
	const char *pc=NULL;
	if(B_OK!=po_Message->FindString("bf_lFile_code",&pc)){
		// restore focus //
		EnablePanels(true);		
		poView->MakeFocus();	
		return;
	}
	poSysSetup->sDictFile = pc;
	poSysSetup->Save();
	BF_Dialog_Alert_Sep("","You must restart BeFar now",NULL,poView);
}
