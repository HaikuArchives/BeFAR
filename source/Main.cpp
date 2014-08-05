//#include "Monitor.h"
#include "Main.h"
#include "BF_Def.h"
#include "BF_GUI_WinMain.h"
#include "BL_Tools.h"
#include <Alert.h>

#include <string.h>
#include <stdio.h>

#include "BF_BetaTesting.h"

BF_App *poApp = NULL;

BF_App::BF_App(const char* pc_AppFileName) : BApplication(APP_SIGNATURE)
{	
	ASSERT(pc_AppFileName);	
	
	poApp = this;
	sAppFileName = pc_AppFileName;
	bAskedQuit = false;
	//
	BF_BetaTesting_InitStatus();  
	if(BF_BETATEST_QUIT==iBetaTestingStatus){	
		be_app->PostMessage(B_QUIT_REQUESTED);
		return;
	}	
	BF_GUI_WinMain *po = new BF_GUI_WinMain();
	po->Show();		
}

const BString	
BF_App::AppFileName()
{
	return BString(sAppFileName);
}

bool BF_App::QuitRequested()
{
	/*
	if(!bAskedQuit){
		bAskedQuit = true;
		BMessage	oMessage(BF_MSG_MAINVIEW_ASK_QUIT);
		BMessenger 	oMessenger(poWinView);
		oMessenger.SendMessage(&oMessage);		
		return false;
	}else{
		return true;
	}
	*/
	return true;
}

void
BF_App::MessageReceived(BMessage * message)
{
	switch(message->what){
	case BF_MSG_MAINVIEW_ANSWER_QUIT:{
		int iQuit = message->FindInt32("bf_iAnswer");
		if(iQuit==0) bAskedQuit = false;
		if(iQuit==1) bAskedQuit = true;
		break;}
	default:
		BApplication::MessageReceived(message);
	}	
}

int
main( int argc, char** argv )
{
	BF_App app(argv[0]);
	app.Run();
	return 0;
}
