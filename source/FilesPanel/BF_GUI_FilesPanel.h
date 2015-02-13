#ifndef __BF_GUI_FILESPANEL_H__
#define __BF_GUI_FILESPANEL_H__

#include "BF_GUI_NodePanel.h"
#include "BF_GUI_OperRoster.h"

/////////////////////////////////////////////////////////////////////////

class BF_GUI_FilesPanel:public BF_GUI_NodePanel{
public:
								BF_GUI_FilesPanel(const BRect & o_Rect,bool b_WinPos_OnLeft);
						  
virtual void 					AttachedToWindow(void);
		void					Pub_GoPath(const char *pc_Path);

						  
private:
		BF_FilesPath			oPath;
		BL_List					lsPathHistory;		
		thread_id				idThreadLoading;
		int32					iThreadLoading_Count,iThreadLoading_Index;
		BL_String				sThreadLoading_FutureFocus;
		uint32					iTempDelType;


//////////////////////////////// inherited methods ////////////////////////////////		
		void 					NavGoParentDir();
virtual	bool 					NavChangeCursor(int i_NewCursor,bool b_DrawNodes=true,bool b_ChangeVars=true);
		void 					NavGoPath(const BF_Path & o_NewPath,
										BString *ps_FocusItem=NULL,
										bool b_LoadNodes=true,
										bool b_AddToHistory=true);		
						
virtual	void					Action_SwitchShowIcons();
virtual	bool					NavEnter();

virtual bool					OnClose();
virtual bool					OnKeyDown(const char *bytes, int32 numBytes);

virtual	void					PrepareKeysMenu();
virtual	void					PrepareCmdLine();
virtual	void					PrepareTopMenuItem(BF_GUI_TopMenu_HItem *po_HItem);
virtual	void					PrepareHotKeys(bool b_Focused);

virtual const BL_String			Path() const;

virtual void 					MessageReceived(BMessage* po_Message);	

virtual	void 					DrawListInfo(bool b_DrawBack);
		
virtual	void					ActionDelete_Run(BF_NodeCollection & lo_Node);
virtual	void					ActionCopyTo_Run(BL_String & s_Path,BF_NodeCollection & lo_Node);
virtual	void					ActionMoveTo_Run(BL_String & s_Path,BF_NodeCollection & lo_Node);
virtual	void					ActionMakeDir_Run(BL_String & s_Title);
virtual	void					ActionRename_Run(BF_NodeCollection & lo_Node,BL_String & s_NewName);

virtual	void					Nodes_Add(BF_Node *po_Node,BF_NodeCollection *plo_Node);
virtual	void					Nodes_Remove(BF_Node *po_Node,BF_NodeCollection *plo_Node);

virtual	void					Action_CopyToBuffer();
virtual	void					Action_PasteFromBuffer();
virtual	void					Action_PasteFromBuffer_Step2(BMessage *po_Message);

////////////////////// self methods ///////////////////////////////////////////////////////////////
		void					Action_Search_0();
		void					Action_Search_1(BMessage* po_Message);		
		void					Action_CopyMoveTo_Run(BL_String & s_Path,BF_NodeCollection & lo_Node,bool b_Move);
		
		void					Action_ChangeCase_0();		
		void					Action_ChangeCase_1(BMessage *po_Message);
		
		void					Action_MakeLinks_StartDialog();
		void					Action_MakeLinks_StartTask(BMessage *po_Message);
		
		void					Action_GoPath_StartDialog();
		void					Action_GoPath_Run(BMessage *po_Message);
		
		void					Action_Run_File(BF_Node *po_Node);		
		void					Action_Edit_File(BF_Node *po_Node=NULL);
		
		void					Action_TextViewer_Start();
		
		void					Action_Tracker_Addons_Start();
		void					Action_Tracker_Addons_Start_LoadAddOns(bool b_FromHome,BL_List &lo_MenuResult,bool b_LoadSIcons);
		void					Action_Tracker_Addons_Run(const char *pc_AddOn);
		
		void					Action_MakeSH_Dialog();
		void					Action_MakeSH_Run(BMessage *po_Message);

		void 					Load_Files(bool b_AlwaysLoadSIcons=false);

		void					PathHistory_Add(BF_Path * po_Path);
		void					PathHistory_GoBack();				
		
		int32 					LoadStyles();
		
		void					HandlingMonitor(BMessage &o_Message);		
		void					HandlingMonitor_Removed(BMessage &o_Message,node_ref & u_Ref);
		void					HandlingMonitor_Moved(BMessage &o_Message,node_ref & u_Ref);		
		void					HandlingMonitor_Created(BMessage &o_Message,node_ref & u_Ref);		
		void					HandlingMonitor_Changed(BMessage &o_Message,node_ref & u_Ref);				
		
		
		void					SayFriend_About_CursorChanged_Ext();
		
/////////////////////////////////////////////////////////////////////////////////////
friend int32 					BF_GUI_FilesPanel_LoadFiles_Thread(void *data);
};


#endif