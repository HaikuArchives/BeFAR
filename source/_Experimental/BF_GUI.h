#ifndef __BF_GUI_H__
#define __BF_GUI_H__

#include <View.h>
#include "BL_Tools.h"


class BF_GUI_View:public BL_Object{
public:		
		BF_GUI_View			*poParent;
		BL_List				loChild;
		BRect				oCoords;
		
							BF_GUI_View(BRect & o_Coords);
							
		void				AddChild(BF_GUI_View *po_Child);
		
		void				Start_Paint(BRect & o_Bounds);
		void				Stop_Paint(BRect & o_Bounds);
		
		void				Bounds(BRect & o_Bounds);
																
};

class BF_GUI_Screen:public BView{
public:		
							BF_GUI_Screen(BRect & o_Rect);
};

extern BF_GUI_Screen *poScreen;


#endif