#ifndef __BF_GUI_WINMAIN_H__
#define __BF_GUI_WINMAIN_H__

#include <Window.h>
#include "BF_GUI_Setup.h"
#include "BF_GUI_Func.h"  
#include "BF_GUI_DlgViews.h"

#define BF_MSG_GUI_LP_SELVOLUME	'_SLV'
#define BF_MSG_GUI_RP_SELVOLUME	'_SRV'

class BF_GUI_ViewMain : public BView {
public:
							BF_GUI_ViewMain(const BRect & o_Rect);
							
		void				MessageReceived(BMessage * message);
virtual void				FrameResized(float width, float height);				
virtual void 				AttachedToWindow(void);

		void				AddPanel(BF_GUI_Panel* po_Panel);
		void				RemovePanel(BF_GUI_Panel* po_Panel);
		
		BF_GUI_Panel*		PanelOnTop(bool b_Left) const ; 
		BF_GUI_Panel*		PanelByIndex(bool b_Left,int i_Index) const; // index =-1 : previous panel	
		BF_GUI_Panel*		FocusedPanel() const;
const	BRect				PanelRect(bool b_LeftPos) const;
		
		BView				*poLastFocus;
private:
		BView				*poBackground;
		BL_Collection		loLeftPanel,loRightPanel,loAllPanel;
		
		void				InitViews();
		void				EnablePanels(bool b_Enable,bool bOperRoster=true);
		void				ShowPanels(bool b_Show);
		
		void				Spec_MouseDown(const BPoint & o_Point,BView *po_FromView);
		
		void				Action_SelVolume_Start(bool b_LeftPos);
		
virtual void				MouseDown(BPoint point);

		void				Action_EmptyTrash();
		void				Action_Compare_Panels();
		
		void				Action_LoadSavePalete(bool b_Load);
		void				Action_SavePalete_1(BMessage * po_Message);
		void				Action_LoadPalete_1(BMessage * po_Message);

		void				Action_Open_FtpPanel(bool b_PosLeft);
		
		void				Action_SelectDict();
		void				Action_SelectDict_1(BMessage *po_Message);
		
		void				Action_MountVolumes();
		
		BF_GUI_Panel*		PanelByIndexFromList(const BL_Collection & lo_Panel,int i_Index) const;

};

class BF_GUI_WinMain : public BWindow {
public:

							BF_GUI_WinMain();
							~BF_GUI_WinMain();
							
		void				MessageReceived(BMessage * message);
virtual	bool 				QuitRequested();	
virtual void				WindowActivated(bool b_Active);
virtual void				FrameResized(float width, float height);				

private:
		BF_GUI_ViewMain		*poView;				
	
};

extern BWindow 			*poWin;
extern BF_GUI_ViewMain 	*poWinView;

#define LOCK_WIN() BAutolock oLock(poWin)

#endif