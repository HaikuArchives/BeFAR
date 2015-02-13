#include <stdio.h>
#include <Autolock.h>
#include "BF_Dict.h"  
#include "BF_GUI_WinMain.h"  
#include "BF_GUI_Func.h"  
#include "BF_GUI_KeysMenu.h"  
#include "BF_GUI_OperRoster.h"  
#include "BF_GUI_CmdLine.h"  

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "OperRoster"

BF_GUI_OperRoster *poOperRoster = NULL;

void							
BF_GUI_OperRoster_Draw_Progress(BView *po_Render,const BRect & o_Rect,uint64 i_Index,uint64 i_Count,const char *pc_Text)
{
	ASSERT(po_Render);
	BRect 	oRect;	
	float	fDelta = i_Count<=0?i_Count:(o_Rect.Width()-10.0) / (float)i_Count ;
	///
	oRect = o_Rect;
	oRect.right = oRect.left +(float)i_Index * fDelta;
	po_Render->SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_PROGRESS_FILLED));
	po_Render->FillRect(oRect/*,B_MIXED_COLORS*/);
	//
	oRect.left = oRect.right+1;
	oRect.right = o_Rect.right;
	po_Render->SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_PROGRESS_EMPTY));
	po_Render->FillRect(oRect);
	//
	/* draw text */	
	BL_String s,s1;
	s<< pc_Text;
	s<<" ";
	//
	s1=i_Index;
	s1.SetDigits();
	s<<s1;
	s<<B_TRANSLATE(BF_DICT_ROSTER_OFF);
	//
	s1=i_Count;
	s1.SetDigits();
	s<<s1;
	//oRect = o_Rect;
	float fWidth = poSysSetup->oFontToolView.oFont.StringWidth(s.String());
	BPoint oPoint(o_Rect.Width()/2-fWidth/2,o_Rect.top + poSysSetup->oFontToolView.fAscent);
	po_Render->SetHighColor(SYS_COLOR(BF_COLOR_DIALOG_BUTTON_TEXT));
	po_Render->SetLowColor(SYS_COLOR(BF_COLOR_DIALOG_BACK));
	po_Render->SetDrawingMode(B_OP_INVERT);
	po_Render->DrawString(s.String(),oPoint);		
	po_Render->SetDrawingMode(B_OP_COPY);
}
///////////////////////////////////////////////////////////////////////////
BF_GUI_OperTask::BF_GUI_OperTask(const char *pc_Name,int32 i_Styles)
{
	ASSERT(poWin && pc_Name);
	sName = pc_Name;
	iStyles = i_Styles;
	iState = BF_GUI_OPERTASK_STATE_PRESTART;
	idThread = 0;	
	iLastProgressTime = 0;
	
	iTotalCount = 0;
	iTotalIndex = 0;
	
	bReqCancel = false;	
	bWaitingScreen = false;
	
	iLastCopyAnswer = BF_GUI_OPERTASK_COPY_DEFAULT;
}

BF_GUI_OperTask::~BF_GUI_OperTask()
{
}

int32							
BF_GUI_OperTask::State()
{
	return iState;
}

int32
BF_GUI_OperTask::Ask_Message(const char *pc_Message,BL_List *plo_MenuItem,const char *pc_ExtMessage)
{
	ASSERT(pc_Message);
	
	// wait free screen 	
	bWaitingScreen = true;
	
	{ //  roster,please, free screen
		oMessage.MakeEmpty();
		oMessage.what = BF_MSG_TO_OPERROSTER_TASK_WAITING_SCREEN;
		oMessage.AddPointer("bf_poTask",this);
		BMessenger oMessenger(poOperRoster);
		oMessenger.SendMessage(&oMessage);		
	}
	
	while(bWaitingScreen){			
		snooze(20000);
		//bWaitingScreen = false;
	}
		
	int32 iResult = BF_Dialog_Alert(B_TRANSLATE(BF_DICT_ROSTER_INFO),pc_Message,plo_MenuItem,pc_ExtMessage);
	
	// free screen
	{
		oMessage.MakeEmpty();
		oMessage.what = BF_MSG_TO_OPERROSTER_TASK_FREE_SCREEN;
		oMessage.AddPointer("bf_poTask",this);
		BMessenger oMessenger(poOperRoster);
		oMessenger.SendMessage(&oMessage);	
	}		
	
	return iResult;
}

BF_GUI_OperTask_Error_Answer		
BF_GUI_OperTask::Ask_Error(const char *pc_Message,bool b_AskAdditional, const char *pc_ExtMessage)
{
	ASSERT(pc_Message);
	
	BL_List *ploMenu = new BL_List();
	ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_OK),""));
	if(b_AskAdditional){
		ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_CANCELALL),""));
	}
	
	int32 iResult = Ask_Message(pc_Message,ploMenu,pc_ExtMessage);
			
	// return result
	if(iResult==0)	return BF_GUI_OPERTASK_ERROR_OK;
	if(iResult==1)	return BF_GUI_OPERTASK_ERROR_CANCEL_ALL;
	return BF_GUI_OPERTASK_ERROR_OK;
}

BF_GUI_OperTask_Copy_Answer		
BF_GUI_OperTask::Ask_Copy_FileExist(const char *pc_NodeName,bool b_AskAboutAppend)
{
	ASSERT(pc_NodeName);	
	
	// check for all 
	if(iLastCopyAnswer!=BF_GUI_OPERTASK_COPY_DEFAULT) return iLastCopyAnswer;
	
	// wait free screen 	
	bWaitingScreen = true;
	
	{ //  roster,please, free screen
		oMessage.MakeEmpty();
		oMessage.what = BF_MSG_TO_OPERROSTER_TASK_WAITING_SCREEN;
		oMessage.AddPointer("bf_poTask",this);
		BMessenger oMessenger(poOperRoster);
		oMessenger.SendMessage(&oMessage);		
	}
	
	while(bWaitingScreen){			
		snooze(20000);
		//bWaitingScreen = false;
	}
	
	BL_String s(B_TRANSLATE(BF_DICT_ROSTER_FILE));
	s<<pc_NodeName;
	s<<B_TRANSLATE(BF_DICT_ROSTER_EXISTS);
	
	BL_List *ploMenu = new BL_List();
	ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_OWERWRITE),"overwrite"));
	ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_SKIP),"skip"));
	ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_OWERWRITEALL),"overwrite_all"));
	ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_SKIPALLERRORS),"skip_all"));
	ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_CANCEL),"cancel"));
	if(b_AskAboutAppend) ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_APPEND),"appens"));
	
	int32 iResult = BF_Dialog_Alert(B_TRANSLATE(BF_DICT_ROSTER_COPYASK),s.String(),ploMenu);
	
	// free screen
	{
		oMessage.MakeEmpty();
		oMessage.what = BF_MSG_TO_OPERROSTER_TASK_FREE_SCREEN;
		oMessage.AddPointer("bf_poTask",this);
		BMessenger oMessenger(poOperRoster);
		oMessenger.SendMessage(&oMessage);	
	}		
			
	// return result //
	BF_GUI_OperTask_Copy_Answer iAnswer = BF_GUI_OPERTASK_COPY_DEFAULT;
	switch(iResult){
	case 0:
		iAnswer =  BF_GUI_OPERTASK_COPY_OVERWRITE;
		break;	
	case 1:
		iAnswer =  BF_GUI_OPERTASK_COPY_SKIP;
		break;
	case 2:
		iAnswer =  BF_GUI_OPERTASK_COPY_OVERWRITE;
		iLastCopyAnswer = iAnswer;
		break;
	case 3:
		iAnswer =  BF_GUI_OPERTASK_COPY_SKIP;
		iLastCopyAnswer = iAnswer;
		break;		
	case 4:
		iAnswer =  BF_GUI_OPERTASK_COPY_CANCEL;
		break;
	case 5:
		ASSERT(b_AskAboutAppend);
		iAnswer =  BF_GUI_OPERTASK_COPY_APPEND;
		break;		
	default:
		iAnswer = BF_GUI_OPERTASK_COPY_SKIP;
	};
		
	return iAnswer;
}

bool							
BF_GUI_OperTask::DePause()
{
	if(BF_GUI_OPERTASK_STATE_PAUSE!=iState) return false;	
	if(B_OK!=resume_thread(idThread)) return false;
	
	iState = BF_GUI_OPERTASK_STATE_RUN;
	
	oMessage.MakeEmpty();
	oMessage.what = BF_MSG_TO_OPERROSTER_SETPROGRESS;
	oMessage.AddPointer("bf_poTask",this);
	BMessenger oMessenger(poOperRoster);
	oMessenger.SendMessage(&oMessage);
	
	return true;
}

bool							
BF_GUI_OperTask::Canceled()
{
	return bReqCancel;
}

bool
BF_GUI_OperTask::Cancel()
{
	switch(State()){
	case BF_GUI_OPERTASK_STATE_PAUSE:
		DePause();	
		break;
	}		
	bReqCancel = true;		
	return true;
}

bool
BF_GUI_OperTask::Pause()
{

	if(BF_GUI_OPERTASK_STATE_RUN!=iState) return false;	
	if(B_OK!=suspend_thread(idThread)) return false;
	
	iState = BF_GUI_OPERTASK_STATE_PAUSE;
	
	oMessage.MakeEmpty();
	oMessage.what = BF_MSG_TO_OPERROSTER_TASK_PAUSED;
	oMessage.AddPointer("bf_poTask",this);
	BMessenger oMessenger(poOperRoster);
	oMessenger.SendMessage(&oMessage);
	
	
	return true;
}

void							
BF_GUI_OperTask::Run()
{			
}

void
BF_GUI_OperTask::Finished()
{
	oMessage.MakeEmpty();
	oMessage.what = BF_MSG_TO_OPERROSTER_TASK_FINISHED;
	oMessage.AddPointer("bf_poTask",this);
	BMessenger oMessenger(poOperRoster);
	oMessenger.SendMessage(&oMessage);		
}

void							
BF_GUI_OperTask::SetProgress(uint64 i_TotalIndex,uint64 i_TotalCount)
{
	iTotalCount = i_TotalCount;
	iTotalIndex = i_TotalIndex;
	
	if(iTotalCount<iTotalIndex){
 		int32 i=0;
 		i++;	
	}
	
	if((iLastProgressTime>0) && (real_time_clock_usecs() - iLastProgressTime)<500000) return;
	iLastProgressTime = real_time_clock_usecs();
	//printf("iLastProgressTime = %i\n",iLastProgressTime);

	oMessage.MakeEmpty();
	oMessage.what = BF_MSG_TO_OPERROSTER_SETPROGRESS;
	oMessage.AddPointer("bf_poTask",this);
	//oMessage.AddString("bf_pcText",s.String());
	BMessenger oMessenger(poOperRoster);
	oMessenger.SendMessage(&oMessage);		
	
	return;
}


int32 
BF_GUI_OperTask_Thread(void *data)
{

	BF_GUI_OperTask *poTask = (BF_GUI_OperTask*)data;
	ASSERT(poTask);
	poTask->iState = BF_GUI_OPERTASK_STATE_RUN;
	poTask->Run();
	poTask->Finished();		
	
	return 0;
}

void							
BF_GUI_OperTask::RunThread()
{			
	idThread = spawn_thread(BF_GUI_OperTask_Thread,"opertask_thread",B_THREAD_SUSPENDED,(void*)this);	
	ASSERT(idThread>0,"can`t start opertask_thread\n");	
	ASSERT(B_OK==resume_thread(idThread));		
	set_thread_priority(idThread,1);	
}

///////////////////////////////////////////////////////////////////////////

BF_GUI_OperRoster_Lenta::BF_GUI_OperRoster_Lenta()
:BView(BRect(-1,-1,-1,-1),"operroster_lenta",B_FOLLOW_LEFT_RIGHT|B_FOLLOW_BOTTOM,B_WILL_DRAW)
{
	iLastProgressTime = 0;
	iProgCount = 0;
	iProgIndex = 0;
	//
	SetFont(&poSysSetup->oFontToolView.oFont);
	SetViewColor(B_TRANSPARENT_COLOR);		
	//
	fSizeY = (int32)poSysSetup->oFontToolView.fHeight+2;
	//
	BMessenger oMessenger(poWinView);
	BMessage   oMessage(BF_MSG_TO_WIN_OPERLENTA_SHOWED);
	oMessage.AddInt32("bf_iSizeY",fSizeY+1);
	oMessenger.SendMessage(&oMessage);
	//
	BRect oRect;
	if(poCmdLine){
		oRect = poCmdLine->Frame();
	}else
	if(poSysKeysMenu){
		oRect = poSysKeysMenu->Frame();
	}else
		ASSERT(false);
	//oRect.bottom = poSysSetup->oFontCMDLine.fHeight + poSysSetup->oFontNode.fHeight -2;
	oRect.bottom = oRect.top-1;
	oRect.top = oRect.bottom - fSizeY;
	
	MoveTo(oRect.left,oRect.top);
	ResizeTo(oRect.Width(),oRect.Height());	
	//		
}
BF_GUI_OperRoster_Lenta::~BF_GUI_OperRoster_Lenta()
{
	BMessenger oMessenger(poWinView);
	BMessage   oMessage(BF_MSG_TO_WIN_OPERLENTA_SHOWED);
	oMessage.AddInt32("bf_iSizeY",(int32)(-1-fSizeY));
	oMessenger.SendMessage(&oMessage);	
}

void							
BF_GUI_OperRoster_Lenta::DrawProg(uint64 i_Index,uint64 i_Count,bool b_MustDraw)
{
	iProgCount = i_Count;
	iProgIndex = i_Index;
	if(!b_MustDraw && (iLastProgressTime>0) && (real_time_clock_usecs() - iLastProgressTime)<500000) return;
	iLastProgressTime = real_time_clock_usecs();	
	Draw(Bounds());
}

void
BF_GUI_OperRoster_Lenta::Draw(BRect o_Rect)
{
	BRect oRect(Bounds());
	SetHighColor(SYS_COLOR(BF_COLOR_BACK));
	FillRect(oRect);		
	
	oRect.left++;
	oRect.right--;
	oRect.top++;
	oRect.bottom--;		
	BF_GUI_OperRoster_Draw_Progress(this,oRect,iProgIndex,iProgCount,B_TRANSLATE(BF_DICT_ROSTER_OPERPROGRESS));
		
	/*
	SetHighColor(SYS_COLOR(BF_COLOR_NODE));
	SetLowColor(SYS_COLOR(BF_COLOR_BACK));
	BL_String s;
	s<<"a zdes tipa kompaktniy variant progress-barov :) click me  ";
	s<<sText;	
	DrawString(s.String(),BPoint(0,poSysSetup->oFontToolView.fAscent));*/
}

void							
BF_GUI_OperRoster_Lenta::MouseDown(BPoint point)
{
	BMessage oMessage(BF_MSG_TO_OPERROSTER_CLOSELENTA);
	BMessenger oMessenger(poOperRoster);
	oMessenger.SendMessage(&oMessage);		
}

///////////////////////////////////////////////////////////////////////////
BF_GUI_OperRoster_Dialog_Item::BF_GUI_OperRoster_Dialog_Item(
	const char *pc_StatText,
	int i_TaskState)
:BF_GUI_ViewMenu_Item(" ","")
{
	sStatText = pc_StatText;
	iProgCount = 0;
	iProgIndex = 0;	
	iTaskState = i_TaskState;
}
void							
BF_GUI_OperRoster_Dialog_Item::Draw(BF_GUI_DlgView_Menu *po_Parent,BView *po_Render,float f_PosY)
{
	po_Parent->SetColor_Text(po_Render,po_Parent->IsFocus());
	
	BPoint oPoint(10,f_PosY);
	oPoint.y += poSysSetup->oFontToolView.fAscent;
	po_Render->DrawString(sStatText.String(),oPoint);	
	
	BRect oRect;
	oRect.Set(10,f_PosY+poSysSetup->oFontToolView.fHeight,
		po_Render->Bounds().Width()-10,f_PosY + poSysSetup->oFontToolView.fHeight*2);
		
	switch(iTaskState){
	case BF_GUI_OPERTASK_STATE_PRESTART:		
		oPoint.y += poSysSetup->oFontToolView.fHeight+1;
		po_Render->DrawString(B_TRANSLATE(BF_DICT_ROSTER_PREPARING),oPoint);
		break;
	case BF_GUI_OPERTASK_STATE_PAUSE:		
		BF_GUI_OperRoster_Draw_Progress(po_Render,oRect,iProgIndex,iProgCount,B_TRANSLATE(BF_DICT_ROSTER_PAUSED));
		/*oPoint.y += poSysSetup->oFontToolView.fHeight+1;
		po_Render->DrawString(" paused ",oPoint);	*/
		break;
	case BF_GUI_OPERTASK_STATE_RUN:
		BF_GUI_OperRoster_Draw_Progress(po_Render,oRect,iProgIndex,iProgCount,B_TRANSLATE(BF_DICT_ROSTER_TOTAL));
		break;
	};
}
///////////////////////////////////////////////////////////////////////////

BF_GUI_OperRoster_Dialog::BF_GUI_OperRoster_Dialog(const BRect & o_Rect)
 :BF_GUI_Dialog(o_Rect,B_TRANSLATE(BF_DICT_ROSTER_TITLE),"operroster_dialog",BMessage(),BG_GUI_DIALOG_WINRESIZE_RESIZE_ALL)
{
}

void
BF_GUI_OperRoster_Dialog::Task_Pause(int i_TaskIndex)
{	
	BF_GUI_OperTask* poTask = poOperRoster->TaskAt(i_TaskIndex);	
	if(!poTask) return;

	switch(poTask->State()){
	case BF_GUI_OPERTASK_STATE_RUN:
		poTask->Pause();	
		break;
	case BF_GUI_OPERTASK_STATE_PAUSE:
		poTask->DePause();	
		break;
	}	
	
}

void
BF_GUI_OperRoster_Dialog::Task_Cancel(int i_TaskIndex)
{	
	BF_GUI_OperTask* poTask = poOperRoster->TaskAt(i_TaskIndex);	
	if(!poTask) return;
	
	poTask->Cancel();
}

void
BF_GUI_OperRoster_Dialog::Minimaze()
{
	if(!poOperRoster->CanMinimase()) return;
	//
	BMessage oMessage(BF_MSG_TO_OPERROSTER_CLOSEDIALOG);
	BMessenger oMessenger(poOperRoster);
	oMessenger.SendMessage(&oMessage);		
	//
	RemoveSelf();
	delete this;	
}

void
BF_GUI_OperRoster_Dialog::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what){
	case BF_MSG_DIALOG_PRESSED_CANCEL:{
		Minimaze();
		break;}
	case BF_MSG_DIALOG_PRESSED_OK:{
		BF_GUI_DialogView *poView;
		ASSERT(B_OK==po_Message->FindPointer("bf_DlgView_Focus",(void**)&poView) && poView);
		if(strcmp( poView->Name(),"menu")==0){
			BF_GUI_DlgView_Menu *poMenu = (BF_GUI_DlgView_Menu*)poView;
			
			switch(poMenu->iNavCursorIndex){
			case 0:{ // pause								
				if(!poTasksMenu) return;
				Task_Pause(poTasksMenu->iNavCursorIndex);
				break;}
			case 1:{ // cancel
				if(!poTasksMenu) return;
				Task_Cancel(poTasksMenu->iNavCursorIndex);
				break;}
			case 2:{ // minimase
				Minimaze();
				break;}
			}
		}
		break;}
	default:
		BF_GUI_Dialog::MessageReceived(po_Message);
	};
}

void
BF_GUI_OperRoster_Dialog::PrepareKeysMenu(bool b_Init)
{
	/*
	if(b_Init){
		poSysKeysMenu->Clear(this);
			
		poSysKeysMenu->SetText(3,"Pause");
		poSysKeysMenu->SetMessage(3,new BMessage(BF_MSG_TO_OPERROSTERDIALOG_TASK_PAUSED),this);			
	}		
	*/
}

///////////////////////////////////////////////////////////////////////////

BF_GUI_OperRoster::BF_GUI_OperRoster()
{
	iViewMode = BF_GUI_OPERROSTER_VIEW_HIDED;
	sOldFocusViewName = "";
	iDisableCount = 0;
	//
	{
		ASSERT(poWin); 
		LOCK_WIN();
		poWin->AddHandler(this);
	}
	//
	poOperRoster = this;
}

BF_GUI_OperRoster::~BF_GUI_OperRoster()
{
	LOCK_WIN();
	poWin->RemoveHandler(this);	
}

void							
BF_GUI_OperRoster::Enable(bool b_Enable)
{
	iDisableCount += !b_Enable?+1:-1;	
	
	if(iDisableCount==0){
		// enable lenta
		UpdateLenta(true);
		// 
		Check_ScreenWaiting();		
		// enable dialog
		BF_GUI_OperRoster_Dialog* poOperDialog = (BF_GUI_OperRoster_Dialog*)poWin->FindView("operroster_dialog");
		if(poOperDialog) poOperDialog->EnableDialog(true);
	}else
	if(iDisableCount==1){
		// disable dialog
		BF_GUI_OperRoster_Dialog* poOperDialog = (BF_GUI_OperRoster_Dialog*)poWin->FindView("operroster_dialog");
		if(poOperDialog) poOperDialog->EnableDialog(false);
	}
}

void
BF_GUI_OperRoster::StoreCurrentFocus()
{
	sOldFocusViewName = "";
	BView *poView =poWin->CurrentFocus();
	if(!poView) return;
	sOldFocusViewName = poView->Name();
}

void
BF_GUI_OperRoster::Check_ScreenWaiting()
{
	if(iDisableCount!=0) return;
	
	BF_GUI_OperTask *poTask;
	for(int i=0;i<loTask.CountItems();i++){
		poTask = TaskAt(i);
		if(!poTask->bWaitingScreen) continue;
		
		Enable(false);
		if(BF_GUI_OPERROSTER_VIEW_LENTA==iViewMode){		
			StoreCurrentFocus();
			BF_GUI_Func_PanelsEnable( false,false );			
		}
		poTask->bWaitingScreen = false;
	}
}

void
BF_GUI_OperRoster::SetFocusToOld()
{
	ASSERT(sOldFocusViewName!="","BF_GUI_OperRoster::SetFocusToOld():sOldFocusViewName=''");

	BAutolock oLocker(poWin);
	
	BView *poView = poWin->FindView(sOldFocusViewName.String());
	ASSERT(poView,"BF_GUI_OperRoster::SetFocusToOld():OldFocusView not found");
	
	BF_GUI_MakeFocusTo(poView);
	sOldFocusViewName = "";	
}

void
BF_GUI_OperRoster::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what){
	//
	case BF_MSG_SETUP_UPDATED:
		break;
	//
	case BF_MSG_TO_OPERROSTER_TASK_WAITING_SCREEN:{
		Check_ScreenWaiting();		
		break;}
	case BF_MSG_TO_OPERROSTER_TASK_FREE_SCREEN:{
		Enable(true);
		if(BF_GUI_OPERROSTER_VIEW_LENTA==iViewMode){
			BF_GUI_Func_PanelsEnable( true,false );	
			SetFocusToOld();
		}
		break;}
	///////
	case BF_MSG_TO_OPERROSTER_TASK_FINISHED:{
		BF_GUI_OperTask *poTask=NULL;
		ASSERT(B_OK==po_Message->FindPointer("bf_poTask",(void**)&poTask) && poTask);
		int iIndex = loTask.IndexOf(poTask);
		ASSERT(iIndex>=0,"BF_MSG_TO_OPERROSTER_TASK_FINISHED");
		
		// remove from dialog
		BF_GUI_OperRoster_Dialog* poOperDialog = (BF_GUI_OperRoster_Dialog*)poWin->FindView("operroster_dialog");
		
		if(poOperDialog && loTask.CountItems()>1){
			poOperDialog->poTasksMenu->DeleteItemAt(iIndex);
		}
						
		// delete task
		loTask.RemoveItem(poTask);		
		DELETE(poTask);		
		
		// check existing unfinished tasks
		if(loTask.CountItems()==0){
			Close();
		}
		
		break;}
	case BF_MSG_TO_OPERROSTER_CLOSELENTA:
		if(0==iDisableCount)	SetViewMode(BF_GUI_OPERROSTER_VIEW_DIALOG);
		break;
	case BF_MSG_TO_OPERROSTER_CLOSEDIALOG:{		
		SetViewMode(BF_GUI_OPERROSTER_VIEW_LENTA);		
		break;}
	case BF_MSG_TO_OPERROSTER_TASK_PAUSED:{		
					
		BF_GUI_OperTask *poTask=NULL;
		ASSERT(B_OK==po_Message->FindPointer("bf_poTask",(void**)&poTask) && poTask);
		int32 iIndex = loTask.IndexOf(poTask);
		if(iIndex<0) return;
		
		switch(iViewMode){
		case BF_GUI_OPERROSTER_VIEW_DIALOG:{
			BF_GUI_OperRoster_Dialog* poOperDialog = (BF_GUI_OperRoster_Dialog*)poWin->FindView("operroster_dialog");
			if(!poOperDialog) return;
			BF_GUI_OperRoster_Dialog_Item *poItem = (BF_GUI_OperRoster_Dialog_Item*)poOperDialog->poTasksMenu->ItemAt(iIndex);
			
			poItem->iTaskState = BF_GUI_OPERTASK_STATE_PAUSE;
			if(iDisableCount==0)  poOperDialog->poTasksMenu->DrawItemExt(poOperDialog->poTasksMenu,iIndex,true);
		
			break;}
/*		case BF_GUI_OPERROSTER_VIEW_LENTA:{
			UpdateLenta();
			break;}*/
		};			
		break;}
	case BF_MSG_TO_OPERROSTER_SETPROGRESS:{
				
		if(iDisableCount!=0) return;
					
		BF_GUI_OperTask *poTask=NULL;
		ASSERT(B_OK==po_Message->FindPointer("bf_poTask",(void**)&poTask) && poTask);
		int32 iIndex = loTask.IndexOf(poTask);
		if(iIndex<0) return;
		
		switch(iViewMode){
		case BF_GUI_OPERROSTER_VIEW_DIALOG:{
			BF_GUI_OperRoster_Dialog* poOperDialog = (BF_GUI_OperRoster_Dialog*)poWin->FindView("operroster_dialog");
			if(!poOperDialog) return;
			BF_GUI_OperRoster_Dialog_Item *poItem = (BF_GUI_OperRoster_Dialog_Item*)poOperDialog->poTasksMenu->ItemAt(iIndex);
			poItem->iProgCount = poTask->iTotalCount;
			poItem->iProgIndex = poTask->iTotalIndex;			
			poItem->iTaskState = BF_GUI_OPERTASK_STATE_RUN;
			poOperDialog->poTasksMenu->DrawItemExt(poOperDialog->poTasksMenu,iIndex,true);
			break;}
		case BF_GUI_OPERROSTER_VIEW_LENTA:{
			UpdateLenta();
			break;}
		};		
		break;}
	case BF_MSG_TO_OPERROSTER_ADDTASK:{
		BF_GUI_OperTask *poTask=NULL;
		ASSERT(B_OK==po_Message->FindPointer("bf_poTask",(void**)&poTask) && poTask);
		AddTask(poTask);
		break;}
	default:
		BHandler::MessageReceived(po_Message);
	}
}

void
BF_GUI_OperRoster::UpdateLenta(bool b_Must)
{
	if(iDisableCount!=0) return;

	if(iViewMode!=BF_GUI_OPERROSTER_VIEW_LENTA) return;
	BF_GUI_OperRoster_Lenta *poLenta = (BF_GUI_OperRoster_Lenta*)poWin->FindView("operroster_lenta");
	if(!poLenta) return;
	uint64 iCount,iIndex;
	CalcTotalProg(iCount,iIndex);
	poLenta->DrawProg(iIndex,iCount,b_Must);
}

void
BF_GUI_OperRoster::UpdateDialog()
{
	BF_GUI_OperRoster_Dialog* poOperDialog = (BF_GUI_OperRoster_Dialog*)poWin->FindView("operroster_dialog");
	if(!poOperDialog) return;
	
	BF_GUI_OperTask *poTask=NULL;
	
	for(int i=0;i<poOperDialog->poTasksMenu->ploMenu->CountItems();i++){
		BF_GUI_OperRoster_Dialog_Item *poItem = (BF_GUI_OperRoster_Dialog_Item*)poOperDialog->poTasksMenu->ItemAt(i);
		poTask = TaskAt(i);
		if(!poTask) continue;
		poItem->iProgCount = poTask->iTotalCount;
		poItem->iProgIndex = poTask->iTotalIndex;			
		poOperDialog->poTasksMenu->DrawItemExt(poOperDialog->poTasksMenu,i,true);	
	}	
}

void
BF_GUI_OperRoster::CalcTotalProg(uint64 & iCount,uint64 & iIndex)
{
	BF_GUI_OperTask *poTask;
	float f=0,f0=0;
	iCount=100;
	iIndex=0;
	
	for(int i=0;i<loTask.CountItems();i++){
		poTask = TaskAt(i);
		f = (float)poTask->iTotalIndex/(float)poTask->iTotalCount*100.0;
		if(i>0)	f0 = (f+f0)/2; else f0=f;
	}
	iIndex = (uint64)f0;
}

status_t						
BF_GUI_OperRoster::AddTask(BF_GUI_OperTask *po_Task)
{
	ASSERT(po_Task);
	loTask.AddItem(po_Task);	
	//
	if(poSysSetup->MainStyle() & BF_SETUP_OPERROSTER_DEFDIALOG)
		SetViewMode(BF_GUI_OPERROSTER_VIEW_DIALOG);
	else
		SetViewMode(BF_GUI_OPERROSTER_VIEW_LENTA);
	
	po_Task->RunThread();
	
	return B_OK;
}

bool 
BF_GUI_OperRoster::CanMinimase()
{
	bool bCanMinimase = true;
	for(int i=0;i<loTask.CountItems();i++){
		bCanMinimase = TaskAt(i)->iStyles & BF_GUI_OPERTASK_STYLE_CANMINIMASE;
		if(!bCanMinimase) break;
	}
	return bCanMinimase;
}

void
BF_GUI_OperRoster::SetViewMode(int32 i_NewViewMode)
{
	BAutolock oLocker(poWin);
		
	// check for existing mode 
	if(i_NewViewMode == iViewMode) return 	;
	
	// handle closing old view_mode
	
	if(BF_GUI_OPERROSTER_VIEW_LENTA==iViewMode){
		BF_GUI_OperRoster_Lenta *poLenta = (BF_GUI_OperRoster_Lenta*)poWin->FindView("operroster_lenta");
		poLenta->RemoveSelf();
		DELETE(poLenta);				
	}else
	if(BF_GUI_OPERROSTER_VIEW_DIALOG==iViewMode){
	
		// check can_minimase dialog //
		if(!CanMinimase()) return;
	
	
		BF_GUI_Func_PanelsEnable( true ,false);
		BF_GUI_OperRoster_Dialog* poOperDialog = (BF_GUI_OperRoster_Dialog*)poWin->FindView("operroster_dialog");
		if(poOperDialog){
			poOperDialog->RemoveSelf();
			DELETE(poOperDialog);
		}
	}
		
	
	// switch to new mode
	if(BF_GUI_OPERROSTER_VIEW_HIDED == i_NewViewMode){
		if(BF_GUI_OPERROSTER_VIEW_DIALOG==iViewMode){
			SetFocusToOld();
		}
		iViewMode = BF_GUI_OPERROSTER_VIEW_HIDED;				
	}else
	if(BF_GUI_OPERROSTER_VIEW_LENTA==i_NewViewMode){				
		iViewMode = BF_GUI_OPERROSTER_VIEW_LENTA;		
		BF_GUI_Func_AddChildToMainView( new BF_GUI_OperRoster_Lenta() );		
		SetFocusToOld();
	}else
	if(i_NewViewMode==BF_GUI_OPERROSTER_VIEW_DIALOG){
		
		StoreCurrentFocus(); 
		
		// disable panels
		BF_GUI_Func_PanelsEnable( false,false );
		BF_GUI_OperRoster_Dialog* poDialog = new BF_GUI_OperRoster_Dialog(BRect(0,0,400,300));
	
		// make tasks_menu
		BRect oRect;	
		{
			BL_List				*ploItem = new BL_List();
			BF_GUI_OperTask		*poTask=NULL;
			for(int i=0;i<loTask.CountItems();i++){
				poTask = TaskAt(i);
				ploItem->AddItem( new BF_GUI_OperRoster_Dialog_Item(poTask->sName.String(),poTask->iState));
			}
			poDialog->LocalBounds(oRect);
			oRect.bottom -= poSysSetup->oFontToolView.fHeight*2-2;
			poDialog->poTasksMenu = new BF_GUI_DlgView_VMenu(oRect,"tasks",B_FOLLOW_ALL,ploItem,
				BF_GUI_DLGVIEW_VMENU_NAV_PARENTINFORM,(float)(int)poSysSetup->oFontToolView.fHeight*3);
			poDialog->AddChild( poDialog->poTasksMenu );
		}
		
		// make divider_line
		oRect.bottom++;
	    oRect.top=oRect.bottom;
	    poDialog->AddChild(new BF_GUI_ViewFrame(oRect,B_FOLLOW_BOTTOM|B_FOLLOW_LEFT_RIGHT));
		
		// make bottom menu 
		BRect oRect1;
		poDialog->LocalBounds(oRect1);
		oRect.left = oRect1.left;
		oRect.right = oRect1.right;	
		oRect.top = oRect.bottom+10;
		oRect.bottom = oRect.top + poSysSetup->oFontToolView.fHeight;
		BL_List *ploMenu = new BL_List();
		ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_ROSTER_PAUSERUN),"pause"));
		ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_CANCEL),"cancel"));
		ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_ROSTER_MINIMISE),"minimase"));
		poDialog->AddMenu(oRect,ploMenu,true);		
		
		// finish 		
		poDialog->MoveToCenter( poWinView );					
		BF_GUI_Func_AddChildToMainView ( poDialog );			
		//
		iViewMode = BF_GUI_OPERROSTER_VIEW_DIALOG;
		//
		UpdateDialog();
	}
}

BF_GUI_OperTask*
BF_GUI_OperRoster::TaskAt(int32 i_Index)
{
	return (BF_GUI_OperTask*)loTask.ItemAt(i_Index);
}

void							
BF_GUI_OperRoster::Close()
{
	SetViewMode(BF_GUI_OPERROSTER_VIEW_HIDED);
}

float							
BF_GUI_OperRoster::fHeightInWinMain()
{
	switch(iViewMode){
	case BF_GUI_OPERROSTER_VIEW_LENTA:{
		BF_GUI_OperRoster_Lenta *poLenta = (BF_GUI_OperRoster_Lenta*)poWin->FindView("operroster_lenta");
		if(!poLenta) return 0;
		return poLenta->Frame().Height();}
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////

void
BF_GUI_OperRoster_Init()
{
	new BF_GUI_OperRoster();
}

void
BF_GUI_OperRoster_AddTask(BF_GUI_OperTask *po_Task)
{
	ASSERT(poOperRoster && po_Task);
	BMessenger 	oMessenger(poOperRoster);
	BMessage	oMessage(BF_MSG_TO_OPERROSTER_ADDTASK);
	oMessage.AddPointer("bf_poTask",po_Task);
	oMessenger.SendMessage(&oMessage);
}

///////////////////////////////////////////////////////////////////////////
