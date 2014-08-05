#include <stdio.h>
#include <stdlib.h>
#include <Locker.h>

#include "BF_Roster.h"
#include "BF_Msg.h"
#include "BF_GUI_NodePanel.h"
#include "BF_GUI_NodePanel_Tools.h"
#include "BF_GUI_DlgViews.h"
#include "BF_GUI_KeysMenu.h"
#include "BF_GUI_CmdLine.h"
#include "BF_GUI_WinMain.h"


BF_GUI_NodePanel_Copy_SpecialItem::BF_GUI_NodePanel_Copy_SpecialItem(
	const char *pc_Title,
	const char *pc_Code,
	BMessage *po_Message)
:BF_GUI_ViewMenu_Item(pc_Title,pc_Code)
{
	ASSERT(po_Message);
	poMessage = po_Message;}

bool					
BF_GUI_NodePanel_Copy_SpecialItem::Invoke(BF_GUI_DlgView_Menu *po_Parent)
{
	int32 iTypes=0;
	ASSERT(B_OK==poMessage->FindInt32(BF_NODEPANEL_COPY_NOTETYPES_FIELD,&iTypes));

	BL_List *ploMenu = new BL_List();
	
	ploMenu->AddItem(new BF_GUI_ViewMenu_Item("ok","ok"));
		
	BF_Dialog_Alert_Sep("test","test",ploMenu,po_Parent->Parent());
	return true;
}