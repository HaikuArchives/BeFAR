#ifndef __BF_GUI_FILESPANEL_SEARCH_H__
#define __BF_GUI_FILESPANEL_SEARCH_H__

#include "Query.h"
#include "BF_GUI_FilesPanel.h"

class BF_GUI_FilesPanel_SearchDialog;

class  BF_GUI_FilesPanel_Search:public BL_Object{
public:				
	
									  	BF_GUI_FilesPanel_Search(BF_GUI_FilesPanel_SearchDialog *po_Dialog);		
		void							Run();
		void							Close();
		
		
		bool							bReqClose;
private:									  	
		BF_GUI_FilesPanel_SearchDialog *poDialogParent;									  	
		BQuery							oQuery;
		BF_Path							oPath;

		bool							bFromCurrentFolder;
		BF_Path							oCurrentFolder;
				
		BL_String						sSearchName,sSearchText;
		BMessage 						oMsgAdd;

		void							Run_OnBootVolume();
		void							Run_OnOtherVolume(const char *pc_Path);
		
		bool							Search_CheckTextFile(const char *pc_PathName,BL_String & s1);		
		
		bool							Search_InFileCheck(BF_Node *po_Node,BL_String &s_FullPath);
		void							Search_AddResult(BF_Node *po_Node,BL_String &s_FullPath);
		void							Message_AddItem(BF_GUI_ViewMenu_Item *po_MenuItem);
};

class BF_GUI_FilesPanel_SearchDialog:public BF_GUI_Dialog{
public:
										BF_GUI_FilesPanel_SearchDialog(
											const BRect & o_Rect,										
											const char *pc_SearchName,
											BF_GUI_FilesPanel *po_Panel,
											BF_Path	 & o_Path);
										~BF_GUI_FilesPanel_SearchDialog();
										
virtual void 							AttachedToWindow(void);
virtual void 							MessageReceived(BMessage* po_Message);	
	
		BL_String						sSearchName,sSearchText;
		bool							bFromCurrentFolder;
		BF_Path							oCurrentFolder;
		BL_List							lsVol;
private:		
		BF_GUI_FilesPanel 				*poPanel;
		BF_GUI_DlgView_VCMenu			*poList;
		BF_GUI_ViewText					*poComment;		
		thread_id						idThreadQuery;
		BF_Path							oPath;
		BF_GUI_FilesPanel_Search 		*poSearch;

virtual	bool							ReadyForClose();		
		
friend	int32 	BF_GUI_FilesPanel_SearchDialog_Thread(void *data);
friend	class  	BF_GUI_FilesPanel_Search;
};
#endif