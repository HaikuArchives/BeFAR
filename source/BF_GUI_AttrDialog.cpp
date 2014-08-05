/*
===============================================
Project:	BeFar
File:		BF_GUI_AttrDialog.cpp
Desc:		
Author:		Baza and Nexus
Created:	??
Changes:	
		
===============================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <File.h>
#include <fs_attr.h>

#include "BF_Dict.h"
#include "BF_Roster.h"
#include "BF_Msg.h"
#include "BF_GUI_Func.h"
#include "BF_GUI_DlgViews.h"
#include "BF_GUI_AttrDialog.h"
#include "BF_GUI_WinMain.h"  // global window pointer


////////////////////////////////////////
//	Inner class definition
class BF_GUI_AttrDialog_Menu : public BF_GUI_DlgView_VCMenu {
public:
		BF_GUI_AttrDialog_Menu(BRect & o_Rect,BF_Path & o_Path,BF_Node & o_Node);
virtual	void	FrameResized(float width, float height);									
		void 		CalcWidths(BRect & o_Rect);
private:
		BF_Node	*poNode;
		BF_Path	*poPath;
		
		void		Load();
};

#define COLTYPE_WIDTH 130

////////////////////////////////////////
BF_GUI_AttrDialog_Menu::BF_GUI_AttrDialog_Menu(BRect & o_Rect,BF_Path & o_Path,BF_Node & o_Node)
	:BF_GUI_DlgView_VCMenu(o_Rect,"attr_list",B_FOLLOW_ALL,new  BL_List(),BF_GUI_DLGVIEW_VMENU_DEFAULT|BF_GUI_DLGVIEW_VMENU_SET_VSCROLLBAR,new BL_List()) 
{
	poNode = &o_Node;
	poPath = &o_Path;		
	
	SetSetupFont(&poSysSetup->oFontMono);
	
	/* set cols_info */
	{
		delete ploCol;
		ploCol = new BL_List();
		ploCol->AddItem( new BF_GUI_DlgView_VCMenu_Column(BF_DictAt(BF_DICT_NAME),0) );
		ploCol->AddItem( new BF_GUI_DlgView_VCMenu_Column(BF_DictAt(BF_DICT_TYPE),0) );
		ploCol->AddItem( new BF_GUI_DlgView_VCMenu_Column(BF_DictAt(BF_DICT_VALUE),0) );
		CalcWidths(o_Rect);
	}		
	Load();
}


////////////////////////////////////////
void					
BF_GUI_AttrDialog_Menu::FrameResized(float width, float height)
{
	BRect oRect(Bounds());
	CalcWidths(oRect);
	Draw(oRect);			
}


////////////////////////////////////////
void					
BF_GUI_AttrDialog_Menu::CalcWidths(BRect & o_Rect)
{
	float 	fWidth = o_Rect.Width(),f1,f2;
	f1 = fWidth - COLTYPE_WIDTH;
	f2 = f1/2;		
	f1-=f2;
	
	BF_GUI_DlgView_VCMenu_Column *po;
	po = (BF_GUI_DlgView_VCMenu_Column*)ploCol->ItemAt(0);
	po->fWidth = f1;
	po = (BF_GUI_DlgView_VCMenu_Column*)ploCol->ItemAt(1);
	po->fWidth = COLTYPE_WIDTH;	
	po = (BF_GUI_DlgView_VCMenu_Column*)ploCol->ItemAt(2);
	po->fWidth = f1;
}


////////////////////////////////////////
void
BF_GUI_AttrDialog_Menu::Load()
{
	BL_String 	s;
	status_t 	uRes;
	s=poPath->Path();
	s<<"/";
	s<<poNode->sName;
	BFile oSrcFile(s.String(),B_READ_ONLY);
	/* open file */	
	uRes = oSrcFile.InitCheck();
	if(B_OK!=uRes){
		BL_System_TranslError(uRes,s);
		BF_Dialog_Alert(BF_DictAt(BF_DICT_ERROR),BF_DictAt(BF_DICT_ATTRDLG_OPENERROR),NULL,s.String());
		return;		
	}	
	/* clear list */
	BL_List	*ploList = new BL_List();	
	/* load attributes */
	char 		pcName[B_ATTR_NAME_LENGTH];
	attr_info	uAttrInfo;
	uint32		iBufMaxSize=255;
	char		*pcBuf = (char*)malloc(iBufMaxSize);
	BF_GUI_ViewMenu_Item	*poItem,*po1;	
	
	oSrcFile.RewindAttrs();
	//
	while(B_OK==oSrcFile.GetNextAttrName(pcName)){
		if(B_OK!=oSrcFile.GetAttrInfo(pcName,&uAttrInfo)) continue;
		/* check buffer size  */
		if(uAttrInfo.size>iBufMaxSize){
			delete pcBuf;
			iBufMaxSize = uAttrInfo.size;
			pcBuf = (char*)malloc(iBufMaxSize);									
		}
		/* read attr */
		oSrcFile.ReadAttr(pcName,uAttrInfo.type,0,
			(void*)pcBuf,uAttrInfo.size);
		/* make item */
		poItem = new BF_GUI_ViewMenu_Item(pcName,"");
		ploList->AddItem(poItem);		
		/* add type_item */
		BL_System_TypeToString(uAttrInfo.type,s);
		po1 = new BF_GUI_ViewMenu_Item(s.String(),"");
		poItem->loColItem.AddItem(po1);		
		/* add value_item */
		if(strcmp(pcName,"BEOS:TYPE")==0){
			s=pcBuf;
		}else{
			switch(uAttrInfo.type){
			case B_INT32_TYPE:
				//BL_Int_Swap(pcBuf);			
				s=*(int32*)pcBuf;
				break;
			case B_UINT32_TYPE:
				s=*(uint32*)pcBuf;
				break;				
			case B_INT8_TYPE:{
				int8 i=*(int8*)pcBuf;
				s=i;
				break;}
			case B_UINT8_TYPE:{
				uint8 i=*(uint8*)pcBuf;
				s=i;
				break;}			
			case B_BOOL_TYPE:{
				bool b = *(bool*)pcBuf;
				s=b?"true":"false";
				break;}	
			case B_STRING_TYPE:
				s=pcBuf;
				break;
			default:
				s="?";
			}		
		}
		po1 = new BF_GUI_ViewMenu_Item(s.String(),"");
		poItem->loColItem.AddItem(po1);
				
	}
	SortList(ploList);
	SetList(ploList);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void BF_GUI_AttrDialog_Run(BF_Path & o_Path,BF_Node & o_Node,BView *po_Owner)
{
	/* disable panels */
	BF_GUI_Func_PanelsEnable(false);

	BRect oRect(poWinView->Bounds()),oRect1;
	oRect.left+=40;
	oRect.right-=40;
	oRect.top+=40;
	oRect.bottom-=40;

	/* make message */
	//
	BMessage oMessage(BF_MSG_PANEL_FOCUS_AND_ENABLE);
	oMessage.AddPointer("bf_focus",po_Owner);	
	/* make dialog */
	BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(oRect,
		BF_DictAt(BF_DICT_ATTRDLG_TITLE),"dialog",oMessage,BG_GUI_DIALOG_WINRESIZE_RESIZE_ALL);

	poDialog->MoveToCenter( poWinView );	

	/* make and insert file_name */
	poDialog->LocalBounds(oRect);		
	oRect.bottom = oRect.top + poSysSetup->oFontToolView.fHeight;
	BF_GUI_ViewText *poFileNameView = new BF_GUI_ViewText(oRect,"file_name",o_Node.sName.String(),
		B_FOLLOW_TOP|B_FOLLOW_LEFT_RIGHT,0);
	poDialog->AddChild(poFileNameView);			
	/* make and insert list */
	oRect.top = oRect.bottom+10;	
	poDialog->LocalBounds(oRect1);		
	oRect.bottom = oRect1.bottom-30;		
	BF_GUI_AttrDialog_Menu *poAttrMenu = new BF_GUI_AttrDialog_Menu(oRect,o_Path,o_Node);
	poDialog->AddChild(poAttrMenu);				
	/* close  menu */	
	oRect.top = oRect.bottom+10;	
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight+2;	
	BL_List *ploItem = new BL_List();
	ploItem->AddItem(new BF_GUI_ViewMenu_Item(BF_DictAt(BF_DICT_CLOSE),""));
	BF_GUI_DlgView_HMenu *poMenu = new BF_GUI_DlgView_HMenu(oRect,"menu",B_FOLLOW_BOTTOM|B_FOLLOW_LEFT_RIGHT,ploItem);
	poDialog->AddChild(poMenu);				
	/* finish */	

	poWinView->AddChild(poDialog );	
}
