#include <stdio.h>
#include <Locker.h>
#include "BF_GUI_FilesPanel.h"
#include "BF_Dict.h"
#include "BF_Roster.h"
#include "BF_GUI_CmdLine.h"
#include "BF_GUI_KeysMenu.h"
#include "BF_GUI_FilesPanel_Tasks.h"
#include "BF_GUI_WinMain.h"
#include "BF_GUI_ImagePanel.h"
#include "BF_GUI_FilesPanel_InfoPanel.h"
#include "BF_GUI_ArcPanel.h"
#include "BF_GUI_TextViewer.h"
#include "BF_GUI_AttrDialog.h"
#include <Clipboard.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "FilesPanel"

/////////////////////////////////////////////////////////////////////////

BF_GUI_FilesPanel::BF_GUI_FilesPanel(const BRect & o_Rect,bool b_WinPos_OnLeft)
:BF_GUI_NodePanel(o_Rect,b_WinPos_OnLeft)
{
	idThreadLoading = 0;
	if(poSysSetup && poSysSetup->oVars.Loaded()){
		BF_GUI_Setup_Vars_Panel *poVar = bWinPos_OnLeft?(&poSysSetup->oVars.oLPanel):(&poSysSetup->oVars.oRPanel);
		ASSERT(poVar);
		iModeFullStyles = poVar->iModeFullStyles;
		iModeColsStyles = poVar->iModeColsStyles;
		iMode = poVar->iMode;
	}
}

void
BF_GUI_FilesPanel::Action_SwitchShowIcons()
{	
	if(!ModeShowSIcon()) Load_Files(true);
	BF_GUI_NodePanel::Action_SwitchShowIcons();
}
 

void 
BF_GUI_FilesPanel::NavGoParentDir()
{
	BF_FilesPath oNewPath = oPath;
	BString sDir;
	bool	bRet = poSysSetup->MainStyle() & BF_SETUP_MAIN_RETURN_REMEMBER;
	if(bRet) oNewPath.LastDir(sDir);
	while(TRUE){
		if(!oNewPath.GoParent()) return;
		NavGoPath(oNewPath,bRet?(&sDir):NULL);
		break;
	}
}

void 
BF_GUI_FilesPanel::NavGoPath(
	const BF_Path & o_NewPath,
	BString *ps_FocusItem,
	bool b_LoadNodes,
	bool b_AddToHistory)
{	
	/* add this path to history */
	if(b_AddToHistory && !oPath.IsEmpty()) PathHistory_Add(&oPath);

	// stop old monitoring //
	if(!(poSysSetup->MainStyle() & BF_SETUP_DEB_NOMONITORING)){
		BF_Roster_StopAllMonitors(this);
	}
	
	/* set new path */
	oPath = o_NewPath;	
	SetHeader(oPath.Path(),(poSysSetup->MainStyle() & BF_SETUP_MAIN_FP_HEADERICON)?oPath.LoadSIcon():NULL);
	/*           */
	if(b_LoadNodes){
		if(ps_FocusItem) sThreadLoading_FutureFocus = *ps_FocusItem;
		Load_Files();
	}					
	/* change path in cmdline */
	PrepareCmdLine();
}

int32 
BF_GUI_FilesPanel::LoadStyles()
{
	int32 iLoadStyles = BF_ROSTER_LOAD_NODE_ALL;
	iLoadStyles = iLoadStyles | (((iMode==BF_NODEPANEL_MODE_FULL)  && (iModeFullStyles & BF_NODEPANEL_FULL_TYPE))?BF_ROSTER_LOAD_TYPE:0);
	iLoadStyles = iLoadStyles | ((ModeShowSIcon())?BF_ROSTER_LOAD_SICON:0);
	return	iLoadStyles;
}

int32 
BF_GUI_FilesPanel_LoadFiles_Thread(void *data)
{
	BF_GUI_FilesPanel *poPanel = (BF_GUI_FilesPanel*)data;
	ASSERT(poPanel);	
	BF_Roster_LoadNodeList_Ext(poPanel->oPath,poPanel->LoadStyles(),poPanel,BF_MSG_FILEPANEL_LOADNODES_STEP,BF_MSG_FILEPANEL_LOADNODES_FINISHED);				
	return 0;
}

void 
BF_GUI_FilesPanel::Load_Files(bool b_AlwaysLoadSIcons)
{		
	loNode.DeleteItems();
	loNode.poPath = &oPath;
	Invalidate(Bounds());
	
	iThreadLoading_Count = 0;
	iThreadLoading_Index = 0;
	
	
	BF_GUI_Func_PanelsEnable(false);
	if(poSysKeysMenu)	poSysKeysMenu->Clear();
	
	idThreadLoading = spawn_thread(BF_GUI_FilesPanel_LoadFiles_Thread,"BeFar:load_files",B_THREAD_SUSPENDED,(void*)this);	
	ASSERT(idThreadLoading>0,"can`t start thread load_files\n");	
	ASSERT(B_OK==resume_thread(idThreadLoading));		
	set_thread_priority(idThreadLoading,B_NORMAL_PRIORITY);	
}


void 
BF_GUI_FilesPanel::DrawListInfo(bool b_DrawBack)
{
	if(idThreadLoading){
		Draw_OpenTransaction();
		
		BL_String s,s1;
		s=B_TRANSLATE(BF_DICT_FILES_LOADING);
		s<<" ";
		s1 << iThreadLoading_Index;
		s1.SetDigits();
		s<<s1;
		s << B_TRANSLATE(BF_DICT_FILES_FROM);
		s1 = iThreadLoading_Count;
		s1.SetDigits();
		s<<s1;		
		DrawBottom(s.String(), b_DrawBack);
		
		BRect oRect(BottomRect());
		Draw_CloseTransaction(&oRect);
		
	}else
		BF_GUI_NodePanel::DrawListInfo(b_DrawBack);	
}


void
BF_GUI_FilesPanel::PathHistory_Add(BF_Path * po_Path)
{
	if(lsPathHistory.CountItems()==poSysSetup->iPathHistorySize) lsPathHistory.DeleteItemAt(0);
	ASSERT(po_Path);
	lsPathHistory.AddItem(new BL_String(po_Path->Path()));	
}

void					
BF_GUI_FilesPanel::PathHistory_GoBack()
{
	BL_String * ps = (BL_String*)lsPathHistory.ItemAt(lsPathHistory.CountItems()-1);
	/* check existing of item */
	if(!ps){
		BF_Dialog_Alert_Sep(B_TRANSLATE(BF_DICT_WARNING),B_TRANSLATE(BF_DICT_FILES_PATHHIST_EMPTY),NULL,this);
		return;
	}
	BF_Path oNewPath(ps->String());
	lsPathHistory.DeleteItemAt(lsPathHistory.CountItems()-1);
	NavGoPath(oNewPath,NULL,true,false);
}

const BL_String					
BF_GUI_FilesPanel::Path() const
{
	BL_String sResult;
	sResult = oPath.Path();
	return sResult;
}

void
BF_GUI_FilesPanel::PrepareHotKeys(bool b_Focused)
{
	BF_GUI_NodePanel::PrepareHotKeys(b_Focused);
	
	if(b_Focused){
		poWin->AddShortcut('d',B_COMMAND_KEY,new BMessage(BF_MSG_FILEPANEL_TRACKER_ADDONS_START),this);				
		//poWin->AddShortcut('m',B_COMMAND_KEY,new BMessage(BF_MSG_FILEPANEL_IMAGEVIEWER_START),this);				
		poWin->AddShortcut('i',B_COMMAND_KEY,new BMessage(BF_MSG_FILEPANEL_SHOW_INFOPANEL),this);
		poWin->AddShortcut('u',B_COMMAND_KEY,new BMessage(BF_MSG_FILEPANEL_SET_FRIENDPATH_TOEQUAL),this);		
		poWin->AddShortcut('r',B_COMMAND_KEY,new BMessage(BF_MSG_FILEPANEL_REFRESH),this);		
		poWin->AddShortcut('a',B_COMMAND_KEY,new BMessage(BF_MSG_FILEPANEL_SHOW_ATTRDIALOG),this);				
		poWin->AddShortcut('s',B_COMMAND_KEY,new BMessage(BF_MSG_FILEPANEL_MAKESH_DIALOG),this);
	}else{		
		poWin->RemoveShortcut('d',B_COMMAND_KEY);
		//poWin->RemoveShortcut('m',B_COMMAND_KEY);
		poWin->RemoveShortcut('i',B_COMMAND_KEY);
		poWin->RemoveShortcut('u',B_COMMAND_KEY);
		poWin->RemoveShortcut('r',B_COMMAND_KEY);
		poWin->RemoveShortcut('a',B_COMMAND_KEY);
		poWin->RemoveShortcut('s',B_COMMAND_KEY);
	}
}

void
BF_GUI_FilesPanel::PrepareCmdLine()
{
	if(!poCmdLine) return;
	
	poCmdLine->SetValues(oPath.Path());
}

void
BF_GUI_FilesPanel::PrepareKeysMenu()
{
	BF_GUI_NodePanel::PrepareKeysMenu();
	uint32 	iKeysModifiers = modifiers();
	
	if(iKeysModifiers & B_RIGHT_OPTION_KEY){
		BMessage *po;
		
		poSysKeysMenu->SetText(5,B_TRANSLATE(BF_DICT_FILES_MKLINK));
		po = new BMessage(BF_MSG_NODEPANEL_MAKELINK_STARTDIALOG);
		poSysKeysMenu->SetMessage(5,po,this);			
		
		poSysKeysMenu->SetText(6,B_TRANSLATE(BF_DICT_RENAME));
		po = new BMessage(BF_MSG_NODEPANEL_RENAME_START);
		poSysKeysMenu->SetMessage(6,po,this);			
		
		poSysKeysMenu->SetText(7,B_TRANSLATE(BF_DICT_SEARCH));
		po = new BMessage(BF_MSG_FILEPANEL_SEARCH_START);
		poSysKeysMenu->SetMessage(7,po,this);	
		
		poSysKeysMenu->SetText(8,B_TRANSLATE(BF_DICT_FILES_TOVOID));
		po = new BMessage(BF_MSG_FILEPANEL_DELETE_VOID);
		poSysKeysMenu->SetMessage(8,po,this);	
				
	}else{
		poSysKeysMenu->SetText(3,B_TRANSLATE(BF_DICT_VIEW));
		poSysKeysMenu->SetMessage(3,new BMessage(BF_MSG_FILEPANEL_TEXTVIEWER_START),this);			
	
		poSysKeysMenu->SetText(4,B_TRANSLATE(BF_DICT_EDIT));
		poSysKeysMenu->SetMessage(4,new BMessage(BF_MSG_FILEPANEL_EDIT),this);			
	
		poSysKeysMenu->SetText(5,B_TRANSLATE(BF_DICT_COPY));
		poSysKeysMenu->SetMessage(5,new BMessage(BF_MSG_NODEPANEL_COPY_START),this);	
		
		poSysKeysMenu->SetText(6,B_TRANSLATE(BF_DICT_MOVE));
		poSysKeysMenu->SetMessage(6,new BMessage(BF_MSG_NODEPANEL_MOVE_START),this);	
	
		poSysKeysMenu->SetText(7,B_TRANSLATE(BF_DICT_FILES_MKDIR));
		poSysKeysMenu->SetMessage(7,new BMessage(BF_MSG_NODEPANEL_MAKEDIR_START),this);			

		poSysKeysMenu->SetText(8,B_TRANSLATE(BF_DICT_FILES_TOTRASH));
		poSysKeysMenu->SetMessage(8,new BMessage(BF_MSG_FILEPANEL_DELETE_TRASH),this);	
	}
}

void
BF_GUI_FilesPanel::PrepareTopMenuItem(BF_GUI_TopMenu_HItem *po_HItem)
{
	BF_GUI_NodePanel::PrepareTopMenuItem(po_HItem);
	po_HItem->AddHItem("",NULL);
	po_HItem->AddHItem(B_TRANSLATE(BF_DICT_MENU_SEARCH),BF_MSG_FILEPANEL_SEARCH_START,this,"Ctrl+F7");	
	po_HItem->AddHItem(B_TRANSLATE(BF_DICT_MENU_CHANGECASE),BF_MSG_FILEPANEL_CHANGE_CASE,this,"");
	po_HItem->AddHItem(B_TRANSLATE(BF_DICT_MENU_TRACKER_ADDONS),BF_MSG_FILEPANEL_TRACKER_ADDONS_START,this,"Alt+D");
	//po_HItem->AddHItem("View image",BF_MSG_FILEPANEL_IMAGEVIEWER_START,this,"Alt+M");	
	po_HItem->AddHItem(B_TRANSLATE(BF_DICT_MENU_SHOWINFO),BF_MSG_FILEPANEL_SHOW_INFOPANEL,this,"Alt+I");
	po_HItem->AddHItem(B_TRANSLATE(BF_DICT_MENU_SETOPPATH),BF_MSG_FILEPANEL_SET_FRIENDPATH_TOEQUAL,this,"Alt+U");
	po_HItem->AddHItem(B_TRANSLATE(BF_DICT_MENU_GOTO),BF_MSG_FILEPANEL_GOPATH_PREPARE,this,"");
	po_HItem->AddHItem(B_TRANSLATE(BF_DICT_MENU_REFRESH),BF_MSG_FILEPANEL_REFRESH,this,"Alt+R");	
	po_HItem->AddHItem(B_TRANSLATE(BF_DICT_MENU_SHOWATTRS),BF_MSG_FILEPANEL_SHOW_ATTRDIALOG,this,"Alt+A");	
	po_HItem->AddHItem(B_TRANSLATE(BF_DICT_MENU_MAKESHLINK),BF_MSG_FILEPANEL_MAKESH_DIALOG,this,"Alt+S");		
}

bool
BF_GUI_FilesPanel::OnKeyDown(const char *bytes, int32 numBytes)
{
	if(numBytes==1 && bytes[0]==B_DELETE){
		iTempDelType = BF_MSG_FILEPANEL_DELETE_TRASH;
		ActionDelete_Start();
	}else
		return BF_GUI_NodePanel::OnKeyDown(bytes,numBytes);
	return true;
}

bool					
BF_GUI_FilesPanel::OnClose()
{
	return false;
}

bool
BF_GUI_FilesPanel::NavEnter()
{	
	BF_Node*	poNode  = Nodes_Focus();
	if(!poNode) return BF_GUI_NodePanel::NavEnter();
	
	
	/* if this node link to dir */
	if(poNode->bLink && BF_NODE_TYPE_DIR==poNode->iType){
		BL_String s,s1;
		s<<oPath.Path();
		s<<"/";
		s<<poNode->sName;
		status_t  uRes = BF_Roster_TraverseLink(s.String(),s1);
		/* check result */
		if(uRes!=B_OK){
			BF_Dialog_Alert_Sep_Error(uRes,this);			
			return false;
		}else{
			/* result is ok */
			BF_Path oNewPath(s1.String());
			NavGoPath(oNewPath);
			return true;
		}
	}
	
	switch(poNode->iType){
	case BF_NODE_TYPE_PARENTDIR:{
		NavGoParentDir();
		break;}
	/* go to select dir */
	case BF_NODE_TYPE_DIR:{
		BF_Path oNewPath = oPath;
		if(!oNewPath.GoLocalDir(poNode->sName)) return(true);		
		NavGoPath(oNewPath,NULL,true);
		break;}		
	case BF_NODE_TYPE_FILE:{

		// open archives //
		if(false)
		{
			// check for type filling //
			if(poNode->sType==""){
				// load mime_type //
				BF_Roster_ReloadNode(oPath.Path(),poNode,BF_ROSTER_LOAD_TYPE);
			}
			// check for type==zip //
			if(poNode->sType=="application/zip"){
				BF_GUI_ArcPanel_OpenFile(oPath.Path(),poNode->Name(),bWinPos_OnLeft);
				break;
			}
		}
		Action_Run_File(poNode);
		break;}
	};
	
	return true;		
}

void					
BF_GUI_FilesPanel::Pub_GoPath(const char *pc_Path)
{
	ASSERT(pc_Path);
	BF_Path o(strlen(pc_Path)>0?pc_Path:"/boot/home");		
	NavGoPath(o);	
}

void
BF_GUI_FilesPanel::AttachedToWindow(void) 
{
	BF_GUI_NodePanel::AttachedToWindow();
	//BF_Path o(bWinPos_OnLeft?"/boot/home":"/boot/home");		
	//NavGoPath(o);
}

void
BF_GUI_FilesPanel::ActionDelete_Run(BF_NodeCollection & lo_Node)
{	
	BF_GUI_NodePanel::ActionDelete_Run(lo_Node);
	BF_GUI_OperRoster_AddTask( new BF_GUI_FilesPanel_DeleteTask(oPath,lo_Node,
		BF_MSG_FILEPANEL_DELETE_TRASH==iTempDelType?BF_FILESPANEL_DELETE_TO_TRASH:BF_FILESPANEL_DELETE_TO_VOID));
}
void
BF_GUI_FilesPanel::Action_CopyMoveTo_Run(BL_String & s_Path,BF_NodeCollection & lo_Node,bool b_Move)
{
	BF_GUI_NodePanel::ActionCopyTo_Run(s_Path,lo_Node);	
	BF_FilesPath oDestPath(s_Path.String());
	
	if(oDestPath.IsStorageKit() || oDestPath.String().FindFirst("//")<0){
		BF_GUI_OperRoster_AddTask( new BF_GUI_FilesPanel_CopyTask(oPath,oDestPath,lo_Node,b_Move));	
	}else{
		BF_GUI_Panel *po = poWinView->PanelOnTop(!bWinPos_OnLeft);		
		if(!po) return;
		BL_String sSelfPath(oPath.Path());
		po->ActionCopyFrom_Run(sSelfPath,lo_Node,b_Move);
		//BF_Dialog_Alert_Sep("Error","sorry, this method is not implemented yet",NULL,this);
	}
}


void
BF_GUI_FilesPanel::ActionCopyTo_Run(BL_String & s_Path,BF_NodeCollection & lo_Node)
{
	BF_GUI_NodePanel::ActionCopyTo_Run(s_Path,lo_Node);
	Action_CopyMoveTo_Run(s_Path,lo_Node,false);
}

void
BF_GUI_FilesPanel::ActionMoveTo_Run(BL_String & s_Path,BF_NodeCollection & lo_Node)
{
	BF_GUI_NodePanel::ActionMoveTo_Run(s_Path,lo_Node);
	Action_CopyMoveTo_Run(s_Path,lo_Node,true);
}

void
BF_GUI_FilesPanel::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what){
	/////// make sh_link
	case BF_MSG_FILEPANEL_MAKESH_DIALOG:
		Action_MakeSH_Dialog();
		break;
	case BF_MSG_FILEPANEL_MAKESH_RUN:
		Action_MakeSH_Run(po_Message);
		break;
	///// go to path
	case BF_MSG_FILEPANEL_GOPATH_PREPARE:{
		Action_GoPath_StartDialog();
		break;}
	case BF_MSG_FILEPANEL_GOPATH_RUN:{
		BF_GUI_Func_PanelsEnable(true);
		MakeFocus();
		Action_GoPath_Run(po_Message);
		break;}
	///// show tracker add-ons //
	case BF_MSG_FILEPANEL_TRACKER_ADDONS_START:
		Action_Tracker_Addons_Start();
		break;
	case BF_MSG_FILEPANEL_TRACKER_ADDONS_SELECTED:{
		BF_GUI_Func_PanelsEnable(true);
		MakeFocus();
		const char *pcItem = po_Message->FindString("menu_code");
		if(!pcItem || pcItem[0]=='\0') return;
		Action_Tracker_Addons_Run(pcItem);
		break;}
	// shoow attributes //
	case BF_MSG_FILEPANEL_SHOW_ATTRDIALOG:{
		BF_Node *poNode = Nodes_Focus();
		if(!poNode) break;
		BF_GUI_AttrDialog_Run(oPath,*poNode,this);
		break;}
	///// edit ///	
	case BF_MSG_FILEPANEL_EDIT:	
		Action_Edit_File();
		break;
	///// show text file
	case BF_MSG_FILEPANEL_TEXTVIEWER_START:{		
		Action_TextViewer_Start();
		break;}
	case BF_MSG_TEXTVIEWER_CLOSED:
		BF_GUI_Func_PanelsShow(true);
		MakeFocus();
		break;
	///////
	case BF_MSG_FILEPANEL_GOPATH_NOW:{
		Action_GoPath_Run(po_Message);
		break;}
	////// set friend_panel to my path//
	case BF_MSG_FILEPANEL_SET_FRIENDPATH_TOEQUAL:{
		LOCK_WIN();
		BF_GUI_Panel *poFriend = poWinView->PanelOnTop(!bWinPos_OnLeft);
		if(poFriend){
			BMessage oMessage(BF_MSG_FILEPANEL_GOPATH_NOW);
			BMessenger oMessenger(poFriend);
			oMessage.AddString("bf_cPath",oPath.Path());
			oMessenger.SendMessage(&oMessage);
		}
		break;}
	///// refresh panel ////
	case BF_MSG_FILEPANEL_REFRESH:{		
		Pub_GoPath(oPath.Path());
		break;}
	///// show info_panel ///
	case BF_MSG_FILEPANEL_SHOW_INFOPANEL:{
		if(!Enabled()) return;
		BF_GUI_Panel *poFriendPanel = poWinView->PanelOnTop(!bWinPos_OnLeft);
		if(poFriendPanel && strstr(poFriendPanel->Name(),"info_panel_")!=NULL){
			// close old info_panel
			poWinView->RemovePanel(poFriendPanel);
		}else{
			// make new info_panel
			BF_GUI_FilesPanel_InfoPanel *poPanel = new BF_GUI_FilesPanel_InfoPanel(poWinView->PanelRect(!bWinPos_OnLeft),!bWinPos_OnLeft);
			poWinView->AddPanel(poPanel);
			SayFriend_About_CursorChanged_Ext();
		}
		break;}
	///// show image //
	case BF_MSG_FILEPANEL_IMAGEVIEWER_START:{
		if(!Enabled()) return;
		BF_GUI_Panel *poFriendPanel = poWinView->PanelOnTop(!bWinPos_OnLeft);
		if(poFriendPanel && strstr(poFriendPanel->Name(),"image_panel_")!=NULL){
			// close old info_panel
			poWinView->RemovePanel(poFriendPanel);
		}else{
			BF_GUI_ImagePanel *poPanel = new BF_GUI_ImagePanel(poWinView->PanelRect(!bWinPos_OnLeft),"image",!bWinPos_OnLeft);
			poWinView->AddPanel(poPanel);
			SayFriend_About_CursorChanged_Ext();			
		}
		break;}
	///// make links ///
	case BF_MSG_NODEPANEL_MAKELINK_STARTDIALOG:
		Action_MakeLinks_StartDialog();
		break;
	case BF_MSG_NODEPANEL_MAKELINK_STARTTASK:
		BF_GUI_Func_PanelsEnable(true);
		MakeFocus();
		Action_MakeLinks_StartTask(po_Message);
		break;
	///// change path ///
	case BF_MSG_FILEPANEL_CHANGE_CASE:
		Action_ChangeCase_0();
		break;
	case BF_MSG_FILEPANEL_CHANGE_CASE_RUN:
		BF_GUI_Func_PanelsEnable(true);
		MakeFocus();
		Action_ChangeCase_1(po_Message);
		break;
	///// set path /////
	case BF_MSG_TO_PANEL_SET_PATH:{
		const char *pcPath=NULL;
		ASSERT(B_OK==po_Message->FindString("bf_cPath",&pcPath) && pcPath);
		BF_Path oNewPath(pcPath);
		NavGoPath(oNewPath);
		break;}
	///// delete files ///
	case BF_MSG_FILEPANEL_DELETE_TRASH:
	case BF_MSG_FILEPANEL_DELETE_VOID:
		iTempDelType = po_Message->what;
		ActionDelete_Start();
		break;
	///// search ///////
	case BF_MSG_FILEPANEL_SEARCH_START:
		Action_Search_0();
		break;
	case BF_MSG_FILEPANEL_SEARCH_SETUP_CLOSE:
		Action_Search_1(po_Message);
		break;
	case BF_MSG_FILEPANEL_SEARCH_CLOSE:{
		printf("BF_MSG_FILEPANEL_SEARCH_CLOSE received\n");
		//
		BF_GUI_Func_PanelsEnable(true);
		MakeFocus();
		//
		const char *pcItem = po_Message->FindString("results_code");
		if(pcItem){
			BEntry oEntry(pcItem);									
			BF_Path oNewPath(pcItem);
			BL_String s;
			oNewPath.LastDir(s);
			oNewPath.GoParent();			
			NavGoPath(oNewPath,&s);
		}		
		break;}
	//// load files ////
	case BF_MSG_FILEPANEL_LOADNODES_STEP:{
		iThreadLoading_Count = po_Message->FindInt32("bf_Count");
		iThreadLoading_Index = po_Message->FindInt32("bf_Index");				
		printf("BF_MSG_FILEPANEL_ADDNODE received \n");		
		DrawListInfo(true);				
		break;}
	case BF_MSG_FILEPANEL_LOADNODES_FINISHED:{
		BF_NodeList *ploNode=NULL;
		ASSERT(B_OK==po_Message->FindPointer("bf_Nodes",(void**)&ploNode) && ploNode);		
		
		BF_GUI_Func_PanelsEnable(true);
		if(poSysKeysMenu) 	PrepareKeysMenu();	
		
		idThreadLoading = 0;
		loNode.DeleteItems();
		loNode.AddList(ploNode);
		ploNode->MakeEmpty();
		DELETE(ploNode);
						
		loNode.Sort(iSortType );
		
		//set new focus //
		if(sThreadLoading_FutureFocus!=""){
			iNavCursorIndex = loNode.IndexByName(sThreadLoading_FutureFocus.String());
			// draw panel ///
			iNavFirstIndex = 0;
			Invalidate(Bounds());
			NavChangeCursor(iNavCursorIndex);
		}else{			
			// draw panel ///
			iNavCursorIndex = 0;
			iNavFirstIndex = 0;			
			Invalidate(Bounds());
		}								
		sThreadLoading_FutureFocus = "";
		
		// prepare new monitoring //
		if(!(poSysSetup->MainStyle() & BF_SETUP_DEB_NOMONITORING)){
			/* stop monitoring */		
			BF_Roster_StopAllMonitors(this);
			/* start monitoring */		
			BF_Roster_StartMonitor_Dir(oPath,this);	
			BF_Roster_StartMonitor_Nodes(loNode,this);
		}
		
		break;}
	//////////////////////////
	case B_NODE_MONITOR:{
		HandlingMonitor(*po_Message);
		break;}
	default:
		BF_GUI_NodePanel::MessageReceived(po_Message);
	};
}

bool 					
BF_GUI_FilesPanel::NavChangeCursor(int i_NewCursor,bool b_DrawNodes,bool b_ChangeVars)
{
	bool bRes = BF_GUI_NodePanel::NavChangeCursor(i_NewCursor,b_DrawNodes,b_ChangeVars);
	SayFriend_About_CursorChanged_Ext();			
	return bRes;
}

void
BF_GUI_FilesPanel::SayFriend_About_CursorChanged_Ext()
{
	BF_Node* poNodeFocus = Nodes_Focus();
	if(!poNodeFocus) return ;
	
	SayFriend_About_CursorChanged(oPath.Path(),poNodeFocus->sName.String(),poNodeFocus->iType);
}


void			
BF_GUI_FilesPanel::ActionMakeDir_Run(BL_String & s_Title)
{
	// check zero title
	if(s_Title==""){
		BF_Dialog_Alert_Sep(B_TRANSLATE(BF_DICT_ERROR),B_TRANSLATE(BF_DICT_FILES_MKFOLDEMPTY),NULL,this);
		return;
	}	
	// check drive for read_only //
	if(oPath.IsVolumeReadOnly()){
		BF_Dialog_Alert_Sep(B_TRANSLATE(BF_DICT_WARNING),B_TRANSLATE(BF_DICT_FILES_VOLREAD),NULL,this);
		return;
	}
	// 
	BL_String s(oPath.Path());
	s<<"/";
	s<<s_Title;
	
	status_t uRes = BF_Roster_MakeDir(s.String());
	if(B_OK!=uRes){
		BL_System_TranslError(uRes,s);
		BF_Dialog_Alert_Sep(B_TRANSLATE(BF_DICT_FILES_ERRMKDIR),s.String(),NULL,this);
	}	
}

void					
BF_GUI_FilesPanel::ActionRename_Run(BF_NodeCollection & lo_OpNode,BL_String & s_NewName)
{
	BF_GUI_NodePanel::ActionRename_Run(lo_OpNode,s_NewName);	
	BF_GUI_OperRoster_AddTask( new BF_GUI_FilesPanel_Rename(oPath,lo_OpNode,s_NewName));	
}

void					
BF_GUI_FilesPanel::Nodes_Add(BF_Node *po_Node,BF_NodeCollection *plo_Node)
{
	BF_GUI_NodePanel::Nodes_Add(po_Node,plo_Node);
	
	if(!(poSysSetup->MainStyle() & BF_SETUP_DEB_NOMONITORING)){
		if(po_Node)		BF_Roster_StartMonitor_Node(po_Node,this);	
		if(plo_Node)	BF_Roster_StartMonitor_Nodes(*plo_Node,this);	
	}
}

void					
BF_GUI_FilesPanel::Nodes_Remove(BF_Node *po_Node,BF_NodeCollection *plo_Node)
{
	BF_GUI_NodePanel::Nodes_Remove(po_Node,plo_Node);
	
	if(!(poSysSetup->MainStyle() & BF_SETUP_DEB_NOMONITORING)){
		if(po_Node)		BF_Roster_StopMonitor_Node(po_Node,this);	
		if(plo_Node)	BF_Roster_StopMonitor_Nodes(*plo_Node,this);	
	}
}

void
BF_GUI_FilesPanel::Action_ChangeCase_0()
{
	BL_List *ploMenu = new BL_List();
	ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_FILES_TOLOWER),"lower"));
	ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_FILES_TOUPPER),"upper"));
	ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_FILES_CAPITALISE),"cap"));
	ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_FILES_CAPITALISEEW),"cap_ew"));
	ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_CANCEL),"cancel"));
	
	BF_Dialog_Alert_Sep(B_TRANSLATE(BF_DICT_FILES_CHANGENAMES),"Method:",ploMenu,this,"",BF_MSG_FILEPANEL_CHANGE_CASE_RUN);
}

void
BF_GUI_FilesPanel::Action_ChangeCase_1(BMessage *po_Message)
{
	ASSERT(po_Message);
		
	int32 iIndex=-1;
	if(B_OK != po_Message->FindInt32("menu",&iIndex) || iIndex>3) return;
	
	BF_NodeCollection *ploNode = Nodes_GetSelecting();		
	BF_GUI_OperRoster_AddTask( new BF_GUI_FilesPanel_ChangeCaseTask(oPath,*ploNode,iIndex));
	DELETE(ploNode);
}

void
BF_GUI_FilesPanel::Action_MakeLinks_StartDialog()
{
	// prepare dest_path
	BL_String sDestPath;
	BF_GUI_Panel *poDestPanel = poWinView->PanelOnTop(!bWinPos_OnLeft);
	sDestPath = poDestPanel?poDestPanel->Path():Path();
		
	// make dialog
	{	
		/* */
		BF_GUI_Func_PanelsEnable(false);
		/* make dialog */	
		BMessage oMessage(BF_MSG_NODEPANEL_MAKELINK_STARTTASK);
		oMessage.AddPointer("bf_focus",this);
				
		BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(BRect(0,0,300,0),
			B_TRANSLATE(BF_DICT_FILES_MKLINKS),"dialog",oMessage,BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER);
		BRect oRect;	
		/* insert edit */
		poDialog->LocalBounds(oRect);	
		oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
		BF_GUI_ViewEdit_Create(oRect,B_TRANSLATE(BF_DICT_PATH),poDialog,"bf_cPath",
						sDestPath.String(),
						B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
		/* menu */								
		poDialog->AddOkCancelMenu(oRect);		
		/* finish */	
		poDialog->SetHeightFromLastChild();
		poDialog->MoveToCenter( poWinView );	
		BF_GUI_Func_AddChildToMainView ( poDialog );		
	}
}	

void					
BF_GUI_FilesPanel::Action_MakeLinks_StartTask(BMessage *po_Message)
{
	ASSERT(po_Message);
	const char *pcDestPath=NULL;
	if(B_OK!=po_Message->FindString("bf_cPath",&pcDestPath) || !pcDestPath) return;
	
	BF_FilesPath oDestPath(pcDestPath);	
	
	BF_NodeCollection *ploNode = Nodes_GetSelecting();		
	BF_GUI_OperRoster_AddTask( new BF_GUI_FilesPanel_MakeLinksTask(oPath,oDestPath,*ploNode));
	DELETE(ploNode);
	
}


//06012000 #include "BF_GUI_TextViewerShell.h"
void
BF_GUI_FilesPanel::Action_TextViewer_Start()
{
	/* debug code 
	printf("cursor %i\n",iNavCursorIndex);
	BL_String s;	
	s<<iNavCursorIndex;
	s<<(bWinPos_OnLeft?" left":"right");
	BF_Dialog_Alert_Sep("info",s.String(),NULL,this);
	*/

	if(!Enabled()) return;
	BF_Node *poNode = Nodes_Focus();
	if(!poNode) return;
	//	
	switch(poNode->iType){
	case BF_NODE_TYPE_FILE:{
		BString s;
		s<<oPath.Path();
		s<<"/";
		s<<poNode->sName;
		
		BF_GUI_TextViewer_Run(s.String());
		
		/* 06012000
		BRect oRect(poWinView->Bounds());
		oRect.bottom-=fNodeHeight;
		// send message to main window 
		BF_GUI_Func_PanelsShow(false);
		// make viewer		
		BF_GUI_TextViewerShell *poViewer=new BF_GUI_TextViewerShell(oRect,s.String(),this);
		poWinView->AddChild(poViewer);
		poViewer->MakeFocus();
		*/
		break;}
	}	
}

void
BF_GUI_FilesPanel::Action_Run_File(BF_Node *po_Node)
{
	if(!po_Node) return;
	
	// try to run entry //
	{
		BL_String s(oPath.Path());
		s<<"/";
		s<<po_Node->Name();
		if(B_OK==BF_Roster_RunEntry(BEntry(s.String()))) return;
	}
	
	if(BF_Roster_RunFile(oPath.Path(),po_Node->Name())==0) return;
		
	
		//BF_Node *poInfo = BF_Roster_LoadNode(oPath.Path(),po_Node->Name(),BF_ROSTER_LOAD_TYPE|BF_ROSTER_LOAD_LICON);
		//if(!poInfo){
		//
	Action_Alert(B_TRANSLATE(BF_DICT_ERROR),B_TRANSLATE(BF_DICT_FILES_CANTRUN));
	return;
		//}
		//delete poInfo;
			
}

void		
BF_GUI_FilesPanel::Action_Edit_File(BF_Node *po_Node)
{
	if(!po_Node) po_Node = Nodes_Focus();
	if(!po_Node || po_Node->iType!=BF_NODE_TYPE_FILE) return;

	Action_Run_File(po_Node);	
}

void		
BF_GUI_FilesPanel::Action_CopyToBuffer()
{
	BF_NodeCollection loSelNode;
	loNode.GetSelecting(loSelNode);
	if(loSelNode.CountItems()==0){
		BF_Node *poNode = Nodes_Focus();
		if(!poNode) return;
		loSelNode.AddItem(poNode);
	}
		
	BL_String 	s;
	BF_Node		*poNode=NULL;
	for(int i=0;i<loSelNode.CountItems();i++){
		poNode = loSelNode.NodeAt(i);
		if(i>0) s<<"\n";
		s<<oPath.Path();
		s<<"/";
		s<<poNode->Name();
	}
	
	if(be_clipboard->Lock()){					
		be_clipboard->Clear();			
		BMessage *po = be_clipboard->Data();
		po->AddData("text/plain", B_MIME_TYPE, s.String(),s.Length());		
		be_clipboard->Commit();
		be_clipboard->Unlock();
	}	
}

void		
BF_GUI_FilesPanel::Action_PasteFromBuffer()
{
	BL_String sData;
	
	Debug_Info("Action_PasteFromBuffer()");
	
	if(be_clipboard->Lock()){					
		be_clipboard->Clear();			
		BMessage *po = be_clipboard->Data();				
		
		const char *pcText;
		ssize_t iTextLen=0;
		status_t uRes = po->FindData("text/plain", B_MIME_TYPE,(const void **)&pcText, &iTextLen);
		be_clipboard->Commit();
		be_clipboard->Unlock();
		
		if(!pcText) sData = pcText;
	}			
	if(sData=="") return;
		
	Debug_Info(sData.String());
}

void					
BF_GUI_FilesPanel::Action_PasteFromBuffer_Step2(BMessage *po_Message)
{
	
}

void					
BF_GUI_FilesPanel::Action_GoPath_StartDialog()
{
	// make dialog
	{	
		/* */
		BF_GUI_Func_PanelsEnable(false);
		/* make dialog */	
		BMessage oMessage(BF_MSG_FILEPANEL_GOPATH_RUN);
		oMessage.AddPointer("bf_focus",this);
				
		BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(BRect(0,0,300,0),
			B_TRANSLATE(BF_DICT_GOTO),"dialog",oMessage,BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER);
		BRect oRect;	
		/* insert edit */
		poDialog->LocalBounds(oRect);	
		oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
		BF_GUI_ViewEdit_Create(oRect,B_TRANSLATE(BF_DICT_PATH),poDialog,"bf_cPath",
						"",
						B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
		/* menu */								
		poDialog->AddOkCancelMenu(oRect);		
		/* finish */	
		poDialog->SetHeightFromLastChild();
		poDialog->MoveToCenter( poWinView );	
		BF_GUI_Func_AddChildToMainView( poDialog );		
	}	
}

void					
BF_GUI_FilesPanel::Action_GoPath_Run(BMessage *po_Message)
{
	ASSERT(po_Message);
	const char *pcDestPath=NULL;
	if(B_OK!=po_Message->FindString("bf_cPath",&pcDestPath) || !pcDestPath) return;
	
	if(oPath==pcDestPath) return;
	
	//bool bExist=BF_Roster_NodeExists(pcDestPath);
	status_t uRes = BF_Roster_MakeDir(pcDestPath);
	
	if(uRes!=B_OK){
		BF_Dialog_Alert_Sep_Error(uRes,this);
		return;
	}
	
	Pub_GoPath(pcDestPath);
}

