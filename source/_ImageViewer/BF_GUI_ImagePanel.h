#ifndef __BF_GUI_IMAGEPANEL_H__
#define __BF_GUI_IMAGEPANEL_H__

#include "BF_GUI_Func.h"

class BF_GUI_ImagePanel;
class BF_GUI_ImagePanel_Viewer:public BL_Object{
public:
	BF_GUI_ImagePanel_Viewer();
	void	Load(BL_String & s_NodePath);
	void	Draw(BView *po_Render,const BRect & o_Rect);
private:
	BBitmap*	mpBitmap;

	void 		DrawOffscreen(BView *po_Render, BRect updateRect);
	BL_String	sNodePath;		
		
friend class BF_GUI_ImagePanel;
};

class BF_GUI_ImagePanel:public BF_GUI_Panel{
public:
	BF_GUI_ImagePanel(
		const BRect& 	o_Rect,
		const char*		pc_FirstName, // any name, but real name will be "name_panel"
		bool			b_WinPos_OnLeft);	

virtual void	Action_Friend_NewCursor(const char *pc_NodePath,const char *pc_NodeName,int32 i_NodeType);
virtual void	MessageReceived(BMessage* po_Message);													
virtual void	FrameResized(float width, float height);
virtual bool 	OnMouseDown(BPoint & o_Point);
virtual bool 	OnMouseUp(BPoint & o_Point);
virtual bool	OnKeyDown(const char *bytes, int32 numBytes);
virtual void	DrawPanel(BRect & o_Rect);

virtual	void	PrepareKeysMenu();
virtual	void	PrepareCmdLine();
virtual void	PrepareTopMenuItem(BF_GUI_TopMenu_HItem *po_HItem);

private:
	BL_String					sNodePath;
	BF_GUI_ImagePanel_Viewer	oViewer;
	void						LoadImage(const char *pc_NodePath);
		
friend class BF_GUI_ImagePanel_Viewer;
};


#endif