/*
===============================================
Project:	BeFar
File:		BF_BetaTesting.cpp
Desc:		Provides functions for checking  and setting beta-testing mode
Author:		Baza
Created:	20.11.99
Modified:	20.11.99
===============================================
*/


#include <Application.h>
#include <InterfaceKit.h>
#include <Window.h>
#include <StorageKit.h>
#include <Roster.h>

#include "BF_Roster.h"
#include "BF_BetaTesting.h"
#include "BF_GUI_WinMain.h"
#include "BF_Msg.h"
#include "BF_GUI_Func.h"
#include "BF_GUI_DlgViews.h"
#include "BF_GUI_Setup.h"

uint32 iBetaTestingStatus=BF_BETATEST_OK;
#define BETA_FILE (const char*)"beta.settings"


void
BF_BetaTesting_InitStatus()
{
	/*
	if(B_OK!=BF_Roster_MakeDir(BEFAR_SETTINGS_DIR)){
		BF_Dialog_Alert_Sep("can`t make settings dir","",NULL,poWin->CurrentFocus());
		return; 
	}
	// check first step
	{
		BL_String s;
		s=BEFAR_SETTINGS_DIR;
		s<< BETA_FILE;
		status_t uRes;
		BEntry oConfigEntry(s.String());		
		if(B_OK!=oConfigEntry.InitCheck() || !oConfigEntry.Exists()){
			// ok...make first step...
			{			
				// create config file
				BFile oConfigFile(&oConfigEntry,B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);			
				uRes = oConfigFile.InitCheck();
				if(B_OK!=uRes){
					Debug_Error("ups.I can`t create config file ",uRes);
					iBetaTestingStatus = BF_BETATEST_QUIT;
					return ;
				}	
			}
			// open intro doc 
			{
				app_info uAppInfo;
				be_app->GetAppInfo(&uAppInfo);
				BEntry 	oEntry(&uAppInfo.ref);
				oEntry.GetParent(&oEntry);
				BPath	oPath;
				oEntry.GetPath(&oPath);
				
				BL_String s(oPath.Path());
				s<<"/doc_beta_start.txt";
			
				BEntry oTextFile(s.String());
				uRes = BF_Roster_RunEntry(oTextFile);
				iBetaTestingStatus = BF_BETATEST_QUIT;
				if(uRes!=B_OK)		Debug_Error("ups.I can`t run doc_beta_start.txt",uRes);
			}
			return ;			
		}
		// check key 
		{
			// open file 
			BFile oConfigFile(&oConfigEntry,B_READ_WRITE);		
			int32 iVersion;	
			// read attr
			ssize_t iCount = oConfigFile.ReadAttr("beta_version",B_INT32_TYPE,0,(void*)&iVersion,sizeof(int32));
			if(iCount<=0){
				iBetaTestingStatus = BF_BETATEST_ASK_KEY;				
				return;
			}
			
		}
	}
	*/
}

void
BF_BetaTesting_CheckKey_Start(BView * po_ViewOwner)
{
	/*
	// disable panels 
	BF_GUI_Func_PanelsEnable(false);	
	//
	BMessage oMessage(BF_MSG_FILEPANEL_BETATEST_INPUTPASSWORD); 
	oMessage.AddPointer("bf_focus",(void*)po_ViewOwner);
	//
	// make dialog 
	BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(BRect(0,0,300,0),
		"Input betatest code","dialog",&oMessage,B_FOLLOW_NONE);	
	BRect oRect;	
	// insert edit 
	poDialog->LocalBounds(oRect);	
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	BF_GUI_ViewEdit *poEdit = new BF_GUI_ViewEdit(oRect,"edit",
					"",B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
	poDialog->AddChild(poEdit);	
	// finish 
	poDialog->SetHeightFromLastChild();
	poDialog->MoveToCenter( poWinView );
	BF_GUI_Func_AddChildToMainView(poDialog);	
	*/
}


void
BF_BetaTesting_CheckKey_Finish(char *pc_Key)
{
/*
	// check key 
	if(strcmp(pc_Key,"parol")==0){
		iBetaTestingStatus = BF_BETATEST_OK;
		
		if(B_OK!=BF_Roster_MakeDir(BEFAR_SETTINGS_DIR)){
			BF_Dialog_Alert_Sep("can`t make settings dir","",NULL,poWin->CurrentFocus());
			return; 
		}
	
		BL_String s;
		s=BEFAR_SETTINGS_DIR;
		s<< BETA_FILE;
		
		BEntry oConfigEntry(s.String());		
		if(B_OK==oConfigEntry.InitCheck() && oConfigEntry.Exists()){
			BFile oConfigFile(&oConfigEntry,B_READ_WRITE);			
			int32 iVersion = 1;
			oConfigFile.WriteAttr("beta_version",B_INT32_TYPE,0,(void*)&iVersion,sizeof(int32));			
			return;
		}
	}
	// bad key, exit
	{
		Debug_Error("Wrong key",B_ERROR);
		exit(-1);
		return;
	}		
*/	
}