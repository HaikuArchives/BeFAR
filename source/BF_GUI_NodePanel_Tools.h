#ifndef __BF_GUI_NODEPANEL_TOOLS_H__
#define __BF_GUI_NODEPANEL_TOOLS_H__

class BF_GUI_NodePanel_Copy_SpecialItem:public BF_GUI_ViewMenu_Item{
public:
								BF_GUI_NodePanel_Copy_SpecialItem(	const char *pc_Title,
														const char *pc_Code,
														BMessage *po_Message);
														
virtual bool					Invoke(BF_GUI_DlgView_Menu *po_Parent);
private:
		BMessage 				*poMessage;
};

#endif //__BF_GUI_NODEPANEL_TOOLS_H__

