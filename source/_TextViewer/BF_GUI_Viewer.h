#ifndef __BF_GUI_VIEWER_H__
#define __BF_GUI_VIEWER_H__

#include <InterfaceKit.h>
#include "BF_GUI_Setup.h"
#include "BF_ToolsView.h"
 
class BF_GUI_Viewer : public BView {
private:
		char 					*pcFileForView;
		BList					*ploString;
		BF_GUI_Status			*poStatus;
		BF_GUI_Text				*poText;
		BView					*poPanel;		

virtual void 					LoadFile();
	
public:
		
								BF_GUI_Viewer(	BRect 	o_frame, 
												const char 	*pcFileForView,
												BView	*po_Panel); 
								~BF_GUI_Viewer();
		
virtual void					KeyDown(const char *bytes, int32 numBytes);
virtual void 					MessageReceived(BMessage* po_Message); 

private:

virtual void					QuitViewer();
virtual void					SearchSample();
};

#endif
