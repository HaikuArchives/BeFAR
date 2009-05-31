#include "BL_Tools.h"
#include "BF_Msg.h"
#include "BF_GUI_Func.h"
#include "BF_GUI_Viewer.h"
#include "BF_GUI_Tools.h"
#include "BF_ToolsView.h"
#include "BF_GUI_KeysMenu.h"
#include "BF_GUI_MessageBox.h"
#include "BF_Viewer_SearchCMD.h"
#include <List.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 
BF_GUI_Viewer::BF_GUI_Viewer(BRect o_frame, const char *pc_FileForView,BView	*po_Panel)
:BView(o_frame,"main_view",B_FOLLOW_ALL_SIDES,B_WILL_DRAW)
//:BView(o_frame,"main_view",B_FOLLOW_ALL_SIDES,B_WILL_DRAW|B_NAVIGABLE|B_FRAME_EVENTS)
{
	ASSERT(po_Panel);
	poPanel = po_Panel;

	BRect oRect;

	pcFileForView = pc_FileForView;

	ploString = new BList();
	LoadFile();
	
	oRect = Bounds();
	oRect.bottom = oRect.top + poSysSetup->oFontNode.fHeight;
	poStatus = new BF_GUI_Status(oRect,poSysSetup,pc_FileForView);
	
	oRect = Bounds();
	oRect.top += poSysSetup->oFontNode.fHeight;
	poText = new BF_GUI_Text(oRect,ploString,poStatus,this);
	if(poSysKeysMenu) poText->PrepareKeysMenu();
		
	AddChild(poStatus);
	AddChild(poText);
};

BF_GUI_Viewer::~BF_GUI_Viewer()
{
	delete(ploString);
}

void 
BF_GUI_Viewer::LoadFile()
{
	FILE *pViewText;
	char	cBuffer[256];
	BString	*psBuffer;
	
	pViewText = fopen(pcFileForView,"r");

	if(pViewText == NULL)
	{
		BF_GUI_MessageBox *po_Message = new BF_GUI_MessageBox(BRect(30,30,300,150),"Error","Error open file file!",this,"Close");
		AddChild(po_Message);
		return;
	};
	
	while(!feof(pViewText))
	{
		for(int i=0; i < 255; i++) { cBuffer[i] = 0; };
		fgets(cBuffer,255,pViewText);
		psBuffer = new BString(cBuffer);
		ploString->AddItem(psBuffer);
	}
	fclose(pViewText);
}

void 
BF_GUI_Viewer::KeyDown(const char *bytes, int32 numBytes)
{
	int32	iFKey;
	
	if ( numBytes == 1 ) {
		switch ( bytes[0] ) {
			case B_DOWN_ARROW:
				poText->Nav_LineDown();
				break;
			case B_UP_ARROW:
				poText->Nav_LineUp();
				break;
			case B_RIGHT_ARROW:
				poText->Nav_LineRight();
				break;
			case B_LEFT_ARROW:
				poText->Nav_LineLeft();
				break;
			case B_PAGE_UP:
				poText->Nav_PageUp();
				break;
			case B_PAGE_DOWN:
				poText->Nav_PageDown();
				break;
			case B_ESCAPE:
				QuitViewer();
				break;
			case B_FUNCTION_KEY:
			{
				BMessage *oMSG = Window()->CurrentMessage();
				if (oMSG)
				{
					oMSG->FindInt32("key",&iFKey);
					if (poText->iKeysModifiers & B_LEFT_SHIFT_KEY || poText->iKeysModifiers & B_RIGHT_SHIFT_KEY)
					{
						switch( iFKey )
						{
							case B_F7_KEY:{
								poText->FindNext();
								break;}
							default:
								BView::KeyDown(bytes, numBytes);
								break;
						}
					}
					else
					{
						switch( iFKey )
						{
							case B_F8_KEY:
								poText->NextCodePage();
								break;
							case B_F3_KEY:
							case B_F10_KEY:{
								QuitViewer();
								break;}
							case B_F7_KEY:{
								SearchSample();
								break;}
							default:
								BView::KeyDown(bytes, numBytes);
								break;
						}
					}
				};
				break;
			}
			default:
				BView::KeyDown(bytes, numBytes);
				break;
			}
		}
		else
		{
			BView::KeyDown(bytes, numBytes);
		};
}


void
BF_GUI_Viewer::QuitViewer()
{
	BMessage oMessage(BF_MSG_VIEWER_CLOSED);		
	BMessenger oMessenger(poPanel);
	oMessenger.SendMessage(&oMessage);
	RemoveSelf();	
	delete this;
	
};


void
BF_GUI_Viewer::MessageReceived(BMessage* po_Message)
{
//	printf("BF_GUI_Viewer::MessageReceived()\n");
	switch(po_Message->what)
	{
		case 1599425348:
		case 1599425365:
			if(poSysKeysMenu)		poText->PrepareKeysMenu();
			break;
		case BF_MSG_VIEWER_CH_CP:
			poText->NextCodePage();
			break;
		case BF_MSG_VIEWER_CLOSED:
			QuitViewer();
			break;
		case BF_MSG_VIEWER_SEARCH:
			switch(po_Message->FindInt32("bf_viewer_search_cmd"))
			{
				case BF_VIEWER_CMD_SEARCH_DIALOG:
					SearchSample();
					break;
				case BF_VIEWER_CMD_SEARCH_FIRST:
					MakeFocus();
					char *pcValue;
					if(B_OK==po_Message->FindString("bf_pcValue",&pcValue) && pcValue)
					{
						poText->FindFirst(pcValue);
					}
					break;
				case BF_VIEWER_CMD_SEARCH_NEXT:
					poText->FindNext();
					break;
			}
			break;
		case BF_MSG_VIEWER_MESSAGE:{			
			char *pc_MenuResult=NULL;
			if(B_OK==po_Message->FindString("buttons",&pc_MenuResult) && pc_MenuResult 
				&& strcmp(pc_MenuResult,"Close") == 0)
			{
				QuitViewer();
			}
			else
			{
				MakeFocus();
			};
			break;}
		default:
			BView::MessageReceived(po_Message);
	}
};

void
BF_GUI_Viewer::SearchSample()
{
	/* prepare message */
	BMessage oMessage(BF_MSG_VIEWER_SEARCH);
	oMessage.AddInt32("bf_viewer_search_cmd",BF_VIEWER_CMD_SEARCH_FIRST);
	oMessage.AddPointer("bf_focus",this);
	/* make dialog */
	BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(BRect(30,30,350,110),
		"Sample For Search","dialog",&oMessage,BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER);
	poDialog->MoveToCenter( this );	
	/* insert views */
	BRect oRect;
	/* insert edit_view */
	poDialog->LocalBounds(oRect);	
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	BF_GUI_ViewEdit *poEdit = new BF_GUI_ViewEdit(oRect,"bf_pcValue",
					poText->GetMatch(),
					B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
	poDialog->AddChild(poEdit);
	/* finish */
	AddChild( poDialog );	

		
	/* old text 
	BMessage oMessage(BF_MSG_VIEWER_SEARCH);
	oMessage.AddInt32("bf_viewer_search_cmd",BF_VIEWER_CMD_SEARCH_FIRST);
	oMessage.AddPointer("bf_focus",this);
	
	BF_GUI_ToolEdit *poToolView = 
		new BF_GUI_ToolEdit(BRect(30,30,350,110),"Sample For Search","",oMessage);
	poToolView->MoveToCenter(this);
	poToolView->poEditView->sValue << poText->GetMatch();
	AddChild(poToolView);		
	poToolView->poEditView->MakeFocus();	
	*/

}
