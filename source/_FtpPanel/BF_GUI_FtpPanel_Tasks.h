#ifndef __BF_GUI_FTPPANEL_TASKS_H__
#define __BF_GUI_FTPPANEL_TASKS_H__

#include "BF_GUI_NodePanel.h"
#include "BF_GUI_OperRoster.h"
#include "FtpClient.h"
#include "BF_GUI_FtpPanel.h"

class BF_GUI_FtpPanel_UploadTask:public BF_GUI_OperTask{
public:
											BF_GUI_FtpPanel_UploadTask(
																BL_String & s_PathSrc,
																BF_GUI_FtpPanel *po_Panel,
																BF_NodeCollection & lo_Node,
																bool b_FilesMove
																);
virtual void								Run();
private:
		uint64								iOperIndex,iOperCount;								
		BF_Path								oPathSrc;
		BF_NodeList 						loNode;
		bool								bFilesMove;		
		
		BF_GUI_FtpPanel 					*poPanel;
		FtpClient 							*poFtp;
		BF_GUI_FtpPanel_Bookmark 			*poBookmark;
		
		void								Put_File(BF_Node *po_Node);
		void								Put_List(BF_NodeCollection *plo_Node);
};

////////////////////////////////////////////////////////////////////////////////////////////////

class BF_GUI_FtpPanel_DownloadTask:public BF_GUI_OperTask{
public:
											BF_GUI_FtpPanel_DownloadTask(
																BL_String & s_PathDest,
																BF_GUI_FtpPanel *po_Panel,
																BF_NodeCollection & lo_Node,
																bool b_FilesMove
																);
virtual void								Run();
private:
		uint64								iOperIndex,iOperCount;								
		BF_Path								oPathDest;
		BF_NodeList 						loNode;
		bool								bFilesMove;		
		
		BF_GUI_FtpPanel 					*poPanel;
		FtpClient 							*poFtp;
		BF_GUI_FtpPanel_Bookmark 			*poBookmark;
		
		void								Get_File(BF_Node *po_Node);
		void								Get_List(BF_NodeCollection *plo_Node);
};

////////////////////////////////////////////////////////////////////////////////////////////////

class BF_GUI_FtpPanel_DeleteTask:public BF_GUI_OperTask{
public:
											BF_GUI_FtpPanel_DeleteTask(
																BF_GUI_FtpPanel *po_Panel,
																BF_NodeCollection & lo_Node
																);
virtual void								Run();
private:
		uint64								iOperIndex,iOperCount;										
		BF_NodeList 						loNode;
		
		BF_GUI_FtpPanel 					*poPanel;
		FtpClient 							*poFtp;
		BF_GUI_FtpPanel_Bookmark 			*poBookmark;
		
		void								Delete_File(BF_Node *po_Node);
		void								Delete_List(BF_NodeCollection *plo_Node);
};

#endif