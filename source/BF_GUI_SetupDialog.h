#ifndef __BF_GUI_SETUPDIALOG_H__
#define __BF_GUI_SETUPDIALOG_H__

#include "BL_Tools.h"
#include "BF_GUI_DlgViews.h"
#include "BF_GUI_Setup.h"

///////////////////////////////////////////////////////////////////  
class BF_GUI_FontView:public BF_GUI_DialogView{
public:
								BF_GUI_FontView(BRect &o_Rect,
												int i_FollowMode);
virtual	void					Draw(BRect o_Rect);
		void					UpdateFont();
		
		BF_GUI_Setup_Font		oFont;
};

class BF_GUI_SetupDialog_FontFamilyItem: public BF_GUI_ViewMenu_Item{
public:
								BF_GUI_SetupDialog_FontFamilyItem(const char *pc_Title,
																	BL_List *plo_StyleItem);
								~BF_GUI_SetupDialog_FontFamilyItem();
virtual void					MakeFocus(BF_GUI_DlgView_Menu *po_Parent);								
		BL_List 				*ploStyleItem;
};

class BF_GUI_SetupDialog_FontItem: public BF_GUI_ViewMenu_Item{
public:
								BF_GUI_SetupDialog_FontItem(
													const char *pc_Title,
													const char *pc_Code,
													BF_GUI_Setup_Font *po_Font,
													BBitmap *po_SIcon=NULL);
virtual bool					Invoke(BF_GUI_DlgView_Menu *po_Parent);
		BF_GUI_Setup_Font		*poFont;
};

class BF_GUI_SetupDialog_FontDialog:public BF_GUI_Dialog{
public:
								BF_GUI_SetupDialog_FontDialog(
										BF_GUI_SetupDialog_FontItem *po_FontItem,
										const BRect & o_Rect,
										const char *pc_Title,		
										const char *pc_Name,
										const BMessage &o_Message,
										uint32	i_WinResizeType, // BG_GUI_DIALOG_WINRESIZE_NONE
										bool b_DoubleBorder=true);										
		BF_GUI_SetupDialog_FontItem 	*poFontItem;
		
virtual void					Save(BMessage& o_Message);
virtual void 					AttachedToWindow(void);
};

///////////////////////////////////////////////////////////////////  

class BF_GUI_ColorsView:public BF_GUI_DialogView{
public:
								BF_GUI_ColorsView(BRect &o_Rect,
												int i_FollowMode,
												rgb_color *pu_Color);
		rgb_color 				*puColor;
virtual	void					Draw(BRect o_Rect);
};

class BF_GUI_ColorView:public BF_GUI_DialogView{
public:
								BF_GUI_ColorView(BRect & o_Rect,
												int32 i_FollowMode,
												uint8 *pi_Color,
												int8   i_ColorIndex,
												BF_GUI_ColorsView *po_Unit);

virtual void					Draw(BRect o_Rect);
		void					DrawCursor(bool b_Show);	
virtual void					MouseDown(BPoint point);		

		uint8 					*piColor;
		int8					iColorIndex;
		BF_GUI_ColorsView 		*poUnit;
};

class BF_GUI_SetupDialog_ColorItem: public BF_GUI_ViewMenu_Item{
public:
								BF_GUI_SetupDialog_ColorItem(
													const char *pc_Title,
													const char *pc_Code,															
													rgb_color *pu_Color);
virtual void					Draw(BF_GUI_DlgView_Menu *po_Parent,BView *po_Render,float f_PosY);
virtual bool					Invoke(BF_GUI_DlgView_Menu *po_Parent);
		rgb_color 				*puColor;
};

///////////////////////////////////////////////////////////////////  
#define  BF_GUI_SETUPDIALOG_FLAGS_MAIN		0
#define  BF_GUI_SETUPDIALOG_FLAGS_FILETASKS	1
#define  BF_GUI_SETUPDIALOG_FLAGS_DEBUG		2

class BF_GUI_SetupDialog:public BF_GUI_Dialog{
public:
								BF_GUI_SetupDialog(BRect & o_Rect,BView *po_View);
								~BF_GUI_SetupDialog();
								
virtual void 					MessageReceived(BMessage* po_Message);
private:
		BF_GUI_DlgView_VMenu 	*poMenu;
		BBitmap					*poFontSIcon;
		BF_GUI_Setup			oSetup;
		uint32					iSetupMainStyle;

		void					InvokeMenu(int32 i_OldCursor,int32 i_NewCursor);
		void					Invoke_FlagsSetup(int i_Type);
		void					Invoke_ColorsSetup();
		
		void					Insert_MainStyle_Check(const char *pc_Title,int i_Style,BRect & o_Rect,BView *po_Dialog,bool b_FirstView);
};

void
BF_GUI_SetupDialog_Run(BView *po_View);

#endif
