#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Clipboard.h>
#include <Locker.h>
//#include <time.h>
#include <NodeMonitor.h>
//#include <InputServerDevice.h>

#include "BF_Dict.h"
#include "BF_Roster.h"
#include "BF_Msg.h"
#include "BF_GUI_NodePanel.h"
#include "BF_GUI_NodePanel_Tools.h"
#include "BF_GUI_DlgViews.h"
#include "BF_GUI_KeysMenu.h"
#include "BF_GUI_CmdLine.h"
#include "BF_GUI_WinMain.h"

#define INFO_SIZE	30 

BF_GUI_NodePanel::BF_GUI_NodePanel(
	const BRect & o_Rect,
	bool b_WinPos_OnLeft)
:BF_GUI_Panel(o_Rect,"file",b_WinPos_OnLeft) 
{
	iPulseActions = BF_NODEPANEL_PULSE_NONE;
	/**/
	fFullName = 0;
	fFullSize = 0;
	fFullDate = 0;
	iNodesOnScreen = 0;
	iMode = BF_NODEPANEL_MODE_FULL;
	iModeFullStyles = BF_NODEPANEL_FULL_SIZE 
		| BF_NODEPANEL_FULL_SICON 
		| BF_NODEPANEL_FULL_NAME 
		| BF_NODEPANEL_FULL_DATE
		//| BF_NODEPANEL_FULL_TYPE
		;
		
	fNodeInfoHeight = 0;
	fNodeInfoHeight = 0;
	idThreadPulse = 0;
	/**/
	//if(BF_PANEL_WINPOS_LEFT!=i_WinPosition){
	{
		iMode = BF_NODEPANEL_MODE_COLS;	
		iModeColsStyles = BF_NODEPANEL_COLS_NAME;
		iModeColsStyles = iModeColsStyles | BF_NODEPANEL_COLS_SICON;
		iColsColCount = 3;
	}
	/**/
	iSortType = BF_NODELIST_SORT_NAME;//BF_NODELIST_SORT_EXT;
	iNavFirstIndex = 0;
	iNavCursorIndex = 0;
	sNodeSelMask = "*.*";
	bSelectDirAlso = false;
	bCanSelect = true;
	/**/
	{
		BF_Node *poNode = new BF_Node(" none ");
		poNode->iType = BF_NODE_TYPE_UNKNOWN;
		loNode.AddItem(poNode);
	}
	//
	sDictDir = BF_DictAt(BF_DICT_PANEL_DIR);
}

BF_GUI_NodePanel::~BF_GUI_NodePanel()
{
	if(idThreadPulse) while(B_OK!=kill_thread(idThreadPulse));
}


void					
BF_GUI_NodePanel::FrameResized(float width, float height)
{
	Calc();
	BF_GUI_Panel::FrameResized(width,height);
}


BRect
BF_GUI_NodePanel::ClientRect()
{
	BRect oRect(Bounds());
	oRect.left = 6;
	oRect.right -= 6;	
	oRect.top = HeaderHeight() + fColTitleHeight;
	oRect.bottom = oRect.top + fNodesHeight;
	
	return oRect;
}

void					
BF_GUI_NodePanel::Calc()
{
	SetFont(&poSysSetup->oFontNode.oFont,B_FONT_ALL);
	//
	fNodeHeight = (float)(int) ( poSysSetup->oFontNode.fHeight );	
	if(	ModeShowSIcon() && fNodeHeight<17) fNodeHeight = 18; 
	fNodeInfoHeight = poSysSetup->oFontNode.fHeight;
	fSelInfoHeight = poSysSetup->oFontNode.fHeight;
	fColTitleHeight = poSysSetup->oFontNode.fHeight+4;
	//
	BRect oRect(Bounds());
	/* reserve place for path title */
	oRect.top += HeaderHeight();		
	oRect.top += fColTitleHeight;
	/* reserve place for bottom line,info line,selected line */
	oRect.bottom -= BottomHeight();
	oRect.bottom -= fNodeInfoHeight;
	oRect.bottom -= fSelInfoHeight;
	fWidth = oRect.Width();
	/* reserve place for vertical lines */
	fWidth -= 6;
	/* calc total count of items */
	iNodesOnScreen = (int)( oRect.Height()/fNodeHeight );
	fNodesHeight = iNodesOnScreen * fNodeHeight;
	/* calc */
	switch(iMode){
	case BF_NODEPANEL_MODE_FULL:{
		fFullDate = 0;
		fFullSize = 0;
		fFullType = 0;	
		fFullSIcon = 0;	
		if(iModeFullStyles & BF_NODEPANEL_FULL_SICON) fFullSIcon=20;
		if(iModeFullStyles & BF_NODEPANEL_FULL_DATE)
			fFullDate= (float) (int ) ( poSysSetup->oFontNode.oFont.StringWidth("99-99-9999 99:99")+5 );
		if(iModeFullStyles & BF_NODEPANEL_FULL_SIZE)
			fFullSize=(float) (int ) ( poSysSetup->oFontNode.oFont.StringWidth("99999999999")+5 );
		fFullName = (float) (int ) ( fWidth -  fFullDate - fFullSize- fFullSIcon );
		if(iModeFullStyles & BF_NODEPANEL_FULL_TYPE){
			fFullType=(float) (int ) fFullName / 3;
			fFullName -= fFullType;
		}
		break;}
	case BF_NODEPANEL_MODE_COLS:{	
		fColsSIcon = 0;
		fColsColWidth = (float)(int) ( fWidth/iColsColCount );
		if(iModeColsStyles & BF_NODEPANEL_COLS_SICON)	fColsSIcon=20;
		fColsName = (float)(int) (fColsColWidth - fColsSIcon-6);
		
		iColsNodesInCol = iNodesOnScreen;
		iNodesOnScreen = iColsNodesInCol*iColsColCount;
		
		break;}
	}		
}

void					
BF_GUI_NodePanel::OnEnable(bool b_Enable)
{
	if(b_Enable){
		if(bNeedUpdateAfterEnable){
			if(!(poSysSetup->MainStyle() & BF_SETUP_DEB_NOSORTING)){
				loNode.Sort(iSortType);
			}
		}
	}else{
		PulseAction_Remove(BF_NODEPANEL_PULSE_ALL);
	}
	BF_GUI_Panel::OnEnable(b_Enable);
}

void
BF_GUI_NodePanel::DrawStringRight(const char *pc_Text,BPoint &o_Point,float f_Width)
{
	float f = poSysSetup->oFontNode.oFont.StringWidth(pc_Text);
	poRender->MovePenTo(o_Point.x+f_Width-f,o_Point.y);
	poRender->DrawString(pc_Text);
}

void
BF_GUI_NodePanel::DrawStringCenter(const char *pc_Text,BPoint &o_Point,float f_Width)
{
	float f = poSysSetup->oFontNode.oFont.StringWidth(pc_Text);
	poRender->MovePenTo(o_Point.x+f_Width/2-f/2,o_Point.y);
	poRender->DrawString(pc_Text);
}


void					
BF_GUI_NodePanel::DrawPanel(BRect & o_Rect)
{
	BF_GUI_Panel::DrawPanel(o_Rect);
	// draw bottom_info 	
	DrawListInfo(false);
	// draw selected bottom_info
	DrawSelListInfo(false);	
	// draw verticals lines 
	DrawLines(false);	
	// draw headers 
	DrawColTitles();
	// draw fileinfo
	DrawNodeInfo();
	// draw items 
	DrawNodes();
}

void 
BF_GUI_NodePanel::DrawLines(bool b_OnlyNodes)
{
	//
	switch(iMode){
	case BF_NODEPANEL_MODE_COLS:{	
		if(poSysSetup->FPanelColsStyle() & BF_SETUP_FPANELMODE_COLS_DRAWLINES){
			BRect oRect = poRender->Bounds();
			oRect.top=HeaderHeight(); 			
			oRect.bottom -= BottomHeight();
			oRect.bottom -= fNodeInfoHeight;
			oRect.bottom -= fSelInfoHeight;
			oRect.left+=4;
			oRect.left--;
			poRender->SetHighColor(SYS_COLOR(BF_COLOR_FRAME));	
			for(uint32 i=0;i<iColsColCount-1;i++){
				oRect.left+=fColsColWidth;
				oRect.right = oRect.left;				
				poRender->StrokeRect(oRect);
			}
		}
		break;}
	case BF_NODEPANEL_MODE_FULL:{
		if(poSysSetup->FPanelFullStyle() & BF_SETUP_FPANELMODE_FULL_DRAWLINES){
			poRender->SetHighColor(SYS_COLOR(BF_COLOR_FRAME));	
			BRect oRect = poRender->Bounds();
			oRect.top=HeaderHeight(); 			
			oRect.bottom -= BottomHeight();
			oRect.bottom -= fNodeInfoHeight;
			oRect.bottom -= fSelInfoHeight;
			oRect.left+=4;
			//
			if(iModeFullStyles & BF_NODEPANEL_FULL_SICON)	oRect.left+=fFullSIcon;
			oRect.left+=fFullName;
			/* line |type */				
			if(iModeFullStyles & BF_NODEPANEL_FULL_TYPE){						
				oRect.right = oRect.left;
				poRender->StrokeRect(oRect);
				oRect.left+=fFullType;
			}			
			/* line |size */				
			if(iModeFullStyles & BF_NODEPANEL_FULL_SIZE){
				oRect.right = oRect.left;
				poRender->StrokeRect(oRect);
				oRect.left+=fFullSize;
			}						
			/* line |date */
			if(iModeFullStyles & BF_NODEPANEL_FULL_DATE){						
				oRect.right = oRect.left;
				poRender->StrokeRect(oRect);		
				oRect.left+=fFullDate;
			}
		}
		break;}
	};
}

void 
BF_GUI_NodePanel::DrawListInfo(bool b_DrawBack)
{
	/* make info */
	BL_String s;
	s<<loNode.SizeAll(false);	
	s.SetDigits();
	s<<BF_DictAt(BF_DICT_PANEL_BYTESIN);
	s<<loNode.CountRealNodes();
	s<<BF_DictAt(BF_DICT_PANEL_FILES);
	DrawBottom(s.String(), b_DrawBack);
}

void
BF_GUI_NodePanel::DrawNodeInfo()
{
	BRect oRect;
	/* erase background  */
	oRect = poRender->Bounds();
	oRect.left+=6;
	oRect.right-=6;
	oRect.bottom -= BottomHeight();
	oRect.top = oRect.bottom - fNodeInfoHeight-1;
	poRender->SetHighColor(SYS_COLOR(BF_COLOR_BACK));
	poRender->FillRect(oRect);		
	
	BF_Node 	*poNode = loNode.NodeAt(iNavCursorIndex);		
		
	/* draw icon */
	if(poNode && poNode->poSIcon){
		BPoint oPoint(6,oRect.top);
		poRender->SetDrawingMode(B_OP_ALPHA);				
		poRender->DrawBitmap(poNode->poSIcon,oPoint);
		poRender->SetDrawingMode(B_OP_COPY);
	}
	/* draw text */	
	BPoint 		o;	
	BL_String 	s;	
	
	o.Set(6+25/*25 for icon */,oRect.bottom-poSysSetup->oFontNode.fDescent);
	poRender->SetHighColor(SYS_COLOR(BF_COLOR_PANEL_INFO_TEXT));	
	poRender->SetLowColor(SYS_COLOR(BF_COLOR_BACK));
	
	/* draw size */	
	if(poNode){
		switch(poNode->iType){
		case BF_NODE_TYPE_PARENTDIR:
			poRender->DrawString("..",o);	
			break;
		case BF_NODE_TYPE_DIR:
			//s=poNode->sName;
			//CheckStringWidth(s,oRect.Width()-12,NULL);
			DrawStringCheck(poNode->sName.String(),oRect.Width()-12,o);	
			break;
		case BF_NODE_TYPE_FILE:
			poRender->DrawString("Size : ",o);
			s = poNode->iSize;
			s.SetDigits();
			DrawStringRight(s.String(),o,oRect.Width()-12-25/*25 for icon */);
			break;
		}
	}
		
}

void
BF_GUI_NodePanel::DrawSelListInfo(bool b_DrawBack)
{
	BRect oRect(poRender->Bounds());
	
	oRect.left+=4;
	oRect.right-=4;		
	oRect.bottom -= BottomHeight() + fNodeInfoHeight;
	oRect.top = oRect.bottom - fSelInfoHeight;	
	/* erase background  */
	if(b_DrawBack){						
		poRender->SetHighColor(SYS_COLOR(BF_COLOR_BACK));
		poRender->FillRect(oRect);
	}
	/* make info */
	//oRect = poRender->Bounds();
	BL_String 	s;
	uint64		iBytes,iCount;		
	float 		fTop = oRect.top + fSelInfoHeight/2;
	BPoint 		o1,o2;
	/* get info */
	/* draw */
	loNode.SelectedInfo(iBytes,iCount);
	if(iCount){
		s<<iBytes;	
		s.SetDigits();
		s<<BF_DictAt(BF_DICT_PANEL_BYTESIN);
		s<<iCount;
		s<<BF_DictAt(BF_DICT_PANEL_FILES);
	    float fWidth = poSysSetup->oFontNode.oFont.StringWidth(s.String());		
		float fLeft = (oRect.Width() - fWidth)/2;
		/* draw lines */		
		/* left line */
		o1.Set(4,fTop);
		o2.Set(fLeft-3,o1.y);
		poRender->SetHighColor(SYS_COLOR(BF_COLOR_FRAME));
		poRender->StrokeLine(o1,o2);	
		/* right line */
		o1.Set(fLeft+fWidth+3,fTop);
		o2.Set(oRect.right-3,o1.y);	
		poRender->SetHighColor(SYS_COLOR(BF_COLOR_FRAME));
		poRender->StrokeLine(o1,o2);	
		/* draw info */
		o1.Set(fLeft,oRect.top + poSysSetup->oFontNode.fAscent);
		poRender->SetHighColor(SYS_COLOR(BF_COLOR_NODE_FOCUSSELECT));
		poRender->SetLowColor(SYS_COLOR(BF_COLOR_BACK));
		poRender->DrawString(s.String(),o1);
	}else{		
		/* full lines */
		o1.Set(4,fTop);
		o2.Set(oRect.right-3,o1.y);
		poRender->SetHighColor(SYS_COLOR(BF_COLOR_FRAME));
		poRender->StrokeLine(o1,o2);	
	}
}

void 
BF_GUI_NodePanel::DrawColTitles()
{	
	switch(iMode){
	case BF_NODEPANEL_MODE_COLS:{	
		BPoint		oPoint(7,HeaderHeight());
		oPoint.y += poSysSetup->oFontNode.fAscent;
		/* print headers */
		/* set colors */
		poRender->SetHighColor(SYS_COLOR(BF_COLOR_HEADER_TEXT));	
		poRender->SetLowColor(SYS_COLOR(BF_COLOR_BACK));			
		oPoint.x = 5 ;//+ fColsColWidth/2;
		//if(iModeColsStyles & BF_NODEPANEL_COLS_SICON) 	oPoint.x+=fColsSIcon;
		for(uint i=0;i<iColsColCount;i++){
			/* header 'SmallIcon' */			
			/* header 'Name' */
			DrawStringCenter(BF_DictAt(BF_DICT_PANEL_COL_NAME),oPoint,fColsColWidth);			
			oPoint.x+=fColsColWidth;
		}
		break;}
	case BF_NODEPANEL_MODE_FULL:{
		BPoint		oPoint(4,HeaderHeight());		
		/* start  */		
		oPoint.y += poSysSetup->oFontNode.fAscent;
		/*---------------*/		
		/* print headers */
		poRender->SetHighColor(SYS_COLOR(BF_COLOR_HEADER_TEXT));	
		poRender->SetLowColor(SYS_COLOR(BF_COLOR_BACK));	
		oPoint.x = 4;
		/* header 'SmallIcon' */
		if(iModeFullStyles & BF_NODEPANEL_FULL_SICON){	
			//DrawStringCenter("Icon",oPoint,fFullSIcon);			
			oPoint.x+=fFullSIcon;
		}
		/* header 'Name' */
		DrawStringCenter(BF_DictAt(BF_DICT_PANEL_COL_NAME),oPoint,fFullName);			
		oPoint.x+=fFullName;
		/* header 'type' */
		if(iModeFullStyles & BF_NODEPANEL_FULL_TYPE){	
			DrawStringCenter(BF_DictAt(BF_DICT_PANEL_COL_TYPE),oPoint,fFullType);			
			oPoint.x+=fFullType;			
		}		
		/* header 'Size' */
		if(iModeFullStyles & BF_NODEPANEL_FULL_SIZE){	
			DrawStringCenter(BF_DictAt(BF_DICT_PANEL_COL_SIZE),oPoint,fFullSize);			
			oPoint.x+=fFullSize;			
		}	
		/* header 'Date' */
		if(iModeFullStyles & BF_NODEPANEL_FULL_DATE){
			DrawStringCenter(BF_DictAt(BF_DICT_PANEL_COL_DATE),oPoint,fFullDate);
			oPoint.x+=fFullDate;
		}
		/**/	
		break;}
	}
}

void 
BF_GUI_NodePanel::DrawNodes(bool b_DrawBackground)
{
	BRect oRect(ClientRect());
	
	// open transaction 
	Draw_OpenTransaction();		
	
	// check background
	if(RenderDeep()==1)	b_DrawBackground = true;
		
	if(b_DrawBackground)
	{	
		DrawBackground(oRect);					
		DrawLines( true );
	}	
	/* draw  nodes */
	if(true)
	{
		BPoint		oPoint(7,HeaderHeight());	
		BRect		oRect;
		bool		bShowFocus = IsFocus() || bFixedFocus;			
		char		pc[500];
		int32		iRealIndex=0;
		int			iLastNode=0;
		
		
		iLastNode = iNavFirstIndex + iNodesOnScreen;
				
		if(iLastNode>loNode.CountItems()) iLastNode = loNode.CountItems();		
		/* scan nodes */			
		for(iRealIndex=iNavFirstIndex;iRealIndex<iLastNode;iRealIndex++)
		{		
			DrawNode(iRealIndex,bShowFocus && (iRealIndex==iNavCursorIndex),oPoint,oRect,pc,NULL,false);
		}
		iRealIndex  = iRealIndex;
	}	
	if(b_DrawBackground){
	}
	// close trans 
	Draw_CloseTransaction(&oRect); 
}

void
BF_GUI_NodePanel::DrawNodeAt(int32 i_Index)
{
	if(i_Index<0) return;
	///
	BRect 	oRect,oRect2;
	BPoint	oPoint;
	char	pc[500];
	bool	bFocused=i_Index==iNavCursorIndex && (IsFocus() || bFixedFocus);
	
	DrawNode(i_Index,bFocused,oPoint,oRect,pc,&oRect2,true);
}

void 
BF_GUI_NodePanel::DrawNode(
	int32 i_NodeIndex,
	bool b_ItCursor,
	BPoint &oPoint,
	BRect & oRect,
	char *pc_Buffer,
	BRect *po_RectBound,
	bool b_DrawBackground,
	bool b_UsePoint)
{	
	/* draw in full_mode */
	switch(iMode){
	case BF_NODEPANEL_MODE_COLS:{
		BF_Node *poNode = (BF_Node*)loNode.NodeAt(i_NodeIndex);
		if(!poNode) return;
		
		uint32 iColIndex = (i_NodeIndex - iNavFirstIndex)/iColsNodesInCol;
		i_NodeIndex -= iColIndex*iColsNodesInCol;
				
		if(!b_UsePoint){
			oPoint.x = 4;
			oPoint.x += iColIndex * fColsColWidth;
			oPoint.y = HeaderHeight() + fColTitleHeight + poSysSetup->oFontNode.fAscent;
			oPoint.y += fNodeHeight * (i_NodeIndex - iNavFirstIndex);		
		}
		
		if(po_RectBound){
			po_RectBound->left = oPoint.x;
			po_RectBound->right  = po_RectBound->left + fColsColWidth-12;
			po_RectBound->top = oPoint.y - poSysSetup->oFontNode.fAscent;
			po_RectBound->bottom = po_RectBound->top + fNodeHeight;
		}		
		/* set color */	
		DrawNode_SetNodeColor(poNode,false);		
		/* draw node */					
		{		
			oPoint.x+=2;
			if(b_ItCursor)			DrawNode_CursorBack(poNode,oRect,oPoint,fColsColWidth-6,true);else 
			if(b_DrawBackground) 	DrawNode_CursorBack(poNode,oRect,oPoint,fColsColWidth-6,false);
			oPoint.x+=2;
			/* print small_icon */
			if(iModeColsStyles & BF_NODEPANEL_COLS_SICON){							
				if(poNode->poSIcon){
					poRender->SetDrawingMode(B_OP_ALPHA);		
					oPoint.y-=poSysSetup->oFontNode.fAscent;
					poRender->DrawBitmap(poNode->poSIcon,oPoint);
					oPoint.y+=poSysSetup->oFontNode.fAscent;
					poRender->SetDrawingMode(B_OP_COPY);
				}
				oPoint.x+=fColsSIcon;
			}
			/* print node_name */
			DrawStringCheck(poNode->sName.String(),fColsName-3,oPoint);
			
			/* for link print dot */
			if(false && poNode->bLink){
				BPoint o1(oPoint);
				o1.y += poSysSetup->oFontNode.fDescent-1;
				float fx1 = oPoint.x + poSysSetup->oFontNode.oFont.StringWidth(poNode->sName.String());
				for(float fx = oPoint.x;fx<fx1;fx+=2.0){			
					StrokeLine(o1,o1);
					o1.x+=2.0;
				}
			}

			oPoint.x+=fColsColWidth;
		}		
		break;}
	case BF_NODEPANEL_MODE_FULL:{
		oPoint.x = 4;
		oPoint.y = HeaderHeight() + fColTitleHeight + poSysSetup->oFontNode.fAscent;
		oPoint.y += fNodeHeight * (i_NodeIndex - iNavFirstIndex);
		if(po_RectBound){
			po_RectBound->left = oPoint.x;
			po_RectBound->right  = po_RectBound->left + fWidth-12;
			po_RectBound->top = oPoint.y - poSysSetup->oFontNode.fAscent;
			po_RectBound->bottom = po_RectBound->top + fNodeHeight;
		}
		BF_Node *poNode = (BF_Node*)loNode.NodeAt(i_NodeIndex);
		if(!poNode) return;
		/* set color */	
		DrawNode_SetNodeColor(poNode,false);
		/* draw node */					
		{		
			oPoint.x+=2;
			/* print small_icon */						
			if(b_ItCursor)			DrawNode_CursorBack(poNode,oRect,oPoint,fFullSIcon + fFullName-5,true);else 
			if(b_DrawBackground) 	DrawNode_CursorBack(poNode,oRect,oPoint,fFullSIcon + fFullName-5,false);			
			oPoint.x+=2;

			/* for link print dot */
			if(poNode->bLink){
				oPoint.x-=1;
				poRender->DrawString(".",oPoint);
				oPoint.x+=1;
			}						

		
			if(iModeFullStyles & BF_NODEPANEL_FULL_SICON){				
				if(poNode->poSIcon){
					poRender->SetDrawingMode(B_OP_ALPHA);		
					oPoint.y-=poSysSetup->oFontNode.fAscent;
					poRender->DrawBitmap(poNode->poSIcon,oPoint);
					oPoint.y+=poSysSetup->oFontNode.fAscent;
					poRender->SetDrawingMode(B_OP_COPY);
				}
				oPoint.x+=fFullSIcon;
			}
			/* print node_name */
			DrawStringCheck(poNode->sName.String(),fFullName-8,oPoint);

			oPoint.x+=fFullName-4;
			/* print node_type */
			if(iModeFullStyles & BF_NODEPANEL_FULL_TYPE){
				oPoint.x+=2;
				
				if(b_ItCursor)			DrawNode_CursorBack(poNode,oRect,oPoint,fFullType-5,true);else
				if(b_DrawBackground) 	DrawNode_CursorBack(poNode,oRect,oPoint,fFullType-5,false);
				
				DrawStringCheck(poNode->sType.String(),fFullType-5,oPoint);
				/*
				if(CheckStringWidth(poNode->sType,fFullType-5,pc_Buffer)){
					poRender->DrawString(poNode->sType.String(),oPoint);
				}else{
					poRender->DrawString(pc_Buffer,oPoint);
				}*/
				//DrawString(poNode->sType.String(),oPoint);
				oPoint.x+=fFullType-2;
			}				
			/* print node_size */
			if(iModeFullStyles & BF_NODEPANEL_FULL_SIZE){
				oPoint.x+=2;
				if(b_ItCursor)			DrawNode_CursorBack(poNode,oRect,oPoint,fFullSize-5,true);else
				if(b_DrawBackground) 	DrawNode_CursorBack(poNode,oRect,oPoint,fFullSize-5,false);
				switch(poNode->iType){
				case BF_NODE_TYPE_FILE:{
					// this is not debug print !!! //
					sprintf(pc_Buffer,"%i",(int)poNode->iSize);	
					DrawStringRight(pc_Buffer,oPoint,fFullSize-5);
					break;}
				case BF_NODE_TYPE_DIR:{
					DrawStringRight(sDictDir.String(),oPoint,fFullSize-5);
					break;}
				};
				//if(poNode->bLink) poRender->DrawString(".",oPoint);
				oPoint.x+=fFullSize-2;
			}
			/* print node_date */
			if(iModeFullStyles & BF_NODEPANEL_FULL_DATE){
				oPoint.x+=2;
				if(b_ItCursor)			DrawNode_CursorBack(poNode,oRect,oPoint,fFullDate-5,true);else
				if(b_DrawBackground) 	DrawNode_CursorBack(poNode,oRect,oPoint,fFullDate-5,false);
				if(poNode->iType!=BF_NODE_TYPE_PARENTDIR){
					strcpy(pc_Buffer,"");
					if(poNode->iType!=BF_NODE_TYPE_PARENTDIR){
						//printf("name %s time %i\n",poNode->sName.String(),poNode->uCreateTime);												
						tm* puTime = localtime(&poNode->uCreateTime);
						char pc1[10];
						ASSERT(puTime);												
						strcpy(pc_Buffer,"");					
						//
						sprintf(pc1,"%i",puTime->tm_mday);
						if(puTime->tm_mday<10) strcat(pc_Buffer,"0");
						strcat(pc_Buffer,pc1);
						strcat(pc_Buffer,".");
						//
						sprintf(pc1,"%i",puTime->tm_mon+1);
						if(puTime->tm_mon<9) strcat(pc_Buffer,"0");
						strcat(pc_Buffer,pc1);					
						strcat(pc_Buffer,".");
						//
						sprintf(pc1,"%i",puTime->tm_year+1900);					
						strcat(pc_Buffer,pc1);					
						strcat(pc_Buffer," ");
						//
						sprintf(pc1,"%i",puTime->tm_hour);
						if(puTime->tm_hour<10) strcat(pc_Buffer,"0");
						strcat(pc_Buffer,pc1);
						strcat(pc_Buffer,":");
						//
						sprintf(pc1,"%i",puTime->tm_min);
						if(puTime->tm_min<10) strcat(pc_Buffer,"0");
						strcat(pc_Buffer,pc1);					
																										
						//sprintf(pc_Buffer,"%i",poNode->uCreateTime);
					}			
					DrawStringRight(pc_Buffer,oPoint,fFullDate-5);
				}
				oPoint.x+=fFullDate-2;
			}
		}
		break;};
	}
}

void 					
BF_GUI_NodePanel::DrawNodeSelf( /* wrapper for prev method  */
	int32 i_NodeIndex,
	bool b_ItCursor,										
	bool b_DrawBackground,
	bool b_OffScreen)
{
	ASSERT(! b_OffScreen);
	
	char	pcBuffer[500];
	BPoint 	oPoint;
	BRect  	oRect;
	if(!b_OffScreen){
		DrawNode(i_NodeIndex,b_ItCursor,oPoint,oRect,pcBuffer,NULL,b_DrawBackground);
	}else{
	/*
		switch(iMode){
		case BF_NODEPANEL_MODE_COLS:{
			BRect oRectOld(0,0,fColsColWidth-1,fNodeHeight-1),oRect;
			BRect oRectNew;
			
			oRectNew.left = 4;
			oRectNew.left += ((i_NodeIndex - iNavFirstIndex)/iColsNodesInCol) * fColsColWidth;
			oRectNew.right = oRectNew.left + oRectOld.Width();
			oRectNew.top = HeaderHeight() + fColTitleHeight;
			oRectNew.top += fNodeHeight * (i_NodeIndex - iNavFirstIndex);		
			oRectNew.bottom = oRectNew.top + oRectOld.Height();
			
			oPoint.x = 0;
			oPoint.y = poSysSetup->oFontNode.fAscent;
			
			
			poRenderBitmap = new BBitmap(oRectOld,B_RGB16,true);		
			poRenderBitmap->AddChild( poRender = new BView(oRectOld,"",B_FOLLOW_ALL,B_WILL_DRAW) );
			poRenderBitmap->Lock();
			poRender->SetViewColor(B_TRANSPARENT_COLOR);						
			
			oRect = oRectOld;
			DrawNode(i_NodeIndex,b_ItCursor,oPoint,oRect,pcBuffer,NULL,true,true);
			
			poRender->Sync();
			poRenderBitmap->Unlock();
			
			DrawBitmap(poRenderBitmap,oRectOld,oRectNew);
								
			delete poRenderBitmap;		
			
			poRenderBitmap = NULL;
			poRender = this;	
			
			break;}			
		default:
			ASSERT(false);
		}
		*/
	}
}

void 
BF_GUI_NodePanel::DrawNode_SetNodeColor(BF_Node *po_Node,bool b_IsFocus)
{
	if(b_IsFocus){
		if(po_Node && po_Node->bSelect)	poRender->SetHighColor(SYS_COLOR(BF_COLOR_NODE_FOCUSSELECT));
		else							poRender->SetHighColor(SYS_COLOR(BF_COLOR_CURSOR_TEXT));
		poRender->SetLowColor(SYS_COLOR(BF_COLOR_CURSOR_BACK));
	}else{
		if(po_Node && po_Node->bSelect){
			poRender->SetHighColor(SYS_COLOR(BF_COLOR_NODE_SELECT));		
			poRender->SetLowColor(SYS_COLOR(BF_COLOR_BACK));
		}else{
			switch(po_Node?po_Node->iType:BF_NODE_TYPE_UNKNOWN){
			case BF_NODE_TYPE_FILE:
				if(po_Node->bLink)
					poRender->SetHighColor(SYS_COLOR(BF_COLOR_NODE_LINK));		
				else
					poRender->SetHighColor(SYS_COLOR(BF_COLOR_NODE));		
				break;
				break;
			case BF_NODE_TYPE_DIR:
				if(po_Node->bLink)
					poRender->SetHighColor(SYS_COLOR(BF_COLOR_NODE_DIR_LINK));		
				else
					poRender->SetHighColor(SYS_COLOR(BF_COLOR_NODE_DIR));		
				break;
			default:
				poRender->SetHighColor(SYS_COLOR(BF_COLOR_NODE));					
			}
			poRender->SetLowColor(SYS_COLOR(BF_COLOR_BACK));
		}
	}
}

void
BF_GUI_NodePanel::DrawNode_CursorBack(
	BF_Node *po_Node
	,BRect &oRect
	,BPoint &oPoint
	,float f_Width
	,bool b_Cursor)
{
	oRect.left=oPoint.x;
	oRect.right=oRect.left+f_Width;
	oRect.top = oPoint.y - poSysSetup->oFontNode.fAscent;
	oRect.bottom = oRect.top+fNodeHeight-1;	
	if(b_Cursor)	
		poRender->SetHighColor(SYS_COLOR(BF_COLOR_CURSOR_BACK));		
	else			
		poRender->SetHighColor(SYS_COLOR(BF_COLOR_BACK));
	poRender->FillRect(oRect);
	//
	DrawNode_SetNodeColor(po_Node,b_Cursor);
}

void
BF_GUI_NodePanel::MouseMoved(BPoint o_Point,uint32 i_Transit,const BMessage *message)
{
	if(B_EXITED_VIEW==i_Transit){
		if(iPulseActions & BF_NODEPANEL_PULSE_MOVEDOWN){
			PulseAction_Remove(BF_NODEPANEL_PULSE_MOVEDOWN);
		}
		if(iPulseActions & BF_NODEPANEL_PULSE_MOVEUP){
			PulseAction_Remove(BF_NODEPANEL_PULSE_MOVEUP);
		}
	}
}

bool
BF_GUI_NodePanel::OnMouseUp(BPoint & o_Point)
{
	if(iPulseActions & BF_NODEPANEL_PULSE_MOVEDOWN){
		PulseAction_Remove(BF_NODEPANEL_PULSE_MOVEDOWN);
	}
	if(iPulseActions & BF_NODEPANEL_PULSE_MOVEUP){
		PulseAction_Remove(BF_NODEPANEL_PULSE_MOVEUP);
	}
	return true;
}



bool
BF_GUI_NodePanel::OnMouseDown(BPoint & o_Point)
{
	if(BF_GUI_Panel::OnMouseDown(o_Point)) return true;
	/* check for focus */
	bool bOldFocus = IsFocus();
	if(!IsFocus()){
		MakeFocus();
	}
	/* check for pressing in colstitle area */		
	if(o_Point.y<=(fColTitleHeight+HeaderHeight())){
		NavUp();
		PulseAction_Add(BF_NODEPANEL_PULSE_MOVEUP);
		return true;
	}	
	/* check for pressing in under_nodes area */		
	if(o_Point.y>(fColTitleHeight+HeaderHeight()+fNodesHeight)){
		NavDown();
		PulseAction_Add(BF_NODEPANEL_PULSE_MOVEDOWN);
		return true;
	}		
	/*  */	
	int32 	iNewCursor=-1;	
	BPoint 	oPoint(o_Point);
		
	oPoint.y -= fColTitleHeight+HeaderHeight();
	
	switch(iMode){
	case BF_NODEPANEL_MODE_COLS:{
		//oPoint.y -= poSysSetup->oFontNode.fAscent + fNodeHeight;
		iNewCursor = (int32)(oPoint.y/fNodeHeight) + iNavFirstIndex;
		iNewCursor+=iColsNodesInCol* int32((o_Point.x-5)/fColsColWidth);
		break;}	
	case BF_NODEPANEL_MODE_FULL:{		
		//oPoint.y -= poSysSetup->oFontNode.fAscent + fNodeHeight;
		iNewCursor = (uint)(oPoint.y/fNodeHeight) + iNavFirstIndex;				
		break;}
	};
	int32 iNum;
	Window()->CurrentMessage()->FindInt32("clicks", &iNum);		
	if (iNum>1)
		NavEnter();			
	else
	if(iNewCursor==iNavCursorIndex && bOldFocus)		
		NavSelect(false);
	else
		NavChangeCursor(iNewCursor);
		
	return true;
}

void
BF_GUI_NodePanel::PrepareTopMenuItem(BF_GUI_TopMenu_HItem *po_HItem)
{
	BF_GUI_Panel::PrepareTopMenuItem(po_HItem);

	ASSERT(po_HItem);	
	
	char pcSortKeyTitle[BF_NODELIST_SORT_UNSORTED][100] = 
		{"Ctrl+F1","Ctrl+F2","Ctlr+F3","","Ctlr+F4"};
	int iSortCode[BF_NODELIST_SORT_UNSORTED] = 
		{BF_NODELIST_SORT_NAME,BF_NODELIST_SORT_SIZE,BF_NODELIST_SORT_DATE,BF_NODELIST_SORT_TYPE,
			BF_NODELIST_SORT_EXT};
	
	for(int i=0;i<BF_NODELIST_SORT_UNSORTED;i++){	
		BMessage *poMessage = new BMessage(BF_MSG_NODEPANEL_SORT);
		poMessage->AddInt32("bf_sorttype",iSortCode[i]);
		poMessage->AddPointer("bf_view",this);
		const char *pc=NULL;
		switch(i){
		case 0:	pc = BF_DictAt(BF_DICT_MENU_SORTNAME);break;
		case 1:	pc = BF_DictAt(BF_DICT_MENU_SORTSIZE);break;
		case 2:	pc = BF_DictAt(BF_DICT_MENU_SORTDATE);break;
		case 3:	pc = BF_DictAt(BF_DICT_MENU_SORTTYPE);break;
		case 4:	pc = BF_DictAt(BF_DICT_MENU_SORTEXT);break;
		default:	ASSERT(FALSE);
		}
		po_HItem->loVItem.AddItem(new BF_GUI_TopMenu_VItem(pc,poMessage,pcSortKeyTitle[i]));			
	}
	po_HItem->AddHItem("",NULL);
	po_HItem->AddHItem(BF_DictAt(BF_DICT_MENU_SHOWHIDE_ICONS),BF_MSG_NODEPANEL_SHOW_ICONS,this,"Alt+P");		
	
	{
		BMessage *poMessage = new BMessage(BF_MSG_NODEPANEL_COLSMODE_SET_COLSCOUNT);
		poMessage->AddInt32("iCount",0);	
		poMessage->AddPointer("bf_view",this);
		po_HItem->AddHItem(BF_DictAt(BF_DICT_MENU_FULLMODE),poMessage,"Alt+0");
	}
	{
		BMessage *poMessage = new BMessage(BF_MSG_NODEPANEL_COLSMODE_SET_COLSCOUNT);
		poMessage->AddInt32("iCount",3);	
		poMessage->AddPointer("bf_view",this);
		po_HItem->AddHItem(BF_DictAt(BF_DICT_MENU_COLSMODE),poMessage,"Alt+1..9");
	}
	po_HItem->AddHItem(BF_DictAt(BF_DICT_MENU_PANELSETUP),BF_MSG_NODEPANEL_SETUP_0,this);
		
	//po_HItem->loVItem.AddItem(new BF_GUI_TopMenu_VItem("Cols mode","vmode_cols","Alt+1..9"));
//	po_HItem->loVItem.AddItem(new BF_GUI_TopMenu_VItem());
}

void
BF_GUI_NodePanel::PrepareKeysMenu()
{
	BF_GUI_Panel::PrepareKeysMenu();
	uint32 	iKeysModifiers = modifiers();
	
	if(iKeysModifiers & B_RIGHT_OPTION_KEY){
		BMessage *po;
		
		poSysKeysMenu->SetText(1,"SrtName");
		po = new BMessage(BF_MSG_NODEPANEL_SORT);
		po->AddInt32("bf_sorttype",BF_NODELIST_SORT_NAME);		
		poSysKeysMenu->SetMessage(1,po,this);
		
		poSysKeysMenu->SetText(2,"SrtSize");
		po = new BMessage(BF_MSG_NODEPANEL_SORT);
		po->AddInt32("bf_sorttype",BF_NODELIST_SORT_SIZE);		
		poSysKeysMenu->SetMessage(2,po,this);
		
		poSysKeysMenu->SetText(3,"SrtDate");
		po = new BMessage(BF_MSG_NODEPANEL_SORT);
		po->AddInt32("bf_sorttype",BF_NODELIST_SORT_DATE);		
		poSysKeysMenu->SetMessage(3,po,this);
		
		poSysKeysMenu->SetText(4,"SrtExt");
		po = new BMessage(BF_MSG_NODEPANEL_SORT);
		po->AddInt32("bf_sorttype",BF_NODELIST_SORT_EXT);		
		poSysKeysMenu->SetMessage(4,po,this);				
	}else{					
		
	}
}

void
BF_GUI_NodePanel::PrepareHotKeys(bool b_Focused)
{
	if(b_Focused){
		/* set keyboard shortcats */
		poWin->AddShortcut(B_BACKSPACE,B_COMMAND_KEY,new BMessage(BF_MSG_FILEPANEL_HISTORY_BACK),this);		
		for(int i=0;i<10;i++){
			BMessage *poMessage = new BMessage(BF_MSG_NODEPANEL_COLSMODE_SET_COLSCOUNT);
			poMessage->AddInt32("iCount",(int32)i);
			poWin->AddShortcut((char)('0'+i),B_COMMAND_KEY,poMessage,this);		
		}
		poWin->AddShortcut('p',B_COMMAND_KEY,new BMessage(BF_MSG_NODEPANEL_SHOW_ICONS),this);		
		poWin->AddShortcut('q',B_COMMAND_KEY,new BMessage(BF_MSG_MAINVIEW_ASK_QUIT),poWinView);				
	}else{			
		/* clear keyboard shortcats */
		poWin->RemoveShortcut(B_BACKSPACE,B_COMMAND_KEY);		
		poWin->RemoveShortcut(B_LEFT_ARROW,B_COMMAND_KEY);		
		for(int i=0;i<10;i++){			
			poWin->RemoveShortcut((char)('1'+i),B_COMMAND_KEY);
		}
		poWin->RemoveShortcut('p',B_COMMAND_KEY);
		poWin->RemoveShortcut('q',B_COMMAND_KEY);
	}				
}

bool
BF_GUI_NodePanel::OnBeforeKeyDown(const char *bytes, int32 numBytes)
{
	if(numBytes==1){
		BMessage *poMsg = Window()->CurrentMessage(); 
		if(poMsg){
			int32 iKey = poMsg->FindInt32("key");		
			
			if(BL_KEY_PLUS == iKey ){
				NavSelectAll_0(iKey);
				return true;
			}else		
			if(BL_KEY_MUL== iKey){
				NavSelectAll_0(iKey);
				return true;
			}else			
			if(BL_KEY_MINUS == iKey){
				NavSelectAll_0(iKey);
				return true;
			};
			
		}
	}		
	return false;
}

bool
BF_GUI_NodePanel::OnKeyDown(const char *bytes, int32 numBytes)
{
	if(numBytes==1 && bytes[0]==B_BACKSPACE && (poSysSetup->MainStyle() & BF_SETUP_NODEPANEL_USE_BACKSPACE)){
		NavGoParentDir();
	}else	
	if(numBytes==1 && bytes[0]==B_ENTER){
		NavEnter();
	}else
	if(numBytes==1 && bytes[0]==B_DOWN_ARROW){
		NavDown();
	}else
	if(numBytes==1 && bytes[0]==B_UP_ARROW){
		NavUp();
	}else
	if(numBytes==1 && bytes[0]==B_RIGHT_ARROW && (BF_NODEPANEL_MODE_COLS==iMode)){
		NavRight();		
	}else
	if(numBytes==1 && bytes[0]==B_LEFT_ARROW && (BF_NODEPANEL_MODE_COLS==iMode)){
		NavLeft();
	}else	
	if(numBytes==1 && bytes[0]==B_UP_ARROW){
		NavUp();
	}else	
	if(numBytes==1 && bytes[0]==B_HOME){
		NavToStart();
	}else
	if(numBytes==1 && bytes[0]==B_END){
		NavToEnd();
	}else	
	if(numBytes==1 && bytes[0]==B_INSERT){
		NavSelect();
	}else		
	if(numBytes==1 && bytes[0]==B_PAGE_UP){
		NavPgUp();
	}else			
	if(numBytes==1 && bytes[0]==B_PAGE_DOWN){
		NavPgDown();
	}else				
	if(numBytes==1 && bytes[0]=='*'){
		NavSelectAll_0(bytes[0]);
	}else		
	if(numBytes==1 && bytes[0]=='+'){
		NavSelectAll_0(bytes[0]);
	}else			
	if(numBytes==1 && bytes[0]=='-'){
		NavSelectAll_0(bytes[0]);
	};
	
	return 	BF_GUI_Panel::OnKeyDown(bytes,numBytes);
}

void 
BF_GUI_NodePanel::Action_Alert(const char *pc_Title,const char *pc_Text)
{
	/* disable panels */
	/* start thread */
	BF_Dialog_Alert_Sep(pc_Title,pc_Text,NULL,this);
		
}

bool 
BF_GUI_NodePanel::NavChangeCursor(int i_NewCursor,bool b_DrawNodes,bool b_ChangeVars)
{
	if(b_ChangeVars){
		if(i_NewCursor<0) i_NewCursor = 0;
		if(i_NewCursor>=loNode.CountItems()) i_NewCursor = loNode.CountItems()-1;
		///
		/* if go up */
		if(i_NewCursor<iNavFirstIndex){
			iNavFirstIndex = i_NewCursor;
			iNavCursorIndex = i_NewCursor;	
			if(b_DrawNodes) 	DrawNodes(true);
		}else
		/* if go down */	
		if((i_NewCursor - iNavFirstIndex+1)>iNodesOnScreen){
			iNavFirstIndex = i_NewCursor - iNodesOnScreen+1;		
			iNavCursorIndex = i_NewCursor;
			if(b_DrawNodes)	DrawNodes(true);
		}else
		/* change on screen */
		{
			if(b_DrawNodes){		
				BRect 	oRect,oRect1,oRect2;
				BPoint	oPoint;
				char	pc[500];
				bool	bFocused=IsFocus() || bFixedFocus;
				
				if(bFocused){
					DrawNode(iNavCursorIndex,false,oPoint,oRect,pc,&oRect1,true);				
					DrawNode(i_NewCursor,true,oPoint,oRect,pc,&oRect2,true);
				}
			}
			iNavCursorIndex = i_NewCursor;	
		}
	}
	DrawNodeInfo();
	
	return true;
}


void 					
BF_GUI_NodePanel::OnFocus(bool b_Focused)
{
	if(!bFixedFocus){		
		/* redraw focus_node */
		BRect 	oRect,oRect1;
		BPoint	oPoint;
		char	pc[500];
		DrawNode(iNavCursorIndex,b_Focused,oPoint,oRect,pc,&oRect1,true);	
	}
	/* call parent */
	BF_GUI_Panel::OnFocus(b_Focused);
	
	if(!b_Focused){
		PulseAction_Remove(BF_NODEPANEL_PULSE_ALL);
	}		

	/* prepare hot keys */
	{
		LOCK_WIN();
		PrepareHotKeys(b_Focused);
	}
	
}

void
BF_GUI_NodePanel::RefreshNodeList()
{

}

void 					
BF_GUI_NodePanel::Action_SwitchShowIcons()
{
	switch(iMode){
	case BF_NODEPANEL_MODE_COLS:{
		if(ModeShowSIcon()) 	iModeColsStyles = iModeColsStyles & (~BF_NODEPANEL_COLS_SICON);
		else					iModeColsStyles = iModeColsStyles | BF_NODEPANEL_COLS_SICON;
		break;}
	case BF_NODEPANEL_MODE_FULL:{
		if(ModeShowSIcon()) 	iModeFullStyles = iModeFullStyles & (~BF_NODEPANEL_FULL_SICON);
		else					iModeFullStyles = iModeFullStyles | BF_NODEPANEL_FULL_SICON;
		break;}			
	}
	Calc();
	RefreshNodeList();
	Invalidate();
}

void
BF_GUI_NodePanel::MessageReceived(BMessage* po_Message)
{
	switch(po_Message->what) {
	case BF_MSG_PULSE:{
		Pulse();
		break;}
	/*=======================*/		
	case BF_MSG_NODEPANEL_RENAME_START:
		ActionRename_Start();
		break;
	case BF_MSG_NODEPANEL_RENAME_RUN:{		
		BF_GUI_Func_PanelsEnable(true);
		MakeFocus();
		
		int32 iResult=-1;
		po_Message->FindInt32("menu",&iResult);
		if(0!=iResult) return;
		
		const char *pcOldName;
		if(B_OK!=po_Message->FindString("bf_cOldName",&pcOldName) || !pcOldName) return;
		const char *pcNewName;
		if(B_OK!=po_Message->FindString("bf_cNewName",&pcNewName) || !pcNewName) return;			
	
	
		BF_NodeCollection loOpNode;
		if(strcmp(pcOldName,"*.*")==0){
			loNode.GetSelecting(loOpNode);
		}else{		
			BF_Node *poNode = loNode.NodeByName(pcOldName);		
			if(!poNode) return;
			loOpNode.AddItem(poNode);
		}
		
		BL_String sNewName(pcNewName);
		/*
		{// rename by mask 
			BL_String sMask(pcNewName);
			if(poNode->ConvertNameByMask(sMask,sNewName)){
				pcNewName = sNewName.String();
			}
		}
		*/
			
		if(sNewName==""){
			BF_Dialog_Alert_Sep("Error",BF_DictAt(BF_DICT_PANEL_FILEEMPTYNAME),NULL,this);
			return;
		}	
		ActionRename_Run(loOpNode,sNewName);
			
		break;}
	//
	case BF_MSG_NODEPANEL_DELETE_START:
		ActionDelete_Start();
		break;
	case BF_MSG_NODEPANEL_DELETE_RUN:{		
		BF_GUI_Func_PanelsEnable(true);
		MakeFocus();
		
		int32 iResult=-1;
		if(B_OK!=po_Message->FindInt32("menu",&iResult)) return;
		if(0!=iResult) return;
		
		BF_NodeCollection* ploSel = Nodes_GetSelecting();
		ActionDelete_Run(*ploSel);
		DELETE(ploSel);				
		break;}		
	//
	case BF_MSG_NODEPANEL_COPY_START:
		ActionCopyTo_Start();
		break;
	case BF_MSG_NODEPANEL_COPY_RUN:{		
		BF_GUI_Func_PanelsEnable(true);
		MakeFocus();
		
		int32 iResult=-1;
		po_Message->FindInt32("menu",&iResult);
		if(B_OK!=po_Message->FindInt32("menu",&iResult)) return;
		if(0!=iResult) return;
		
		const char *pcPath;
		if(B_OK!=po_Message->FindString("bf_cPath",&pcPath) || !pcPath) return;
		BL_String sPath(pcPath);
		
		BF_NodeCollection* ploSel = Nodes_GetSelecting();
		ActionCopyTo_Run(sPath,*ploSel);
		DELETE(ploSel);		
		
		break;}		
	case BF_MSG_NODEPANEL_MOVE_START:
		ActionMoveTo_Start();
		break;		
	case BF_MSG_NODEPANEL_MOVE_RUN:{		
		BF_GUI_Func_PanelsEnable(true);
		MakeFocus();
		
		int32 iResult=-1;
		po_Message->FindInt32("menu",&iResult);
		if(0!=iResult) return;
		
		const char *pcPath;
		if(B_OK!=po_Message->FindString("bf_cPath",&pcPath) || !pcPath) return;
		BL_String sPath(pcPath);

		
		BF_NodeCollection* ploSel = Nodes_GetSelecting();
		ActionMoveTo_Run(sPath,*ploSel);
		DELETE(ploSel);		
		
		break;}				
	case BF_MSG_NODEPANEL_MAKEDIR_START:
		ActionMakeDir_Start(); 
		break;		
	case BF_MSG_NODEPANEL_MAKEDIR_RUN:{		
		BF_GUI_Func_PanelsEnable(true);
		MakeFocus();
		
		int32 iResult=-1;
		po_Message->FindInt32("menu",&iResult);
		if(0!=iResult) return;
		
		const char *pcTitle;
		if(B_OK!=po_Message->FindString("bf_cName",&pcTitle) || !pcTitle) return;
		BL_String sTitle(pcTitle);
		
		ActionMakeDir_Run(sTitle);		
		break;}						
	/*=======================*/	
	case BF_MSG_NODEPANEL_SETUP_0:
		PanelSetup_0();
		break;
	case BF_MSG_NODEPANEL_SETUP_1:
		PanelSetup_1(po_Message);
		break;
	case BF_MSG_NODEPANEL_SORT:{
		BF_NodeList_SortType iSort;
		iSort = (BF_NodeList_SortType)po_Message->FindInt32("bf_sorttype");
		ChangeSortType(iSort);
		break;
		};
	case BF_MSG_NODEPANEL_SHOW_ICONS:{
		Action_SwitchShowIcons();  
		break;}
	case BF_MSG_NODEPANEL_COLSMODE_SET_COLSCOUNT:{
		int32 iCount=0;
		ASSERT(B_OK==po_Message->FindInt32("iCount",&iCount) && iCount>=0 && iCount<10);
		if( iColsColCount == (uint32)iCount) break;
		switch(iMode){
		case BF_NODEPANEL_MODE_FULL:{		
			bool bIconLoaded = ModeShowSIcon();
			if(iCount>0)	iMode = BF_NODEPANEL_MODE_COLS; else break;
			if(!bIconLoaded && ModeShowSIcon()) RefreshNodeList();
			iColsColCount = iCount;
			Calc();
			Invalidate();		
			break;}		
		case BF_NODEPANEL_MODE_COLS:{
			bool bIconLoaded = ModeShowSIcon();
			if(iCount==0)	iMode = BF_NODEPANEL_MODE_FULL; 
			if(!bIconLoaded && ModeShowSIcon()) RefreshNodeList();
			iColsColCount = iCount;
			Calc();
			Invalidate();
			break;}
		}
		break;}
	/*=======================*/				
	case BF_MSG_NODEPANEL_INPUTMASK:{
		NavSelectAll_1(po_Message);
		break;};	
	default:{
		//printf("uknown message %i\n",po_Message->what);
		BF_GUI_Panel::MessageReceived(po_Message);
		break;}  	
	}
}


BF_Node*	
BF_GUI_NodePanel::Nodes_Focus()
{
	return (BF_Node*)loNode.ItemAt(iNavCursorIndex);
}

BF_NodeCollection*
BF_GUI_NodePanel::Nodes_GetSelecting()
{
	BF_NodeCollection *ploSelNode = new BF_NodeCollection();
	loNode.GetSelecting(*ploSelNode);
	if(ploSelNode->CountItems()==0){
		BF_Node *poNode = Nodes_Focus();
		if(!poNode) return(FALSE);		
		ploSelNode->AddItem(poNode);
	}
	return ploSelNode;
}

void					
BF_GUI_NodePanel::Nodes_SetSelectingByName( BF_NodeList* plo_ReqNode,uint i_SelAction,bool b_ReqUpdate)
{
	BF_Node *poSrcNode,*poSelfNode;	
	///////
	if(plo_ReqNode){
		for(int i=0;i<plo_ReqNode->CountItems();i++){
			poSrcNode = plo_ReqNode->NodeAt(i);
			poSelfNode = loNode.NodeByName(poSrcNode->sName.String());
			if(poSelfNode) poSelfNode->SetSelect(i_SelAction);
		}	
	}else{
		loNode.SetSelectAll(i_SelAction);
	}
	//
	if(b_ReqUpdate){
		if(Enabled()){		
			Draw(Bounds());
		}else
			bNeedUpdateAfterEnable = true;
	}
}

void
BF_GUI_NodePanel::Nodes_GetAll(BF_NodeList	& lo_ResultNode,bool b_ReqIcons)
{
	lo_ResultNode.DeleteItems();

	BF_Node *poNode;	
	for(int i=0;i<loNode.CountItems();i++){
		poNode = loNode.NodeAt(i);
		lo_ResultNode.AddItem(new BF_Node(poNode,b_ReqIcons));
	}
}

void					
BF_GUI_NodePanel::Nodes_Add(BF_Node *po_Node, BF_NodeCollection *plo_Node)
{
	if(po_Node)		loNode.AddItem(po_Node);
	if(plo_Node)	loNode.AddList(plo_Node);
	if(Enabled()){
		if(!(poSysSetup->MainStyle() & BF_SETUP_DEB_NOSORTING)){
			loNode.Sort(iSortType);	
		}
		Draw(Bounds());
	}else
		bNeedUpdateAfterEnable = true;
	if(iNavCursorIndex>=loNode.CountItems()) NavChangeCursor(iNavCursorIndex,false);

}

void					
BF_GUI_NodePanel::Nodes_Remove( BF_Node *po_Node, BF_NodeCollection *plo_Node)
{
		
	if(po_Node)		loNode.RemoveItem(po_Node);
	if(plo_Node) 	loNode.RemoveList(plo_Node);
	if(!po_Node && !plo_Node) loNode.DeleteItems();
	if(Enabled()){
		if(!(poSysSetup->MainStyle() & BF_SETUP_DEB_NOSORTING)){
			loNode.Sort(iSortType);	
		}
		Draw(Bounds());
	}else
		bNeedUpdateAfterEnable = true;
	//
	NavChangeCursor(iNavCursorIndex,Enabled());
}

void					
BF_GUI_NodePanel::Nodes_Update( BF_Node *po_Node, BF_NodeCollection *plo_Node)
{
	if(Enabled()){
		if(!(poSysSetup->MainStyle() & BF_SETUP_DEB_NOSORTING)){
			loNode.Sort(iSortType );	
		}
		Draw(Bounds());
	}else
		bNeedUpdateAfterEnable = true;
}

bool					
BF_GUI_NodePanel::ModeShowSIcon()
{
	if(BF_NODEPANEL_MODE_FULL==iMode && (iModeFullStyles & BF_NODEPANEL_FULL_SICON)) return true;
	if(BF_NODEPANEL_MODE_COLS==iMode && (iModeColsStyles & BF_NODEPANEL_COLS_SICON)) return true;	
	return false;
}

void
BF_GUI_NodePanel::SetModeShowSIcon(bool b_ShowIcons)
{
	if(BF_NODEPANEL_MODE_FULL==iMode){
		if(b_ShowIcons)
			iModeFullStyles = iModeFullStyles | BF_NODEPANEL_FULL_SICON;
		else
			iModeFullStyles = iModeFullStyles & (~BF_NODEPANEL_FULL_SICON);
	}else
	if(BF_NODEPANEL_MODE_COLS==iMode){
		if(b_ShowIcons)
			iModeColsStyles = iModeColsStyles | BF_NODEPANEL_COLS_SICON;		
		else
			iModeColsStyles = iModeColsStyles & (~BF_NODEPANEL_COLS_SICON);		
	}
}


void
BF_GUI_NodePanel::ChangeSortType(BF_NodeList_SortType i_NewSortType)
{
	Window()->Lock();
	//
	iSortType = i_NewSortType;
	loNode.Sort(iSortType );	
	DrawNodes(true);
	//
	Window()->Unlock();
}


/////////////////////////////////////////////////////////////////////////////////////////////
bool
BF_GUI_NodePanel::NavUp()
{
	/* check for first node */
	if(0==iNavCursorIndex) return false;
	
	if(iNavCursorIndex == iNavFirstIndex){
		switch(iMode){
		case BF_NODEPANEL_MODE_COLS:
			DrawMove_ColsUp();
			return true;
			break;
		case BF_NODEPANEL_MODE_FULL:{
			DrawMove_FullUp();			
			return true;
			break;}
		}
	}	
	NavChangeCursor(iNavCursorIndex-1);
	return true;			
}

bool 
BF_GUI_NodePanel::NavDown()
{
	/* check limit */
	if((iNavCursorIndex+1)>=loNode.CountItems()) return false;
	
	int iNewCursor = iNavCursorIndex+1;
	/* check finishing screen */
	if(iNewCursor >= (iNavFirstIndex+iNodesOnScreen)){	
		switch(iMode){
		case BF_NODEPANEL_MODE_COLS:{
			DrawMove_ColsDown();			
			return true;
			break;}
		case BF_NODEPANEL_MODE_FULL:{
			DrawMove_FullDown();			
			return true;
			break;}
		}
	}
	/* go to new pos */
	NavChangeCursor(iNewCursor);
	
	return true;			
}

bool
BF_GUI_NodePanel::NavRight()
{
	if(iNavCursorIndex == (loNode.CountItems()-1))  return false;
	switch(iMode){
	case BF_NODEPANEL_MODE_COLS:{
		if (iColsColCount==1) break;
		uint32 iNewCursor = iNavCursorIndex+iColsNodesInCol;
		if(iNewCursor>(uint32)(loNode.CountItems())) break;
		if(iNewCursor< (iNavFirstIndex + iColsNodesInCol * iColsColCount)) break;
		if( (iNewCursor+iColsNodesInCol-1)>(uint32)(loNode.CountItems())) break;
				
		DrawMove_ColsRight();
		
		return true;								
		break;}
	}
	NavChangeCursor(iNavCursorIndex+iColsNodesInCol);
	return true;
}


bool
BF_GUI_NodePanel::NavLeft()
{
	if(iNavCursorIndex == 0)  return false;

	switch(iMode){
	case BF_NODEPANEL_MODE_COLS:{
		if (iColsColCount==1) break;
		int32 iNewCursor = iNavCursorIndex - iColsNodesInCol;
		if(iNewCursor<0 || ((iNewCursor-iColsNodesInCol)<0) )break;				
		if( iNewCursor >= iNavFirstIndex) break;
		if( (iNewCursor-(int32)iColsNodesInCol)<0) break;
		
		DrawMove_ColsLeft();
		
		return true;								
		break;}
	}
	NavChangeCursor(iNavCursorIndex - iColsNodesInCol);
	return true;
}

bool
BF_GUI_NodePanel::NavPgDown()
{
	/* check for last node */
	if(iNavCursorIndex == (loNode.CountItems()-1))  return false;

	NavChangeCursor(iNavCursorIndex + iNodesOnScreen );			
	
	return true;			
}

bool
BF_GUI_NodePanel::NavPgUp()
{
	/* check for first node */
	if(iNavCursorIndex == 0)  return false;
	NavChangeCursor(iNavCursorIndex - iNodesOnScreen);	
	return true;			
}

bool
BF_GUI_NodePanel::NavSelect(bool b_CursorGoNext)
{
	if(!bCanSelect) return false;
	BF_Node *poNode = loNode.NodeAt(iNavCursorIndex);
	if(!poNode) return(false);
	poNode->SetSelect(BF_NODE_SELECT_INVERT);		
	if(b_CursorGoNext && iNavCursorIndex<(loNode.CountItems()-1))	NavDown();	else DrawNodeAt(iNavCursorIndex);
	DrawSelListInfo(true);
	return  true;
}

bool
BF_GUI_NodePanel::NavToStart()
{
	if(0==iNavCursorIndex) return(false );
	NavChangeCursor(0);
	return(true);
}

bool
BF_GUI_NodePanel::NavToEnd()
{
	if((loNode.CountItems()-1)==iNavCursorIndex) return(false );
	NavChangeCursor(loNode.CountItems()-1);
	return(true);
}

bool 
BF_GUI_NodePanel::NavEnter()
{	

	BF_Node *poNode = loNode.NodeAt(iNavCursorIndex);
	if(!poNode) return(false);
	
	switch(poNode->iType){
	/* go to parent dir */
	case BF_NODE_TYPE_PARENTDIR:{
		NavGoParentDir();
		return true;
		break;}
	}
	return false;
}



void					
BF_GUI_NodePanel::NavGoParentDir()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
int32 
BF_GUI_NodePanel_PulseThread(void *data)
{
	ASSERT(data);
	BView *poView = (BView*)data;
	BMessage oMessage(BF_MSG_PULSE);
	BMessenger oMessenger(poView);
	
	while(true){
		snooze(50000);
		oMessenger.SendMessage(&oMessage);
	}
}

void
BF_GUI_NodePanel::PulseAction_Add(int32 i_NewActions)
{
	iPulseActions = iPulseActions | i_NewActions;
	
	if(!idThreadPulse){		
		idThreadPulse = spawn_thread(BF_GUI_NodePanel_PulseThread,"BeFar:nodepanel_pulse",B_THREAD_SUSPENDED,(void*)this);	
		ASSERT(idThreadPulse>0,"can`t start thread BeFar:nodepanel_pulse\n");	
		ASSERT(B_OK==resume_thread(idThreadPulse));		
		set_thread_priority(idThreadPulse,2);
	}	
}

void					
BF_GUI_NodePanel::Pulse(void)
{
	if(iPulseActions & BF_NODEPANEL_PULSE_MOVEDOWN)	NavDown();
	if(iPulseActions & BF_NODEPANEL_PULSE_MOVEUP)	NavUp();	
}

void
BF_GUI_NodePanel::PulseAction_Remove(int32 i_Actions)
{
	if(iPulseActions==BF_NODEPANEL_PULSE_NONE) return;
	iPulseActions = iPulseActions & (!i_Actions);
	if(iPulseActions==BF_NODEPANEL_PULSE_NONE){
		while(B_OK!=kill_thread(idThreadPulse));
		idThreadPulse = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

/*=====================================================================*/
/* select files														   */
/*=====================================================================*/

bool
BF_GUI_NodePanel::NavSelectAll_0(int32 i_Key)
{
	BMessage oMessage(BF_MSG_NODEPANEL_INPUTMASK);
	oMessage.AddPointer("bf_focus",this);
	//
	BString sTitle;
	switch(i_Key){
	case BL_KEY_MUL :	sTitle=BF_DictAt(BF_DICT_PANEL_SEL_INVERT);break;
	case BL_KEY_PLUS :	sTitle=BF_DictAt(BF_DICT_PANEL_SEL_SET);break;		
	case BL_KEY_MINUS :	sTitle=BF_DictAt(BF_DICT_PANEL_SEL_UNSET);break;			
	};
	oMessage.AddInt32("bf_iKey",i_Key);		
	/* */
	BF_GUI_Func_PanelsEnable(false);
	/* make dialog */	
	BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(BRect(0,0,300,0),
		sTitle.String(),"dialog",oMessage,BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER);	
	BRect oRect;	
	/* insert edit */
	poDialog->LocalBounds(oRect);	
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	BF_GUI_ViewEdit_Create(oRect,BF_DictAt(BF_DICT_PANEL_MASK),poDialog,"edit",
					sNodeSelMask.String(),
					B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
	/* insert check */
	oRect.top = oRect.bottom+5;
	oRect.bottom = oRect.top + poSysSetup->oFontToolView.fHeight;
	BF_GUI_ViewCheck *poCheck = new BF_GUI_ViewCheck(oRect,"dir_also",
					BF_DictAt(BF_DICT_PANEL_FOLDERALSO),bSelectDirAlso,
					B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
	poDialog->AddChild(poCheck);
	/* menu */
	poDialog->AddOkCancelMenu(oRect);
	/* finish */	
	poDialog->SetHeightFromLastChild();
	poDialog->MoveToCenter( poWinView );	
	BF_GUI_Func_AddChildToMainView ( poDialog );	
	/* */		
	return true;
}

void
BF_GUI_NodePanel::NavDeselectAll()
{
	loNode.SetSelectAll(BF_NODE_SELECT_OFF,NULL,BF_NODELIST_NODETYPE_ALL);	

	if(Enabled()){	
		DrawNodes(true);
		DrawSelListInfo(true);
	}else
		bNeedUpdateAfterEnable=true;
}

void
BF_GUI_NodePanel::NavSelectAll_1(BMessage *po_Message)
{	
	bool	bOk;
	ASSERT(B_OK==po_Message->FindBool(BF_GUI_DIALOG_MESSAGE_DATA_OK,&bOk));
	if(bOk){
		char 	*pcEditResult;
		ASSERT(B_OK==po_Message->FindString("edit",(const char**)&pcEditResult));
		ASSERT(B_OK==po_Message->FindBool("dir_also",&bSelectDirAlso));
		/* prep  */
		uint32 iWhatSelect = 0;
		iWhatSelect=iWhatSelect | BF_NODELIST_NODETYPE_FILES;
		iWhatSelect=iWhatSelect | BF_NODELIST_NODETYPE_LINKS;
		if(bSelectDirAlso)
		iWhatSelect=iWhatSelect | BF_NODELIST_NODETYPE_DIRS;
		/* */
		int32 iKey;		
		ASSERT(B_OK==po_Message->FindInt32("bf_iKey",(int32*)&iKey));
		switch(iKey){
		case BL_KEY_MUL :
			loNode.SetSelectAll(BF_NODE_SELECT_INVERT,pcEditResult,iWhatSelect);break;
		case BL_KEY_PLUS:
			loNode.SetSelectAll(BF_NODE_SELECT_ON,pcEditResult,iWhatSelect);break;
		case BL_KEY_MINUS:
			loNode.SetSelectAll(BF_NODE_SELECT_OFF,pcEditResult,iWhatSelect);break;			
		}
		sNodeSelMask = pcEditResult;
		
		DrawNodes(true);
		DrawSelListInfo(true);
	}
	//
	BF_GUI_Func_PanelsEnable(true);
	MakeFocus();
}
/////////////////////////////////////////////////////////////////////
void
BF_GUI_NodePanel::PanelSetup_0()
{
	/* prepare message */
	BMessage oMessage(BF_MSG_NODEPANEL_SETUP_1);
	oMessage.AddPointer("bf_focus",this);
	/* */
	BF_GUI_Func_PanelsEnable(false);
	/* make dialog */	
	BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(BRect(0,0,200,0),
		BF_DictAt(BF_DICT_PANEL_SETUP),"dialog",oMessage,BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER);	
	/* resize dialog */	
	BRect oRect;
	poDialog->LocalBounds(oRect);		
	BF_GUI_ViewCheck *poCheck;	
	/* show icons */
	//roRect.top = oRect.bottom+5;
	oRect.bottom = oRect.top + poSysSetup->oFontToolView.fHeight;
	poCheck = new BF_GUI_ViewCheck(oRect,"show_icons",
					BF_DictAt(BF_DICT_PANEL_SHOWICONS),ModeShowSIcon(),
					B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
	poDialog->AddChild(poCheck);		
	/* full_mode */
	switch(iMode){
	case BF_NODEPANEL_MODE_FULL:{
		// date 
		oRect.top = oRect.bottom+5;
		oRect.bottom = oRect.top + poSysSetup->oFontToolView.fHeight;
		poCheck = new BF_GUI_ViewCheck(oRect,"full_date",
					BF_DictAt(BF_DICT_PANEL_SHOWDATES),iModeFullStyles & BF_NODEPANEL_FULL_DATE,
					B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
		poDialog->AddChild(poCheck);
		// size	
		oRect.top = oRect.bottom+5;
		oRect.bottom = oRect.top + poSysSetup->oFontToolView.fHeight;
		poCheck = new BF_GUI_ViewCheck(oRect,"full_size",
					BF_DictAt(BF_DICT_PANEL_SHOWSIZES),iModeFullStyles & BF_NODEPANEL_FULL_SIZE,
					B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
		poDialog->AddChild(poCheck);		
		// type
		oRect.top = oRect.bottom+5;
		oRect.bottom = oRect.top + poSysSetup->oFontToolView.fHeight;
		poCheck = new BF_GUI_ViewCheck(oRect,"full_type",
					BF_DictAt(BF_DICT_PANEL_SHOWTYPES),iModeFullStyles & BF_NODEPANEL_FULL_TYPE,
					B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
		poDialog->AddChild(poCheck);		
		break;}
	}
	////////////////////			
	/* menu */
	poDialog->AddOkCancelMenu(oRect);		
	/* finish */
	poDialog->SetHeightFromLastChild();
	poDialog->MoveToCenter( poWinView );			
	BF_GUI_Func_AddChildToMainView ( poDialog );	
	/* */		
}

void
BF_GUI_NodePanel::PanelSetup_1(BMessage *po_Message)
{
	BF_GUI_Func_PanelsEnable(true);	
	MakeFocus();

	bool bOk;
	ASSERT(B_OK==po_Message->FindBool(BF_GUI_DIALOG_MESSAGE_DATA_OK,&bOk));				
	if(bOk && po_Message->FindInt32("menu")==0){			
		//
		ASSERT(B_OK==po_Message->FindBool("show_icons",&bOk));
		SetModeShowSIcon(bOk);
		//		
		if(B_OK==po_Message->FindBool("full_date",&bOk)){
			if(bOk)	iModeFullStyles = iModeFullStyles | BF_NODEPANEL_FULL_DATE;
			else	iModeFullStyles = iModeFullStyles & (~BF_NODEPANEL_FULL_DATE);
		}			
		//
		if(B_OK==po_Message->FindBool("full_size",&bOk)){
			if(bOk)	iModeFullStyles = iModeFullStyles | BF_NODEPANEL_FULL_SIZE;
			else	iModeFullStyles = iModeFullStyles & (~BF_NODEPANEL_FULL_SIZE);			
		}
		//
		if(B_OK==po_Message->FindBool("full_type",&bOk)){
			if(bOk)	iModeFullStyles = iModeFullStyles | BF_NODEPANEL_FULL_TYPE;
			else	iModeFullStyles = iModeFullStyles & (~BF_NODEPANEL_FULL_TYPE);			
		}
		//
		Calc();
		RefreshNodeList();
		Invalidate();
	}		
}

void
BF_GUI_NodePanel::ActionMakeDir_Start()
{
	/* */
	BF_GUI_Func_PanelsEnable(false);
	/* make dialog */	
	BMessage oMessage(BF_MSG_NODEPANEL_MAKEDIR_RUN);
	oMessage.AddPointer("bf_focus",this);
			
	BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(BRect(0,0,300,0),
		BF_DictAt(BF_DICT_PANEL_CREATEFOLDER_TITLE),"dialog",oMessage,BG_GUI_DIALOG_WINRESIZE_NONE);
	BRect oRect;	
	/* insert edit */
	poDialog->LocalBounds(oRect);	
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	BF_GUI_ViewEdit_Create(oRect,BF_DictAt(BF_DICT_NAME),poDialog,"bf_cName",
					"New folder",
					B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
	/* menu */								
	poDialog->AddOkCancelMenu(oRect);
	
	/* finish */	
	poDialog->SetHeightFromLastChild();
	poDialog->MoveToCenter( poWinView );	
	BF_GUI_Func_AddChildToMainView ( poDialog );	
}

void
BF_GUI_NodePanel::ActionMakeDir_Run(BL_String & s_Title)
{
}

void					
BF_GUI_NodePanel::ActionCopyFrom_Run(BL_String & s_Path,BF_NodeCollection & lo_Node,bool b_Move)
{
}

void
BF_GUI_NodePanel::ActionCopyTo_Start()
{
	// prepare dest_path
	BL_String sDestPath;
	BF_GUI_Panel *poDestPanel = poWinView->PanelOnTop(!bWinPos_OnLeft);
	if(poDestPanel)
		sDestPath = poDestPanel->Path();
	else
		sDestPath = Path();
		
	//
	if(poSysSetup->MainStyle() & BF_SETUP_MAIN_ASK_COPY){	
		/* */
		BF_GUI_Func_PanelsEnable(false);
		/* make dialog */	
		BMessage oMessage(BF_MSG_NODEPANEL_COPY_RUN);
		oMessage.AddPointer("bf_focus",this);
		oMessage.AddInt32(BF_NODEPANEL_COPY_NOTETYPES_FIELD,BF_NODELIST_NODETYPE_ALL);
				
		BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(BRect(0,0,300,0),
			BF_DictAt(BF_DICT_COPYTO),"dialog",oMessage,BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER);	
		BRect oRect;	
		/* insert edit */
		poDialog->LocalBounds(oRect);	
		oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
		BF_GUI_ViewEdit_Create(oRect,BF_DictAt(BF_DICT_PATH),poDialog,"bf_cPath",
						sDestPath.String(),
						B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
		// menu 
		{
			BL_List	*ploMenu = new BL_List();
			ploMenu->AddItem(new BF_GUI_ViewMenu_Item(BF_DictAt(BF_DICT_OK),"ok"));
			ploMenu->AddItem(new BF_GUI_ViewMenu_Item(BF_DictAt(BF_DICT_CANCEL),"cancel"));
			ploMenu->AddItem(new BF_GUI_NodePanel_Copy_SpecialItem(BF_DictAt(BF_DICT_SPECIAL),"special",poDialog->Message()));
			poDialog->AddMenu(oRect,ploMenu);
		}
		
		/* finish */	
		poDialog->SetHeightFromLastChild();
		poDialog->MoveToCenter( poWinView );	
		BF_GUI_Func_AddChildToMainView ( poDialog );	
	}else{		
		BF_NodeCollection* ploSel = Nodes_GetSelecting();
		ActionCopyTo_Run(sDestPath,*ploSel);
		DELETE(ploSel);						
	}
}



void
BF_GUI_NodePanel::ActionCopyTo_Run(BL_String & s_Path,BF_NodeCollection & lo_Node)
{
	if(poSysSetup->MainStyle() & BF_SETUP_NODEPANEL_DESELECT_AFTER_COPY) NavDeselectAll();
}

void
BF_GUI_NodePanel::ActionMoveTo_Start()
{
	// prepare dest_path
	BL_String sDestPath;
	BF_GUI_Panel *poDestPanel = poWinView->PanelOnTop(!bWinPos_OnLeft);
	if(poDestPanel)
		sDestPath = poDestPanel->Path();
	else
		sDestPath = Path();
		
	if(poSysSetup->MainStyle() & BF_SETUP_MAIN_ASK_COPY){		
		/* */
		BF_GUI_Func_PanelsEnable(false);
		/* make dialog */	
		BMessage oMessage(BF_MSG_NODEPANEL_MOVE_RUN);
		oMessage.AddPointer("bf_focus",this);
				
		BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(BRect(0,0,300,0),
			BF_DictAt(BF_DICT_MOVETO),"dialog",oMessage,BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER);	
		BRect oRect;	
		/* insert edit */
		poDialog->LocalBounds(oRect);	
		oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
		BF_GUI_ViewEdit_Create(oRect,BF_DictAt(BF_DICT_PATH),poDialog,"bf_cPath",
						sDestPath.String(),
						B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);
		/* menu */								
		//BF_Dialog_Alert_Sep("Question","Copy files ?",ploMenu,this,NULL,BF_MSG_NODEPANEL_COPY_START);
		poDialog->AddOkCancelMenu(oRect);
		
		/* finish */	
		poDialog->SetHeightFromLastChild();
		poDialog->MoveToCenter( poWinView );	
		BF_GUI_Func_AddChildToMainView ( poDialog );	
	}else{		
		BF_NodeCollection* ploSel = Nodes_GetSelecting();
		ActionMoveTo_Run(sDestPath,*ploSel);
		DELETE(ploSel);						
	}
}

void
BF_GUI_NodePanel::ActionMoveTo_Run(BL_String & s_Path,BF_NodeCollection & lo_Node)
{
	if(poSysSetup->MainStyle() & BF_SETUP_NODEPANEL_DESELECT_AFTER_MOVE) NavDeselectAll();
}

void
BF_GUI_NodePanel::ActionDelete_Start()
{
	if(poSysSetup->MainStyle() & BF_SETUP_MAIN_ASK_DELETE){		
		BL_List *ploMenu = new BL_List();
		ploMenu->AddItem(new BF_GUI_ViewMenu_Item(BF_DictAt(BF_DICT_YES),"yes"));
		ploMenu->AddItem(new BF_GUI_ViewMenu_Item(BF_DictAt(BF_DICT_NO),"no"));
		BF_Dialog_Alert_Sep(BF_DictAt(BF_DICT_QUESTIONS),BF_DictAt(BF_DICT_PANEL_DELETEFILES),
			ploMenu,this,NULL,BF_MSG_NODEPANEL_DELETE_RUN);
	}else{
		BF_NodeCollection* ploSel = Nodes_GetSelecting();
		ActionDelete_Run(*ploSel);
		DELETE(ploSel);				
	}
}

void
BF_GUI_NodePanel::ActionDelete_Run(BF_NodeCollection & lo_Node)
{
}

void
BF_GUI_NodePanel::ActionRename_Start()
{		
	uint64				iSelCount = loNode.CountSelected();
	BF_Node				*poNode = Nodes_Focus();
	BL_String			s;
	
	if(iSelCount==0 && !poNode){
		BF_Dialog_Alert_Sep(BF_DictAt(BF_DICT_WARNING),BF_DictAt(BF_DICT_PANEL_PANELEMPTY),NULL,this);
		return;
	}
	/*
	if(poNode->Type()==BF_NODE_TYPE_PARENTDIR){
		BF_Dialog_Alert_Sep("Warning","Sorry, but I can`t rename this node",NULL,this);
		return;
	}
	*/
	/* */
	BF_GUI_Func_PanelsEnable(false);
	/* make dialog */	
	BMessage oMessage(BF_MSG_NODEPANEL_RENAME_RUN);
	oMessage.AddPointer("bf_focus",this);
	oMessage.AddString("bf_cOldName",iSelCount==0?poNode->Name():"*.*");
			
	BF_GUI_Dialog *poDialog = new BF_GUI_Dialog(BRect(0,0,300,0),
		BF_DictAt(BF_DICT_RENAME),"dialog",oMessage,BG_GUI_DIALOG_WINRESIZE_MOVE_CENTER);
	BRect oRect;	
	/* insert comment */
	poDialog->LocalBounds(oRect);		
	oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;
	if(iSelCount==0){
		s=BF_DictAt(BF_DICT_PANEL_FROM);
		s<<" ";
		s<<poNode->Name();	
		poDialog->AddChild(new BF_GUI_ViewText(oRect,"old_name",s.String(),B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_WILL_DRAW,false));
		oRect.top = oRect.bottom+5;
		oRect.bottom = oRect.top+poSysSetup->oFontToolView.fHeight;					
	}
	/* insert edit */	
	BF_GUI_ViewEdit_Create(oRect,BF_DictAt(BF_DICT_PANEL_TO),poDialog,"bf_cNewName",
					iSelCount==0?poNode->Name():"*.*",
					B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP,B_NAVIGABLE);					
	/* menu */								
	poDialog->AddOkCancelMenu(oRect);
	
	/* finish */	
	poDialog->SetHeightFromLastChild();
	poDialog->MoveToCenter( poWinView );	
	BF_GUI_Func_AddChildToMainView ( poDialog );	
}


void
BF_GUI_NodePanel::ActionRename_Run(BF_NodeCollection & lo_Node,BL_String & s_NewName)
{
}

void
BF_GUI_NodePanel::StoreVars(BF_GUI_Setup_Vars_Panel & o_Setup) const
{
	BF_GUI_Panel::StoreVars(o_Setup);
	o_Setup.sPath = Path();
	o_Setup.iModeFullStyles = iModeFullStyles;
	o_Setup.iModeColsStyles =iModeColsStyles;
	o_Setup.iMode = iMode;
	
}
