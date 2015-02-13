#include <stdio.h>
#include <Locker.h>
#include <Directory.h>

#include "BL_File.h"
#include "BF_GUI_FilesPanel.h"
#include "BF_Dict.h"
#include "BF_Roster.h"
#include "BF_GUI_KeysMenu.h"
#include "BF_GUI_WinMain.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "MakeSH"

void	BF_GUI_FilesPanel::Action_MakeSH_Dialog()
{
	// prepare dest_path
	BL_String sStorePath;
	BF_GUI_Panel *poDestPanel = poWinView->PanelOnTop(!bWinPos_OnLeft);
	sStorePath = poDestPanel?poDestPanel->Path():Path();

	// prepare file name
	BL_String sTargetPath(Path()),sSHName;
	{
		BF_Node *poNode = Nodes_Focus();
		if(poNode){
			sSHName = poNode->Name();
			sSHName << ".sh";
			sTargetPath << "/";
			sTargetPath << poNode->Name();;
		}else{
			sSHName << "run.sh";
		}
	}

	{	
		BF_GUI_Func_PanelsEnable(false);
		/* make dialog */	
		BMessage oMessage(BF_MSG_FILEPANEL_MAKESH_RUN);
		oMessage.AddPointer("bf_focus",this);
				
		BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(BRect(0,0,300,0),
			B_TRANSLATE(BF_DICT_FILES_MKSHLINK),"dialog",oMessage,BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER);
		BRect oRect;	
		/* insert Target */
		poDialog->LocalBounds(oRect);	
		oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
		BF_GUI_ViewEdit_Create(oRect,B_TRANSLATE(BF_DICT_TARGETFILE),poDialog,"bf_cTargetPath",
						sTargetPath.String(),
						B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
		
		/* insert Name */
		oRect.top = oRect.bottom+5;		
		oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
		BF_GUI_ViewEdit_Create(oRect,B_TRANSLATE(BF_DICT_SCRIPTNAME),poDialog,"bf_cScriptName",
						sSHName.String(),
						B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
		/* insert StorePath */
		oRect.top = oRect.bottom+5;		
		oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
		BF_GUI_ViewEdit_Create(oRect,B_TRANSLATE(BF_DICT_INFOLDER),poDialog,"bf_cStorePath",
						sStorePath.String(),
						B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
		/* insert Arguments */
		oRect.top = oRect.bottom+5;
		oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
		BF_GUI_ViewEdit_Create(oRect,B_TRANSLATE(BF_DICT_ARGS),poDialog,"bf_cArgs",
						"",
						B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
		/* insert Check_Copy_Icons */
		oRect.top = oRect.bottom+5;
		oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
		poDialog->AddChild(new BF_GUI_ViewCheck(oRect,"bf_bCopyIcons",
												B_TRANSLATE(BF_DICT_FILES_MKSHLINK_SETICONS),true,
												B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE));
		/* menu */								
		poDialog->AddOkCancelMenu(oRect);		
		/* finish */	
		poDialog->SetHeightFromLastChild();
		poDialog->MoveToCenter( poWinView );	
		BF_GUI_Func_AddChildToMainView ( poDialog );		
	}
}

void		BF_GUI_FilesPanel::Action_MakeSH_Run(BMessage *po_Message)
{
	Debug_Info("!!!");
	ASSERT(po_Message);
	// init vars //
	BL_String sTargetPath,sScriptName,sStorePath,sArgs;
	
	if(		B_OK!=sStorePath.GetFromMessage(po_Message,"bf_cStorePath") 	||
				B_OK!=sScriptName.GetFromMessage(po_Message,"bf_cScriptName")	||
				B_OK!=sTargetPath.GetFromMessage(po_Message,"bf_cTargetPath")				
				)
	{		
		BF_GUI_Func_PanelsEnable(true);
		MakeFocus();
		return;
	}
	sArgs.GetFromMessage(po_Message,"bf_cArgs");
	bool bCopyIcons=true;
	if(B_OK!=po_Message->FindBool("bf_bCopyIcons",&bCopyIcons)) bCopyIcons=true;
	
	// open folder //
	BDirectory oDir(sStorePath.String());
	status_t uRes = oDir.InitCheck();
	if(uRes!=B_OK){
			BL_String sError;
			if(B_ENTRY_NOT_FOUND==uRes)	sError=B_TRANSLATE(BF_DICT_FILES_MKSH_ERROR2); else	BL_System_TranslError(uRes,sError);
			BF_Dialog_Alert_Sep(B_TRANSLATE(BF_DICT_FILES_MKSH_ERROR1),sError.String(),NULL,this);
			return;
	}
	// create file //
	BL_File oFile(&oDir,sScriptName.String(),B_WRITE_ONLY|B_CREATE_FILE|B_FAIL_IF_EXISTS);
	uRes = oFile.InitCheck();
	if(B_OK!=uRes){
		BL_String sError;
		switch(uRes){
		case B_FILE_EXISTS:
			sError = B_TRANSLATE(BF_DICT_FILES_MKSH_ERROR3);
			break;
		case B_PERMISSION_DENIED:
			sError = B_TRANSLATE(BF_DICT_FILES_MKSH_ERROR4);
			break;
		default:
			BL_System_TranslError(uRes,sError);
		}
		BF_Dialog_Alert_Sep(B_TRANSLATE(BF_DICT_FILES_MKSH_ERROR5),sError.String(),NULL,this);
		return;
	}
	
	// write first line //
	oFile.WriteString("#!/bin/sh");
	
	// write data //
	BL_String sData(sTargetPath);
	if(sArgs!=""){
		sData<<" ";
		sData<<sArgs;
	}	
	oFile.WriteString(sData);
	
	// set permisions //
	mode_t uPerms = 0;
	oFile.GetPermissions(&uPerms);
	oFile.SetPermissions(uPerms|S_IXUSR);
	
	// copy icon-attibutes //
	if(bCopyIcons){
		BL_File oFileSrc(sTargetPath.String(),B_READ_ONLY);
		if(B_OK==oFileSrc.InitCheck()){
			oFile.CopyAttributeFrom("BEOS:M:STD_ICON",oFileSrc);
			oFile.CopyAttributeFrom("BEOS:L:STD_ICON",oFileSrc);
		}
	}
	
	// close file //
	oFile.Unset();
	
	BF_GUI_Func_PanelsEnable(true);
	MakeFocus();
}


