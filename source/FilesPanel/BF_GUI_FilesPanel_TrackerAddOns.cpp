#include <stdio.h>
#include "BF_GUI_FilesPanel.h"
#include "BF_Roster.h"
#include "BF_Dict.h"
#include "BF_GUI_CmdLine.h"
#include "BF_GUI_KeysMenu.h"
#include "BF_GUI_FilesPanel_Tasks.h"
#include "BF_GUI_WinMain.h"

#define ADDONS_FOLDER_SYS 	"/boot/home/config/add-ons/Tracker"
#define ADDONS_FOLDER_HOME 	"/boot/system/add-ons/Tracker"
#define WIN_TITLE "Tracker Add-Ons"

void
BF_GUI_FilesPanel::Action_Tracker_Addons_Start_LoadAddOns(bool b_FromHome,BL_List &lo_MenuResult,bool b_LoadSIcons)
{
	BF_NodeList 			loNode;
	BF_GUI_ViewMenu_Item 	*po;
	BL_String				s;
	BF_Node 				*poNode;		

	BF_Path oPath(b_FromHome?ADDONS_FOLDER_HOME:ADDONS_FOLDER_SYS);
	BF_Roster_LoadNodeList(oPath,loNode,BF_ROSTER_LOAD_NODE_FILE|BF_ROSTER_LOAD_NODE_LINK|(b_LoadSIcons?BF_ROSTER_LOAD_SICON:0));	
	
	for(int i=0;i<loNode.CountItems();i++){
		poNode = (BF_Node*)loNode.ItemAt(i);
		// prepare code //
		s = oPath.Path();
		s<<"/";
		s<<poNode->sName;
		// make new menu_item //
		po = new BF_GUI_ViewMenu_Item(poNode->sName.String(),s.String() );
		lo_MenuResult.AddItem(po);
		if(b_LoadSIcons && poNode->poSIcon)	po->poSIcon = new BBitmap( poNode->poSIcon );
	}		
}

void
BF_GUI_FilesPanel::Action_Tracker_Addons_Start()
{	

	bool    	bShowIcons = poSysSetup->MainStyle() & BF_SETUP_FILESPANEL_TRACKERADDONS_SHOWICONS;
	BL_List 	*ploMenu = new BL_List();
	
	Action_Tracker_Addons_Start_LoadAddOns(true,*ploMenu,bShowIcons);
	Action_Tracker_Addons_Start_LoadAddOns(false,*ploMenu,bShowIcons);
		
	if(ploMenu->CountItems()==0){
		BF_Dialog_Alert_Sep(BF_DictAt(BF_DICT_ERROR),BF_DictAt(BF_DICT_TRADDS_NONE),NULL,this);
		return;
	}
	
	BF_GUI_Func_PanelsEnable(false);	
		
	/* prepare message for dialog */
	BMessage oMessage(BF_MSG_FILEPANEL_TRACKER_ADDONS_SELECTED);			
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
		if(bShowIcons) fMaxWidth+=19;
	}	
	/* check width */
	{
		float fTitleWidth = poSysSetup->oFontToolView.oFont.StringWidth(WIN_TITLE)+40;
		if(fTitleWidth>fMaxWidth) fMaxWidth = fTitleWidth;
	}
	/* prepare rect for dialog */
	BRect oDlgRect(30,30,0,0);	
	if(!bWinPos_OnLeft){
		BRect oRect(poWinView->Bounds());
		oDlgRect.left+=oRect.Width()/2;		
	}
	/* make dialog */	
	BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(oDlgRect,WIN_TITLE,"dialog",oMessage,
		bWinPos_OnLeft?BG_GUI_DIALOG_WINRESIZE_NONE:BG_GUI_DIALOG_WINRESIZE_MOVE_RCENTER,false);	
	/**/
	float fHeight = poSysSetup->oFontToolView.fHeight;
	if(fHeight<19 && bShowIcons) fHeight = 19;
	
	BRect oMenuRect(0,0,0,0);
	poDialog->LocalBounds(oMenuRect);
	oMenuRect.right = oMenuRect.left + fMaxWidth;
	oMenuRect.bottom = oMenuRect.top + 5 + fHeight*ploMenu->CountItems();
		
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

#include <be/add-ons/tracker/TrackerAddOn.h>


int32 
BF_GUI_FilesPanel_Action_Tracker_Addons_Run_Thread(void *data)
{
	BMessage *poMessage = (BMessage*)data;
	if(!poMessage) return -1;
	
	entry_ref	uEntryRef;
	if(B_OK!=poMessage->FindRef("folder_ref",&uEntryRef)) return -1;
	
	void (*Func_AddOn)(entry_ref dir_ref, BMessage *msg, void *);
	if(B_OK!=poMessage->FindPointer("func",(void**)&Func_AddOn) || !Func_AddOn) return -1;

	(*Func_AddOn)(uEntryRef,poMessage,NULL);		
	
	DELETE(poMessage);
	return -1;
}

void					
BF_GUI_FilesPanel::Action_Tracker_Addons_Run(const char *pc_AddOn)
{
	ASSERT(pc_AddOn);
		
	image_id 	uImage;	
	uImage = load_add_on(pc_AddOn);
	if(0==uImage) return;
	
	BEntry 		oEntry(oPath.Path());
	entry_ref	uEntryRef;
	
	if(B_OK!=oEntry.GetRef(&uEntryRef)) return;
	
	void (*Func_AddOn)(entry_ref dir_ref, BMessage *msg, void *);
	if(B_OK!=get_image_symbol(uImage, "process_refs", B_SYMBOL_TYPE_TEXT, (void**)&Func_AddOn)) return;
	
	BMessage 			*poMessage = new BMessage();	
	BF_NodeCollection	loSelNode;
	loNode.GetSelecting(loSelNode);
	if(loSelNode.CountItems()==0) loSelNode.AddItem(Nodes_Focus());
	if(loSelNode.CountItems()==0) return;
	ASSERT(poMessage);
	
	BF_Node 	*poNode=NULL;
	entry_ref	uNodeRef;	
	BL_String	s;
	for(int iNode=0;iNode<loSelNode.CountItems();iNode++){
		poNode = loSelNode.NodeAt(iNode);
		s=oPath.Path();
		s<<"/";
		s<<poNode->sName;
		if(B_OK!=oEntry.SetTo(s.String()) || B_OK!=oEntry.GetRef(&uNodeRef)) continue;
		poMessage->AddRef("refs",&uNodeRef);
	}
	
	
	poMessage->AddRef("folder_ref",&uEntryRef);
	poMessage->AddPointer("func",(void*)Func_AddOn);
	
	// run thread //
	thread_id idThread = spawn_thread(BF_GUI_FilesPanel_Action_Tracker_Addons_Run_Thread,"tracker_addon_thread",B_THREAD_SUSPENDED,(void*)poMessage);	
	ASSERT(idThread>0,"can`t start thread\n");	
	ASSERT(B_OK==resume_thread(idThread));		
	set_thread_priority(idThread,1);				
}
