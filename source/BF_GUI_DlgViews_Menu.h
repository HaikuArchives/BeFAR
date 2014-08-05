#ifndef __BF_GUI_TOOLS_MENU_H__
#define __BF_GUI_TOOLS_MENU_H__

#include "BF_GUI_DlgViews.h"


enum{
	BF_GUI_TOOLMENU_MOVE_UP=0,
	BF_GUI_TOOLMENU_MOVE_DOWN,
	BF_GUI_TOOLMENU_MOVE_POS,
	BF_GUI_TOOLMENU_MOVE_END,
	BF_GUI_TOOLMENU_MOVE_PG_UP,	
	BF_GUI_TOOLMENU_MOVE_PG_DOWN
};

class BF_GUI_DlgView_Menu;

class BF_GUI_ViewMenu_Item:public BL_Object{
public:
		BL_String				sTitle,sCode;
		float					fWidth;		
		BBitmap					*poSIcon;
		BL_List					loColItem;
		bool					bSelected;
		
								BF_GUI_ViewMenu_Item(const char *pc_Title,
													const char *pc_Code,													
													BBitmap *po_SIcon=NULL); // will be deleted
								BF_GUI_ViewMenu_Item(BF_GUI_ViewMenu_Item *po_Src);
								~BF_GUI_ViewMenu_Item();
								
		void					Set(BF_GUI_ViewMenu_Item *po_Src);

virtual	float					CalcWidth(BF_GUI_Setup_Font * po_Font);		
virtual void					Draw(BF_GUI_DlgView_Menu *po_Parent,BView *po_Render,float f_PosY);	
virtual bool					Invoke(BF_GUI_DlgView_Menu *po_Parent){return false;};
virtual void					MakeFocus(BF_GUI_DlgView_Menu *po_Parent);
};

float
BF_GUI_ViewMenu_CalcMaxWidth(BL_List *plo_Menu,BF_GUI_Setup_Font * po_Font,bool b_ShowIcons);
float
BF_GUI_ViewMenu_CalcTotalWidth(BL_List *plo_Menu,BF_GUI_Setup_Font * po_Font,bool b_ShowIcons);

class BF_GUI_DlgView_Menu:public BF_GUI_DialogView{
public:
		BL_List 				*ploMenu; // list of BF_GUI_ViewMenu_Item
		int32					iNavCursorIndex;
		bool					bDestroyMenu,bHideCursorOnDefocus;
		
								BF_GUI_DlgView_Menu(
										const BRect 	&o_Rect,
										const char	*pc_Name,
										uint32	i_FollowType,
										BL_List *plo_Menu);
								~BF_GUI_DlgView_Menu();
								
virtual	void					MakeFocus(bool	b_Focused = true);
virtual void 					AttachedToWindow(void);
virtual	void					KeyDown(const char *bytes, int32 numBytes);
virtual	void					Draw(BRect o_Rect);		
virtual	void					DrawItems(BView *po_Render);
		// virtual method, empty in this class
virtual	void					DrawItem(BView *po_Render,int32 i_Index,bool b_ReqDrawBack);
		// virtual method
virtual void					SaveToMessage(BMessage *po_Message);
		//
		void					SetColor_Text(BView * po_Render,bool b_IsFocused,BF_GUI_ViewMenu_Item *po_Item=NULL);
		void					SetColor_Back(BView * po_Render,bool b_IsFocused);
		//
virtual	void					SetList(BL_List *plo_NewMenu);
		void					SortList(BL_List *plo_List);		
		void					DeleteItemAt(int i_Index);
		//
virtual	float					Height();
		BF_GUI_ViewMenu_Item*   ItemAt(int32 i_Index);
		BF_GUI_ViewMenu_Item*	ItemByTitle(const char *pc_Title);
		BF_GUI_ViewMenu_Item*   FocusItem();
		
virtual	void					AddItem(BF_GUI_ViewMenu_Item* po_Item,bool b_SetCursorOnItem=false);
		
virtual	void					NavChangeCursor(int32 i_NewCursor,int i_DirectMove,bool b_Redraw=true);
virtual	void					NavEnter();
virtual	void					NavEscape();

protected:		
		void					Draw_FocusItemPointer(BPoint & o_PointStart,BView *po_Render);
};

#define  BF_GUI_DLGVIEW_VMENU_DEFAULT				0x00001
#define  BF_GUI_DLGVIEW_VMENU_SICON					0x00002
#define  BF_GUI_DLGVIEW_VMENU_NAV_PARENTINFORM		0x00004
#define  BF_GUI_DLGVIEW_VMENU_CAN_SELECTING			0x00008
#define  BF_GUI_DLGVIEW_VMENU_SET_VSCROLLBAR		0x00010


class BF_GUI_DlgView_VMenu:public BF_GUI_DlgView_Menu{
public:
								BF_GUI_DlgView_VMenu(
										const BRect 	&o_Rect,
										const char	*pc_Name,
										uint32	i_FollowType,
										BL_List *plo_Menu,
										uint32	i_Style = BF_GUI_DLGVIEW_VMENU_DEFAULT,
										float	f_NewItemHeight=0);
public:	
virtual	void					DrawItem(BView *po_Render,int32 i_Index,bool b_ReqDrawBack);
		void					DrawItemExt(BView *po_Render,int32 i_Index,bool b_ReqDrawBack);
virtual	void					NavChangeCursor(int32 i_NewCursor,int i_DirectMove,bool b_Redraw=true);
virtual	void					DrawItems(BView *po_Render);

virtual void 					AttachedToWindow(void);
virtual	void					KeyDown(const char *bytes, int32 numBytes);
virtual	void					FrameResized(float width, float height);
virtual void					MouseDown(BPoint point);

virtual	float					Height();
virtual	void					SetList(BL_List *plo_NewMenu);
virtual	void					AddItem(BF_GUI_ViewMenu_Item* po_Item,bool b_SetCursorOnItem=false);

		void					SetVScroll(BF_GUI_VScrollBar *po_VScroll);
protected:
		uint32					iStyle;				
		float					fNewItemHeight;
		void					NavSelect();		
		BF_GUI_VScrollBar		*poVScroll;
		
virtual void					SaveToMessage(BMessage *po_Message);		
public:
		int32					iFirstIndex;
		int32					iPageSize;
		
private:
		void 					CreateVScroll();
};

class BF_GUI_DlgView_VCMenu_Column:public BL_Object{
public:
		BL_String				sName;
		float					fWidth;
		
								BF_GUI_DlgView_VCMenu_Column(const char *pc_ColName,float f_Width);
};

class BF_GUI_DlgView_VCMenu:public BF_GUI_DlgView_VMenu{
public:
								BF_GUI_DlgView_VCMenu(
										const BRect 	&o_Rect,
										const char	*pc_Name,
										uint32	i_FollowType,
										BL_List *plo_Menu,
										uint32	i_Style, // BF_GUI_DLGVIEW_VMENU_DEFAULT
										BL_List	 *plo_Col);										
								~BF_GUI_DlgView_VCMenu();
virtual	void					DrawItem(BView *po_Render,int32 i_Index,bool b_ReqDrawBack);
protected:
		BL_List					*ploCol;
};


class BF_GUI_DlgView_HMenu:public BF_GUI_DlgView_Menu{
public:
		
								BF_GUI_DlgView_HMenu(
										const BRect &o_Rect,
										const char	*pc_Name,
										uint32	i_FollowType,
										BL_List *plo_Menu);
public:	
virtual	void					DrawItem(BView *po_Render,int32 i_Index,bool b_ReqDrawBack);
virtual void					MouseDown(BPoint point);

		float					CalcItemXPos(int32 i_Index);

protected:
		float					fItemsWidth;		
public:
		bool					bItemsInCenter;		
};

#endif