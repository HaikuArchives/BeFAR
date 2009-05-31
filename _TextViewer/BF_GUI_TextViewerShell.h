#ifndef __BF_GUI_VIEWER_H__
#define __BF_GUI_VIEWER_H__

#include <InterfaceKit.h>
#include "BF_GUI_Setup.h"
#include "BF_GUI_TextViewer.h"

class BF_GUI_Status;

class BF_GUI_TextViewerShell : public BView {
private:
		BL_String				sFileName;
		BF_GUI_Status			*poStatus;
		BF_GUI_TextViewer		*poText;
		BView					*poPanel;		

	
public:
		
								BF_GUI_TextViewerShell(	BRect 	o_frame, 
												const char 	*pcFileForView, // changed  by baza
												BView	*po_Panel);
								~BF_GUI_TextViewerShell();
		
virtual void					KeyDown(const char *bytes, int32 numBytes);
virtual void 					MessageReceived(BMessage* po_Message); 
virtual void 					MakeFocus(bool focused = true);	

private:

virtual void					QuitViewer();
virtual void					SearchSample();
};

#endif
