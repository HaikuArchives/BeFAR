#ifndef ____BF_GUI_MESSAGEBOX____
#define ____BF_GUI_MESSAGEBOX____

#include <InterfaceKit.h>
#include "BF_GUI_Tools.h"

class BF_GUI_MessageBox : public BF_GUI_Dialog {
public:
					BF_GUI_MessageBox(BRect  o_Bounds, char *pc_Title, char *pc_MessageText, BView *po_ParentView, char *pc_ButtonText);	 
private:
	BF_GUI_DlgView_HMenu	*po_Buttons;
};

#endif
