#include <InterfaceKit.h>
#include "BF_ToolsView.h"
#include "BF_GUI_KeysMenu.h"
#include "BF_Msg.h"
#include "BF_GUI_Tools.h"
#include "BF_GUI_MessageBox.h"
#include "BF_Viewer_SearchCMD.h"

#include <stdio.h>
#include <SupportKit.h>

/*===========================================================================
                                    BF_GUI_Status
===========================================================================*/
// Constructor
BF_GUI_Status::BF_GUI_Status(BRect o_frame, BF_GUI_Setup *po_Setup, char *pc_FileForView) : BView(o_frame,"status_view",B_FOLLOW_LEFT_RIGHT,B_WILL_DRAW)
{
	poSysSetup = po_Setup;
	SetViewColor(SYS_COLOR(BF_COLOR_CURSOR_BACK));
	SetFont(&poSysSetup->oFontNode.oFont);
	BStringView *poFileName = new BStringView(BRect(0,0,StringWidth(pc_FileForView),poSysSetup->oFontNode.fHeight),"file_name",pc_FileForView);
	AddChild(poFileName);
	
	o_frame.bottom = poSysSetup->oFontNode.fHeight;
	o_frame.left = o_frame.right - 50;
	o_frame.right -= 5;
	poPos = new BStringView(o_frame,"POS","0%",B_FOLLOW_RIGHT|B_FOLLOW_TOP);
	poPos->SetAlignment(B_ALIGN_RIGHT);
	AddChild(poPos);
	
	o_frame.left -= 50;
	o_frame.right -= 50;
	poCode = new BStringView(o_frame,"CODE","UTF8",B_FOLLOW_RIGHT|B_FOLLOW_TOP);
	poCode->SetAlignment(B_ALIGN_LEFT);
	AddChild(poCode);
}


void
BF_GUI_Status::MessageReceived(BMessage* po_Message)
{
	float fPOS;
	
	switch(po_Message->what)
	{
		case BF_MSG_VIEWER_MOVE:
			if(po_Message->FindFloat("Position",&fPOS) == B_OK)
			{
				SetPos(fPOS);
			};
			break;
		default:
			BView::MessageReceived(po_Message);
	}
}

// display position on the status bar
void
BF_GUI_Status::SetPos(float f_Pos)
{
	char cBuffer[10];
	int iProc = (int)(f_Pos * 100);
	sprintf(cBuffer,"%u%%",(iProc > 100) ? 100 : iProc);
	poPos->SetText(cBuffer);
};


// display codepage on the status bar
void 
BF_GUI_Status::SetCodePage(uint32 i_CodePage)
{
	char *pcCodePage[] = {"UTF8","DOS","WIN","KOI8R"};
	
	poCode->SetText(pcCodePage[i_CodePage]);
};

/*===========================================================================
                                    BF_GUI_Text
===========================================================================*/
BF_GUI_Text::BF_GUI_Text(	
	BRect o_frame,
	BList *plo_String, 
	BF_GUI_Status *po_Status,
	BHandler *po_Handler
):BView(	o_frame,"text_view",B_FOLLOW_ALL_SIDES,B_WILL_DRAW)
{
	ploString = plo_String;
	poStatus = po_Status;
	iCodePage = CP_UTF8;
	SetViewColor(SYS_COLOR(BF_COLOR_BACK));
	SetFont(&poSysSetup->oFontNode.oFont);
	bFullPage = false;
	poHandler = po_Handler;	
	iStartSel = 0;
	iStopSel = 0;
	iMatchLine = 0;
	poMatch = NULL;
	bDrawSel = false;
}

BF_GUI_Text::~BF_GUI_Text()
{
	if(poMatch != NULL) delete(poMatch);
}

void 
BF_GUI_Text::Draw(BRect oRect)
{
	DrawPage();
}


void
BF_GUI_Text::MouseDown(BPoint o_point)
{
	printf("BF_GUI_Text::MouseDown()\n");
	BRect oRect = Bounds();
	if (o_point.y > oRect.top && o_point.y < oRect.top + oRect.Height() / 2)
	{
		Nav_PageUp();
	}
	else
	{
		Nav_PageDown();
	}
};

// clear current page
void BF_GUI_Text::ClearView()
{
	BRect oRect = Bounds();
	SetHighColor(SYS_COLOR(BF_COLOR_BACK));
	FillRect(oRect);
};

// draw current page
void 
BF_GUI_Text::DrawPage()
{
	BRect	oRect;
	BPoint	oPoint(3,poSysSetup->oFontNode.fHeight);
	int32	iCurLine;
	BString	*psLine;
	char	cBuffer[256];
	
	oRect = Bounds();
	
	SetHighColor(SYS_COLOR(BF_COLOR_NODE));
	SetLowColor(SYS_COLOR(BF_COLOR_BACK));
	for(iCurLine = 0;oPoint.y < oRect.bottom + poSysSetup->oFontNode.fHeight && iCurLine < ploString->CountItems();iCurLine++)
	{
		psLine = (BString *) ploString->ItemAt(iCurLine);

		ToUtf8(psLine->String(),cBuffer);

		BString oCurStr(cBuffer);
		oCurStr.ReplaceAll("\r","\x0");
		oCurStr.ReplaceAll("\n","\x0");
		oCurStr.ReplaceAll("\t","        ");
		if (bDrawSel && iMatchLine == iCurLine)
		{
			BString oBuffer;
			// draw first part of the string
			iStartSel = oCurStr.FindFirst(poMatch->String(),iStartSel);
			iStopSel = poMatch->CountChars();
			oCurStr.MoveInto(oBuffer,0,iStartSel);
			DrawString(oBuffer.String(),oPoint);
			// draw selected part of the string
			SetHighColor(SYS_COLOR(BF_COLOR_CURSOR_BACK));
			float fStartPoint = StringWidth(oBuffer.String());
			float fWidth = StringWidth(poMatch->String());
			BRect oRect(oPoint.x + fStartPoint,oPoint.y - poSysSetup->oFontNode.fHeight,oPoint.x + fStartPoint + fWidth,oPoint.y);
			FillRect(oRect);
			SetHighColor(0,0,0);
			SetLowColor(SYS_COLOR(BF_COLOR_CURSOR_BACK));
			oCurStr.MoveInto(oBuffer,0,iStopSel);
			DrawString(oBuffer.String());
			// draw rest part of the string
			SetHighColor(SYS_COLOR(BF_COLOR_NODE));
			SetLowColor(SYS_COLOR(BF_COLOR_BACK));
			DrawString(oCurStr.String());
		}
		else
		{
			// draw string without selection
			DrawString(oCurStr.String(),oPoint);
		};
		oPoint.y += poSysSetup->oFontNode.fHeight;
	};
//-------------------------------- change display position ----------------------------
/*
	BMessage *poMSG = new BMessage(BF_MSG_VIEWER_MOVE);
	float fPOS = oPoint.y / (poSysSetup->oFontNode.fHeight * ploString->CountItems());
	poMSG->AddFloat("Position",fPOS);
	BMessenger oMessenger(poStatus);
	oMessenger.SendMessage(poMSG);
*/
//------------------------------ TRUE if page is full ---------------------------------
	bFullPage = (oPoint.y < oRect.bottom + poSysSetup->oFontNode.fHeight) ? false : true;
};

// Line down action
void 
BF_GUI_Text::Nav_LineDown()
{
	if (bFullPage)
	{
		ScrollBy(0,20);
		ClearMatch();
	};		
};

// Line up action
void 
BF_GUI_Text::Nav_LineUp()
{
	BRect oRect = Bounds();
	
	if (oRect.top > 0)
	{
		ScrollBy(0,(oRect.top > 20) ? -20 : -oRect.top);
		ClearMatch();
	};		
};

// Key left action
void 
BF_GUI_Text::Nav_LineLeft()
{
	BRect oRect = Bounds();
	
	if (oRect.left > 0)
	{
		ScrollBy(-10,0);
		ClearMatch();
	};
};

// Key right action
void 
BF_GUI_Text::Nav_LineRight()
{
	ScrollBy(10,0);
	ClearMatch();
};

// page up action
void 
BF_GUI_Text::Nav_PageUp()
{
	BRect	oRect = Bounds();
	
	if (oRect.top <= 0) return;

	if (2 * oRect.top - oRect.bottom > 0)
	{
		ScrollBy(0,-oRect.Height());
	}
	else
	{
		ScrollBy(0,-oRect.top);
	};
	ClearMatch();
}

// Page down action
void 
BF_GUI_Text::Nav_PageDown()
{
	if (bFullPage)
	{
		BRect oRect = Bounds();
		if (2 * oRect.bottom - oRect.top > poSysSetup->oFontNode.fHeight * ploString->CountItems())
		{
			ScrollTo(oRect.left,poSysSetup->oFontNode.fHeight * ploString->CountItems() - oRect.Height());
		}
		else
		{
			ScrollBy(0,oRect.Height());
		}
		ClearMatch();
	}
}


// set and display next in list codepage
void 
BF_GUI_Text::NextCodePage()
{
	iCodePage = (iCodePage < CP_KOI) ? iCodePage + 1 : CP_UTF8;
	poStatus->SetCodePage(iCodePage);
	ClearView();
	DrawPage();
};


void
BF_GUI_Text::PrepareKeysMenu()
{
	if(!poSysKeysMenu) return;
	iKeysModifiers = modifiers();
	
	poSysKeysMenu->Clear();
	if (iKeysModifiers & B_LEFT_SHIFT_KEY || iKeysModifiers & B_RIGHT_SHIFT_KEY)
	{
		poSysKeysMenu->SetText(7,"Next");
		
		BMessage *poMessage = new BMessage(BF_MSG_VIEWER_SEARCH);
		poMessage->AddInt32("bf_viewer_search_cmd",BF_VIEWER_CMD_SEARCH_NEXT);
		
		poSysKeysMenu->SetMessage(7,poMessage,poHandler);
	}
	else
	{
		poSysKeysMenu->SetText(3,"Close");
		poSysKeysMenu->SetText(8,"CP");
		poSysKeysMenu->SetText(10,"Close");
		poSysKeysMenu->SetText(7,"Search");

		poSysKeysMenu->SetMessage(3,new BMessage(BF_MSG_VIEWER_CLOSED),poHandler);
		poSysKeysMenu->SetMessage(8,new BMessage(BF_MSG_VIEWER_CH_CP),poHandler);
		poSysKeysMenu->SetMessage(10,new BMessage(BF_MSG_VIEWER_CLOSED),poHandler);
		BMessage *poMessage = new BMessage(BF_MSG_VIEWER_SEARCH);
		poMessage->AddInt32("bf_viewer_search_cmd",BF_VIEWER_CMD_SEARCH_DIALOG);
		poSysKeysMenu->SetMessage(7,poMessage,poHandler);
	};
}

void
BF_GUI_Text::Find(char *pc_Sample, uint32 i_StartLine, uint32 i_StartCol)
{
	uint32	iColumn = i_StartCol;
	BString *poString;
	char cSample[256];
	bool bFound = false;
	BF_GUI_MessageBox *po_Message;

	ClearMatch();
	if (pc_Sample == NULL) 
	{
		FromUtf8(poMatch->String(),cSample);
	}
	else
	{
		FromUtf8(pc_Sample,cSample);
	}

	for(int32 i = i_StartLine; i < ploString->CountItems();i++)
	{
		poString = (BString *) ploString->ItemAt(i);
		if (poString->FindFirst(cSample,iColumn) != B_ERROR)
		{
			iMatchLine = i;
			bFound = true;
			break;
		};
		iColumn = 0;
	};
	if (bFound)
	{
		if (pc_Sample != NULL)
		{
			if (poMatch != NULL) delete(poMatch);
			poMatch = new BString(pc_Sample);
		};
		bDrawSel = true;
		BRect oRect = Bounds();
		float y = iMatchLine * poSysSetup->oFontNode.fHeight;
		ScrollTo(oRect.left,y);
		iStartSel = iColumn;
		DrawPage();
	}
	else
	{
		bDrawSel = false;
		if (i_StartLine)
		{
			po_Message = new BF_GUI_MessageBox(BRect(30,30,300,150),"Error","Not more samles found!",Parent(),"Ok");
		}
		else
		{
			po_Message = new BF_GUI_MessageBox(BRect(30,30,300,150),"Error","The sample is not found in this file!",Parent(),"Ok");
		}
		Parent()->AddChild(po_Message);
	};
};

void
BF_GUI_Text::FindFirst(char *pc_Sample)
{

	iMatchLine = 0;
	iStartSel = 0;
	iStopSel = 0;

	Find(pc_Sample);
};

void
BF_GUI_Text::FindNext()
{
	Find(NULL,iMatchLine,iStartSel + iStopSel);
};

void
BF_GUI_Text::ToUtf8(const char *pc_Source,char *pc_Dest)
{
	int32	iIN;
	int32	iOUT;
	int32	iStatus;
	uint32	iCP = GetConversion();

		if ( iCodePage == CP_UTF8 )
		{
			strcpy(pc_Dest,pc_Source);
		}
		else
		{
			iIN = strlen(pc_Source);
			iOUT = 255;
			convert_to_utf8(iCP,pc_Source,&iIN,pc_Dest,&iOUT,&iStatus);
			pc_Dest[iOUT] = 0;
		}
}

void
BF_GUI_Text::FromUtf8(const char *pc_Source,char *pc_Dest)
{
	int32	iIN;
	int32	iOUT;
	int32	iStatus;
	uint32	iCP = GetConversion();

		if ( iCodePage == CP_UTF8 )
		{
			strcpy(pc_Dest,pc_Source);
		}
		else
		{
			iIN = strlen(pc_Source);
			iOUT = 255;
			convert_from_utf8(iCP,pc_Source,&iIN,pc_Dest,&iOUT,&iStatus);
			pc_Dest[iOUT] = 0;
		}
}


uint32
BF_GUI_Text::GetConversion()
{
	uint32	iCP = 0;
		switch (iCodePage )
		{
			case CP_DOS:
				iCP = B_MS_DOS_866_CONVERSION;
				break;
			case CP_WIN:
				iCP = B_MS_WINDOWS_1251_CONVERSION;
				break;
			case CP_KOI:
				iCP = B_KOI8R_CONVERSION;
				break;
		};
		return(iCP);
};

void
BF_GUI_Text::ClearMatch( void )
{
	if(bDrawSel)
	{
		bDrawSel = false;
		ClearView();
		DrawPage();
	};
}

char
*BF_GUI_Text::GetMatch( void )
{
	if(poMatch == NULL)
	{
		return(NULL);
	}
	else
	{
		return poMatch->String(); 
	};
};
