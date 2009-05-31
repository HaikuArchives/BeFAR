#ifndef __BF_GUI_OPERROSTER_H__
#define __BF_GUI_OPERROSTER_H__

#include "BL_Tools.h"
#include "BF_Node.h"
#include "BF_GUI_DlgViews.h"

class 	BF_GUI_OperRoster;
int32 	BF_GUI_OperTask_Thread(void *data);
void	BF_GUI_OperRoster_Draw_Progress(BView *po_Render,const BRect & o_Rect,uint64 i_Index,uint64 i_Count,const char *pc_Text);

///////////////////////////////////////////////////////////////////////////
typedef enum{
	BF_GUI_OPERTASK_COPY_DEFAULT=-1,
	BF_GUI_OPERTASK_COPY_OVERWRITE=0,
	BF_GUI_OPERTASK_COPY_SKIP,	
	BF_GUI_OPERTASK_COPY_APPEND,	
	BF_GUI_OPERTASK_COPY_CANCEL,	
}BF_GUI_OperTask_Copy_Answer;

typedef enum{
	BF_GUI_OPERTASK_ERROR_OK=0,
	BF_GUI_OPERTASK_ERROR_CANCEL_ALL,
}BF_GUI_OperTask_Error_Answer;

#define BF_GUI_OPERTASK_STYLE_DEFAULT		0
#define BF_GUI_OPERTASK_STYLE_CANMINIMASE	1

#define BF_GUI_OPERTASK_STATE_PRESTART		0
#define BF_GUI_OPERTASK_STATE_PAUSE			1
#define BF_GUI_OPERTASK_STATE_RUN			2

class BF_GUI_OperTask:public BL_Object/*BHandler*/{
public:
									BF_GUI_OperTask(const char *pc_Name,int32 i_Styles = BF_GUI_OPERTASK_STYLE_CANMINIMASE);
									~BF_GUI_OperTask();

virtual 	void							Run();				// must be caled before childs_code
		bool							Pause();			
		bool							DePause();			
		bool							Cancel();			

public:				
		BL_String						sName;		
		
		int32							State();

protected:		
		int32							iStyles;

		void							SetProgress(uint64 i_TotalIndex,uint64 i_TotalCount);		
		BF_GUI_OperTask_Copy_Answer		Ask_Copy_FileExist(const char *pc_NodeName,bool b_AskAboutAppend=true);
		BF_GUI_OperTask_Error_Answer	Ask_Error(const char *pc_Message,bool b_AskAdditional=true, const char *pc_ExtMessage=NULL);
		int32							Ask_Message(const char *pc_Message,BL_List *plo_MenuItem,const char *pc_ExtMessage=NULL);
		bool							Canceled();
						
private:
		int32							iState;	
		thread_id						idThread;
		BMessenger 						oMessenger;
		BMessage						oMessage;
		bigtime_t						iLastProgressTime;		
		
		uint64							iTotalCount,iTotalIndex;
		
		bool							bWaitingScreen;
		bool							bReqCancel;
		
		BF_GUI_OperTask_Copy_Answer		iLastCopyAnswer;
				

		void							RunThread();
		void							Finished();
		
friend class BF_GUI_OperRoster;
friend int32 BF_GUI_OperTask_Thread(void *data);
};

///////////////////////////////////////////////////////////////////////////

class BF_GUI_OperRoster_Lenta:public BView{
public:
										BF_GUI_OperRoster_Lenta();	
										~BF_GUI_OperRoster_Lenta();		
virtual	void							Draw(BRect o_Rect);												
virtual void							MouseDown(BPoint point);
		void							DrawProg(uint64 i_Index,uint64 i_Count,bool b_MustDraw);

		BL_String						sText;
private:		
		bigtime_t						iLastProgressTime;
		uint64							iProgCount,iProgIndex;
		
		float 							fSizeY;
};

///////////////////////////////////////////////////////////////////////////
class BF_GUI_OperRoster_Dialog_Item:public BF_GUI_ViewMenu_Item{
public:
										BF_GUI_OperRoster_Dialog_Item(const char *pc_StatText,int i_TaskState);
virtual void							Draw(BF_GUI_DlgView_Menu *po_Parent,BView *po_Render,float f_PosY);

		BL_String						sStatText;
		uint64							iProgCount,iProgIndex;
		uint32							iTaskState;		
private:
};
///////////////////////////////////////////////////////////////////////////
class BF_GUI_OperRoster_Dialog:public BF_GUI_Dialog{
public:	
										BF_GUI_OperRoster_Dialog(const BRect & o_Rect);
virtual void 							MessageReceived(BMessage* po_Message);

		void							Minimaze();		
		void							Task_Pause(int i_TaskIndex);
		void							Task_Cancel(int i_TaskIndex);

		BF_GUI_DlgView_VMenu			*poTasksMenu;		
		
private:
		void							PrepareKeysMenu(bool b_Init);
};

///////////////////////////////////////////////////////////////////////////
#define BF_GUI_OPERROSTER_VIEW_HIDED	0
#define BF_GUI_OPERROSTER_VIEW_DIALOG	1
#define BF_GUI_OPERROSTER_VIEW_LENTA	2


class BF_GUI_OperRoster:public BHandler{
public:
										BF_GUI_OperRoster();
										~BF_GUI_OperRoster(); //!!!
										
virtual void							MessageReceived(BMessage* po_Message);		
		void							SetViewMode(int32 i_NewViewMode);
		bool 							CanMinimase();
		
		float							fHeightInWinMain();
virtual void							Enable(bool b_Enable);	

		BF_GUI_OperTask					*TaskAt(int32 i_Index);

private:
		BL_List							loTask;
		int								iViewMode;
		BL_String						sOldFocusViewName;
		int32							iDisableCount;
		
		void							CalcTotalProg(uint64 & iCount,uint64 & iIndex);
		void							UpdateLenta(bool b_Must=false);
		void							UpdateDialog();
		
		void							Check_ScreenWaiting();
				
		status_t						AddTask(BF_GUI_OperTask *po_Task);				
		
		void							StoreCurrentFocus();
		void							SetFocusToOld();
		
		void							Close();
};
extern BF_GUI_OperRoster *poOperRoster;
///////////////////////////////////////////////////////////////////////////
void
BF_GUI_OperRoster_Init();

void
BF_GUI_OperRoster_AddTask(BF_GUI_OperTask *po_Task);

///////////////////////////////////////////////////////////////////////////

#endif