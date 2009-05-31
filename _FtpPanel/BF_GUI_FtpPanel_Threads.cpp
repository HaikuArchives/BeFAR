#include "Directory.h"

#include "BF_Dict.h"
#include "BF_Roster.h"
#include "BF_GUI_KeysMenu.h"
#include "BF_GUI_WinMain.h"

#include "BF_GUI_FtpPanel.h"
#include "BF_GUI_FtpPanel_Tasks.h"

int32 
BF_GUI_FtpPanel__Act_Connect_Thread(void *data)
{	
	BF_GUI_FtpPanel		*poPanel = (BF_GUI_FtpPanel*) data;
			
	BF_GUI_FtpPanel_Bookmark	oSite;
	FtpClient				*poClient=NULL;
	// init vars //
	{
		BF_GUI_FtpPanel		*poPanel;
		poPanel = (BF_GUI_FtpPanel*) data;
		
		BF_GUI_FtpPanel_NodeBookmark *poNode = (BF_GUI_FtpPanel_NodeBookmark*)poPanel->Nodes_Focus();
		if(!poNode || !poNode->poBookmark) return -1;
		poPanel->poBookmark = poNode->poBookmark;
				
		oSite = *poNode->poBookmark;
		poClient = poPanel->poClient;				
	}		
	//open the connection
	if(poClient->connect(oSite.sHost.String(),oSite.sUserName.String()
		,oSite.sUserPassword.String()
	)){
		poClient->setPassive(oSite.bPassive);
		
		//change the pwd to the local pwd
		string sPath;
		if(!poClient->pwd(sPath)){
			BMessage userPassMessage(BF_FTP_MSG_MESSAGE);
			userPassMessage.AddString("message",BF_DictAt(BF_DICT_FTP_ERRGETNAME));
			poWin->PostMessage(&userPassMessage,poPanel);
			return -4;		
		}		
		
		// list ftp_nodes //
		BL_String sList;
		poClient->ls(sList);
		
		BMessage userPassMessage(BF_FTP_MSG_CONNECTED);
		userPassMessage.AddString("list", sList.String());
		userPassMessage.AddString("path", sPath.c_str());
		poWin->PostMessage(&userPassMessage,poPanel);
	}else{
		BMessage userPassMessage(BF_FTP_MSG_MESSAGE);
		userPassMessage.AddString("message", BF_DictAt(BF_DICT_FTP_ERRCONNECT));		
		poWin->PostMessage(&userPassMessage,poPanel);
		return -4;	
	}
	
	// finish	
	return 0;	
}

int32 
BF_GUI_FtpPanel__Act_ChangeDir_Thread(void *data)
{
	ASSERT(data);
	BF_GUI_FtpPanel		*poPanel = (BF_GUI_FtpPanel*) data;			
	FtpClient			*poClient = poPanel->poClient;
	BF_Node				*poNode = poPanel->Nodes_Focus();	
	if(!poNode) return -1;
	
	if(poClient->cd(poNode->sName.String())){						
		// list ftp_nodes //
		BL_String sList;
		poClient->ls(sList);
		
		//change the pwd to the local pwd
		string sPath;
		if(!poClient->pwd(sPath)){
			BMessage userPassMessage(BF_FTP_MSG_MESSAGE);
			userPassMessage.AddString("message", BF_DictAt(BF_DICT_FTP_ERRGETNAME));
			poWin->PostMessage(&userPassMessage,poPanel);
			return -4;		
		}		
		
		BMessage userPassMessage(BF_FTP_MSG_DIR_CHANGED);		
		userPassMessage.AddString("list", sList.String());		
		userPassMessage.AddString("path", sPath.c_str());		
		poWin->PostMessage(&userPassMessage,poPanel);
	}else{
		BMessage userPassMessage(BF_FTP_MSG_MESSAGE);
		userPassMessage.AddString("message", "Can`t change dir");		
		poWin->PostMessage(&userPassMessage,poPanel);
	}
	return 0;
}

int32 
BF_GUI_FtpPanel__Act_ReloadDir_Thread(void *data)
{
	ASSERT(data);
	BF_GUI_FtpPanel		*poPanel = (BF_GUI_FtpPanel*) data;			
	FtpClient			*poClient = poPanel->poClient;

	// list ftp_nodes //
	BL_String sList;
	poClient->ls(sList);
		
	BMessage userPassMessage(BF_FTP_MSG_DIR_RELOAD);		
	userPassMessage.AddString("list", sList.String());		
	poWin->PostMessage(&userPassMessage,poPanel);
	return 0;
}

int32
BF_GUI_FtpPanel__Act_MakeDir_Thread(void* data)
{
	BF_GUI_FtpPanel		*poPanel=NULL;
	BL_String			sName;
	{
		BMessage 			*poMessage = (BMessage*)data;	
		const char			*pcName;	
		ASSERT(B_OK==poMessage->FindPointer("panel",(void**)&poPanel) && poPanel);
		ASSERT(B_OK==poMessage->FindString("name",&pcName) && pcName);
		sName = pcName;
	}	
	FtpClient			*poClient = poPanel->poClient;
	
	if(poClient->makeDir(sName.String())){
			
	}
	
	BMessage userPassMessage(BF_FTP_MSG_DIR_RELOAD_START);		
	poWin->PostMessage(&userPassMessage,poPanel);
	
	return 0;	
}
