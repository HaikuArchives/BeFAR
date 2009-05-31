#include "BF_GUI_MessageBox.h"
#include "BF_Msg.h"

BF_GUI_MessageBox::BF_GUI_MessageBox(BRect  o_Bounds, char *pc_Title, char *pc_MessageText, BView *po_ParentView, char *pc_ButtonText) 
	: BF_GUI_Dialog(o_Bounds,pc_Title,"",NULL,B_FOLLOW_H_CENTER | B_FOLLOW_V_CENTER)
{
		/* make new message */		
		oMessage.what = BF_MSG_VIEWER_MESSAGE; 
		oMessage.AddPointer("bf_focus",(void*)po_ParentView);
		/* move self to center */		
		MoveToCenter(po_ParentView);
		/* add text_view */
		BRect oRect;
		LocalBounds(oRect);
		oRect.top += 10;
		oRect.bottom = oRect.top + poSysSetup->oFontToolView.fHeight * 2 + 4;
		AddChild(new BF_GUI_ViewText(oRect,"text",pc_MessageText,
			B_FOLLOW_H_CENTER | B_FOLLOW_TOP,B_WILL_DRAW));
		/* add menu */
		LocalBounds(oRect);
		oRect.bottom-=5;
		oRect.top=oRect.bottom - poSysSetup->oFontToolView.fHeight+2;
		oRect.left+=10;
		oRect.right-=10;		
		BL_List *ploMenu = new BL_List();
		ploMenu->AddItem(new BF_GUI_ViewMenu_Item(pc_ButtonText,pc_ButtonText));
		po_Buttons = new BF_GUI_DlgView_HMenu(oRect,"buttons",B_FOLLOW_BOTTOM,ploMenu);
		AddChild(po_Buttons);					
};
