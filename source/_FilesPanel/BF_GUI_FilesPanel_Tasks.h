#ifndef __BF_GUI_FILESPANEL_TASKS_H__
#define __BF_GUI_FILESPANEL_TASKS_H__

#include "BF_GUI_NodePanel.h"
#include "BF_GUI_OperRoster.h"

/////////////////////////////////////////////////////////////////////////

class BF_GUI_FilesPanel_Task:public BF_GUI_OperTask{
public:
										BF_GUI_FilesPanel_Task(const char *pc_Name,int32 i_Styles = BF_GUI_OPERTASK_STYLE_CANMINIMASE);
protected:
			bool							Check_DestFolder(BF_FilesPath &oPathDest);
			
			void							RenameNodes(
												const BF_NodeCollection & lo_Node,
												const BF_FilesPath & o_SrcPath,
												const BL_String &s_NewName);
};

/////////////////////////////////////////////////////////////////////////

typedef enum{
	BF_FILESPANEL_DELETE_TO_TRASH,
	BF_FILESPANEL_DELETE_TO_VOID
}BF_GUI_FilesPanel_DeleteTask_Type;

class BF_GUI_FilesPanel_DeleteTask:public BF_GUI_OperTask{
public:
											BF_GUI_FilesPanel_DeleteTask(
																BF_FilesPath o_Path,
																BF_NodeCollection & lo_Node,
																BF_GUI_FilesPanel_DeleteTask_Type i_DeleteType);
virtual void								Run();								
private:
		uint64								iOperIndex,iOperCount;								
		BF_FilesPath						oPath;
		BF_NodeList 						loNode;
		BF_GUI_FilesPanel_DeleteTask_Type	iDeleteType;
				
		BL_String							sTrashPath;
		BL_List 							lsSysPath;
				
		bool								PrepareTrashPath();
		bool								DeleteList(BF_NodeCollection & lo_Node);
		
		bool								CheckFolder_ForSys(const char *pc_Folder);
};	
/////////////////////////////////////////////////////////////////////////

//#define BF_GUI_FILESPANEL_COPY_BUFFER_SIZE (uint64)16384
#define BF_GUI_FILESPANEL_COPY_BUFFER_SIZE (uint64)65536
//#define BF_GUI_FILESPANEL_COPY_BUFFER_SIZE (uint64)393216
//#define BF_GUI_FILESPANEL_COPY_BUFFER_SIZE (uint64)1024000

class BF_GUI_FilesPanel_CopyTask:public BF_GUI_FilesPanel_Task{
public:
											BF_GUI_FilesPanel_CopyTask(
																BF_FilesPath& o_PathSrc,
																BF_FilesPath& o_PathDest,
																BF_NodeCollection & lo_Node,
																bool b_FilesMove
																);
virtual void								Run();
private:
		uint64								iOperIndex,iOperCount;								
		BF_FilesPath						oPathSrc,oPathDest;
		BF_NodeList 						loNode;
		bool								bFilesMove;
		char								pcBuffer[BF_GUI_FILESPANEL_COPY_BUFFER_SIZE];
		
		bool								Copy_Nodes(	BF_NodeCollection *plo_Node,
														BString& s_NewPath,
														bool b_DeleteAfterCopy);
		bool 								Copy_Atributes(BNode & o_NodeSrc,BNode & o_NodeDest);
														
		void								Move_Nodes();		
};
/////////////////////////////////////////////////////////////////////////
class BF_GUI_FilesPanel_ChangeCaseTask:public BF_GUI_OperTask{
public:
											BF_GUI_FilesPanel_ChangeCaseTask(
																BF_FilesPath& o_Path,
																BF_NodeCollection & lo_Node,																
																int32 i_CaseType);
virtual void								Run();
private:
		uint64								iOperIndex,iOperCount;								
		BF_NodeList 						loNode;
		BF_FilesPath					 	oPath;
		int32								iCaseType;					
};	
/////////////////////////////////////////////////////////////////////////
class BF_GUI_FilesPanel_Rename:public BF_GUI_FilesPanel_Task{
public:
											BF_GUI_FilesPanel_Rename(
																BF_FilesPath& o_Path,
																BF_NodeCollection & lo_Node,																
																BL_String & s_NewName);
virtual void								Run();
private:		
		BF_NodeList 						loNode;
		BF_FilesPath					 	oPath;
		BL_String 							sNewName;
};	
/////////////////////////////////////////////////////////////////////////
class BF_GUI_FilesPanel_MakeLinksTask:public BF_GUI_FilesPanel_Task{
public:
											BF_GUI_FilesPanel_MakeLinksTask(
																BF_FilesPath& o_PathSrc,
																BF_FilesPath& o_PathDest,
																BF_NodeCollection & lo_Node
																);
virtual void								Run();
private:
		BF_FilesPath						oPathSrc,oPathDest;
		BF_NodeList 						loNode;	
		BL_String							sMask;	
};
/////////////////////////////////////////////////////////////////////////
#endif