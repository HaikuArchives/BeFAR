#ifndef __BF_GUI_ARCPANEL_H__
#define __BF_GUI_ARCPANEL_H__

#include "BF_GUI_NodePanel.h" 
#include "BL_Tools.h"

class BF_GUI_ArcPanel:public BF_GUI_NodePanel{
public:
						BF_GUI_ArcPanel(const BRect & o_Rect,bool b_WinPos_OnLeft);
						~BF_GUI_ArcPanel();
		void			Init(BL_String s_ArcFilename, BL_String s_InnerDir);
virtual void			RefreshNodeList();
virtual	bool			NavEnter();		
virtual void 			MessageReceived(BMessage* po_Message);
virtual bool			OnKeyDown(const char *bytes, int32 numBytes);

private:
	BBitmap		*poFolderSIcon, *poDocSIcon;

	BL_String		sArcFilename, // name of archive file for which panel has been created
				sInnerDir,	// name of directory currently open in the archive
				sTempFile;	// name of temp file where we redirect archiver output
};

void
BF_GUI_ArcPanel_OpenFile(const char *pc_Path,const char *pc_FileName,bool b_PosLeft);



#define IS_EMPTY(s) (s.CountChars() == 0)
#define IS_EMPTY_PTR(ps) (ps->CountChars() == 0)

#endif