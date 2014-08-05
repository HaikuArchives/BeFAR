#ifndef __BF_GUI_TOPMENU_H__
#define __BF_GUI_TOPMENU_H__

#include <View.h>
#include <Window.h>
#include <Messenger.h>
#include "BL_Tools.h"
#include "BF_GUI_DlgViews.h"

class BF_GUI_TopMenu;
/////////////////////////////////////////////////////////////////////
class BF_GUI_TopMenu_HItem:public BF_GUI_ViewMenu_Item{
public:
								BF_GUI_TopMenu_HItem(const char *pc_Title,
													BBitmap *po_SIcon=NULL);
													
		void					AddHItem	(const char *pc_Title,
											BMessage *po_Message, // will be deleted
											const char *pc_KeyLabel="");
		void					AddHItem	(const char *pc_Title,
											int32	i_MessageWhat,
											BView   *po_Owner,
											const char *pc_KeyLabel="");											
													
		BL_List					loVItem;
};
/////////////////////////////////////////////////////////////////////
class BF_GUI_TopMenu_VItem:public BF_GUI_ViewMenu_Item{
public:
								BF_GUI_TopMenu_VItem(	const char *pc_Title,
														BMessage *po_Message, // will be deleted
														const char *pc_KeyLabel="");
								~BF_GUI_TopMenu_VItem();
													
virtual	float					CalcWidth(BF_GUI_Setup_Font * po_Font);																	
		BL_String				sKeyLabel;		
		float					fKeyWidth;		
		BMessage 				*poMessage;									
};
/////////////////////////////////////////////////////////////////////
class BF_GUI_TopVMenu:public BF_GUI_DlgView_VMenu{
public:				
								BF_GUI_TopVMenu(BRect & o_Rect,BL_List *plo_Menu,BF_GUI_TopMenu	*po_HMenu);
virtual	void					KeyDown(const char *bytes, int32 numBytes);
virtual	void					NavEnter();							
virtual	void					NavEscape();
virtual	void					DrawItem(BView *po_Render,int32 i_Index,bool b_ReqDrawBack);
private:
		BF_GUI_TopMenu			*poHMenu;

};
/////////////////////////////////////////////////////////////////////

class BF_GUI_TopMenu:public BF_GUI_DlgView_HMenu{
public:
								BF_GUI_TopMenu(const BRect 	&o_Rect,
											BL_List 	*plo_HItem);
virtual	void					NavEnter();							
virtual	void					NavEscape();
virtual	void					NavChangeCursor(int32 i_NewCursor,int i_DirectMove,bool b_Redraw=true);

virtual	void					MakeFocus(bool b_Focused=true);
virtual	void					Show(void);
virtual void 					MessageReceived(BMessage* po_Message);

		BView*					poParent;		
private:		
		BF_GUI_TopVMenu			*poVMenu;
		BF_GUI_DlgPanel			*poVMenuPanel;
		int						iHCursor;
		
		void					OpenVMenu(bool b_FocusHMenu=true);
		void					CloseVMenu();
		void					Invoke(bool b_Ok);
};

void BF_GUI_TopMenu_Run();
/////////////////////////////////////////////////////////////////////
#endif

