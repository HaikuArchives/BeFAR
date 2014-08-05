/*
===============================================
Project:	BeFar
File:		BF_GUI_ArcPanel.cpp
Desc:		Provides archive files viewing in BeFar panels, uses standard 
		archive commands with output redirecting.
Author:		Nexus
CoAuthor:   Baza
Created:	05.11.99
Modified:	14.03.2000
Version:	0.1C
===============================================
*/
		
#include "BF_GUI_ArcPanel.h"
#include "BL_Tools.h"	// I'm using BL_String from there
#include <Be.h>
#include <stdio.h>
#include "TextFile.h"
#include "BF_GUI_KeysMenu.h"
#include "BF_GUI_Func.h"
#include "BF_GUI_WinMain.h"

///////////////////////////////////////
// Misc defines

//#define TEMPORARY_FILE	"/boot/home/1111.txt"
#define LIST_ZIP	"/boot/beos/bin/zipinfo -s %s >%s"
#define LIST_TAR	"/boot/beos/bin/tar -t -v -f%s > %s"


///////////////////////////////////////
//	TO ROSTER:
#define BF_LISTING_STYLE_ZIP	"BLZP"
#define BF_LISTING_STYLE_TAR	"BLTR"
#define BF_LISTING_STYLE_GZ		"BLGZ"

/*
bool BF_Roster_ParseNodeList(BF_Path &o_Path,BF_NodeList & lo_Node,int i_Style, uint32 u_ListingStyle)
{
	lo_Node.DeleteItems();	
	lo_Node.poPath = &o_Path;
//	BDirectory  oDir(o_Path.Path());
	BEntry 		oEntry;
	char		pc[500];
	BF_Node		*poBFNode;
	BFile 		oFile;
	BNodeInfo	oInfo;
	struct stat st;
	BL_String s;
	s= o_Path.Path();
	if((BF_ROSTER_LOAD_NODE_PARENTDIR & i_Style) && s.CountChars()>1){
		poBFNode = new BF_Node("..");
		poBFNode->iType = BF_NODE_TYPE_PARENTDIR;
		lo_Node.AddItem(poBFNode);
	}
//	if(B_OK!=oDir.Rewind()) return(false);	
//	while(B_OK==oDir.GetNextEntry(&oEntry)){
//		poBFNode = BF_Roster_LoadNodeList_Entry(oEntry,oFile,pc,&st,i_Style,oInfo);
//		if(poBFNode) lo_Node.AddItem(poBFNode);
//	}
	return(true);
}
*/
//	ENDOF TO ROSTER


///////////////////////////////////////
BF_GUI_ArcPanel::BF_GUI_ArcPanel(const BRect & o_Rect,bool b_WinPos_OnLeft)
:BF_GUI_NodePanel(o_Rect,b_WinPos_OnLeft)
{
	{ // load folder icon ///
		poFolderSIcon = NULL;
		BMimeType	oMimeType;
		if(B_OK==oMimeType.SetTo("application/x-vnd.Be-directory")){
			poFolderSIcon = new BBitmap(BRect(0,0,15,15),B_CMAP8);
			if(B_OK!=oMimeType.GetIcon(poFolderSIcon,B_MINI_ICON)){
				DELETE(poFolderSIcon);
			}
		}
		poDocSIcon	= NULL;
		if(B_OK==oMimeType.SetTo("application/octet-stream")){
			poDocSIcon = new BBitmap(BRect(0,0,15,15),B_CMAP8);
			if(B_OK!=oMimeType.GetIcon(poDocSIcon,B_MINI_ICON)){
				DELETE(poDocSIcon);
			}
		}	
	}
	
	Calc();

	// TODO: Init() should be called outside with *real* filename
	//Init("/boot/home/test.zip", "");
	// 	
}


///////////////////////////////////////
void BF_GUI_ArcPanel::Init(BL_String s_ArcFilename, BL_String s_InnerDir) 
{
	
	if(sArcFilename != s_ArcFilename) {
		sArcFilename = s_ArcFilename;
		BPath temp_path;
		find_directory(B_SYSTEM_TEMP_DIRECTORY, &temp_path);
		sTempFile = temp_path.Path();
		BL_String str(sArcFilename);
		str.Remove(0, str.FindLast('/'));
		sTempFile.Append(str);
		sTempFile.Append(".tmp");
		char param[1024];
		sprintf(param, LIST_ZIP, sArcFilename.String(), sTempFile.String());
	    system(param); // execute archiver command
	}
	sInnerDir = s_InnerDir;
	RefreshNodeList();
	BL_String str(sArcFilename);
	str.Remove(0, str.FindLast('/')+1);
	BBitmap *poPackageSIcon = NULL;
	BMimeType	oMimeType;
	if(B_OK==oMimeType.SetTo("application/zip")){ //  doesn't work. can't even imagine where do they get this icon.
		poPackageSIcon = new BBitmap(BRect(0,0,15,15),B_CMAP8);
		if(B_OK!=oMimeType.GetIcon(poPackageSIcon,B_MINI_ICON)){
			DELETE(poPackageSIcon);
		}
	}
	SetHeader(str.String(),poPackageSIcon);
	
	RefreshNodeList();
}

///////////////////////////////////////
BF_GUI_ArcPanel::~BF_GUI_ArcPanel() {
	
	// TODO: delete temp file here
	
	DELETE(poFolderSIcon);
	DELETE(poDocSIcon);
}

///////////////////////////////////////
void
BF_GUI_ArcPanel::RefreshNodeList() 
{
   	KTextFile 	temp(sTempFile.String(), B_READ_ONLY);
   	BL_String 	str, perms, sSize, sDate;
   	status_t 	scode = temp.ReadString(&str); // first string with common info
   	BF_NodeCollection	loNewNode;
		
	BF_Node *poNode = new BF_Node("..");		
	poNode->iType = BF_NODE_TYPE_PARENTDIR;
	loNewNode.AddItem(poNode);
	
   	while (B_OK == scode) {   	
   		scode = temp.ReadString(&str);
   		str.MoveInto(perms, 0, 10); // move permissions string
   		if((perms.CountChars() == 0) || (perms[0] >= '0')&&(perms[0] <= '9'))
   			break;
   		str.Remove(0, 8); // delete " 2.3 be " string
		while(str[0] == ' ')
			str.Remove(0,1); // remove leading zeros
    		int32 iPos = str.FindFirst(' ', 0);
    		if(iPos>=0){
	    		str.MoveInto(sSize, 0, iPos);
	    		str.Remove(0,9); // delete " bx defN " 
	    		str.MoveInto(sDate, 0, 15);
	    		str.Remove(0,1);
	    	}

		// now there is only filepath in str
		if(str == sInnerDir) {
			continue;
		}
		iPos = str.FindFirst(sInnerDir,0);
		if(iPos == 0)
			str.Remove(0, sInnerDir.CountChars());
		else
			continue;
    		iPos = str.FindFirst('/',0);
    		if((iPos < (str.CountChars()-1)) && (iPos > 0))
   			continue; // throw it out if not in current dir
    		if(iPos > 0)
	    		str.Remove(iPos,1); // remove last slash
	
		poNode = new BF_Node(str.String());

		switch(perms[0]) {
		case 'd':
			poNode->iType = BF_NODE_TYPE_DIR;
			if(poFolderSIcon) poNode->CopySIconFrom(poFolderSIcon);
			break;
		case '-':
			poNode->iType = BF_NODE_TYPE_FILE;
			if(poDocSIcon) poNode->CopySIconFrom(poDocSIcon);
			break;			
		case 'l':
			poNode->iType = BF_NODE_TYPE_LINK;
			if(poDocSIcon) poNode->CopySIconFrom(poDocSIcon);
			break;
		}
		poNode->iSize = atoi(sSize.String());
		loNewNode.AddItem(poNode);
   	} 
	loNewNode.Sort(iSortType );			
	loNode.DeleteItems();
	Nodes_Add(NULL,&loNewNode);
}


///////////////////////////////////////
bool
BF_GUI_ArcPanel::NavEnter() {
	BF_Node *poNode = Nodes_Focus();
	if(!poNode) return true;
			
	switch(poNode->iType){
	case BF_NODE_TYPE_PARENTDIR:{
		if(IS_EMPTY(sInnerDir))
			return true;	// go out from archive panel.
		sInnerDir.RemoveLast("/");
		int32 iPos = sInnerDir.FindLast('/');
		if(iPos == -1)
			iPos = 0;
		sInnerDir.Remove(iPos, sInnerDir.CountChars() - iPos);
					
		if(sInnerDir.CountChars() > 0)
			sInnerDir += '/';
		Init(sArcFilename.String(), sInnerDir.String());
		DrawNodes(true);
		DrawListInfo(true);
		DrawSelListInfo(true);
		NavChangeCursor(0);
		if(poSysKeysMenu) PrepareKeysMenu();
		return true;
		}
	case BF_NODE_TYPE_DIR:{
		BL_String str(sInnerDir.String());
		str.Append(poNode->sName.String());
		str += '/';
		Init(sArcFilename, str);
		DrawNodes(true);
		DrawListInfo(true);
		DrawSelListInfo(true);
		NavChangeCursor(0);
		if(poSysKeysMenu) PrepareKeysMenu();
		return true;}	
	}
	return true;
}


///////////////////////////////////////
void 
BF_GUI_ArcPanel::MessageReceived(BMessage* po_Message) {
	BF_GUI_NodePanel::MessageReceived(po_Message);	
}


///////////////////////////////////////
bool
BF_GUI_ArcPanel::OnKeyDown(const char *bytes, int32 numBytes) {
	return BF_GUI_NodePanel::OnKeyDown(bytes, numBytes);
}

///////////////////////////////////////
void
BF_GUI_ArcPanel_OpenFile(const char *pc_Path,const char *pc_FileName,bool b_PosLeft)
{
	ASSERT(pc_Path && pc_FileName);
	
	BL_String s(pc_Path);
	s<<"/";
	s<<pc_FileName;
	
	BF_GUI_ArcPanel *po;	
	po = new  BF_GUI_ArcPanel(poWinView->PanelRect(b_PosLeft),b_PosLeft);
	po->Init(s,BL_String());
	
	poWinView->AddPanel(po);	
}
