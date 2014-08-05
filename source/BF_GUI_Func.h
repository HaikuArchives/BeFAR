#ifndef __BF_GUI_FUNC_H__
#define __BF_GUI_FUNC_H__

#include <View.h>
#include <Window.h>
#include "BL_Tools.h"
#include "BF_GUI_TopMenu.h"
#include "BF_Node.h"


#define BF_PANEL_WINPOS_LEFT  true
#define BF_PANEL_WINPOS_RIGHT false

class BF_GUI_Panel:public BView{
public:
		bool					bWinPos_OnLeft;
		
								BF_GUI_Panel(
	 								const BRect& 	o_Rect,
	 								const char*		pc_FirstName, // any name, but real name will be "name_panel"
									bool			b_WinPos_OnLeft);
									
								~BF_GUI_Panel();

///////////////////////  cannot be inherited  ///////////////////////////////////////////////////

virtual void					MouseDown(BPoint point); // not for childs //
virtual void					MouseUp(BPoint point);	 // not for childs //
virtual void					KeyDown(const char *bytes, int32 numBytes); // not for childs //
virtual void					Draw(BRect o_Rect); // don`t make this methods in childs !!, use Draw_Panel()

		void					Enable(bool b_Enable);
		bool					Enabled();
		bool					Close();

		void					Run_TopMenu();				

virtual void 					MakeFocus(bool focused = true);		

		float					HeaderHeight();
		float					BottomHeight();		
		BRect					BottomRect();

		void					SetHeader(const char *pc_Header,BBitmap * po_SIcon=NULL);
		
		void 					DrawBackground(const BRect & o_Rect) const;
		void					Draw_OpenTransaction();
		void					Draw_CloseTransaction(BRect * po_RectDest=NULL);  
		void					DrawStringCheck(const char *pc_Text,float f_Width,BPoint & o_Point,BFont *po_Font=NULL);

		void					SayFriend_About_CursorChanged(	const char 	*pc_NodePath,
																const char 	*pc_NodeName,
																int			 i_NodeType);


virtual	void					Nodes_GetAll(BF_NodeList& lo_ResultNode,bool b_ReqIcons);   	// mozet nasledovastsya
virtual	void					Nodes_SetSelectingByName( BF_NodeList* plo_ReqNode,uint i_SelAction,bool b_ReqUpdate=true); // mozet nasledovastsya
		
///////////////////////  for childs ///////////////////////////////////////////////////

virtual void					StoreVars(BF_GUI_Setup_Vars_Panel & o_Setup) const;
									
virtual void					MessageReceived(BMessage* po_Message);
virtual void					FrameResized(float width, float height);

virtual bool 					OnMouseDown(BPoint & o_Point);
virtual bool 					OnMouseUp(BPoint & o_Point);

virtual bool					OnKeyDown(const char *bytes, int32 numBytes);
virtual bool					OnBeforeKeyDown(const char *bytes, int32 numBytes);

virtual void					DrawPanel(BRect & o_Rect);

virtual	void					NavPressInHeader(const BPoint & o_Point);								
				

virtual bool					OnClose();	// if can`t be close, then return false
virtual	void					OnEnable(bool b_Enable);
virtual void					OnFocus(bool b_Focused);											
			
virtual	BRect					ClientRect();		
		
virtual	void					DrawHeader(bool b_IsFocus);
virtual	void 					DrawBottom(const char *pc_Text,bool b_DrawBack);

virtual	void					PrepareKeysMenu();
virtual	void					PrepareCmdLine();
virtual void					PrepareTopMenuItem(BF_GUI_TopMenu_HItem *po_HItem); // must be calling from child

virtual const BL_String 		Path()const;

virtual	void					ActionCopyFrom_Run(BL_String & s_Path,BF_NodeCollection & lo_Node,bool b_Move);
virtual	void					Action_CopyToBuffer(){};
virtual	void					Action_PasteFromBuffer(){};
virtual void					Action_Friend_NewCursor(const char *pc_NodePath,const char *pc_NodeName,int32 i_NodeType){};

virtual	void					SetupUpdated();

/////////////////////////////////// values ////////////////////////////

private:
		int32					iDisableCount;		
		int32					iRenderDeep;// counter for nested render_transaction 
		
public:		
		bool					bFixedFocus,bNeedUpdateAfterEnable;
		BL_String				sHeader;
		BBitmap					*poHeaderSIcon;

protected:
		BView					*poRender;// offscreen view for quick rendrer, default=this
		BBitmap					*poRenderBitmap;		
		
		int32					RenderDeep()const {return iRenderDeep;} ;
		
};


void
BF_GUI_Func_PanelsEnable(bool b_Enable,bool b_OperRosterAlso=true);

void
BF_GUI_Func_PanelsShow(bool b_Show);

void
BF_GUI_Func_RedrawDialogs();

bool
BF_GUI_Func_EnableDialog(const char *pc_Name,bool b_Enable);

void
BF_GUI_Func_CloseDialog(BView *po_Dialog);

bool
BF_GUI_Func_AddChildToMainView(BView *po_View);

void
BF_GUI_MakeFocusTo(BView *po_View);

#endif