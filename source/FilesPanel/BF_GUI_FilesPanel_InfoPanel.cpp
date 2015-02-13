#include "stdio.h"

#include "BF_Dict.h"
#include "BF_Roster.h"
#include "BF_Msg.h"
#include "BF_GUI_DlgViews.h"
#include "BF_GUI_KeysMenu.h"

#include "BF_GUI_FilesPanel_InfoPanel.h"

#include "BF_Roster.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "InfoPanel"

  
BF_GUI_FilesPanel_InfoPanel::BF_GUI_FilesPanel_InfoPanel(
	const 	BRect & o_Rect,
	bool	b_WinPos_OnLeft
):BF_GUI_Panel(o_Rect,"info",b_WinPos_OnLeft) 
{
	bNodeReady = false;
	SetHeader(B_TRANSLATE(BF_DICT_INFOPANEL_TITLE));
}


void					
BF_GUI_FilesPanel_InfoPanel::DrawPanel(BRect &o_Rect)
{
	BF_GUI_Panel::DrawPanel(o_Rect);
	
	float	fHeight = poSysSetup->oFontNode.fHeight;
	/* draw header */	
	//DrawHeader(IsFocus());
	/* draw bottom */	
	DrawBottom("", TRUE);
	//////////////////////////////////////////////////
	if(!bNodeReady) return;
	
	float 		fy = HeaderHeight() + fHeight;
	BPoint 		o,o1,o2;
	BL_String	s,s1;
	BRect		oRect;
	
	
	poRender->SetHighColor(SYS_COLOR(BF_COLOR_NODE));
	poRender->SetLowColor(SYS_COLOR(BF_COLOR_BACK));	
	/* draw icon */	
	if(oNode.poLIcon){
		o.Set(10,fy);
		poRender->SetDrawingMode(B_OP_ALPHA);		
		poRender->DrawBitmap(oNode.poLIcon,o);
		poRender->SetDrawingMode(B_OP_COPY);				
	}	
	/* draw file name */	
	o.Set(50,fy+poSysSetup->oFontToolView.fAscent);
	s="";
	s<<oNode.sName;
	poRender->DrawString(s.String(),o);
	fy += fHeight+5;
	/* draw path */
	o.Set(50,fy+poSysSetup->oFontToolView.fAscent);
	s="";
	s<<oPath.Path();
	poRender->DrawString(s.String(),o);
	fy += fHeight+5;	
	/* draw size */
	o.Set(10,fy+poSysSetup->oFontToolView.fAscent);
	s=B_TRANSLATE(BF_DICT_INFOPANEL_SIZE);
	s1=oNode.iSize;
	s1.SetDigits();
	s<<s1;
	poRender->DrawString(s.String(),o);
	fy += fHeight+5;	
	/* draw date */
	o.Set(10,fy+poSysSetup->oFontToolView.fAscent);
	s=B_TRANSLATE(BF_DICT_INFOPANEL_DATE);
	char pcBuffer[200]="";
	{
		tm* puTime = localtime(&oNode.uCreateTime);; 
		if(puTime){
			sprintf(pcBuffer,"%i.%i.%i",
				puTime->tm_mday,
				puTime->tm_mon+1,
				puTime->tm_year+1900);				
		}
	}
	s<<pcBuffer;
	poRender->DrawString(s.String(),o);
	fy += fHeight+5;		
	/* draw perms */
	s=B_TRANSLATE(BF_DICT_INFOPANEL_PERMS);
	oNode.PermsTo(s1);
	s<<s1;
	o.Set(10,fy+poSysSetup->oFontToolView.fAscent);
	poRender->DrawString(s.String(),o);
	fy += fHeight+5;
	/* draw mime_type */
	s=B_TRANSLATE(BF_DICT_INFOPANEL_MIME);
	s<<oNode.sType;
	o.Set(10,fy+poSysSetup->oFontToolView.fAscent);
	poRender->DrawString(s.String(),o);
	fy += fHeight+5;		
	/* draw line */
	oRect = poRender->Bounds();
	o1.x = 5;
	o1.y = fy;
	o2 = o1;
	o2.x =  oRect.Width()-10;	
	poRender->SetHighColor(SYS_COLOR(BF_COLOR_FRAME));
	poRender->StrokeLine(o1,o2);	
	fy += 5;
	/////////////////////// draw curr volume /////////////
	poRender->SetHighColor(SYS_COLOR(BF_COLOR_NODE));
	poRender->SetLowColor(SYS_COLOR(BF_COLOR_BACK));
	/* draw icon */	
	if(oVol.poLIcon){
		o.Set(10,fy);
		poRender->SetDrawingMode(B_OP_ALPHA);		
		poRender->DrawBitmap(oVol.poLIcon,o);
		poRender->SetDrawingMode(B_OP_COPY);				
	}	
	/* draw file name */	
	o.Set(50,fy+poSysSetup->oFontToolView.fAscent);
	s="";
	s<<oVol.sName;
	poRender->DrawString(s.String(),o);
	fy += fHeight*2+5*2;
	
	if(oVol.bLoadedOk)
	{		
		o.Set(10,fy+poSysSetup->oFontToolView.fAscent);
		s=B_TRANSLATE(BF_DICT_INFOPANEL_CAPACITY);
		s1=oVol.iCapacity;
		s1.SetDigits();
		s<<s1;
		poRender->DrawString(s.String(),o);
		fy += fHeight+5;		
		//
		o.Set(10,fy+poSysSetup->oFontToolView.fAscent);
		s=B_TRANSLATE(BF_DICT_INFOPANEL_FREEBYTES);
		s1=oVol.iFree;
		s1.SetDigits();
		s<<s1;
		poRender->DrawString(s.String(),o);
		fy += fHeight+5;		
		// removable
		if(oVol.bRemovable){
			o.Set(10,fy+poSysSetup->oFontToolView.fAscent);
			s=B_TRANSLATE(BF_DICT_INFOPANEL_REMOVEABLE);
			poRender->DrawString(s.String(),o);
			fy += fHeight+5;		
		}
		// readonly
		if(oVol.bRemovable){
			o.Set(10,fy+poSysSetup->oFontToolView.fAscent);
			s=B_TRANSLATE(BF_DICT_READONLY);
			poRender->DrawString(s.String(),o);
			fy += fHeight+5;			
		}
	}else{
		o.Set(10,fy+poSysSetup->oFontToolView.fAscent);
		s=B_TRANSLATE(BF_DICT_CANTLOADINFO);
		poRender->DrawString(s.String(),o);
		fy += fHeight+5;			
	}
}

void
BF_GUI_FilesPanel_InfoPanel::Action_Friend_NewCursor(const char *pc_NodePath,const char *pc_NodeName,int32 i_NodeType)
{
	if(pc_NodePath && pc_NodeName){
		// load path
		BF_Path oPath(pc_NodePath);
		oNode.sName = pc_NodeName;			
		bNodeReady = true;		
		BF_Roster_ReloadNode(oPath.Path(),&oNode,BF_ROSTER_LOAD_LICON|BF_ROSTER_LOAD_TYPE|BF_ROSTER_LOAD_PERMS);		

		// load current volume
		oVol.Load(oPath.GetVolume().String(),BF_ROSTER_LOAD_LICON);
	}else{
		bNodeReady = false;		
	}
	if(Parent()) Invalidate();
}

void					
BF_GUI_FilesPanel_InfoPanel::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what){
	default:
		BF_GUI_Panel::MessageReceived(po_Message);
	}
}

/*		
void					
BF_GUI_FilesPanel_InfoPanel::SetNode(const char *pc_File)
{
	if(pc_File){
		BF_Path oPath;

		ASSERT(po_Node);
		oNode.Set(po_Node);
		oPath.Set(o_Path);
		bNodeReady = true;		
		BF_Roster_ReloadNode(&oPath,&oNode,BF_ROSTER_LOAD_LICON|BF_ROSTER_LOAD_TYPE|BF_ROSTER_LOAD_PERMS);
		//
		// load vol  //
		BL_String sVol;
		oPath.GetVolume(sVol,true);
		oVol.sName = sVol;
		BF_Roster_ReloadNode(NULL,&oVol,BF_ROSTER_LOAD_LICON);
		//
		oVol.LoadInfo();
	}else{
		bNodeReady = false;
	}
	if(Parent()) Invalidate();
}
*/
