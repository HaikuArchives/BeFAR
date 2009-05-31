/*
===============================================
Project:	BeFAR
File:		BF_GUI_TextViewer.cpp   
Desc:		Provides viewing textfile.
Author:		by Baza, special thanks to Pavel Popov
Created:	06.01.2000
Modified:	16 jul 2000
===============================================
*/

#include <stdio.h>
#include <UTF8.h>

#include "BL_Tools.h"
#include "BL_File.h"

#include "BF_Msg.h"
#include "BF_Dict.h"
#include "BF_GUI_KeysMenu.h"
#include "BF_GUI_DlgViews.h"
#include "BF_GUI_TextViewer.h"
#include "BF_GUI_Func.h"
#include "BF_GUI_WinMain.h"

///////////////////////////////////////////////////////////////////////////////////
BF_TextFile_Line::BF_TextFile_Line(const BL_String & s_Src,off_t i_Pos,int32 i_Bytes)
{
	s = s_Src;
	iBytes=i_Bytes;
	iPos = i_Pos;
}	

///////////////////////////////////////////////////////////////////////////////////
BF_TextFile_Buffer::BF_TextFile_Buffer()
{	
	iFileSize = 0;
}

void				
BF_TextFile_Buffer::SetFileNameTo(const char *pc_FileName)
{
	ASSERT(pc_FileName);
	sFileName = pc_FileName;
}

off_t				
BF_TextFile_Buffer::FileSize()
{
	if(iFileSize>0) return iFileSize;
	
	OpenFile();	
	if(B_OK!=oFile.GetSize(&iFileSize)) return 0;
	CloseFile();
	return iFileSize;
}

void
BF_TextFile_Buffer::CloseFile()
{
	oFile.Unset();
}

status_t
BF_TextFile_Buffer::OpenFile()
{	
	ASSERT(sFileName!="");
	CloseFile();
	return oFile.SetTo(sFileName.String(),B_READ_ONLY);		
}

int32 
BF_TextFile_Buffer::LoadText_BeforePos(off_t & i_LastPos,int32 i_LinesCount,BL_List & lo_Data)
{
	off_t iLastPos = i_LastPos;
	lo_Data.DeleteItems();


	while(lo_Data.CountItems()<i_LinesCount){

		bool 	bFrom0 = iLastPos<=5000;
		off_t 	iFromPos = bFrom0?0:(iLastPos - 5000);
	
		BL_List loData1;
		int32 iReqCount = i_LinesCount - lo_Data.CountItems();
		int32 iLoadedLines = LoadText(iFromPos,100000000,loData1,true,i_LastPos);
		if(iLoadedLines <= iReqCount){
			lo_Data.AddList(&loData1,0);
			loData1.MakeEmpty();
			break;
		}else
		if(iLoadedLines > iReqCount){
			for(int i=0;i<(iLoadedLines - iReqCount);i++){
				loData1.DeleteItemAt(0);
				
			}
			lo_Data.AddList(&loData1,0);
			loData1.MakeEmpty();
			break;
		}else{
			iLastPos -= 5000;
		}
	}		
	
	BF_TextFile_Line *poLine = (BF_TextFile_Line*)lo_Data.ItemAt(0);
	ASSERT(poLine);
	i_LastPos = poLine->Pos();
	
	return lo_Data.CountItems();
}

int32
BF_TextFile_Buffer::LoadText_FromNextLine(off_t & i_StartPos,int32 i_LinesCount,BL_List & lo_Data,bool b_DeleteFirstLines)
{	
	// calc new pos //
	BF_TextFile_Line *poLine = (BF_TextFile_Line*)lo_Data.LastItem();
	if(!poLine) return 0;
	off_t iNewPos = poLine->Pos()+poLine->Bytes();

	int32 iLinesLoaded = LoadText(iNewPos,i_LinesCount,lo_Data,false);
	if(iLinesLoaded>0){
		i_StartPos = iNewPos;		
		
		if(b_DeleteFirstLines) for(int i=0;i<iLinesLoaded;i++) 	lo_Data.DeleteItemAt(0);
		
		BF_TextFile_Line *poLine = (BF_TextFile_Line*)lo_Data.ItemAt(0);
		if(!poLine) return 0;
		i_StartPos = poLine->Pos();
				
	}
	
	return iLinesLoaded;
}

int32
BF_TextFile_Buffer::LoadText(off_t i_StartPos,int32 i_LinesCount,BL_List & lo_Data,bool b_DeleteData,off_t i_LastPos)
{
	status_t 	uRes=B_OK;
	int32 		iLinesLoaded=0;
	off_t		iPos=0;
	BL_String	s;
	// open file //
	uRes = OpenFile();
	if(B_OK!=uRes) return -1;	
	if(b_DeleteData) lo_Data.DeleteItems();
	
	// seek pos //	
	iPos = oFile.Seek(i_StartPos,SEEK_SET);
	if(iPos!=i_StartPos) return -1;
	
	// load lines
	BF_TextFile_Line	*poLine;
	for(int iLine=0;iLine<i_LinesCount;iLine++){
		if(B_OK!=oFile.ReadString(&s)) break;
		
		poLine = new BF_TextFile_Line(s,iPos,oFile.Position()-iPos);
		lo_Data.AddItem(poLine);
		
		iLinesLoaded++;
		iPos = oFile.Position();
		if((i_LastPos>=0) && (iPos>=i_LastPos)) break;
	}	
	oFile.GetSize(&iFileSize);
	CloseFile();
	return iLinesLoaded;
}

/*===========================================================================
                                    BF_GUI_TextViewer_StatusBar
===========================================================================*/

BF_GUI_TextViewer_StatusBar::BF_GUI_TextViewer_StatusBar(BRect & o_Rect,const char *pc_FileName)
:BView(o_Rect,"textviewer_status",B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_WILL_DRAW)
{
	iFilePos = 0;
	iPageSize = 0;
	iFileSize = 0;
	iCodePage = CP_UTF8;
	ASSERT(pc_FileName);
	sFileName = pc_FileName;	
	
	SetViewColor(B_TRANSPARENT_COLOR);
	SetFont(&poSysSetup->oFontNode.oFont);	
	
}

void			
BF_GUI_TextViewer_StatusBar::Draw(BRect o_Rect)
{
	BRect 		oRect;
	BPoint		oPoint;
	BL_String	s,s1;
	//
	SetHighColor(SYS_COLOR(BF_COLOR_TEXTVIEWER_STATUS_BACK));
	oRect = Bounds();	
	FillRect(oRect);
	//		
	SetHighColor(SYS_COLOR(BF_COLOR_TEXTVIEWER_STATUS_TEXT)); 
	SetLowColor(SYS_COLOR(BF_COLOR_TEXTVIEWER_STATUS_BACK));	
	//
	oPoint.x = 5;
	oPoint.y = poSysSetup->oFontNode.fAscent;
	// draw file_pos/size //
	s=BF_DictAt(BF_DICT_TEXTVIEWER_POS);
	s<<": ";
	s1=iFilePos;		
	s<<*s1.SetDigits();
	s<<"..";
	s1=(iFilePos+iPageSize);
	s<<*s1.SetDigits();
	s<<"/";
	s1=iFileSize;
	s<<*s1.SetDigits();
	DrawString(s.String(),oPoint);
	oPoint.x += poSysSetup->oFontNode.oFont.StringWidth(s.String())+10;
	// draw startcol //	
	s=BF_DictAt(BF_DICT_TEXTVIEWER_COL);
	s<<":";
	s<<iStartColumn;
	DrawString(s.String(),oPoint);
	oPoint.x += poSysSetup->oFontNode.oFont.StringWidth(s.String())+10;
	// draw code_page //
	s = BF_DictAt(BF_DICT_TEXTVIEWER_CP);
	s<<": ";
	switch(iCodePage){
	case CP_UTF8:
		s<<"UTF8";
		break;
	case CP_DOS:
		s<<"DOS866";
		break;
	case CP_WIN:
		s<<"WIN1251";
		break;
	case CP_KOI:
		s<<"KOI8R";	
		break;
	default:
		s<<"unknown";
	};
	DrawString(s.String(),oPoint);
	oPoint.x += poSysSetup->oFontNode.oFont.StringWidth(s.String())+10;
	// draw file_name //
	{		
		s=BF_DictAt(BF_DICT_FILE);
		s<<": ";
		s<<sFileName;
		DrawString(s.String(),oPoint);
	}
}

void				
BF_GUI_TextViewer_StatusBar::SetVarsTo(off_t i_FilePos,off_t i_PageSize,off_t i_FileSize,int32 i_CodePage,int32 i_StartColumn)
{
	iFilePos = i_FilePos;
	iPageSize = i_PageSize;
	iFileSize = i_FileSize;
	iCodePage = i_CodePage;
	iStartColumn = i_StartColumn;
	if(Parent()) Invalidate(Bounds());
}

/*===========================================================================
                                    BF_GUI_TextViewer
===========================================================================*/
BF_GUI_TextViewer::BF_GUI_TextViewer(	
	BRect o_frame,
	const char 	*pcFileForView
):BView(o_frame,"textviewer_",B_FOLLOW_ALL_SIDES,B_WILL_DRAW|B_NAVIGABLE)
{		
	ASSERT(pcFileForView);
	sFileName = pcFileForView ;
	
	poLastFocus = poWin->CurrentFocus();
	
		
	iCodePage = CP_UTF8;
	SetViewColor(B_TRANSPARENT_COLOR);
	SetFont(&poSysSetup->oFontNode.oFont);	
	bWrapped = false;
	bShowSpec = false;
	
	iFilePos = 0;
	iFileSize = 0;
	iStartColumn = 0;
	iSearchPos = -1;
	
	oBuffer.SetFileNameTo(pcFileForView);
	
	CalcVars();
	
	LoadPage();
	UpdateStatusBar();
		
}

BF_GUI_TextViewer::~BF_GUI_TextViewer()
{	
}


void
BF_GUI_TextViewer::CalcVars()
{
	fFontHeight = poSysSetup->oFontNode.fHeight;
	iLinesCount = (int32) ( Bounds().Height()/fFontHeight);
}

void
BF_GUI_TextViewer::CloseViewer()
{
	BF_GUI_TextViewer_StatusBar *poStatus = (BF_GUI_TextViewer_StatusBar*)Parent()->FindView("textviewer_status");
	if(poStatus){
		poStatus->RemoveSelf();
		DELETE(poStatus);
	}
	
	if(poLastFocus){
		BMessage oMessage(BF_MSG_TEXTVIEWER_CLOSED);		
		BMessenger oMessenger(poLastFocus);
		oMessenger.SendMessage(&oMessage);
	}	
	RemoveSelf();	
	delete this;
}

void		
BF_GUI_TextViewer::KeyDown(const char *bytes, int32 numBytes)
{	
	if ( numBytes == 1 )
	switch ( bytes[0] ) {
		case B_RIGHT_ARROW:
			Nav_Right();
			return;		
		case B_LEFT_ARROW:
			Nav_Left();
			return;					
		case B_UP_ARROW:
			Nav_LineUp();
			return;	
		case B_DOWN_ARROW:
			Nav_LineDown();
			return;
		case B_PAGE_DOWN:
			Nav_PageDown();
			return;
		case B_PAGE_UP:
			Nav_PageUp();
			return;			
		case B_ESCAPE:
			CloseViewer();
			return;
		case B_HOME:{
			Nav_GoPos(0);
			return;}
		case B_END:
			Nav_End();
			return;
	}	
	if(poSysKeysMenu && poSysKeysMenu->OnKeyDown(bytes,numBytes)) return;	
	BView::KeyDown(bytes,numBytes);
}

void				
BF_GUI_TextViewer::Search_Step0()
{
	/* */
	BF_GUI_Func_PanelsEnable(false);
	/* make dialog */	
	BMessage oMessage(BF_MSG_TEXTVIEWER_SEARCH1);
	oMessage.AddPointer("bf_focus",this);
	oMessage.AddPointer("bf_oldfocus",poWin->CurrentFocus());
			
	BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(BRect(0,0,300,0),
		BF_DictAt(BF_DICT_TEXTVIEWER_SEARCH_DLG),"dialog",oMessage,BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER);
	BRect oRect;	
	/* insert edit */
	poDialog->LocalBounds(oRect);	
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	BF_GUI_ViewEdit_Create(oRect,BF_DictAt(BF_DICT_TEXTVIEWER_SEARCH_DLGTEXT),poDialog,"bf_cText",
					sSearchText.String(),B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
	/* menu */								
	poDialog->AddOkCancelMenu(oRect);	
	/* finish */	
	poDialog->SetHeightFromLastChild();
	poDialog->MoveToCenter( poWinView );	
	BF_GUI_Func_AddChildToMainView ( poDialog );	
}

void
BF_GUI_TextViewer::Search_Step1(BMessage & o_Message)
{
	BF_GUI_Func_PanelsEnable(true);
	MakeFocus(true);
	
	const char *pcText=NULL;
	if(B_OK!=o_Message.FindString("bf_cText",&pcText) || !pcText) return;
	
	iSearchPos = -1;
	sSearchText = pcText;
	
	Search_Run();
}

void
BF_GUI_TextViewer::Search_Run()
{
	BF_TextFile_Line *po=NULL;
	int32 iLine=0;
	
	if(iSearchPos<0) iSearchPos++;

	int32	iOldPos = iSearchPos;
	int32	iCycles = 0;
	int32	iPosInLine = -1;
	BL_List lo;
	
	while(true){	
		int32 iLinesLoaded = oBuffer.LoadText(iSearchPos,100,lo);
		
		for(iLine=0;iLine<iLinesLoaded;iLine++){
			po = (BF_TextFile_Line*)lo.ItemAt(iLine);
			iPosInLine = po->s.FindFirst(sSearchText.String());
			if(iPosInLine>=0) break;
		}		
		
		if(iPosInLine || (iLinesLoaded<iSearchPos))break;	
		
		po = (BF_TextFile_Line*)lo.LastItem();
		ASSERT(po);
		iSearchPos = po->Pos() + po->Bytes();			
	}
	
	if(iPosInLine>=0){
		loData.DeleteItems();
		loData.AddList(&lo);
		lo.MakeEmpty();
		
		po = (BF_TextFile_Line*)lo.FirstItem();
		ASSERT(po);
		iFilePos = po->Pos();
		
		iSearchLine = iLine;
		iSearchPosInLine = iPosInLine;
	}
	Invalidate(Bounds());
}

void
BF_GUI_TextViewer::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what){
	case 1599425348:
	case 1599425365:
		PrepareKeysMenu();
		break;
	// search text //
	case BF_MSG_TEXTVIEWER_SEARCH:{
		Search_Step0();
		break;}
	case BF_MSG_TEXTVIEWER_SEARCH1:{
		Search_Step1(*po_Message);
		break;}
	///
	case BF_MSG_TEXTVIEWER_SHOW_SPEC:{
		bShowSpec = !bShowSpec;
		poSysKeysMenu->SetText(9,BF_DictAt(bShowSpec?BF_DICT_TEXTVIEWER_SHOWSPEC:BF_DICT_TEXTVIEWER_HIDESPEC));
		Invalidate(Bounds());
		break;}
	case BF_MSG_TEXTVIEWER_CLOSED:
		CloseViewer();
		break;
	case BF_MSG_TEXTVIEWER_CH_CP:
		ChangeCodePage();
		break;
	default:
		BView::MessageReceived(po_Message);
	}
}

void
BF_GUI_TextViewer::ChangeCodePage()
{
	// set new code page //
	iCodePage = (iCodePage==(CP_END-1))?0:iCodePage+1;
	Draw(Bounds());
	// update status bar
	UpdateStatusBar();	
}

void 
BF_GUI_TextViewer::Draw(BRect o_Rect)
{	
	// create render //
	BRect 	oRect(Bounds());
	BView	*pv = this;
	
	BBitmap oB(oRect,B_RGBA32,TRUE);	
	oB.AddChild(pv = new BView(oRect,"",B_FOLLOW_ALL,B_WILL_DRAW));	
	oB.Lock();		
	
	// init render //
	pv->SetFont(&poSysSetup->oFontNode.oFont);	
	pv->SetViewColor(B_TRANSPARENT_COLOR);	
									
	// draw background //
	pv->SetHighColor(SYS_COLOR(BF_COLOR_TEXTVIEWER_BACK));
	oRect = pv->Bounds();	
	pv->FillRect(oRect);
	//
	BL_String s;
	//
	pv->SetHighColor(SYS_COLOR(BF_COLOR_TEXTVIEWER_TEXT)); 
	pv->SetLowColor(SYS_COLOR(BF_COLOR_TEXTVIEWER_BACK));
	//
	for(int iLine=0;iLine<loData.CountItems();iLine++){
		DrawLine(pv,iLine,false,false,s);
	}
	
	/* close render */	
	pv->Sync();
	oB.Unlock();
	DrawBitmap(&oB,oRect);
}

void
BF_GUI_TextViewer::DrawLine(BView *po_Render,int i_Line,bool b_InitRender,bool b_DrawBackground,BL_String & s)
{
	ASSERT(po_Render);
	BPoint 		oPoint(5 - iStartColumn*10,i_Line * fFontHeight + poSysSetup->oFontNode.fAscent);
	BF_TextFile_Line 	*po = (BF_TextFile_Line*)loData.ItemAt(i_Line);

	if(!po) return;	

	if(b_DrawBackground){
		BRect oRect(0,i_Line*fFontHeight,Bounds().Width(),i_Line*fFontHeight+fFontHeight);
		po_Render->SetHighColor(SYS_COLOR(BF_COLOR_TEXTVIEWER_BACK));		
		po_Render->FillRect(oRect);
	}

	if(b_InitRender){
		po_Render->SetHighColor(SYS_COLOR(BF_COLOR_TEXTVIEWER_TEXT)); 
		po_Render->SetLowColor(SYS_COLOR(BF_COLOR_TEXTVIEWER_BACK));
	}
	
	const char *pcBuffer=pcConvertBuffer;
	
	if((po->s.Length()+1)<=0x1000){
		ToUtf8(po->String(),pcConvertBuffer);		
	}else{		
		pcBuffer = po->String();
	}		
	
	s=pcBuffer;	
	if(!bShowSpec){
		s.ReplaceAll("\r","\x0");
		s.ReplaceAll("\n","\x0");
		s.ReplaceAll("\t","        ");
	}
	
	po_Render->DrawString(s.String(),oPoint);
	
	if((iSearchPos>=0) && (i_Line == iSearchLine)){
		po_Render->DrawString("!!!",oPoint);
	}
}

void
BF_GUI_TextViewer::MouseDown(BPoint o_point)
{

}
void 			
BF_GUI_TextViewer::MakeFocus(bool b_Focused)
{
	BView::MakeFocus(b_Focused);
	PrepareKeysMenu();
}

void
BF_GUI_TextViewer::PrepareKeysMenu()
{
	int32 iKeysModifiers = modifiers();
	
	poSysKeysMenu->Clear();
	if (iKeysModifiers & B_LEFT_SHIFT_KEY || iKeysModifiers & B_RIGHT_SHIFT_KEY)
	{
		poSysKeysMenu->SetText(7,BF_DictAt(BF_DICT_TEXTVIEWER_NEXT));		
		poSysKeysMenu->SetMessage(7,new BMessage(BF_MSG_TEXTVIEWER_SEARCH_NEXT),this);
	}
	else
	{
		poSysKeysMenu->SetText(3,BF_DictAt(BF_DICT_TEXTVIEWER_CLOSE));
		poSysKeysMenu->SetText(7,BF_DictAt(BF_DICT_SEARCH));
		poSysKeysMenu->SetText(8,BF_DictAt(BF_DICT_TEXTVIEWER_CODEPAGE));
		poSysKeysMenu->SetText(9,BF_DictAt(BF_DICT_TEXTVIEWER_SHOWSPEC));
		poSysKeysMenu->SetText(10,BF_DictAt(BF_DICT_TEXTVIEWER_CLOSE));

		poSysKeysMenu->SetMessage(3,new BMessage(BF_MSG_TEXTVIEWER_CLOSED),this);
		poSysKeysMenu->SetMessage(7,new BMessage(BF_MSG_TEXTVIEWER_SEARCH),this);
		poSysKeysMenu->SetMessage(8,new BMessage(BF_MSG_TEXTVIEWER_CH_CP),this);
		poSysKeysMenu->SetMessage(9,new BMessage(BF_MSG_TEXTVIEWER_SHOW_SPEC),this);
		poSysKeysMenu->SetMessage(10,new BMessage(BF_MSG_TEXTVIEWER_CLOSED),this);
	};
}

void			
BF_GUI_TextViewer::LoadPage()
{	
	int32 iLinesLoaded = oBuffer.LoadText(iFilePos,iLinesCount,loData);
	iFileSize = oBuffer.FileSize();
	if(Parent())	Draw(Bounds());
}

bool				
BF_GUI_TextViewer::Nav_PageUp()
{
	if(iFilePos<=0) return false;	
	
	BF_TextFile_Line *po = (BF_TextFile_Line*)loData.FirstItem();
	if(!po) return false;
	
	iFilePos = po->Pos();
	int32 iLoaded = oBuffer.LoadText_BeforePos(iFilePos,iLinesCount,loData);
	iFileSize = oBuffer.FileSize();
	
	if(iLoaded<iLinesCount){
		oBuffer.LoadText_FromNextLine(iFilePos,iLinesCount - iLoaded,loData,false);
	}
	iFileSize = oBuffer.FileSize();
	
	

	// redraw view
	Draw(Bounds());
	// update status_bar	
	UpdateStatusBar();	
	
	/*
	// move up
	off_t 		iNewFilePos = iFilePos;
	BL_String	*ps=NULL,s;
			
	// open file
	BL_File oFile;
	if(B_OK!=OpenFile(oFile)) return false;
	
	// load lines
	int iLine=0;
	while(iNewFilePos>0 && iLine<iLinesCount){	
		// set new pos
		iNewFilePos-=2;
		if(iNewFilePos!=oFile.Seek(iNewFilePos,SEEK_SET)) return false;
		// calc new pos //
		if(B_OK!=oFile.FindPrevEOL(iNewFilePos)) return false;
		if(iNewFilePos!=0) iNewFilePos++;
		// set new pos
		if(iNewFilePos!=oFile.Seek(iNewFilePos,SEEK_SET)) return false;
		// load new line //
		if(B_OK!=oFile.ReadString(&s)) return false;
		// set new vars //	
		if(iNewFilePos>0){
			iFilePos-=s.Length()+1;
			iLine++;
		}else{
			iNewFilePos = 0;
			iFilePos = 0;
		}
		// delete bottom line		
		lsData.DeleteItemAt(lsData.CountItems()-1);
		// add new line at top
		ps = new BL_String(s.String());
		lsData.AddItem(ps,0);	
	}
	
	// redraw view
	Draw(Bounds());
	// update status_bar
	UpdateStatusBar();
	*/
	return true;
}

bool
BF_GUI_TextViewer::Nav_LineUp()
{
	if(iFilePos<=0) return false;
	
	BL_List loUpLine;
	if(0==oBuffer.LoadText_BeforePos(iFilePos,1,loUpLine)) return false;
	
	loData.AddList(&loUpLine,0);
	loUpLine.MakeEmpty();

	if(loData.CountItems()>iLinesCount){
		loData.DeleteItemAt(loData.CountItems()-1);
	}
	
		
	// redraw view
	{
		BL_String s;
		
		// move image down
		BRect oRectFull(Bounds());
		BRect oRectNew(0,fFontHeight,oRectFull.right,loData.CountItems()*fFontHeight),oRectOld(oRectNew);
		oRectOld.OffsetBy(0,0-fFontHeight);
		CopyBits(oRectOld,oRectNew);
		// draw last line
		DrawLine(this,0,true,true,s);
		
	}
	// update status_bar
	UpdateStatusBar();
	return true;
}

status_t
BF_GUI_TextViewer::OpenFile(BL_File & o_File)
{
	status_t uRes=B_ERROR;
	o_File.SetTo(sFileName.String(),B_READ_ONLY);	
	uRes = o_File.InitCheck();	
	return uRes;
}

bool
BF_GUI_TextViewer::Nav_LineDown()
{
	if(1!=oBuffer.LoadText_FromNextLine(iFilePos,1,loData)) return false;
	iFileSize = oBuffer.FileSize();
	
	BF_TextFile_Line *po = (BF_TextFile_Line*)loData.LastItem();
	if(!po) return false;

	// redraw view
	BL_String s;
	{
		// move image up
		BRect oRectFull(Bounds());
		BRect oRectOld(0,fFontHeight,oRectFull.right,loData.CountItems()*fFontHeight),oRectNew(oRectOld);
		oRectNew.OffsetBy(0,0-fFontHeight);
		CopyBits(oRectOld,oRectNew);
		// draw last line
		DrawLine(this,loData.CountItems()-1,true,true,s);		
	}	
	// update status_bar
	UpdateStatusBar();
	return true;
}

bool				
BF_GUI_TextViewer::Nav_PageDown()
{
	if(loData.CountItems()<iLinesCount) return false;
	
	BF_TextFile_Line *po = (BF_TextFile_Line*)loData.LastItem();
	if(!po) return false;
	
	iFilePos = po->Pos();
	oBuffer.LoadText(iFilePos,iLinesCount,loData);
	iFileSize = oBuffer.FileSize();

	// redraw view
	Draw(Bounds());
	// update status_bar	
	UpdateStatusBar();
	
	/*
	if(lsData.CountItems()<iLinesCount) return false;
	// move down
	off_t 		iNewFilePos = iFilePos;
	BL_String	*ps=NULL,s;
	
	// calc last_pos //
	for(int i=0;i<lsData.CountItems();i++){
		ps=(BL_String*)lsData.ItemAt(i);
		if(!ps) continue;
		iNewFilePos+=ps->Length()+1;
	}
	// open file
	BL_File oFile;
	if(B_OK!=OpenFile(oFile)) return false;
	// set new pos
	if(iNewFilePos>oFile.Seek(iNewFilePos,SEEK_SET)) return false;
	// load new lines
	int32 iLine=0;
	while(iLine<iLinesCount){
		// load line
		if(B_OK!=oFile.ReadString(&s)) break;
		// set new vars
		ps=(BL_String*)lsData.ItemAt(0);
		iFilePos+=ps->Length()+1;
		iLine++;
		// delete first line		
		lsData.DeleteItemAt(0);
		// add new line in the end of list
		ps = new BL_String(s.String());
		lsData.AddItem(ps);	
	}
	// redraw view
	Draw(Bounds());
	// update status_bar	
	UpdateStatusBar();*/
	return true;
}

bool				
BF_GUI_TextViewer::Nav_End()
{	
	if(loData.CountItems()<iLinesCount) return true;
	
	iFilePos = iFileSize-1;
	if(0==oBuffer.LoadText_BeforePos(iFilePos,iLinesCount,loData)) return false;
	
	// redraw view
	Draw(Bounds());
	// update status_bar
	UpdateStatusBar();
	
	return true;

/*
	// move up
	off_t 		iNewFilePos = iFileSize;
	BL_String	*ps=NULL,s;
			
	// open file
	BL_File oFile;
	if(B_OK!=OpenFile(oFile)) return false;
	// get new file_size
	if(B_OK!=oFile.GetSize(&iFileSize)) return false;
	iNewFilePos = iFileSize;
	
	lsData.DeleteItems();
	
	// load lines
	int iLine=0;
	while(iNewFilePos>0 && iLine<iLinesCount){	
		// set new pos
		iNewFilePos-=2;
		if(iNewFilePos!=oFile.Seek(iNewFilePos,SEEK_SET)) return false;
		// calc new pos //
		if(B_OK!=oFile.FindPrevEOL(iNewFilePos)) return false;
		if(iNewFilePos!=0) iNewFilePos++;
		// set new pos
		if(iNewFilePos!=oFile.Seek(iNewFilePos,SEEK_SET)) return false;
		// load new line //
		if(B_OK!=oFile.ReadString(&s)) break;
		// set new vars //	
		if(iNewFilePos>0){
			iFilePos = iNewFilePos;
			iLine++;
		}else{
			iNewFilePos = 0;
			iFilePos = 0;
		}		
		// add new line at top
		ps = new BL_String(s.String());
		lsData.AddItem(ps,0);	
	}
	
	// redraw view
	Draw(Bounds());
	// update status_bar
	UpdateStatusBar();
	return true;
*/
}

void
BF_GUI_TextViewer::Nav_GoPos(off_t i_NewFilePos)
{
	if(i_NewFilePos==iFilePos) return;
	if(i_NewFilePos>iFileSize){
		ASSERT(false);
	}
	//
	iFilePos = i_NewFilePos;
	LoadPage();	
	// update status	
	UpdateStatusBar();
}

void
BF_GUI_TextViewer::UpdateStatusBar()
{
	// calc page_size //
	off_t iPageSize=0;
	{
		BF_TextFile_Line *po=NULL;
		for(int i=0;i<loData.CountItems();i++){
			po=(BF_TextFile_Line*)loData.ItemAt(i);
			if(!po) continue;
			iPageSize+=po->s.Length()+1;
		}
	}
	// update statusbar //
	BView *poParent = Parent();
	if(!poParent) poParent = poWinView;
	if(!poParent) return;
	
	BF_GUI_TextViewer_StatusBar *poStatus = (BF_GUI_TextViewer_StatusBar*)poParent->FindView("textviewer_status");
	if(poStatus)	poStatus->SetVarsTo(iFilePos,iPageSize,iFileSize,iCodePage,iStartColumn);
}


void
BF_GUI_TextViewer::ToUtf8(const char *pc_Source,char *pc_Dest)
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
BF_GUI_TextViewer::FromUtf8(const char *pc_Source,char *pc_Dest)
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
BF_GUI_TextViewer::GetConversion()
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

bool
BF_GUI_TextViewer::Nav_Right()
{
	iStartColumn++;

	Draw(Bounds());
	UpdateStatusBar();	

	return true;	
}

bool
BF_GUI_TextViewer::Nav_Left()
{
	if(0==iStartColumn) return false;
	iStartColumn--;		

	Draw(Bounds());
	UpdateStatusBar();	
	
	return true;
}

////////////////////////////////////////////////////////////////////////////////

void  
BF_GUI_TextViewer_Run(const char *pc_File)
{
	BF_GUI_Func_PanelsShow(false);

	// make status_bar 	
	BRect oRect(poWinView->Bounds());
	oRect.bottom = poSysSetup->oFontNode.fHeight;
	BF_GUI_TextViewer_StatusBar *poStatus = new BF_GUI_TextViewer_StatusBar(oRect,pc_File);
	BF_GUI_Func_AddChildToMainView ( poStatus );	
	// make viewer_view
	oRect = poWinView->Bounds();
	oRect.top += poSysSetup->oFontNode.fHeight+1;
	oRect.bottom-=poSysKeysMenu->Bounds().Height()+1;

	BF_GUI_TextViewer *po = new BF_GUI_TextViewer(oRect,pc_File);
	BF_GUI_Func_AddChildToMainView ( po );
	po->MakeFocus();
}