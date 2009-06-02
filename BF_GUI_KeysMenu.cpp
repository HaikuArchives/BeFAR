#include <string.h>
#include <stdio.h>
#include <String.h>
#include "BL_Tools.h"
#include "BF_Msg.h"
#include "BF_GUI_KeysMenu.h"

BF_GUI_KeysMenu			*poSysKeysMenu = NULL;

BF_GUI_KeysMenu_Item::
BF_GUI_KeysMenu_Item(BRect o_frame, const char *pc_ButtonNum,const char *pc_ButtonText, BMessage *po_Message, BHandler *po_Handler)
 : BView(o_frame,"buttons_item",B_FOLLOW_TOP,B_WILL_DRAW)
{
	ASSERT(pc_ButtonText && pc_ButtonNum);
	/* init self vars */
	poMessage = NULL;
	poHandler = NULL;	
	//SetViewColor(SYS_COLOR(BF_COLOR_CURSOR_BACK));
	SetViewColor(B_TRANSPARENT_COLOR);
	sNum = pc_ButtonNum;
	sText = pc_ButtonText;
};

BF_GUI_KeysMenu_Item::
~BF_GUI_KeysMenu_Item()
{
	SetMessage();
};

bool
BF_GUI_KeysMenu_Item::Invoke()
{
	if (poMessage != NULL && poHandler != NULL){
		BMessenger oMessenger(poHandler);
		oMessenger.SendMessage(new BMessage(*poMessage)); 
		return true;
	}else
		return false;
}

void					
BF_GUI_KeysMenu_Item::MouseDown(BPoint point)
{
	Invoke();
}
										
void					
BF_GUI_KeysMenu_Item::SetText(const char *pc_Text)
{
	sText= pc_Text;
	if(sText.Length()>10) sText.Remove(10,sText.Length()-9);	
}

 
void 
BF_GUI_KeysMenu_Item::Draw(BRect o_Rect)
{
	BRect oRect = Bounds();

	/* draw background for number*/		
	oRect.right = oRect.left + StringWidth("99");
	SetHighColor(0,0,0);
	FillRect(oRect);

	/* draw background for text */
	BRect oRect1 = Bounds();	
	oRect1.left = oRect.right+1;
	SetHighColor(SYS_COLOR(BF_COLOR_CURSOR_BACK));
	FillRect(oRect1);
	
	/* draw number */
	SetHighColor(209,209,209);
	SetLowColor(0,0,0);	
	BPoint oPoint(oRect.left + (oRect.Width() - StringWidth(sNum.String()))/2,
		oRect.bottom - 1);
	DrawString(sNum.String(),oPoint);				
	
	/* draw text */
	SetHighColor(0,0,0);
	SetLowColor(SYS_COLOR(BF_COLOR_CURSOR_BACK));
	oPoint.x = oRect1.left + 3;
	DrawString(sText.String(),oPoint);	
	//
};

void
BF_GUI_KeysMenu_Item::SetMessage(BMessage *po_Message, BHandler *po_Handler)
{
	if (poMessage)		delete(poMessage);
	/* old code 
	if (poMessage != NULL)		delete(poMessage);
	*/
	poMessage		= po_Message;
	poHandler		= po_Handler;
}

/*===========================================================================
                                    BF_GUI_KeysMenu
===========================================================================*/
BF_GUI_KeysMenu::BF_GUI_KeysMenu(BRect o_frame) 
: BView(o_frame,"buttons_view",B_FOLLOW_BOTTOM|B_FOLLOW_LEFT_RIGHT,
	B_WILL_DRAW|B_FRAME_EVENTS)
{
	poSysKeysMenu = this;

	//SetViewColor(SYS_COLOR(BF_COLOR_CURSOR_BACK));
	SetViewColor(B_TRANSPARENT_COLOR);
	
	int iCounter;
	float fStep = o_frame.Width() / 10;
	
	BRect oRect = Bounds();
	oRect.right = fStep;
	for(iCounter = 0; iCounter < 10; iCounter++, oRect.left += fStep, oRect.right += fStep)
	{
		switch(iCounter)
		{
			case 0:
				poButtonList[iCounter] = new BF_GUI_KeysMenu_Item(oRect,"1","");
				break;
			case 1:
				poButtonList[iCounter] = new BF_GUI_KeysMenu_Item(oRect,"2","");
				break;
			case 2:
				poButtonList[iCounter] = new BF_GUI_KeysMenu_Item(oRect,"3","");
				break;
			case 3:
				poButtonList[iCounter] = new BF_GUI_KeysMenu_Item(oRect,"4","");
				break;
			case 4:
				poButtonList[iCounter] = new BF_GUI_KeysMenu_Item(oRect,"5","");
				break;
			case 5:
				poButtonList[iCounter] = new BF_GUI_KeysMenu_Item(oRect,"6","");
				break;
			case 6:
				poButtonList[iCounter] = new BF_GUI_KeysMenu_Item(oRect,"7","");
				break;
			case 7:
				poButtonList[iCounter] = new BF_GUI_KeysMenu_Item(oRect,"8","");
				break;
			case 8:
				poButtonList[iCounter] = new BF_GUI_KeysMenu_Item(oRect,"9","");
				break;
			case 9:
				poButtonList[iCounter] = new BF_GUI_KeysMenu_Item(oRect,"10","");
				break;
		};
		AddChild(poButtonList[iCounter]);
	};
}

void
BF_GUI_KeysMenu::FrameResized(float f_width, float f_height)
{
	BView::FrameResized(f_width,f_height);

	int iCounter;	
	BRect oRect = Bounds();
	float fStep = oRect.Width() / 10;
	oRect.right = fStep;
	
	for(iCounter = 0; iCounter < 10; iCounter++, oRect.left += fStep, oRect.right += fStep)
	{
		poButtonList[iCounter]->ResizeTo(oRect.Width(),oRect.Height());
		poButtonList[iCounter]->MoveTo(oRect.left,oRect.top);
	}
} 

void					
BF_GUI_KeysMenu::SetText(uint8 i_Index,const char *pc_Text)
{	
	i_Index--;
	ASSERT(i_Index<10,"BF_GUI_KeysMenu::SetText() : i_Index>9 !!1");
	poButtonList[i_Index]->SetText(pc_Text);		
	poButtonList[i_Index]->Invalidate();
}

void					
BF_GUI_KeysMenu::Clear()
{
	for(int i=0;i<10;i++){
		SetText(i+1,"");
		SetMessage(i+1,NULL,NULL);
	}
}
  
void
BF_GUI_KeysMenu::SetMessage(uint8 i_Index, BMessage *po_Message , BHandler *po_Handler )
{
	i_Index--;
	ASSERT(i_Index<10,"BF_GUI_KeysMenu::SetMessage() : i_Index>9 !!1");
	poButtonList[i_Index]->SetMessage(po_Message,po_Handler);
	poButtonList[i_Index]->Invalidate();
}

bool					
BF_GUI_KeysMenu::OnKeyDown(const char *bytes, int32 numBytes)
{		
	if(bytes[0] == B_FUNCTION_KEY){
		int32 iKey;
		BMessage *poMsg = Window()->CurrentMessage();
		if(!poMsg) return false;
		poMsg->FindInt32("key", &iKey);
		
		if(iKey>=B_F1_KEY && iKey<=B_F10_KEY){		
			BF_GUI_KeysMenu_Item* poItem = (BF_GUI_KeysMenu_Item*)poButtonList[(int)(iKey - B_F1_KEY)];
			if(poItem->Invoke()) return true;
		}
		
	}
	return false;
}
void 					
BF_GUI_KeysMenu::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what) {
	case BF_MSG_SETUP_UPDATED:{		
		for(int i=0;i<10;i++){
			if(!poButtonList[i]) continue;
			poButtonList[i]->Draw(poButtonList[i]->Bounds());
		}
		break;}
	default:
		BView::MessageReceived(po_Message);
	}
}