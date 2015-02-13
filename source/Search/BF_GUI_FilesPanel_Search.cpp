/*
===============================================
Project:	BeFar
File:		BF_GUI_FilesPanel_Search.cpp
Desc:		Methods for Search files
Author:		Baza
Created:	20.11.99
Modified:	20.11.99
===============================================
*/
#include <Volume.h>
#include <Path.h>

#include "BF_Dict.h"
#include "BF_GUI_FilesPanel_Search.h"
#include "BF_GUI_Func.h"
#include "BF_GUI_WinMain.h"
#include "BF_Roster.h"
#include "BL_File.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Search"

void
BF_GUI_FilesPanel::Action_Search_0()
{
	///////////////////////////////
	/* prepare message */
	BMessage oMessage(BF_MSG_FILEPANEL_SEARCH_SETUP_CLOSE);
	oMessage.AddPointer("bf_focus",this);
	oMessage.AddPointer("bf_current_folder",Path().String());
	/* */
	BF_GUI_Func_PanelsEnable(false);
	/* make dialog */	
	BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(BRect(0,0,300,0),
		B_TRANSLATE(BF_DICT_SEARCH),"dialog",oMessage,BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER);
	/* resize dialog */	
	BRect oRect;
	poDialog->LocalBounds(oRect);	
	
	/* name */
	poDialog->LocalBounds(oRect);	
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	BF_GUI_ViewEdit_Create(oRect,B_TRANSLATE(BF_DICT_FILESEARCH_FILENAME),poDialog,"name","*",
					B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);	
	oRect.top = oRect.bottom + 5;					
	/* text */	
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	BF_GUI_ViewEdit_Create(oRect,B_TRANSLATE(BF_DICT_FILESEARCH_BYTEXT),poDialog,"text","",
					B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);					
	oRect.top = oRect.bottom + 5;					
	// check "from current folder" //
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	poDialog->AddChild(new BF_GUI_ViewCheck(oRect,"from_current_folder",
		B_TRANSLATE(BF_DICT_FILESEARCH_FROMCURFOLDER),
		false,B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE));
	oRect.top = oRect.bottom + 5;					
	/* divider */						
	oRect.bottom = oRect.top;
	poDialog->AddChild(new BF_GUI_ViewFrame(oRect,B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP));
	oRect.top = oRect.bottom + 5;
	/* comment */	
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	poDialog->AddChild(new BF_GUI_ViewText(oRect,"comment",B_TRANSLATE(BF_DICT_FILESEARCH_ONVOLUMES)
			,B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_WILL_DRAW,false));
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

void
BF_GUI_FilesPanel::Action_Search_1(BMessage* po_Message)
{
	ASSERT(po_Message);
	
	bool	bOk;
	ASSERT(B_OK==po_Message->FindBool(BF_GUI_DIALOG_MESSAGE_DATA_OK,&bOk));
	if(!bOk){	
		BF_GUI_Func_PanelsEnable(true);
		MakeFocus();	
		return;
	}		
	
	// get other //
	
	const 	char *pcFileName = po_Message->FindString("name");
	const 	char *pcText = po_Message->FindString("text");
	bool	bFromCurrentFolder=false;
	ASSERT(B_OK==po_Message->FindBool("from_current_folder",&bFromCurrentFolder));;
	
	ASSERT(pcFileName && pcText);
	
	BRect oRect(poWinView->Bounds());
	oRect.left+=40;
	oRect.right-=40;
	oRect.top+=40;
	oRect.bottom-=40;
	BF_GUI_FilesPanel_SearchDialog *poDialog = new BF_GUI_FilesPanel_SearchDialog(oRect,
		pcFileName,this,oPath);
	poDialog->sSearchText = pcText;
	poDialog->bFromCurrentFolder = bFromCurrentFolder;
	
	poDialog->oCurrentFolder.SetTo(po_Message->FindString("bf_current_folder"));
	
	// get volume_list//	
	BL_Get_SList_FromMessage(*po_Message,"volumes_sel",poDialog->lsVol);
		
	poDialog->MoveToCenter( poWinView );			
	BF_GUI_Func_AddChildToMainView ( poDialog );		
}

BF_GUI_FilesPanel_SearchDialog::BF_GUI_FilesPanel_SearchDialog(
	const BRect & o_Rect,
	const char *pc_SearchName,
	BF_GUI_FilesPanel *po_Panel,
	BF_Path	 & o_Path)
:BF_GUI_Dialog(o_Rect,B_TRANSLATE(BF_DICT_SEARCH),"search",BMessage(),BG_GUI_DIALOG_WINRESIZE_RESIZE_ALL)
{
	oMessage.what = BF_MSG_FILEPANEL_SEARCH_CLOSE;
	oMessage.AddPointer("bf_focus",po_Panel);
	//
	sSearchName = pc_SearchName;		
	poPanel = po_Panel;
	idThreadQuery = 0;
	poSearch = NULL;
	oPath = o_Path;
	bFromCurrentFolder = false;
	//// nodes list ////
	BRect oRect;
	LocalBounds(oRect);
	oRect.bottom-=poSysSetup->oFontToolView.fHeight*3+5;
	BL_List *ploCol = new BL_List();
	float fWidthSize = poSysSetup->oFontToolView.oFont.StringWidth("1234567890121")+10;
	ploCol->AddItem(new BF_GUI_DlgView_VCMenu_Column(B_TRANSLATE(BF_DICT_NAME),oRect.Width()-fWidthSize));
	ploCol->AddItem(new BF_GUI_DlgView_VCMenu_Column(B_TRANSLATE(BF_DICT_NAME),fWidthSize));
	poList = new BF_GUI_DlgView_VCMenu(oRect,"results",B_FOLLOW_ALL,new BL_List(),BF_GUI_DLGVIEW_VMENU_SICON,
		ploCol);
	AddChild(poList);
	/////
	oRect.top = oRect.bottom + 5;
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	poComment = new BF_GUI_ViewText(oRect,"comment",B_TRANSLATE(BF_DICT_FILESEARCH_PROGRESS),B_FOLLOW_BOTTOM|B_FOLLOW_LEFT_RIGHT,0,false);
	AddChild(poComment);
	/////
	BL_List *ploMenu = new BL_List();
	ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_GOTO),"go"));
	ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_CANCEL),"cancel"));
	AddMenu(oRect,ploMenu);	
}

BF_GUI_FilesPanel_SearchDialog::~BF_GUI_FilesPanel_SearchDialog()
{
	if(idThreadQuery) kill_thread(idThreadQuery);
}

BF_GUI_FilesPanel_Search::BF_GUI_FilesPanel_Search(BF_GUI_FilesPanel_SearchDialog *po_Dialog)
{
	ASSERT(po_Dialog);
	poDialogParent = po_Dialog;
	bReqClose = false;
	{
		LOCK_WIN();
		oPath = poDialogParent->oPath;	
		sSearchName = poDialogParent->sSearchName;
		sSearchText =  poDialogParent->sSearchText;
		poDialogParent->poSearch = this;
		bFromCurrentFolder = poDialogParent->bFromCurrentFolder;
		oCurrentFolder = poDialogParent->oCurrentFolder;
	}	
	oMsgAdd.what = BF_MSG_FILEPANEL_SEARCH_ADDITEM;
}

void
BF_GUI_FilesPanel_Search::Message_AddItem(BF_GUI_ViewMenu_Item *po_MenuItem)
{
	ASSERT(po_MenuItem);
	oMsgAdd.MakeEmpty();
	oMsgAdd.AddPointer("bf_Item",po_MenuItem);
	
	BMessenger 	oMessenger(poDialogParent);	
	oMessenger.SendMessage(&oMsgAdd);		
}

void							
BF_GUI_FilesPanel_Search::Run_OnOtherVolume(const char *pc_Path)
{
	// load nodes //
	ASSERT(pc_Path);
	BF_Path 	oPath(pc_Path);
	BF_NodeList loNode;
	BL_String 	s;
	if(!BF_Roster_LoadNodeList(oPath,loNode,BF_ROSTER_LOAD_NODE_ALL_REAL)) return;

	BF_Node *poNode=NULL;
	
	Debug_Info(pc_Path);

	//scan files and links //	
	for(int i=0;i<loNode.CountItems();i++){
		poNode = loNode.NodeAt(i);
		
		// drop links //
		if(poNode->bLink && poNode->iType==BF_NODE_TYPE_DIR) continue;
		
		// check name //
		if(poNode->CheckByMask(sSearchName.String())){	
			s = pc_Path;
			s<<"/";
			s<<poNode->sName;
			Search_AddResult(poNode,s);			
		}
		// is it folder ? //
		if(poNode->iType==BF_NODE_TYPE_DIR){
			s = pc_Path;
			s<<"/";
			s<<poNode->sName;
			Run_OnOtherVolume(s.String());
		}
		//
		loNode.DeleteItem(poNode);
		i--;
	}
}

// open file as text_file and check for text inside //
bool
BF_GUI_FilesPanel_Search::Search_CheckTextFile(const char *pc_PathName,BL_String & s1)
{
	BL_File 	oFile(pc_PathName,B_READ_ONLY);
	if(B_OK!=oFile.InitCheck()) return false;
	while(B_OK==oFile.ReadString(&s1)){
		if(bReqClose) return false;
		if(s1.FindFirst(poDialogParent->sSearchText.String())>=0){
			return true;
		}
	}
	return false;
}

void							
BF_GUI_FilesPanel_Search::Run_OnBootVolume()
{		
	status_t uRes;
	BL_String s,s1,s2;
	
	// init query //
	oQuery.Clear();
	
	s="/boot";
	// set volume //
	{
		s = (const char*)oPath.GetVolume(true);
		entry_ref uRef;
		uRes = BF_Roster_LoadEntryRef(s.String(),uRef);
		if(uRes!=B_OK){
			BF_Dialog_Alert_Sep_Error(uRes,poDialogParent);
			return;
		}
		BVolume oVol(uRef.device);
		uRes = oVol.InitCheck();
		if(uRes!=B_OK){
			BF_Dialog_Alert_Sep_Error(uRes,poDialogParent);		
			return;
		}
		uRes = oQuery.SetVolume(&oVol);
		if(uRes!=B_OK){
			BF_Dialog_Alert_Sep_Error(uRes,poDialogParent);
			return;
		}
	}				
	// set filter by name //
	oQuery.PushAttr("name");
	oQuery.PushString(sSearchName.String());	
	oQuery.PushOp(B_EQ);
	// start search
	uRes = 	oQuery.Fetch();
	if(B_OK!=uRes){
		BF_Dialog_Alert_Sep_Error(uRes,poDialogParent);		
		return;
	}
	//
	BEntry 		oEntry,oEntryParent;
	BPath 		oPath;
	char 		pc[500];
	BF_Node		oNode;
	
	
	while(B_OK==oQuery.GetNextEntry(&oEntry)){
		oEntry.GetName(pc);
		s1 = pc;		
		oEntry.GetParent(&oEntryParent);
		oPath.SetTo(&oEntryParent);
		s = oPath.Path();
		s<<"/";
		s<<s1;
		// check for current_folder
		if(bFromCurrentFolder){
			if(!oCurrentFolder.InFolder(oPath.Path())) continue;
		}
		//
		BF_Node *poNode = BF_Roster_LoadNode(oPath.Path(),s1.String(),BF_ROSTER_LOAD_SICON);
		// check for text_string //
		if(sSearchText!=""){
			oNode.Set(poNode,true);
			DELETE(poNode);
			poNode = &oNode;
			// check for type=file
			if(poNode->iType!=BF_NODE_TYPE_FILE) continue;
			// update info 
			if(poDialogParent->poComment){
				s2=B_TRANSLATE(BF_DICT_FILESEARCH_PROGRESS);
				s2<<s1;
				LOCK_WIN();
				poDialogParent->poComment->sTitle = s2;
				poDialogParent->poComment->Draw(poDialogParent->poComment->Bounds());
			}
			
			// open file as text_file
			BL_File 	oFile(s.String(),B_READ_ONLY);
			bool		bFounded=false;
			if(B_OK!=oFile.InitCheck()) continue;
			while(B_OK==oFile.ReadString(&s1)){
				if(bReqClose) return;
				if(s1.FindFirst(poDialogParent->sSearchText.String())>=0){
					bFounded = true;
					break;
				}
			}
			if(!bFounded)	continue;
			// update info 
			if(poDialogParent->poComment){
				LOCK_WIN();
				poDialogParent->poComment->sTitle = B_TRANSLATE(BF_DICT_FILESEARCH_PROGRESS);
				poDialogParent->poComment->Draw(poDialogParent->poComment->Bounds());
			}
		}
		///
		if(bReqClose) return;		
		// add item to list ///		
		Search_AddResult(poNode,s);
		/*
		poItem = new BF_GUI_ViewMenu_Item(s.String(),s.String(),NULL,poNode?new BBitmap(poNode->poSIcon):NULL);		
		switch(poNode->iType){
		case BF_NODE_TYPE_DIR:
			s="Dir";
			break;
		default:			
			s = poNode->iSize;
		}
		poItem->loColItem.AddItem(new BF_GUI_ViewMenu_Item(s.String(),""));
		// send add_message to parent
		Message_AddItem(poItem);
		*/
		// delete node //
		if(poNode!=&oNode)	DELETE(poNode);
		//				
	}
	return;	
	
}

bool
BF_GUI_FilesPanel_Search::Search_InFileCheck(BF_Node *po_Node,BL_String &s_FullPath)
{
	ASSERT(po_Node);
	
	if(sSearchText=="") return true;	
	if(po_Node->iType!=BF_NODE_TYPE_FILE) return false;

	BL_String s;	
	
	// update info 
	if(poDialogParent->poComment){

		s=B_TRANSLATE(BF_DICT_FILESEARCH_PROGRESS);
		s<<po_Node->sName;
		LOCK_WIN();
		poDialogParent->poComment->sTitle = s;
		poDialogParent->poComment->Draw(poDialogParent->poComment->Bounds());
	}
	
	////////

	// open file as text_file
	BL_File 	oFile(s_FullPath.String(),B_READ_ONLY);
	bool		bFounded=false;
	if(B_OK!=oFile.InitCheck()) return false;
	while(B_OK==oFile.ReadString(&s)){
		if(bReqClose) return false;
		if(s.FindFirst(poDialogParent->sSearchText.String())>=0){
			bFounded = true;
			break;
		}
	}	
	// update info 
	if(poDialogParent->poComment){	
		LOCK_WIN();
		poDialogParent->poComment->sTitle = B_TRANSLATE(BF_DICT_FILESEARCH_PROGRESS);
		poDialogParent->poComment->Draw(poDialogParent->poComment->Bounds());
	}
	return  bFounded;
}

void
BF_GUI_FilesPanel_Search::Search_AddResult(BF_Node *po_Node,BL_String &s_FullPath)
{
	ASSERT(po_Node);
	
	if(!po_Node->poSIcon) BF_Roster_ReloadNode(s_FullPath.String(),po_Node,BF_ROSTER_LOAD_SICON,true);
	
	if(!Search_InFileCheck(po_Node,s_FullPath)) return;
	
	// add item to list ///		
	BF_GUI_ViewMenu_Item *poItem = new BF_GUI_ViewMenu_Item(s_FullPath.String(),s_FullPath.String(),
		(po_Node && po_Node->poSIcon)?new BBitmap(po_Node->poSIcon):NULL);		
	switch(po_Node->iType){
	case BF_NODE_TYPE_DIR:
		s_FullPath=B_TRANSLATE(BF_DICT_DIR);
		break;
	default:			
		s_FullPath = po_Node->iSize;
	}
	poItem->loColItem.AddItem(new BF_GUI_ViewMenu_Item(s_FullPath.String(),""));
	// send add_message to parent
	Message_AddItem(poItem);
}

void							
BF_GUI_FilesPanel_Search::Run()
{			
	// scan volumes //	
	for(int iVol=0;iVol<poDialogParent->lsVol.CountItems();iVol++){		
		BL_String	*ps = poDialogParent->lsVol.StringAt(iVol);
		ASSERT(ps);
		/*		
		BF_Volume	oVol;		
		if(!oVol.Load(ps->String(),BF_ROSTER_LOAD_NORMAL)) continue;		
		if(oVol.bBootVolume){
		*/
		
		if(*ps=="boot"){
			Run_OnBootVolume();
		}else{			
			BL_String s("/");
			s<<*ps;
			Run_OnOtherVolume(s.String());
		}
	}		
	
	// search in boot_folder
	//Run_BootVolume();
	// searh in ...	
	/// list ////	
}

void							
BF_GUI_FilesPanel_Search::Close()
{
	oMsgAdd.what = BF_MSG_FILEPANEL_SEARCH_THREAD_FINISHED;
	
	BMessenger 	oMessenger(poDialogParent);	
	oMessenger.SendMessage(&oMsgAdd);		
}


int32 
BF_GUI_FilesPanel_SearchDialog_Thread(void *data)
{	
	BF_GUI_FilesPanel_SearchDialog *poDialog = (BF_GUI_FilesPanel_SearchDialog*)data;	
	//
	BF_GUI_FilesPanel_Search oSearch(poDialog);
	oSearch.Run();				
	oSearch.Close();
	return 0;
}

bool					
BF_GUI_FilesPanel_SearchDialog::ReadyForClose()
{
	if(poSearch) poSearch->bReqClose = true;
	return 0==idThreadQuery;	
}

void
BF_GUI_FilesPanel_SearchDialog::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what){	
	case BF_MSG_FILEPANEL_SEARCH_THREAD_FINISHED:{
		idThreadQuery = 0;
		poSearch = NULL;
		
		BL_String s;
		s = B_TRANSLATE(BF_DICT_FILESEARCH_FOUNDED);
		s<<" ";
		s<<poList->ploMenu->CountItems();
		s<<" ";
		s<<B_TRANSLATE(BF_DICT_FILESEARCH_FILES);
		poComment->sTitle = s;
		poComment->Draw(poComment->Bounds());
		break;}
	case BF_MSG_FILEPANEL_SEARCH_ADDITEM:{
		BF_GUI_ViewMenu_Item *poItem = NULL;		
		ASSERT(B_OK==po_Message->FindPointer("bf_Item",(void**)&poItem) && poItem);
		
		poList->AddItem(poItem);
		/*
		poList->ploMenu->AddItem(poItem);
		if(poList->ploMenu->CountItems()<=poList->iPageSize){
			poList->Draw(poList->Bounds());
			if(poList->iNavCursorIndex<0) poList->NavChangeCursor(0,BF_GUI_TOOLMENU_MOVE_POS);
		}
		*/
		// update comment //
		{
			BL_String s;
			s=B_TRANSLATE(BF_DICT_FILESEARCH_FOUNDED);
			s<<" ";
			s<<poList->ploMenu->CountItems();
			s<<" ";
			s<<B_TRANSLATE(BF_DICT_FILESEARCH_FILES);
			poComment->sTitle = s;
			poComment->Draw(poComment->Bounds());
		}
		//
		break;}
	default:
		BF_GUI_Dialog::MessageReceived(po_Message);
	}
}

void 					
BF_GUI_FilesPanel_SearchDialog::AttachedToWindow(void)
{
	BF_GUI_Dialog::AttachedToWindow();
	//////////////
	idThreadQuery = spawn_thread(BF_GUI_FilesPanel_SearchDialog_Thread,"search",B_THREAD_SUSPENDED,(void*)this);	
	ASSERT(idThreadQuery>0,"can`t start search_thread\n");	
	ASSERT(B_OK==resume_thread(idThreadQuery));		
	set_thread_priority(idThreadQuery,1);			
	//
}
