#ifndef __BF_GUI_CMDLINE_H__
#define __BF_GUI_CMDLINE_H__

#include <View.h>
#include <Window.h>
#include "BL_Tools.h"
#include "BF_GUI_DlgViews.h"


class BF_GUI_CmdLine:public BView{
public:
								BF_GUI_CmdLine(const BRect & o_Rect);

		void					SetValues(const char *pc_Path=NULL,const char *pc_Cmd=NULL,bool b_Redraw=true);

virtual bool					OnKeyDown(const char *bytes, int32 numBytes);
virtual void					Draw(BRect o_Rect);
virtual void 					MessageReceived(BMessage* po_Message);
virtual void					Enable(bool b_Enable);		
protected:
		BL_String				sPath,sVisPath,sCmd;		
		BF_GUI_ViewEdit			*poEdit;
		int32					iDisableCount;
		float 					fWidth;
		BL_List					lsCommand;
		
		void					Action_Enter();		
		void 					ShowHistory();
};

extern BF_GUI_CmdLine *poCmdLine;

#endif