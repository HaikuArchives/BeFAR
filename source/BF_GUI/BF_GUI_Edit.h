#ifndef __BF_GUI_EDIT_h__
#define	__BF_GUI_EDIT_h__

#include <String.h>
#include <Message.h>
#include <View.h>
#include <Window.h>

#include "BL_Tools.h"	
#include "BF_GUI_Setup.h"	
#include "BF_Msg.h"	

class BF_GUI_ViewEdit:public BF_GUI_DialogView{
public:
		BL_String				sValue;
		bool					bSelected;		
		
		bool					bSysKeysHandle,bParentCall,bAlwaysCursor,bCursorActive;
		rgb_color				oColBack,oColSel;
		int						iDisableCount;
	
								BF_GUI_ViewEdit(const BRect &o_Rect,
												const char*pc_Name,
												const char *pc_Value,
												uint32	i_FollowMode,
												uint32	i_Flags);	
								~BF_GUI_ViewEdit();									
virtual	void					Draw(BRect o_Rect);		
virtual void					KeyDown(const char *bytes, int32 numBytes);
virtual bool					OnKeyDown(const char *bytes, int32 numBytes);
virtual void					Pulse(void);
virtual void 					AttachedToWindow(void);
virtual void 					MakeFocus(bool focused = true);		
virtual void 					MessageReceived(BMessage* po_Message);	

virtual void					Enable(bool b_Enable);
virtual bool					Enabled();
virtual	void					OnEnable(bool b_Enable);


virtual void					SetText(const char *pc_NewText,bool b_GoEnd=false);

virtual void					SaveToMessage(BMessage *po_Message);
private:
		int32							iNavIndex,iNavFirstChar;
		bool							bPulseCursorShow;
		
		thread_id 				idThreadPulse;
		
		void							ClearSelecting(bool b_Remove = false);
		void 							DrawCursor(bool b_Show);
		void							NavGo(int32 i_NewCursor,bool b_FullRedraw=false);
		
		void							DrawBody(BRect o_Rect);
												
};

BF_GUI_ViewEdit*
BF_GUI_ViewEdit_Create(	const BRect &o_Rect,
						const char*pc_Comment,
						BView *po_Parent,
						const char*pc_Name,
						const char *pc_Value,
						uint32	i_FollowMode,
						uint32	i_Flags);
						
						
#endif// __BF_GUI_EDIT_h__
