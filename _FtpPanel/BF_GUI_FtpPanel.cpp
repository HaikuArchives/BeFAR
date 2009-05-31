#include "Directory.h"
#include "Mime.h"

#include "BF_Dict.h"
#include "BF_Roster.h"
#include "BF_GUI_KeysMenu.h"
#include "BF_GUI_WinMain.h"

#include "BF_GUI_FtpPanel.h"
#include "BF_GUI_FtpPanel_Tasks.h"



#define SETTINGS_FTP_DIR	(const char*)"/boot/home/config/settings/BeFAR/ftp"

/////////////////////////////////////////////////////////////////

void					
BF_FtpPath::SetFromFtp(const char *pc_Path)
{
	BL_String s(pc_Path);		
	int i1 = s.FindFirst("\"");		
	BL_String s1(s);
	if(i1>=0) s.CopyInto(s1,0,i1);
	
	BF_Path::SetTo(s1.String());
}

/////////////////////////////////////////////////////////////////

BF_GUI_FtpPanel_Bookmark::BF_GUI_FtpPanel_Bookmark()
{
	iPort = 21;
	bPassive = true;
	
	bAscii_Mode = false;
}

void					
BF_GUI_FtpPanel_Bookmark::operator = (BF_GUI_FtpPanel_Bookmark & o_Source)
{
	sHost = o_Source.sHost;
	sUserName = o_Source.sUserName;
	sUserPassword = o_Source.sUserPassword;
	sProxy = o_Source.sProxy;
	sFolder = o_Source.sFolder;
	iPort = o_Source.iPort;	
	bPassive = o_Source.bPassive;		
	bAscii_Mode = o_Source.bAscii_Mode;			
}

/////////////////////////////////////////////////////////////////
BF_GUI_FtpPanel_NodeBookmark::BF_GUI_FtpPanel_NodeBookmark()
{
	poBookmark = NULL;
}

BF_GUI_FtpPanel_NodeBookmark::~BF_GUI_FtpPanel_NodeBookmark()
{
	if(poBookmark) DELETE(poBookmark);
}


/////////////////////////////////////////////////////////////////
BF_GUI_FtpPanel::BF_GUI_FtpPanel(const BRect & o_Rect,bool b_WinPos_OnLeft)
:BF_GUI_NodePanel(o_Rect,b_WinPos_OnLeft)
{	
	//iModeColsStyles = iModeColsStyles | BF_NODEPANEL_COLS_SICON;
	//iColsColCount = 2;
	//bCanSelect = false;
	//
	iVisMode = BF_FTPPANEL_VISMODE_BOOKMARKS;
	iState = BF_FTPPANEL_STATE_DEFAULT;
	poClient = new FtpClient(this);
	uConnectThread = 0;
	poBookmark = NULL;	
	oBookmarkDir.SetTo(SETTINGS_FTP_DIR,NULL);
	{ // load icons ///
		poFolderSIcon = BL_Load_SIconFromMIME(BL_MIME_FOLDER);
		poBookmarkSIcon = BL_Load_SIconFromMIME(BL_MIME_BOOKMARK);
		poDocSIcon	= BL_Load_SIconFromMIME(BL_MIME_FILE);
	}
	///
	Calc();
	RefreshNodeList();
	///	
}

const BL_String	
BF_GUI_FtpPanel::Path() const
{
	BL_String sResult("//ftp:");
	return sResult;
}

BF_GUI_FtpPanel::~BF_GUI_FtpPanel()
{
	if(poClient)		DELETE(poClient);
	if(poFolderSIcon) 	DELETE(poFolderSIcon);
	if(poBookmarkSIcon) DELETE(poBookmarkSIcon);
	if(poDocSIcon) 		DELETE(poDocSIcon);
}

void
BF_GUI_FtpPanel::PrepareKeysMenu()
{
	BF_GUI_NodePanel::PrepareKeysMenu();
	uint32 	iKeysModifiers = modifiers();
	
	if(iKeysModifiers & B_RIGHT_OPTION_KEY){	
	}else{					
		switch(iVisMode){
		case BF_FTPPANEL_VISMODE_BOOKMARKS:{
			if(iState==BF_FTPPANEL_STATE_DEFAULT){
				poSysKeysMenu->SetText(4,BF_DictAt(BF_DICT_EDIT));
				poSysKeysMenu->SetMessage(4,new BMessage(BF_FTP_MSG_EDIT_BOOKMARK),this);
				poSysKeysMenu->SetText(7,BF_DictAt(BF_DICT_CREATE));
				poSysKeysMenu->SetMessage(7,new BMessage(BF_FTP_MSG_BOOKMARK_CREATE),this);			
				poSysKeysMenu->SetText(8,BF_DictAt(BF_DICT_DELETE));
				poSysKeysMenu->SetMessage(8,new BMessage(BF_FTP_MSG_BOOKMARK_DELETE),this);
			}
			break;}
		case BF_FTPPANEL_VISMODE_FTPLIST:{
			if(iState==BF_FTPPANEL_STATE_CONNECTED){
				poSysKeysMenu->SetText(3,BF_DictAt(BF_DICT_FTP_DISCONNECT));
				poSysKeysMenu->SetMessage(3,new BMessage(BF_FTP_MSG_DISCONNECT),this);			
				poSysKeysMenu->SetText(5,BF_DictAt(BF_DICT_COPY));
				poSysKeysMenu->SetMessage(5,new BMessage(BF_MSG_NODEPANEL_COPY_START),this);			
				poSysKeysMenu->SetText(7,BF_DictAt(BF_DICT_FTP_MAKEDIR));
				poSysKeysMenu->SetMessage(7,new BMessage(BF_MSG_NODEPANEL_MAKEDIR_START),this);			
				poSysKeysMenu->SetText(8,BF_DictAt(BF_DICT_DELETE));
				poSysKeysMenu->SetMessage(8,new BMessage(BF_MSG_NODEPANEL_DELETE_START),this);	
			}
			break;}
		};
	}				
}


void
BF_GUI_FtpPanel::RefreshNodeList()
{	
	switch(iVisMode){
	case BF_FTPPANEL_VISMODE_BOOKMARKS:{
		LoadBookmarks();
		break;}
	}
}


#define BF_GUI_KEY_SYS  B_CONTROL_KEY | B_OPTION_KEY | B_COMMAND_KEY

void 							
BF_GUI_FtpPanel::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what){
	////////////////////////////////	
	case BF_FTP_MSG_DIR_RELOAD_START:
		Act_ReloadDir_Start();
		break;
	case BF_FTP_MSG_DIR_CHANGED:{
		SetHeader(BF_DictAt(BF_DICT_FTP_CONNECTED),poFolderSIcon);
		uConnectThread = 0;
		iState = BF_FTPPANEL_STATE_CONNECTED;
		BL_String s;
		s = po_Message->FindString("list");
		Ftp_ListLoaded(s);
		//
		break;}	
	case BF_FTP_MSG_DIR_RELOAD:{
		SetHeader(BF_DictAt(BF_DICT_FTP_CONNECTED),poFolderSIcon);
		uConnectThread = 0;
		iState = BF_FTPPANEL_STATE_CONNECTED;
		BL_String s;
		s = po_Message->FindString("list");
		Ftp_ListLoaded(s);
		break;}
	////////////////////////////////	
	case BF_FTP_MSG_CONNECTED:{
		oPath.SetFromFtp( po_Message->FindString("path") );		
		uConnectThread = 0;
		iState = BF_FTPPANEL_STATE_CONNECTED;
		BL_String s;		
		s = po_Message->FindString("list");		
		Ftp_ListLoaded(s);
		//
		break;}
	////////////////////////////////	
	case BF_FTP_MSG_MESSAGE:{
		char *pcMessage=NULL;
		pcMessage = (char*)po_Message->FindString("message");
		BF_Dialog_Alert_Sep(BF_DictAt(BF_DICT_FTP_ERROR),pcMessage,NULL,this);
		break;}
	////////////////////////////////
	case BF_FTP_MSG_BOOKMARK_CREATE:{
		Bookmark_Create();
		break;}
	case BF_FTP_MSG_BOOKMARK_DELETE:{
		Bookmark_Delete();
		break;}
	case BF_FTP_MSG_BOOKMARK_DELETE_ANSWER:{
		int32 iResult=-1;
		ASSERT(B_OK==po_Message->FindInt32("menu",&iResult));
		if(iResult==0) Bookmark_Delete_Yes();
		/* enable panels */
		BF_GUI_Func_PanelsEnable(true);
		MakeFocus();
		//
		break;}
	case BF_FTP_MSG_EDIT_BOOKMARK:{			
		Bookmark_Edit();		
		break;}
	//////////////////////////////// 
	case BF_FTP_MSG_BOOKMARK_EDIT:{				
		/* enable panels */
		BF_GUI_Func_PanelsEnable(true);
		MakeFocus();
		/* change bookmark_info */
		bool bOk;
		ASSERT(B_OK==po_Message->FindBool(BF_GUI_DIALOG_MESSAGE_DATA_OK,&bOk));				
		if(bOk && po_Message->FindInt32("menu")==0){			
		
			BF_GUI_FtpPanel_NodeBookmark *poNode = (BF_GUI_FtpPanel_NodeBookmark*)Nodes_Focus();			
			if(!poNode) return;
			char *pcValue;
			BL_String sOldTitle(poNode->poBookmark->sTitle.String());
			
			ASSERT(poNode->poBookmark);						
			
			ASSERT(B_OK==po_Message->FindString("title",(const char**)&pcValue) && pcValue);
			poNode->poBookmark->sTitle = pcValue;
			poNode->sName = pcValue;
			
			ASSERT(B_OK==po_Message->FindString("host",(const char**)&pcValue) && pcValue);
			poNode->poBookmark->sHost = pcValue;			
			ASSERT(B_OK==po_Message->FindString("user_name",(const char**)&pcValue) && pcValue);
			poNode->poBookmark->sUserName = pcValue;
			ASSERT(B_OK==po_Message->FindString("user_password",(const char**)&pcValue) && pcValue);
			poNode->poBookmark->sUserPassword = pcValue;
			ASSERT(B_OK==po_Message->FindString("folder",(const char**)&pcValue) && pcValue);
			poNode->poBookmark->sFolder = pcValue;
			ASSERT(B_OK==po_Message->FindBool("passive",&poNode->poBookmark->bPassive));
			ASSERT(B_OK==po_Message->FindBool("ascii",&poNode->poBookmark->bAscii_Mode));
			
			Bookmark_Save(sOldTitle.String(),poNode);  
															
			DrawNodeAt(iNavCursorIndex);
		}
		break;}
	////////////////////////////////		
	case BF_FTP_MSG_DISCONNECT:{
		Ftp_Disconnect();
		break;}
	////////////////////////////////
	default:
		BF_GUI_NodePanel::MessageReceived(po_Message);
	};
}

bool							
BF_GUI_FtpPanel::NavEnter()
{	
	///
	switch(iVisMode){
	case BF_FTPPANEL_VISMODE_BOOKMARKS:{
		Act_Connect();
		break;}
	case BF_FTPPANEL_VISMODE_FTPLIST:{
		switch(iState){
		case BF_FTPPANEL_STATE_CONNECTED:{
			BF_Node *poNode = Nodes_Focus();
			if(!poNode) return true;
			
			switch(poNode->iType){
			case BF_NODE_TYPE_PARENTDIR:{
				Act_ChangeDir_Start(poNode);
				return true;
				break;}
			case BF_NODE_TYPE_DIR:{
				Act_ChangeDir_Start(poNode);
				return true;
				break;}
			} 
			
			break;}
		}
		break;}
	};
	return true;
}

void
BF_GUI_FtpPanel::Bookmark_Edit()
{
	BF_GUI_FtpPanel_NodeBookmark *poNode = (BF_GUI_FtpPanel_NodeBookmark*)Nodes_Focus();
	if(!poNode) return;
	ASSERT(poNode->poBookmark);
	
	BMessage oMessage(BF_FTP_MSG_BOOKMARK_EDIT);
	oMessage.AddPointer("bf_focus",this);
	
	/* */
	BF_GUI_Func_PanelsEnable(false);
	/* make dialog */	
	BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(BRect(0,0,300,0),
		BF_DictAt(BF_DICT_FTP_EDITBKM),"dialog",oMessage,BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER);	
	//poDialog->SetSizeBy( 300 , poSysSetup->oFontToolView.fHeight*7 + 5*6 );
	//poDialog->MoveToCenter( poWinView );
	BRect oRect;	
	////////////////////
	poDialog->LocalBounds(oRect);	
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	BF_GUI_ViewCheck 	*poCheck;
	/* insert title */		
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	BF_GUI_ViewEdit_Create(oRect,BF_DictAt(BF_DICT_FTP_BKM_TITLE),poDialog,"title",poNode->poBookmark->sTitle.String(),
					B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);	
	/* insert host */		
	oRect.top = oRect.bottom+5;
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	BF_GUI_ViewEdit_Create(oRect,BF_DictAt(BF_DICT_FTP_BKM_HOST),poDialog,"host",poNode->poBookmark->sHost.String(),
					B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
	///
	oRect.top = oRect.bottom+5;
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	BF_GUI_ViewEdit_Create(oRect,BF_DictAt(BF_DICT_FTP_BKM_USERNAME),poDialog,"user_name",poNode->poBookmark->sUserName.String(),
					B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
	///
	oRect.top = oRect.bottom+5;
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	BF_GUI_ViewEdit_Create(oRect,BF_DictAt(BF_DICT_FTP_BKM_PWD),poDialog,"user_password",poNode->poBookmark->sUserPassword.String(),
					B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
	// folder  //
	oRect.top = oRect.bottom+5;
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	BF_GUI_ViewEdit_Create(oRect,BF_DictAt(BF_DICT_FTP_BKM_FOLDER),poDialog,"folder",poNode->poBookmark->sFolder.String(),
					B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
	// insert "passive"
	oRect.top = oRect.bottom+5;
	oRect.bottom = oRect.top + poSysSetup->oFontToolView.fHeight;
	poCheck = new BF_GUI_ViewCheck(oRect,"passive",
					"passive",poNode->poBookmark->bPassive,
					B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
	poDialog->AddChild(poCheck);
	// insert "ascii"
	oRect.top = oRect.bottom+5;
	oRect.bottom = oRect.top + poSysSetup->oFontToolView.fHeight;
	poCheck = new BF_GUI_ViewCheck(oRect,"ascii",
					"ascii mode",poNode->poBookmark->bAscii_Mode,
					B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
	poDialog->AddChild(poCheck);	
		
	/* menu */
	poDialog->AddOkCancelMenu(oRect);
	/* finish */
	poDialog->SetHeightFromLastChild();
	poDialog->MoveToCenter( poWinView );
	BF_GUI_Func_AddChildToMainView( poDialog );		
	/* */			
}

////////////////////////////////////////////////////////////////////////////////////////////
void
BF_GUI_FtpPanel::LoadBookmarks()
{	
	BF_NodeList		loBookmarkNode(false);

	/// load bookmarks from config_dir //
	{			
		BF_Roster_LoadNodeList(oBookmarkDir,loBookmarkNode,BF_ROSTER_LOAD_NORMAL | BF_ROSTER_LOAD_SICON | BF_ROSTER_LOAD_NODE_FILE);
	}	
	BF_GUI_FtpPanel_NodeBookmark	*poNode; 
	char pc[400];
	status_t uRes;
	BF_Node *poBookmarkNode;
	BDirectory oDir(SETTINGS_FTP_DIR);	
	uRes = oDir.InitCheck();
	BF_NodeCollection loBukNode;
	
	for(int i=0;i<loBookmarkNode.CountItems();i++){
		poBookmarkNode = loBookmarkNode.NodeAt(i);   
		
		BFile oFile(&oDir,poBookmarkNode->sName.String(),B_READ_ONLY);
		uRes = oFile.InitCheck();
	
		poNode = new BF_GUI_FtpPanel_NodeBookmark();
		poNode->iType = BF_NODE_TYPE_FILE;
		poNode->poBookmark = new BF_GUI_FtpPanel_Bookmark();
		poNode->CopySIconFrom( poBookmarkSIcon );


		poNode->poBookmark->sTitle = poBookmarkNode->sName;
		if(oFile.ReadAttr("host",B_STRING_TYPE,0,pc,400)>0) poNode->poBookmark->sHost=pc;
		if(oFile.ReadAttr("user_name",B_STRING_TYPE,0,pc,400)>0) poNode->poBookmark->sUserName=pc;
		if(oFile.ReadAttr("user_password",B_STRING_TYPE,0,pc,400)>0) poNode->poBookmark->sUserPassword=pc;		
		if(oFile.ReadAttr("folder",B_STRING_TYPE,0,pc,400)>0) poNode->poBookmark->sFolder=pc;		
		if(oFile.ReadAttr("proxy",B_STRING_TYPE,0,pc,400)>0) poNode->poBookmark->sProxy=pc;		
		oFile.ReadAttr("passive",B_BOOL_TYPE,0,&poNode->poBookmark->bPassive,sizeof(bool));
		oFile.ReadAttr("ascii",B_BOOL_TYPE,0,&poNode->poBookmark->bAscii_Mode,sizeof(bool));
		oFile.ReadAttr("port",B_INT32_TYPE,0,&poNode->poBookmark->iPort,sizeof(B_INT32_TYPE));				
		
		
		poNode->sName = poNode->poBookmark->sTitle;		
		
		loBukNode.AddItem(poNode);
	}
	SetHeader(BF_DictAt(BF_DICT_FTP_PANELTITLE),poFolderSIcon);
	loNode.DeleteItems();
	Nodes_Add(NULL,&loBukNode);
}
////////////////////////////////////////////////////////////////////////////////////////////
void
BF_GUI_FtpPanel::Bookmark_Save(const char *pc_OldTitle,BF_GUI_FtpPanel_NodeBookmark *po_Node)
{
	BL_String s,s1;
	ASSERT(po_Node);
	
	if(B_OK!=BF_Roster_MakeDir(SETTINGS_FTP_DIR)){
		Action_Alert(BF_DictAt(BF_DICT_ERROR),BF_DictAt(BF_DICT_FTP_CANTMAKESETDIR));
		return;
	}	
	BF_GUI_FtpPanel_Bookmark *poBookmark = po_Node->poBookmark;
	ASSERT(poBookmark);
	
	/* kill old file */	
	s=SETTINGS_FTP_DIR;
	s<<"/";
	s<<pc_OldTitle;
	BF_Roster_RemoveNode(s.String());
	
	/* create new file */
	BDirectory oDir(SETTINGS_FTP_DIR);
	BFile 		oFile;
	status_t	uErr;
	
	uErr = oDir.CreateFile(poBookmark->sTitle.String(),&oFile,false);	
	if(uErr!=B_OK){
		s = BF_DictAt(BF_DICT_FTP_CANTMAKESETFILE);
		BL_System_TranslError(uErr,s1);
		s<<s1;
		Action_Alert(BF_DictAt(BF_DICT_ERROR),s1.String());
		return;
	}
	
	/* save attributes */	
	oFile.WriteAttr("host",B_STRING_TYPE,0,poBookmark->sHost.String(),poBookmark->sHost.Length()+1);
	oFile.WriteAttr("user_name",B_STRING_TYPE,0,poBookmark->sUserName.String(),poBookmark->sUserName.Length()+1);
	oFile.WriteAttr("user_password",B_STRING_TYPE,0,poBookmark->sUserPassword.String(),poBookmark->sUserPassword.Length()+1);
	oFile.WriteAttr("proxy",B_STRING_TYPE,0,poBookmark->sProxy.String(),poBookmark->sProxy.Length()+1);
	oFile.WriteAttr("folder",B_STRING_TYPE,0,poBookmark->sFolder.String(),poBookmark->sFolder.Length()+1);
	oFile.WriteAttr("passive",B_BOOL_TYPE,0,&poBookmark->bPassive,sizeof(poBookmark->bPassive));
	oFile.WriteAttr("ascii",B_BOOL_TYPE,0,&poBookmark->bAscii_Mode,sizeof(poBookmark->bAscii_Mode));
	oFile.WriteAttr("port",B_INT32_TYPE,0,&poBookmark->iPort,sizeof(B_INT32_TYPE));
}
////////////////////////////////////////////////////////////////////////////////////////////


void
BF_GUI_FtpPanel::Act_Connect()
{
	if(!Nodes_Focus()) return;
	
	iState = BF_FTPPANEL_STATE_CONNECTING;
	SetHeader(BF_DictAt(BF_DICT_FTP_CONNECTING),poFolderSIcon);
	if(poSysKeysMenu) PrepareKeysMenu();
	
	uConnectThread = spawn_thread(BF_GUI_FtpPanel__Act_Connect_Thread, "ftp_connect", 
		B_NORMAL_PRIORITY, this);				
	resume_thread(uConnectThread);	
		
	/*
	BF_Dialog_Alert_Sep("Connecting to ",poBookmark->sHost.String(),
		new BL_List(),this,NULL,BF_FTP_MSG_CONNECT_BREAK);
	*/		
}
////////////////////////////////////////////////////////////////////////////////////////////

void 					
BF_GUI_FtpPanel::Action_SwitchShowIcons()
{
	if(!ModeShowSIcon()) Ftp_LoadIcons();
	
	BF_GUI_NodePanel::Action_SwitchShowIcons();
}

void
BF_GUI_FtpPanel::Ftp_LoadIcons()
{
	switch(iVisMode){	
	case BF_FTPPANEL_VISMODE_FTPLIST:{
		BF_Node 	*poNode;	
		BMimeType 	oMimeType;
		BL_String	s;
		
		for(int i=0;i<loNode.CountItems();i++){
			poNode = loNode.NodeAt(i);
			
			switch(poNode->iType){
			case BF_NODE_TYPE_PARENTDIR:
			case BF_NODE_TYPE_DIR:
				if(poFolderSIcon)	poNode->CopySIconFrom(poFolderSIcon);
				break;
			default:
				if(	poDocSIcon )			poNode->CopySIconFrom(poDocSIcon);		
			}
		} 
		break;}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
void
BF_GUI_FtpPanel::Ftp_ListLoaded(const  BL_String & s_List)
{

	loNode.DeleteItems();
	
	int 		iStart = 0,i1,i2;
	BL_String 	sNode,s;
	BF_Node		*poNode;
	char 		pc[5];
	bool		bParentLoaded = false;
	
	pc[0] = (char) 0x0D;
	pc[1] = (char) 0x0A;	
	pc[2] = '\0';		
	
	BMimeType oMimeType;
	
	while(true){
		i1 = s_List.FindFirst(pc,iStart);
		if(i1<0) break;
		//
		s_List.CopyInto(sNode,iStart,i1-iStart);
		iStart = i1+2;
		//
		if(sNode.FindFirst("total")==0) continue;						
		poNode = new BF_Node();
		loNode.AddItem(poNode);
		// name 
		sNode.CopyInto(poNode->sName,55,sNode.Length()-55);
		Debug_Info(poNode->sName.String());
		
		poNode->sName.LTrim();
		poNode->sName.RTrim();
		/*
		poNode->sName.RemoveFirst(" ");
		poNode->sName.RemoveLast(" ");
		*/
		
		// check spec_names 

		if(poNode->sName==".."){			
			poNode->iType = BF_NODE_TYPE_PARENTDIR;
			bParentLoaded = true;
			continue;
		}
		if(poNode->sName=="."){			
			poNode->iType = poNode->iType = BF_NODE_TYPE_PARENTDIR;;
			continue;
		}
		
		// size
		i2 = sNode.FindFirst(" ",24);
		if(i2>0){			
			sNode.CopyInto(s,i2,41-i2);
			poNode->iSize = s.Int32();
		}
		// node type 
		if(sNode.FindFirst("d")==0) poNode->iType = BF_NODE_TYPE_DIR;	else
		poNode->iType = BF_NODE_TYPE_FILE;
		//		
	}
	
	if(!bParentLoaded){
		poNode = new BF_Node("..");		
		poNode->iType = BF_NODE_TYPE_PARENTDIR;
		loNode.AddItem(poNode);
	}
	
	loNode.Sort(iSortType );		
	iVisMode = BF_FTPPANEL_VISMODE_FTPLIST;
	if(ModeShowSIcon()) Ftp_LoadIcons();
	
	NavChangeCursor(0);	
	Draw(Bounds());
	
	s = "ftp : ";
	s << oPath.Path();
	SetHeader(s.String(),poFolderSIcon);
	
	if(poSysKeysMenu) PrepareKeysMenu();
}
////////////////////////////////////////////////////////////////////////////////////////////

void
BF_GUI_FtpPanel::Act_ChangeDir_Start(BF_Node *po_Node)
{
	ASSERT(po_Node);
		
	iState = BF_FTPPANEL_STATE_DIR_CHANGING;
	SetHeader(BF_DictAt(BF_DICT_FTP_DIRCHANGING),poFolderSIcon);
	if(poSysKeysMenu) PrepareKeysMenu();
	
	uConnectThread = spawn_thread(BF_GUI_FtpPanel__Act_ChangeDir_Thread, "ftp_change_dir",	B_NORMAL_PRIORITY, this);				
	resume_thread(uConnectThread);		
	
}

void
BF_GUI_FtpPanel::Act_ReloadDir_Start()
{
	if(BF_FTPPANEL_STATE_CONNECTED!=iState){
		BF_Dialog_Alert_Sep(BF_DictAt(BF_DICT_ERROR),BF_DictAt(BF_DICT_FTP_NOTREADY),NULL,this);
		return; 
	}
	
	iState = BF_FTPPANEL_STATE_DIR_RELOAD;
	SetHeader(BF_DictAt(BF_DICT_FTP_DIRRELOADING),poFolderSIcon);
	if(poSysKeysMenu) PrepareKeysMenu();
	
	uConnectThread = spawn_thread(BF_GUI_FtpPanel__Act_ReloadDir_Thread, "ftp_reload_dir",B_NORMAL_PRIORITY,this);
	resume_thread(uConnectThread);			
}

void
BF_GUI_FtpPanel::ActionMakeDir_Run(BL_String & s_Title)
{
	if(BF_FTPPANEL_STATE_CONNECTED!=iState){
		BF_Dialog_Alert_Sep(BF_DictAt(BF_DICT_ERROR),BF_DictAt(BF_DICT_FTP_NOTREADY),NULL,this);
		return; 
	}
	
	BMessage *poMessage = new BMessage();
	poMessage->AddPointer("panel",this);
	poMessage->AddString("name",s_Title.String());
	
	uConnectThread = spawn_thread(BF_GUI_FtpPanel__Act_MakeDir_Thread, "ftp_make_dir",B_NORMAL_PRIORITY,(void*)poMessage);
	resume_thread(uConnectThread);
}

void
BF_GUI_FtpPanel::ActionDelete_Run(BF_NodeCollection & lo_Node)
{
	BF_GUI_OperRoster_AddTask(new BF_GUI_FtpPanel_DeleteTask(this,lo_Node));
}

////////////////////////////////////////////////////////////////////////////////////////////
void
BF_GUI_FtpPanel::Bookmark_Delete()
{
	// make quit_dialog //		
	BL_List *ploMenuItem = new BL_List();
	ploMenuItem->AddItem(new BF_GUI_ViewMenu_Item(BF_DictAt(BF_DICT_YES),""));
	ploMenuItem->AddItem(new BF_GUI_ViewMenu_Item(BF_DictAt(BF_DICT_NO),""));
	BF_Dialog_Alert_Sep("",BF_DictAt(BF_DICT_FTP_BKM_ASKDEL),ploMenuItem,this,NULL,BF_FTP_MSG_BOOKMARK_DELETE_ANSWER);
}

void
BF_GUI_FtpPanel::Bookmark_Delete_Yes()
{
	BF_GUI_FtpPanel_NodeBookmark *poNode = (BF_GUI_FtpPanel_NodeBookmark*)Nodes_Focus();
	if(!poNode) return;				
	BF_Roster_RemoveNode(oBookmarkDir,poNode->sName.String());		
	loNode.DeleteItemAt(iNavCursorIndex);
	
	DrawNodes(true);
	DrawListInfo(true);
	DrawSelListInfo(true);
	NavChangeCursor(iNavCursorIndex);			
}

void
BF_GUI_FtpPanel::Bookmark_Create()
{
	BF_GUI_FtpPanel_NodeBookmark *poNode = new 	BF_GUI_FtpPanel_NodeBookmark();
	poNode->poBookmark = new BF_GUI_FtpPanel_Bookmark();
	poNode->poBookmark->sTitle = "untitled";
	poNode->sName = poNode->poBookmark->sTitle;
	poNode->CopySIconFrom(poBookmarkSIcon);
	poNode->poBookmark->sUserName = "anonymous";
			
	loNode.AddItem(poNode);
	
	DrawNodes(true);
	DrawListInfo(true);
	DrawSelListInfo(true);
	NavChangeCursor(loNode.IndexOf(poNode));				
	
	Bookmark_Edit();
}
////////////////////////////////////////////////////////////////////////////////////////////

void
BF_GUI_FtpPanel::Ftp_Disconnect()
{	
	DELETE(poClient);
	poClient = new FtpClient();
	poBookmark = NULL;
	
	iVisMode = 	BF_FTPPANEL_VISMODE_BOOKMARKS;	
	iState = BF_FTPPANEL_STATE_DEFAULT;
	
	RefreshNodeList();
	DrawNodes(true);
	DrawListInfo(true);
	DrawSelListInfo(true);
	NavChangeCursor(0);
	
	if(poSysKeysMenu) PrepareKeysMenu();
}
////////////////////////////////////////////////////////////////////////////////////////////
void							
BF_GUI_FtpPanel::PrepareTopMenuItem(BF_GUI_TopMenu_HItem *po_HItem)
{
	BF_GUI_NodePanel::PrepareTopMenuItem(po_HItem);
	po_HItem->AddHItem("",NULL);
	po_HItem->AddHItem(BF_DictAt(BF_DICT_FTP_RELOADFOLDER),BF_FTP_MSG_DIR_RELOAD_START,this,"");
}
////////////////////////////////////////////////////////////////////////////////////////////
void
BF_GUI_FtpPanel::ActionCopyFrom_Run(BL_String & s_Path,BF_NodeCollection & lo_Node,bool b_Move)
{
	if(!poBookmark) return;
	BF_GUI_OperRoster_AddTask(new BF_GUI_FtpPanel_UploadTask(s_Path,this,lo_Node,b_Move));
}
////////////////////////////////////////////////////////////////////////////////////////////
void							
BF_GUI_FtpPanel::ActionCopyTo_Run(BL_String & s_Path,BF_NodeCollection & lo_Node)
{
	if(!poBookmark) return;
	BF_GUI_OperRoster_AddTask(new BF_GUI_FtpPanel_DownloadTask(s_Path,this,lo_Node,false));		
}
////////////////////////////////////////////////////////////////////////////////////////////
