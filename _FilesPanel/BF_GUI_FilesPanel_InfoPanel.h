#ifndef __BF_GUI_FILESPANEL_INFOPANEL_H__
#define __BF_GUI_FILESPANEL_INFOPANEL_H__

#include "BF_GUI_Func.h"
#include "BF_Node.h"

class BF_GUI_FilesPanel_InfoPanel:public BF_GUI_Panel{
public:
								BF_GUI_FilesPanel_InfoPanel(
												const 	BRect & o_Rect,
												bool	b_WinPos_OnLeft);
							
virtual	void					DrawPanel(BRect &o_Rect);							
virtual	void					MessageReceived(BMessage* po_Message);
virtual	void					Action_Friend_NewCursor(const char *pc_NodePath,const char *pc_NodeName,int32 i_NodeType);

private:
		BF_Node					oNode;
		BF_Path					oPath;
		BF_Volume				oVol;
		bool					bNodeReady;
		
		//void					SetNode(const char *pc_File);
};

#endif