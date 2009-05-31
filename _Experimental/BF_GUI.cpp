#include "BF_GUI.h"
  
BF_GUI_Screen *poScreen = NULL;

BF_GUI_View::BF_GUI_View(BRect & o_Coords)
{
	oCoords = o_Coords;
	poParent = NULL;
}

void				
BF_GUI_View::Bounds(BRect & o_Bounds)
{
	o_Bounds.left = 0;
	o_Bounds.right = 0;	
	o_Bounds.right = oCoords.Width();
	o_Bounds.right = oCoords.Height();
}

void				
BF_GUI_View::AddChild(BF_GUI_View *po_Child)
{
	ASSERT(po_Child);
	loChild.AddItem(po_Child);
}

//////////////////////////////////////////////

BF_GUI_Screen::BF_GUI_Screen(BRect & o_Rect)
:BView(o_Rect,"bf_screen",B_FOLLOW_ALL,B_WILL_DRAW|B_FRAME_EVENTS)
{
}