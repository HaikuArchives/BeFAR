/*
===============================================
Project:	BeFar
File:		BF_BetaTesting.cpp
Desc:		??
Author:		Milan Rusek
Created:	26.11.99
Modified:	??
===============================================
*/

#include <TranslationUtils.h>
#include "BF_GUI_ImagePanel.h"
#include "BL_Tools.h"

/////////////////////////////////////////////////////////////////////////////////

BF_GUI_ImagePanel_Viewer::BF_GUI_ImagePanel_Viewer()
{
	
}

void					
BF_GUI_ImagePanel_Viewer::Load(BL_String & s_NodePath)
{
	sNodePath = s_NodePath;
	mpBitmap = BTranslationUtils::GetBitmapFile(sNodePath.String());
	//	your code here
}

void						
BF_GUI_ImagePanel_Viewer::Draw(BView *po_Render,const BRect & o_Rect)
{
	/// change this code to real
	if (mpBitmap)
	{
		if (!po_Render) return;
		po_Render->SetDrawingMode(B_OP_COPY);
		po_Render->DrawBitmap(mpBitmap, o_Rect, o_Rect);
		po_Render->SetDrawingMode(B_OP_ALPHA);
		po_Render->FillRect(o_Rect);
	}
	else
	{
		BPoint oPoint(o_Rect.left,o_Rect.top + o_Rect.Height()/2 - poSysSetup->oFontNode.fAscent);		
		po_Render->SetFont(&poSysSetup->oFontNode.oFont);
		po_Render->SetHighColor(SYS_COLOR(BF_COLOR_NODE));
		po_Render->DrawString(sNodePath.String(),oPoint);
	}
}

/////////////////////////////////////////////////////////////////////////////////

BF_GUI_ImagePanel::BF_GUI_ImagePanel(
	const BRect&	o_Rect,
	const char*		pc_FirstName, 
	bool			b_WinPos_OnLeft
):BF_GUI_Panel(o_Rect,pc_FirstName,b_WinPos_OnLeft)
{	
	// your code here 
	SetHeader("ImageViewer");
}

void	
BF_GUI_ImagePanel::Action_Friend_NewCursor(const char *pc_NodePath,const char *pc_NodeName,int32 i_NodeType)
{
	BL_String s;
	ASSERT(pc_NodePath && pc_NodeName);
	s<<pc_NodePath;
	s<<"/";
	s<<pc_NodeName;
	LoadImage(s.String());	
}

void
BF_GUI_ImagePanel::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what){
	default:
		BF_GUI_Panel::MessageReceived(po_Message);
	}
}

void			
BF_GUI_ImagePanel::FrameResized(float width, float height)
{
	BF_GUI_Panel::FrameResized(width,height);
	// your code here 
}

bool 			
BF_GUI_ImagePanel::OnMouseDown(BPoint & o_Point)
{
	// your code here 
	return BF_GUI_Panel::OnMouseDown(o_Point);
}

bool
BF_GUI_ImagePanel::OnMouseUp(BPoint & o_Point)
{
	// your code here 
	return BF_GUI_Panel::OnMouseUp(o_Point);
}

bool
BF_GUI_ImagePanel::OnKeyDown(const char *bytes, int32 numBytes)
{
	// your code here 
	return BF_GUI_Panel::OnKeyDown(bytes,numBytes);
}

void
BF_GUI_ImagePanel::DrawPanel(BRect & o_Rect)
{
	BF_GUI_Panel::DrawPanel(o_Rect);
	DrawBottom("",true);
	// your code here 
	oViewer.Draw(poRender,ClientRect());
}

void
BF_GUI_ImagePanel::PrepareKeysMenu()
{
	BF_GUI_Panel::PrepareKeysMenu();
	// your code here 
}

void					
BF_GUI_ImagePanel::PrepareCmdLine()
{
	BF_GUI_Panel::PrepareCmdLine();
	// your code here 
}

void					
BF_GUI_ImagePanel::PrepareTopMenuItem(BF_GUI_TopMenu_HItem *po_HItem)
{
	BF_GUI_Panel::PrepareTopMenuItem(po_HItem);
	// your code here 
}
///////////////////////////////////////////////////////
void					
BF_GUI_ImagePanel::LoadImage(const char *pc_NodePath)
{
	ASSERT(pc_NodePath);
	sNodePath = pc_NodePath;	
	
	oViewer.Load(sNodePath);	
	Draw(Bounds());
}