#ifndef __BF_GUI_FTPPANEL_H__
#define __BF_GUI_FTPPANEL_H__

#include "FtpClient.h"
#include "BF_GUI_NodePanel.h"
//#include "BF_GUI_FtpPanel_Tasks.h"

enum{
	BF_FTP_MSG 	= 'BF10',
	
	BF_FTP_MSG_EDIT_BOOKMARK,	
	BF_FTP_MSG_BOOKMARK_EDIT,
	BF_FTP_MSG_BOOKMARK_DELETE,
	BF_FTP_MSG_BOOKMARK_DELETE_ANSWER,	
	BF_FTP_MSG_BOOKMARK_CREATE,
	
	BF_FTP_MSG_CONNECT_BREAK,
	BF_FTP_MSG_CONNECTED,
	BF_FTP_MSG_MESSAGE,
	BF_FTP_MSG_DIR_CHANGED,
	BF_FTP_MSG_DISCONNECT,
	BF_FTP_MSG_DIR_RELOAD_START,
	BF_FTP_MSG_DIR_RELOAD,
	
	BF_FTP_MSG_LAST
};

/////////////////////////////////////////////////////////////////
class BF_FtpPath:public BF_Path{
public:
		void					SetFromFtp(const char *pc_Path);
};
/////////////////////////////////////////////////////////////////

class BF_GUI_FtpPanel_Bookmark:public BL_Object{
public:
		BL_String					sTitle,sHost,sUserName,sUserPassword;
		BL_String					sProxy;
		BL_String					sFolder;
		int32						iPort;
		bool						bPassive,bAscii_Mode;
		
									BF_GUI_FtpPanel_Bookmark();								
virtual void						operator = (BF_GUI_FtpPanel_Bookmark & o_Source);
};

/////////////////////////////////////////////////////////////////

class BF_GUI_FtpPanel_NodeBookmark:public BF_Node{
public:
		BF_GUI_FtpPanel_Bookmark  	*poBookmark;
									BF_GUI_FtpPanel_NodeBookmark();
									~BF_GUI_FtpPanel_NodeBookmark();
};


/////////////////////////////////////////////////////////////////

#define BF_FTPPANEL_VISMODE_BOOKMARKS	0
#define BF_FTPPANEL_VISMODE_FTPLIST		1

#define BF_FTPPANEL_STATE_DEFAULT		0
#define BF_FTPPANEL_STATE_CONNECTING	1
#define BF_FTPPANEL_STATE_CONNECTED		2
#define BF_FTPPANEL_STATE_DIR_CHANGING	3
#define BF_FTPPANEL_STATE_DIR_RELOAD	4

class BF_GUI_FtpPanel:public BF_GUI_NodePanel{
public:
										BF_GUI_FtpPanel(const BRect & o_Rect,bool b_WinPos_OnLeft);
										~BF_GUI_FtpPanel();

//////////////////////// from parent methods ///////////////////////////////
virtual	bool							NavEnter();		
virtual void 							MessageReceived(BMessage* po_Message);

private:		

virtual const BL_String					Path() const;
virtual	void							PrepareKeysMenu();		
virtual void							PrepareTopMenuItem(BF_GUI_TopMenu_HItem *po_HItem);
virtual	void 							Action_SwitchShowIcons();

virtual	void							ActionMakeDir_Run(BL_String & s_Title);
virtual	void							ActionCopyTo_Run(BL_String & s_Path,BF_NodeCollection & lo_Node);
virtual	void							ActionCopyFrom_Run(BL_String & s_Path,BF_NodeCollection & lo_Node,bool b_Move);
virtual	void							ActionDelete_Run(BF_NodeCollection & lo_Node);
//////////////////////// self methods ///////////////////////////////

virtual void							RefreshNodeList();

		void							LoadBookmarks();
		void							Bookmark_Create();
		void							Bookmark_Edit();
		void							Bookmark_Delete(); 
		void							Bookmark_Delete_Yes();
		void							Bookmark_Save(const char *pc_OldTitle,BF_GUI_FtpPanel_NodeBookmark *po_Node);		  
		
		void							Act_Connect();		
		void							Act_ChangeDir_Start(BF_Node *po_Node);
		void							Act_ReloadDir_Start();
				
		void							Ftp_ListLoaded(const  BL_String & s_List);
		void							Ftp_LoadIcons();						
		void							Ftp_Disconnect();

		int32							iVisMode;
		int32							iState;
		
		FtpClient						*poClient;
		BF_FtpPath						oPath;
		BF_FilesPath					oBookmarkDir;
		BF_GUI_FtpPanel_Bookmark		*poBookmark;
		
		BBitmap							*poFolderSIcon,*poBookmarkSIcon;
		BBitmap							*poDocSIcon;
				
		thread_id						uConnectThread;		

friend int32 BF_GUI_FtpPanel__Act_Connect_Thread(void *data);
friend int32 BF_GUI_FtpPanel__Act_ChangeDir_Thread(void *data);
friend int32 BF_GUI_FtpPanel__Act_ReloadDir_Thread(void *data);
friend int32 BF_GUI_FtpPanel__Act_MakeDir_Thread(void *data);
friend class BF_GUI_FtpPanel_UploadTask;
friend class BF_GUI_FtpPanel_DeleteTask;
friend class BF_GUI_FtpPanel_DownloadTask;
		
};

int32 	BF_GUI_FtpPanel__Act_Connect_Thread(void *data);
int32 	BF_GUI_FtpPanel__Act_ChangeDir_Thread(void *data);
int32 	BF_GUI_FtpPanel__Act_ReloadDir_Thread(void *data);
int32	BF_GUI_FtpPanel__Act_MakeDir_Thread(void* data);

#endif