#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BF_Roster.h"
#include "BF_Msg.h"
#include "BF_GUI_NodePanel.h"
#include "BF_GUI_DlgViews.h"


void					
BF_GUI_NodePanel::DrawMove_ColsDown()
{
		
	/* draw current node in unfocused state */
	DrawNodeSelf(iNavCursorIndex,false,true);
	// prep full rect for first col
	BRect oFirstRect(poRender->Bounds());
	oFirstRect.left+=4;
	oFirstRect.right = oFirstRect.left + fColsColWidth-3;		
	oFirstRect.top += HeaderHeight() + fColTitleHeight + fNodeHeight;	
	oFirstRect.bottom = oFirstRect.top + fNodesHeight - fNodeHeight;
	// move cols up //
	BRect 	oColRectOld( oFirstRect ),oColRectNew,oNodeTopOld,oNodeTopNew;
	for(int32 iCol=0;iCol<(int32)iColsColCount;iCol++){			
		/* move top_node to left_bottom */		
		if(iCol>0){			
			oNodeTopOld = oColRectOld;
			oNodeTopOld.top -= fNodeHeight;
			oNodeTopOld.bottom = oNodeTopOld.top + fNodeHeight-1;
			oNodeTopNew = oColRectOld;
			oNodeTopNew.top = oNodeTopNew.bottom-fNodeHeight;
			oNodeTopNew.bottom--;
			oNodeTopNew.left -= fColsColWidth;
			oNodeTopNew.right -= fColsColWidth;
			poRender->CopyBits(oNodeTopOld,oNodeTopNew);				
		}
		//
		oColRectNew = oColRectOld;
		oColRectNew.top -= fNodeHeight;
		oColRectNew.bottom -= fNodeHeight;		
		poRender->CopyBits(oColRectOld,oColRectNew);
		/* swift rect to right */
		oColRectOld.left += fColsColWidth;
		oColRectOld.right += fColsColWidth;
	}	
	/* move cursor_vars down */
	NavChangeCursor(iNavCursorIndex+1,false);
	/* draw current node in focused state */
	DrawNodeSelf(iNavCursorIndex,true,true);	
}

void					
BF_GUI_NodePanel::DrawMove_ColsUp()
{		
	/* draw current node in unfocused state */
	DrawNodeSelf(iNavCursorIndex,false,true);
	// prep full rect for last col
	BRect oFirstRect(poRender->Bounds());
	oFirstRect.left+=4 + fColsColWidth*(iColsColCount-1);
	oFirstRect.right = oFirstRect.left + fColsColWidth-3;		
	oFirstRect.top += HeaderHeight() + fColTitleHeight;	
	oFirstRect.bottom = oFirstRect.top + fNodesHeight - fNodeHeight;
	// move cols down //
	BRect 	oColRectOld( oFirstRect ),oColRectNew,oNodeTopOld,oNodeTopNew;
	
	for(int32 iCol=iColsColCount-1;iCol>=0;iCol--){			
		/* move bottom_node to right_top */
		if(iCol<((int32)iColsColCount-1)){			
			oNodeTopOld = oColRectOld;
			oNodeTopOld.top = oNodeTopOld.bottom/*+1*/;
			oNodeTopOld.bottom  = oNodeTopOld.top + fNodeHeight/* -1*/;
			oNodeTopNew = oColRectOld;
			oNodeTopNew.bottom = oNodeTopNew.top + fNodeHeight -1;
			oNodeTopNew.left += fColsColWidth;
			oNodeTopNew.right += fColsColWidth;
			poRender->CopyBits(oNodeTopOld,oNodeTopNew);				
		}
		//
		oColRectNew = oColRectOld;
		oColRectNew.top += fNodeHeight;
		oColRectNew.bottom += fNodeHeight;		
		poRender->CopyBits(oColRectOld,oColRectNew);
		/* swift rect to right */
		oColRectOld.left -= fColsColWidth;
		oColRectOld.right -= fColsColWidth;
	}	
	/* move cursor_vars down */
	NavChangeCursor(iNavCursorIndex-1,false);
	/* draw current node in focused state */	
	DrawNodeSelf(iNavCursorIndex,true,true);	
}

void					
BF_GUI_NodePanel::DrawMove_ColsRight()
{
	/* draw current node in unfocused state */
	DrawNodeSelf(iNavCursorIndex,false,true);
	// prep full rect for move
	BRect oOldRect(poRender->Bounds());
	oOldRect.left += 4 + fColsColWidth;
	oOldRect.right = oOldRect.left + (iColsColCount-1) * fColsColWidth -2;
	oOldRect.top += HeaderHeight() + fColTitleHeight;	
	oOldRect.bottom = oOldRect.top + fNodesHeight;
			
	// move cols down //
	BRect oNewRect(oOldRect);
 	oNewRect.left -= fColsColWidth;
 	oNewRect.right -= fColsColWidth;

	//
	poRender->CopyBits(oOldRect,oNewRect);
	/* move cursor_vars down */
	iNavCursorIndex += iColsNodesInCol;
	iNavFirstIndex += iColsNodesInCol;	
	NavChangeCursor(iNavCursorIndex + iColsNodesInCol,false,false);
	/* draw nodes from last_col*/
	{
		int iNode = iNavFirstIndex + iColsNodesInCol * (iColsColCount-1);
		for(uint i = 0;i < iColsNodesInCol;i++){
			DrawNodeSelf(i+iNode,(i+iNode)==iNavCursorIndex,true);
		}
	}
}

void					
BF_GUI_NodePanel::DrawMove_ColsLeft()
{
	/* draw current node in unfocused state */
	DrawNodeSelf(iNavCursorIndex,false,true);
	// prep full rect for move
	BRect oOldRect(poRender->Bounds());
	oOldRect.left += 4; 
	oOldRect.right = oOldRect.left + (iColsColCount-1) * fColsColWidth -2;
	oOldRect.top += HeaderHeight() + fColTitleHeight;	
	oOldRect.bottom = oOldRect.top + fNodesHeight;
			
	// move cols down //
	BRect oNewRect(oOldRect);
 	oNewRect.left += fColsColWidth;
 	oNewRect.right += fColsColWidth;

	//
	poRender->CopyBits(oOldRect,oNewRect);
	/* move cursor_vars left */
	iNavCursorIndex -= iColsNodesInCol;
	iNavFirstIndex -= iColsNodesInCol;	
	NavChangeCursor(iNavCursorIndex,false,false);
	/* draw nodes from last_col*/
	{
		for(int32 i = iNavFirstIndex;i < iNavFirstIndex+(int32)iColsNodesInCol;i++){
			DrawNodeSelf(i,i==iNavCursorIndex,true);
		}
	}
}


void					
BF_GUI_NodePanel::DrawMove_FullUp()
{
	/* draw current node in unfocused state */
	DrawNodeSelf(iNavCursorIndex,false,true);
	
	// prep old rect
	BRect oOldRect(poRender->Bounds());
	oOldRect.left += 4 ;
	oOldRect.right -= 4 ;
	oOldRect.top += HeaderHeight() + fColTitleHeight;	
	oOldRect.bottom = oOldRect.top + fNodesHeight - fNodeHeight;
	// prep new  rect
	BRect oNewRect(oOldRect);
	oNewRect.top += fNodeHeight;
	oNewRect.bottom += fNodeHeight;
    
    /* copy rect */  
	poRender->CopyBits(oOldRect,oNewRect);		

	/* move cursor */
	iNavCursorIndex --;
	iNavFirstIndex --;
	NavChangeCursor(iNavCursorIndex,false,false);
		
	/* draw current node in focused state */	
	DrawNodeSelf(iNavCursorIndex,true,true);		

}

void					
BF_GUI_NodePanel::DrawMove_FullDown()
{
	/* draw current node in unfocused state */
	DrawNodeSelf(iNavCursorIndex,false,true);
	
	// prep old rect
	BRect oOldRect(poRender->Bounds());
	oOldRect.left += 4 ;
	oOldRect.right -= 4 ;
	oOldRect.top += HeaderHeight() + fColTitleHeight;	
	oOldRect.bottom = oOldRect.top + fNodesHeight;
	oOldRect.top +=  fNodeHeight;
	// prep new  rect
	BRect oNewRect(oOldRect);
	oNewRect.top -= fNodeHeight;
	oNewRect.bottom -= fNodeHeight;
    // copy rect
	poRender->CopyBits(oOldRect,oNewRect);		

	/* move cursor */
	iNavCursorIndex ++;
	iNavFirstIndex ++;
	NavChangeCursor(iNavCursorIndex + iColsNodesInCol,false,false);
		
	/* draw current node in focused state */	
	DrawNodeSelf(iNavCursorIndex,true,true);		
}