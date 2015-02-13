#include <stdio.h>
#include <Application.h>
#include <Autolock.h>

#include "BF_GUI_Misc.h"

#include "BF_Def.h"
#include "BF_Dict.h"
#include "BF_Roster.h"
#include "BF_Msg.h"
#include "BF_GUI_WinMain.h"
#include "BF_GUI_KeysMenu.h"
#include "BF_GUI_CmdLine.h"
#include "BF_GUI_TopMenu.h"
#include "BF_BetaTesting.h"
//
#include "BF_GUI_OperRoster.h"
//
#include "BF_GUI_FilesPanel.h"
#include "BF_GUI_FtpPanel.h"
#include "BF_GUI_ArcPanel.h"
#include "BF_GUI_ImagePanel.h"

#include "BF_GUI_SetupDialog.h"

#include "BF_GUI_OperRoster.h"  

BWindow 			*poWin=NULL; 
BF_GUI_ViewMain 	*poWinView=NULL;

BF_GUI_ViewMain::BF_GUI_ViewMain(const BRect & o_Rect)
:BView(o_Rect,"win_view",B_FOLLOW_ALL,B_WILL_DRAW|B_FRAME_EVENTS)
{
	rgb_color oCol;
	RGB_SET(oCol,0,0,0);
	SetViewColor(oCol);	
	poWinView = this;
	poLastFocus = NULL;	
	/////	
	InitViews();			
	/////
	BF_GUI_OperRoster_Init();
}   

void
BF_GUI_ViewMain::AddPanel(BF_GUI_Panel* po_Panel)
{
	ASSERT(po_Panel);	
	
	BAutolock oLocker(poWin);
	if(!oLocker.IsLocked()){		
		return;
	}

	// hide old panel //
	BF_GUI_Panel* 	poLastPanel = PanelOnTop(po_Panel->bWinPos_OnLeft);
	bool			bThisSideInFocus = false;
	if(poLastPanel){	
		bThisSideInFocus = poLastPanel->IsFocus();
		if(bThisSideInFocus) MakeFocus(false);
		poLastPanel->Enable(false);
		poLastPanel->Hide();
	}

	// add and show new panel //	
	BL_Collection *ploPanel = (po_Panel->bWinPos_OnLeft?&loLeftPanel:&loRightPanel);
	ASSERT( ploPanel );
	
	ploPanel->AddItem( po_Panel );
	loAllPanel.AddItem( po_Panel );
	AddChild ( po_Panel );			
	
	if(bThisSideInFocus) po_Panel->MakeFocus(true);
}

void				
BF_GUI_ViewMain::RemovePanel(BF_GUI_Panel* po_Panel)
{		
	ASSERT(po_Panel);

	BAutolock oLocker(poWin);
	
	BView *poCurrentFocus = Window()->CurrentFocus();
	
	bool bOnTop = (po_Panel == PanelOnTop(po_Panel->bWinPos_OnLeft));	
	
	BL_Collection *ploPanel = (po_Panel->bWinPos_OnLeft?&loLeftPanel:&loRightPanel);
	ASSERT(ploPanel);
	ploPanel->RemoveItem( po_Panel );		
	
	po_Panel->RemoveSelf();
	loAllPanel.RemoveItem( po_Panel );		
	
	if(bOnTop){
		BF_GUI_Panel* poLastPanel = PanelOnTop(po_Panel->bWinPos_OnLeft);
		if(!poLastPanel){
			poLastPanel = PanelOnTop(!po_Panel->bWinPos_OnLeft);
		}
		if(poLastPanel){
			poLastPanel->Show();
			poLastPanel->Enable(true);
			if(poCurrentFocus==po_Panel) poLastPanel->MakeFocus();
		};
	}
	
	DELETE(po_Panel);
}

BF_GUI_Panel*	
BF_GUI_ViewMain::FocusedPanel() const
{
	BF_GUI_Panel* po=NULL;
	BView *poFocus = poWin->CurrentFocus();	
	if(!poFocus) return NULL;
	
	po = PanelOnTop(true);
	if(po==poFocus) return po;
	po = PanelOnTop(false);
	if(po==poFocus) return po;
	return NULL;
}

BF_GUI_Panel*		
BF_GUI_ViewMain::PanelByIndexFromList(const BL_Collection & lo_Panel,int i_Index) const
{	
	if(i_Index<0) i_Index = loLeftPanel.CountItems()-2;
	if(i_Index<0) return NULL;
	BF_GUI_Panel *po = (BF_GUI_Panel*)lo_Panel.ItemAt(i_Index);
	return po;
}

BF_GUI_Panel*		
BF_GUI_ViewMain::PanelByIndex(bool b_Left,int i_Index) const
{
	if(b_Left)
		return PanelByIndexFromList(loLeftPanel,i_Index);
	else
		return PanelByIndexFromList(loRightPanel,i_Index);
}


BF_GUI_Panel*		
BF_GUI_ViewMain::PanelOnTop(bool b_Left) const
{
	return PanelByIndex(b_Left,b_Left?(loLeftPanel.CountItems()-1):(loRightPanel.CountItems()-1));
}

const BRect
BF_GUI_ViewMain::PanelRect(bool b_LeftPos) const
{
	BRect  oRect(Bounds());
	
	float fBotHeight = poSysSetup->oFontNode.fHeight + poSysSetup->oFontCMDLine.fHeight;
	float fWidthPanel = oRect.Width()/2;

	if(b_LeftPos){		
		oRect.bottom-=fBotHeight;
		oRect.right = oRect.left + fWidthPanel;	
	}else{
		oRect.bottom-=fBotHeight;
		oRect.left+=fWidthPanel;
		oRect.right = oRect.left + fWidthPanel;	
	}
	
	return oRect;
			
}

void
BF_GUI_ViewMain::InitViews()
{			
	//
	BRect oRect;	
	
	/* set command_view */	
	
	/* BF_MSG_SETUP_UPDATED
	oRect = Bounds();
	oRect.top = oRect.bottom-fBotHeight;
	oRect.bottom-= fBotHeight;
	poBackground = new BView(oRect,"background",B_FOLLOW_LEFT_RIGHT|B_FOLLOW_BOTTOM,
		B_WILL_DRAW);	
	poBackground->SetViewColor(0,0,0);	
	AddChild(poBackground);				
	*/
	/* make  files panel */		
	
	//	default left panel 		
	
	BF_GUI_FilesPanel *po1;
	po1 = new BF_GUI_FilesPanel(PanelRect(true),BF_PANEL_WINPOS_LEFT);	
	AddPanel(po1);	
	po1->Calc();
		
	//	default right panel 		
	
	/*BF_GUI_FtpPanel *po2;
	po2 = new 	BF_GUI_FtpPanel(oRect,BF_PANEL_WINPOS_RIGHT);			
	AddPanel(po2,false);	*/
	/*BF_GUI_ImagePanel *po2;
	po2 = new 	BF_GUI_ImagePanel(oRect,"image",BF_PANEL_WINPOS_RIGHT);			
	AddPanel(po2,false);	*/	
		
	BF_GUI_FilesPanel *po2;	
	po2 = new BF_GUI_FilesPanel(PanelRect(false),BF_PANEL_WINPOS_RIGHT);	
	AddPanel(po2);	
	po2->Calc();
	
	//	
	/* make bottom command_line */		
	oRect = Bounds();
	oRect.bottom -= poSysSetup->oFontNode.fHeight;
	oRect.top = oRect.bottom-poSysSetup->oFontCMDLine.fHeight;
	AddChild( new BF_GUI_CmdLine(oRect) );

	/* make bottom keys_menu */	
	oRect = Bounds();
	oRect.top = oRect.bottom-poSysSetup->oFontNode.fHeight+1;
	poSysKeysMenu = new BF_GUI_KeysMenu(oRect);
	AddChild(poSysKeysMenu);
	//
	//poPanelLeft->PrepareKeysMenu();
	//	
	poWin->AddShortcut('n',B_COMMAND_KEY,new BMessage(BF_MSG_MAINVIEW_RUN_BEFAR),this);		
	//		
}

void 				
BF_GUI_ViewMain::AttachedToWindow(void)
{
	BF_GUI_FilesPanel *poPanel = NULL;
	
	poPanel = (BF_GUI_FilesPanel*)PanelOnTop(true);
	if(poPanel)	poPanel->Pub_GoPath(poSysSetup->oVars.Loaded()?poSysSetup->oVars.oLPanel.sPath.String():"/boot/home");	
	poPanel = (BF_GUI_FilesPanel*)PanelOnTop(false);
	if(poPanel)	poPanel->Pub_GoPath(poSysSetup->oVars.Loaded()?poSysSetup->oVars.oRPanel.sPath.String():"/boot/home");	
	
	poPanel = (BF_GUI_FilesPanel*)PanelOnTop(true);
	if(poPanel) poPanel->MakeFocus();
	//	
	//poPanelLeft->ChangePath(BF_Path("/"));		
	////poPanelRight->ChangePath(BF_Path("/boot/home/mail/BeOS/UserTalks"));
	//poPanelRight->ChangePath(BF_Path("/boot/home"));
	//		
	BView::AttachedToWindow();

	if(iBetaTestingStatus==BF_BETATEST_ASK_KEY) BF_BetaTesting_CheckKey_Start(PanelOnTop(true));
}

void				
BF_GUI_ViewMain::FrameResized(float width, float height)
{
	//BView::FrameResized(width,height);
	//return;

	BMessage oMessage(BF_MSG_VIEW_MAINWIN_RESIZED);
	oMessage.AddFloat("width",width);
	oMessage.AddFloat("height",height);

	Window()->Lock();
	for(int i=0;i<CountChildren();i++){
		//ChildAt(i)->FrameResized(width,height);	
		BMessenger oMessenger(ChildAt(i));
		oMessenger.SendMessage(&oMessage);	
	}
	Window()->Unlock();
	/*
	printf("resize\n");
	//
	Window()->Lock();
	BRect oRect=Bounds();
	poPanelLeft->ResizeTo(oRect.Width()/2-1,oRect.Height()-CMD_SIZE);
	//
	oRect=Bounds();
	poPanelRight->MoveTo(oRect.Width()/2,0);
	poPanelRight->ResizeTo(oRect.Width()/2,oRect.Height()-CMD_SIZE);
	Window()->Unlock();
	*/
}

void
BF_GUI_ViewMain::Spec_MouseDown(const BPoint & o_Point,BView *po_FromView)
{
	BView *poView;
	BRect oChildFrame;
	for(int iChild=CountChildren()-1;iChild>=0;iChild--){
		poView = ChildAt(iChild);
		if(poView==po_FromView) continue;
		
		oChildFrame = poView->Frame();		
		if(oChildFrame.Contains(o_Point)){
			BPoint oPoint(o_Point);
			oPoint.x -= oChildFrame.left;
			oPoint.y -= oChildFrame.top;			
			poView->MouseDown(oPoint);
			return;
		}		
	}
}

void				
BF_GUI_ViewMain::MouseDown(BPoint point)
{
	Spec_MouseDown(point,this);
}

void
BF_GUI_ViewMain::MessageReceived(BMessage * po_Message) 
{	
	ASSERT(po_Message);

	switch(po_Message->what){	
	//////////
	case BF_MSG_MAINVIEW_SELECTDICT:
		Action_SelectDict();
		break;
	case BF_MSG_MAINVIEW_SELECTDICT_1:
		Action_SelectDict_1(po_Message);
		break;
	/////////	
	case BF_MSG_SETUP_UPDATED:{					
		if(po_Message->HasPointer("BF_GUI_ViewMain::sender")) return;
		BView *poView;
		BMessage oMessage(BF_MSG_SETUP_UPDATED);		
		oMessage.AddPointer("BF_GUI_ViewMain::sender",(void*)this);
		for(int i=0;i<CountChildren();i++){
			poView = ChildAt(i);
			if(!poView) continue;
			BMessenger oMessenger(poView);
			oMessenger.SendMessage(&oMessage);			
		}
		break;}
	/////////	
	case BF_MSG_MAINVIEW_COMPART_PANELS:
		Action_Compare_Panels();
		break;
	case BF_MSG_MAINVIEW_SELVOL_FINISH:{
		EnablePanels(true);
		
		BView *poView=NULL;
		ASSERT(B_OK==po_Message->FindPointer("bf_panel_focus",(void**)&poView) && poView);
		poView->MakeFocus();
		
		bool bPosLeft;
		ASSERT(B_OK==po_Message->FindBool("bf_bPosLeft",&bPosLeft));						
		BF_GUI_Panel *poPanel = PanelByIndex(bPosLeft,0);
		if(!poPanel) return;
		
		// check this panel for top_position
		while(true){
			BF_GUI_Panel *poTopPanel = PanelOnTop(bPosLeft);
			if(poPanel==poTopPanel) break;
			// close top panel
			if(!poTopPanel->Close()) return;
		}
		
		const char *pcPath=NULL;
		if(B_OK!=po_Message->FindString("menu_code",&pcPath) || !pcPath) return;
		
		
		if(strcmp(pcPath,":ftp")==0){
			Action_Open_FtpPanel(bPosLeft);
			break;
		}
		
		BL_String sPath("/");
		sPath<<pcPath;
				
		BMessage	oMessage(BF_MSG_TO_PANEL_SET_PATH);
		BMessenger	oMessenger(poPanel);
		oMessage.AddString("bf_cPath",sPath.String());
		oMessenger.SendMessage(&oMessage);						
		
		break;}
	case BF_MSG_MAINVIEW_SELVOL_LEFT:
		Action_SelVolume_Start(true);
		break;
	case BF_MSG_MAINVIEW_SELVOL_RIGHT:
		Action_SelVolume_Start(false);
		break;
	case BF_MSG_MAINVIEW_MOUNTVOLUMES:
		Action_MountVolumes();
		break;
	case BF_MSG_MAINVIEW_ABOUT:{
		BL_List *poList = new BL_List();
		poList->AddItem(new BF_GUI_ViewMenu_Item("cool!","ok"));
		poList->AddItem(new BF_GUI_ViewMenu_Item("sweet!","ok"));
		BF_Dialog_Alert_Sep("About box","BeFAR, developed by Baza & BeRussia Team",
			poList,poWin->CurrentFocus(),"baza@benews.ru  ,  https://github.com/HaikuArchives/BeFAR");
		break;}
	case BF_MSG_MAINVIEW_EMPTY_TRASH:
		Action_EmptyTrash();
		break;
	// oper_roster hide self dialog, show self lenta and requere resize all panel
	case BF_MSG_TO_WIN_OPERLENTA_SHOWED:{
		int32 iSizeY = 0;
		ASSERT(B_OK==po_Message->FindInt32("bf_iSizeY",&iSizeY));
		BRect oRect;
		for(int i=0;i<loAllPanel.CountItems();i++){
			BF_GUI_Panel *poPanel = (BF_GUI_Panel*)loAllPanel.ItemAt(i);
			oRect = poPanel->Frame();
			poPanel->ResizeTo(oRect.Width(),oRect.Height()-iSizeY);
		}
		break;}
	//
	case BF_MSG_MAINVIEW_SAVE_PALLETE:
		Action_LoadSavePalete(false);
		break;
	case BF_MSG_MAINVIEW_SAVE_PALLETE_1:
		Action_SavePalete_1(po_Message);
		break;
	case BF_MSG_MAINVIEW_LOAD_PALLETE:
		Action_LoadSavePalete(true);
		break;
	case BF_MSG_MAINVIEW_LOAD_PALLETE_1:
		Action_LoadPalete_1(po_Message);
		break;
	//
	case BF_MSG_MAINVIEW_MAINSETUP_SETDEFAULT:{
		poSysSetup->InitDefault();
		BMessenger oMessenger(poWin);
		BMessage   oMessage(BF_MSG_SETUP_UPDATED);
		oMessenger.SendMessage(&oMessage);		
		break;}
	case BF_MSG_MAINVIEW_MAINSETUP_LOAD:{
		poSysSetup->Load();
		BMessenger oMessenger(poWin);
		BMessage   oMessage(BF_MSG_SETUP_UPDATED);
		oMessenger.SendMessage(&oMessage);		
		break;}
	case BF_MSG_MAINVIEW_MAINSETUP_SAVE:	
		poSysSetup->Save();
		break;
	case BF_MSG_MAINVIEW_MAINSETUP_0:
		BF_GUI_SetupDialog_Run( Window()->CurrentFocus() );
		break;	
	case BF_MSG_FILEPANEL_BETATEST_INPUTPASSWORD:{
		char *pcValue=NULL;
		ASSERT(B_OK==po_Message->FindString("edit",(const char**)&pcValue) && pcValue);
		BF_BetaTesting_CheckKey_Finish(pcValue);
		//
		EnablePanels(true);		
		BF_GUI_Panel* poPanel = PanelOnTop(true);
		ASSERT(poPanel);
		poPanel->MakeFocus();			
		break;}
	case BF_MSG_MAINVIEW_MOUSE_DOWN:{
		BPoint oPoint;
		BView  *poFromView;
		ASSERT(B_OK==po_Message->FindPoint("bf_point",&oPoint));		
		ASSERT(B_OK==po_Message->FindPointer("bf_view",(void**)&poFromView));
		Spec_MouseDown(oPoint,poFromView);		
		break;}
	case BF_MSG_MAINVIEW_RUN_BEFAR:{
		BL_String sPath;
		BF_Path::GetPathForCurrentApp(sPath);	
		BF_Roster_RunFile(sPath.String(),"BeFar");
		break;};
	case BF_MSG_SHOWPANELS:{
		bool bShow; 
		ASSERT(B_OK==po_Message->FindBool("bf_bShow",&bShow));
		ShowPanels(bShow);
		break;}
	case BF_MSG_ENABLEPANELS:{
		bool bEnable=false;
		ASSERT(B_OK==po_Message->FindBool("bf_bEnable",&bEnable));
		bool bOperRoster;
		ASSERT(B_OK==po_Message->FindBool("bf_bOperRoster",&bOperRoster));		
		EnablePanels(bEnable,bOperRoster);
		break;}		
	case BF_MSG_MAINVIEW_ASK_QUIT:{
	
		// check setup_style 
		if(!(poSysSetup->MainStyle() & BF_SETUP_MAIN_ASK_EXIT)){
			be_app->PostMessage(B_QUIT_REQUESTED);
			return;
		}	
		// check for existing quit_dialog //		
		if(poLastFocus) break;		
		/* check for view_panel */
		poLastFocus = Window()->CurrentFocus();
		if(!poLastFocus) break;
		{
			BL_String s;
			s = poLastFocus->Name();
			if(s.FindFirst("_panel")<0){
				poLastFocus = NULL;
				break;		
			}
		}
		//set  BF_App::bAskedQuit		
		BMessage	oMessage(BF_MSG_MAINVIEW_ANSWER_QUIT);
		oMessage.AddInt32("bf_iAnswer",1);
		BMessenger 	oMessenger(be_app);
		oMessenger.SendMessage(&oMessage);
		// make quit_dialog //		
		BL_List *ploMenuItem = new BL_List();
		ploMenuItem->AddItem(new BF_GUI_ViewMenu_Item("Yes",""));
		ploMenuItem->AddItem(new BF_GUI_ViewMenu_Item("No",""));
		BF_Dialog_Alert_Sep("Quit","Really?",ploMenuItem,this,NULL,BF_MSG_MAINVIEW_ANSWER_QUIT);
		break;}		
	case BF_MSG_MAINVIEW_ANSWER_QUIT:{
		int32 iResult=-1;
		ASSERT(B_OK==po_Message->FindInt32("menu",&iResult));
		if(iResult==0){
			be_app->PostMessage(B_QUIT_REQUESTED);
		}else{
			//clear BF_App::bAskedQuit		
			BMessage	oMessage(BF_MSG_MAINVIEW_ANSWER_QUIT);
			oMessage.AddInt32("bf_iAnswer",0);
			BMessenger 	oMessenger(be_app);
			oMessenger.SendMessage(&oMessage);		
			//		
			BF_GUI_Func_PanelsEnable(true);	
			ASSERT(poLastFocus);
			poLastFocus->MakeFocus(true);
			poLastFocus = NULL;
		}
		break;}
	default:
		BView::MessageReceived(po_Message);
	}
}

/*===========================================================================*/
void				
BF_GUI_ViewMain::EnablePanels(bool b_Enable,bool bOperRoster)
{
	BF_GUI_Panel *poPanel;
	for(int i=0;i<loAllPanel.CountItems();i++){
		poPanel = (BF_GUI_Panel*)loAllPanel.ItemAt(i);
		poPanel->Enable(b_Enable);		
	};	
	if(poCmdLine)	poCmdLine->Enable(b_Enable);
	if(bOperRoster && poOperRoster)	poOperRoster->Enable(b_Enable);
}

void				
BF_GUI_ViewMain::ShowPanels(bool b_Show)
{
	BView *po;
	for(int i=0;i<loAllPanel.CountItems();i++){
		po = ((BView*)loAllPanel.ItemAt(i));
		if(b_Show) 	po->Show();	
		else 		po->Hide();
	};	
	if(poCmdLine){
		if(b_Show) 	poCmdLine->Show();
		else		poCmdLine->Hide();
	}
}

/*===========================================================================*/

#include <ListView.h> /// !!!!!!

/*
class ItemTest:public BListItem{
public:
	virtual	void		DrawItem(BView *owner,
							BRect bounds,
							bool complete = false){};
};
*/

BF_GUI_WinMain::BF_GUI_WinMain()
:BWindow(BRect(50,100,700,550), 
	(BString("BeFar ")<<VERSION_STRING).String() ,
	B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{	
	// load setup // 
	poSysSetup = new BF_GUI_Setup();
	poSysSetup->Load(false);	
	
	// load dictionary //
//	BF_Dict_Load(poSysSetup->sDictFile.String());
	
	// set new win_frame //
	if(poSysSetup->oVars.Loaded()){
		MoveTo(poSysSetup->oVars.oWinFrame.left,poSysSetup->oVars.oWinFrame.top);
		ResizeTo(poSysSetup->oVars.oWinFrame.right-poSysSetup->oVars.oWinFrame.left,poSysSetup->oVars.oWinFrame.bottom-poSysSetup->oVars.oWinFrame.top);
	}
	poWin = this;
	
	
	
	poView = new BF_GUI_ViewMain(Bounds());
	AddChild(poView);	
	
	SetSizeLimits(300.0,10000.0,200.0,10000.0);
}

BF_GUI_WinMain::~BF_GUI_WinMain()
{
	if(poSysSetup->MainStyle() & BF_SETUP_AUTOSAVE) poSysSetup->Save();
}

bool
BF_GUI_WinMain::QuitRequested()
{	
	//be_app->PostMessage(B_QUIT_REQUESTED);		
	
	BMessage	oMessage(BF_MSG_MAINVIEW_ASK_QUIT);
	BMessenger 	oMessenger(poWinView);
	oMessenger.SendMessage(&oMessage);		
	
	return false;
}

void				
BF_GUI_WinMain::WindowActivated(bool b_Active)
{
	//BBitmap *po;
	//BF_GUI_GetBitmapFromResources(po,200);


	if(b_Active){
		BView *poFocus = CurrentFocus();
		if(poFocus){
			BMessenger 	oMessenger(poFocus);
			BMessage	oMessage(BF_MSG_TOTAL_WIN_RECEIVE_FOCUS);
			oMessenger.SendMessage(&oMessage);
		}
	}
}

void				
BF_GUI_WinMain::FrameResized(float width, float height)
{
	BWindow::FrameResized(width,height);
	//poView->FrameResized(width,height);
}

void
BF_GUI_WinMain::MessageReceived(BMessage * po_Message) 
{	
	switch(po_Message->what){
	case BF_MSG_SETUP_UPDATED:{
		BView *poView;
		BMessage oMessage(BF_MSG_SETUP_UPDATED);		
		for(int i=0;i<CountChildren();i++){
			poView = ChildAt(i);
			BMessenger oMessenger(poView);
			oMessenger.SendMessage(&oMessage);			
		}
		break;}
	default:
		BWindow::MessageReceived(po_Message);
	}
}
