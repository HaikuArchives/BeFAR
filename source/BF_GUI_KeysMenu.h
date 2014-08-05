#ifndef _BF_GUI_KeysMenu_Item_
#define _BF_GUI_KeysMenu_Item_

#include <InterfaceKit.h>
#include "BF_GUI_Setup.h"

class BF_GUI_KeysMenu_Item : public BView
{
public:
								BF_GUI_KeysMenu_Item(BRect o_frame, const char *pc_ButtonNum,const char *pc_ButtonText, BMessage *po_Message = NULL, BHandler *po_Handler = NULL);
								~BF_GUI_KeysMenu_Item();
								
virtual void 					Draw(BRect oRect);
		
		void					SetText(const char *pc_Text);
								/* po_Message  will be store in object and destroyed in destructor */
		void					SetMessage(BMessage *po_Message = NULL, BHandler *po_Handler = NULL);
virtual	void					MouseDown(BPoint point);
		bool					Invoke();
private:		
		BL_String					sNum;
		BL_String				sText;
		BMessage				*poMessage;
		BHandler				*poHandler;
};

 
class BF_GUI_KeysMenu : public BView
{
public:
	
								BF_GUI_KeysMenu(BRect o_frame);
virtual void 					FrameResized(float f_width, float f_height);
						
								// i_Index in [1..10]
		void					SetText(uint8 i_Index,const char *pc_Text);
		void					Clear();   
		void					SetMessage(uint8 i_Index, BMessage *po_Message = NULL, BHandler *po_Handler = NULL);
virtual bool					OnKeyDown(const char *bytes, int32 numBytes);
virtual void 					MessageReceived(BMessage* po_Message);	
		
private:
		BF_GUI_KeysMenu_Item	*poButtonList[10];
};
 
 
extern BF_GUI_KeysMenu			*poSysKeysMenu;
 
#endif
