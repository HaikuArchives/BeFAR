#include <stdlib.h>	
#include <stdio.h>

#include "BF_Dict.h"	
#include "BL_Tools.h"	
#include "BF_GUI_Setup.h"	

#include "BF_Roster.h"
#include "BF_GUI_DlgViews.h"
#include "BF_GUI_WinMain.h"
#include "BF_GUI_NodePanel.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Setup"

BF_GUI_Setup *poSysSetup = NULL;

//////////////////////////////////////////////////////////////////////////

#define VARS_FILE "vars.settings"

BF_GUI_Setup_Vars_Panel::BF_GUI_Setup_Vars_Panel()
{
	iModeFullStyles = 0;
	iModeColsStyles = 0;
	iMode = 0;
	sPath = "";
}

void					
BF_GUI_Setup_Vars_Panel::Load(BF_ConfigFile &o_File)
{
	Clear();
	
	BL_String s,sValue,sName;
	while(B_OK==o_File.ReadString(&s)){
		if(s=="}") break;else
		if(s=="full_styles{"){
		  	iModeFullStyles = 0;
			while(B_OK==o_File.ReadString(&s)){
				if(s=="name") iModeFullStyles = iModeFullStyles|BF_NODEPANEL_FULL_NAME;else
				if(s=="date") iModeFullStyles = iModeFullStyles|BF_NODEPANEL_FULL_DATE;else
				if(s=="size") iModeFullStyles = iModeFullStyles|BF_NODEPANEL_FULL_SIZE;else
				if(s=="type") iModeFullStyles = iModeFullStyles|BF_NODEPANEL_FULL_TYPE;else
				if(s=="sicon") iModeFullStyles = iModeFullStyles|BF_NODEPANEL_FULL_SICON;else
				if(s=="}") break;
			}
		}else
		if(s=="cols_styles{"){
			iModeColsStyles = 0;
			while(B_OK==o_File.ReadString(&s)){
				if(s=="name") iModeColsStyles = iModeColsStyles|BF_NODEPANEL_COLS_NAME;else
				if(s=="sicon") iModeColsStyles = iModeColsStyles|BF_NODEPANEL_COLS_SICON;else
				if(s=="}") break;
			}
		}else		
		{
			o_File.VarUnpack(s,sName,sValue);
			if(sName=="path") sPath = sValue;else
			if(sName=="mode"){
				if(sValue == "full") iMode = BF_NODEPANEL_MODE_FULL;else
				if(sValue == "cols") iMode = BF_NODEPANEL_MODE_COLS;else
				{}
			}else
			{}
			sName = "";
		}
	}
}

void					
BF_GUI_Setup_Vars_Panel::Clear()
{
	iModeFullStyles = 0;
	iModeColsStyles = 0;
	iMode = 0;
	sPath = "";
}

BF_GUI_Setup_Vars::BF_GUI_Setup_Vars()
{
	bLoaded = false;
}

status_t					
BF_GUI_Setup_Vars::Load()
{	
	bLoaded = false;

	BL_String 	s,s1;
	s = BEFAR_SETTINGS_DIR;
	s<<VARS_FILE;
	
	BF_ConfigFile oFile(s.String(),B_READ_WRITE);
	status_t uRes = oFile.InitCheck();
	
	if(uRes!=B_OK)	return uRes;
	
	BL_String sName,sValue;
	
	while(B_OK==oFile.ReadString(&s)){
		if(s=="left_panel{") oLPanel.Load(oFile);else
		if(s=="right_panel{") oRPanel.Load(oFile);else
		{
			if(B_OK!=oFile.VarUnpack(s,sName,sValue)) continue;				
			if(sName=="win_x") oWinFrame.left = sValue.Float();else
			if(sName=="win_y") oWinFrame.top = sValue.Float();else
			if(sName=="win_x1") oWinFrame.right = sValue.Float();else
			if(sName=="win_y1") oWinFrame.bottom = sValue.Float();else		
			{}
		}
	}	
	
	if(oLPanel.sPath=="" || oRPanel.sPath=="") return B_ERROR;
	
	bLoaded = true;
	return B_OK;
	
}
void					
BF_GUI_Setup_Vars::SavePanel(BF_ConfigFile &o_File,const BF_GUI_Setup_Vars_Panel & o_Panel,bool b_LeftPanel)
{
	o_File.WriteString(b_LeftPanel?"left_panel{":"right_panel{");	
	o_File.WriteVar("path",o_Panel.sPath.String());
	
	o_File.WriteString("full_styles{");	
	o_File.WriteStyle(o_Panel.iModeFullStyles,BF_NODEPANEL_FULL_NAME,"name");
	o_File.WriteStyle(o_Panel.iModeFullStyles,BF_NODEPANEL_FULL_DATE,"date");
	o_File.WriteStyle(o_Panel.iModeFullStyles,BF_NODEPANEL_FULL_SIZE,"size");
	o_File.WriteStyle(o_Panel.iModeFullStyles,BF_NODEPANEL_FULL_TYPE,"type");
	o_File.WriteStyle(o_Panel.iModeFullStyles,BF_NODEPANEL_FULL_SICON,"sicon");
	o_File.WriteString("}");
	
	o_File.WriteString("cols_styles{");	
	o_File.WriteStyle(o_Panel.iModeColsStyles,BF_NODEPANEL_COLS_NAME,"name");
	o_File.WriteStyle(o_Panel.iModeColsStyles,BF_NODEPANEL_COLS_SICON,"sicon");
	o_File.WriteString("}");
	
	if(o_Panel.iMode==BF_NODEPANEL_MODE_FULL) o_File.WriteVar("mode","full");else
	if(o_Panel.iMode==BF_NODEPANEL_MODE_COLS) o_File.WriteVar("mode","cols");
		
	o_File.WriteString("}");	
}

void					
BF_GUI_Setup_Vars::Save()
{
	oLPanel.Clear();
	oRPanel.Clear();
 
	// fill vars//
	{
		ASSERT(poWin);
		oWinFrame = poWin->Frame();
		// left panel //
		BF_GUI_NodePanel *po = (BF_GUI_NodePanel*)poWinView->PanelByIndex(BF_PANEL_WINPOS_LEFT,0);		
		if(po)	po->StoreVars(oLPanel);
		// right panel //
		po = (BF_GUI_NodePanel*)poWinView->PanelByIndex(BF_PANEL_WINPOS_RIGHT,0);		
		if(po)	po->StoreVars(oRPanel);
	}
	//
	BL_String sFilePath(BEFAR_SETTINGS_DIR);
	sFilePath<<VARS_FILE;
	
	BF_ConfigFile oFile(sFilePath.String(),B_READ_WRITE|B_CREATE_FILE|B_ERASE_FILE);
	status_t uErr = oFile.InitCheck();
		
	if(uErr!=B_OK){
		BL_String s(B_TRANSLATE(BF_DICT_SETUP_ERR1)),s1;
		BL_System_TranslError(uErr,s1);
		s<<s1;
		BF_Dialog_Alert_Sep(s1.String(),"",NULL,poWin->CurrentFocus());
		return;
	}
			
	oFile.WriteVar("win_x",oWinFrame.left);
	oFile.WriteVar("win_y",oWinFrame.top);
	oFile.WriteVar("win_x1",oWinFrame.right);
	oFile.WriteVar("win_y1",oWinFrame.bottom);
	
	// save styles //
	SavePanel(oFile,oLPanel,true);
	SavePanel(oFile,oRPanel,false);
}



//////////////////////////////////////////////////////////////////////////
status_t				
BF_ConfigFile::WriteVar(const char *pc_Name,float f_Value)
{
	BL_String s(pc_Name);
	s<<"=";
	s<<f_Value;
	return WriteString(s);		
}

status_t				
BF_ConfigFile::WriteVar(const char *pc_Name,const char *pc_Value)
{
	BString s(pc_Name);
	s<<"=";
	s<<pc_Value;
	return WriteString(s);
}

status_t	
BF_ConfigFile::VarUnpack(BL_String & s_Var,BL_String & s_Name,BL_String & s_Value)
{
	int iPos = s_Var.FindFirst('=');
	if(iPos<=0) return B_ERROR;
	s_Var.CopyInto(s_Name,0,iPos);
	s_Var.CopyInto(s_Value,iPos+1,s_Var.Length()-iPos-1);
	return B_OK;
}


void					
BF_GUI_Setup_Color::operator = (BF_GUI_Setup_Color & o_From)
{
	rgb = o_From.rgb;
}

void					
BF_GUI_Setup_Color::Save(BF_ConfigFile & o_File)
{
	BL_String s;
	s<<sCode;
	s<<"=";
	s<<(uint32)rgb.red;
	s<<",";
	s<<(uint32)rgb.green;
	s<<",";
	s<<(uint32)rgb.blue;
	o_File.WriteString(&s);
}

void					
BF_GUI_Setup_Font::Set(const char *pc_Name,int i_Styles,int i_Flags,float f_Size)
{
	sName = pc_Name;
	iStyles = i_Styles;
	iFlags = i_Flags;
	fSize = f_Size;
	MakeFont();
}

int
BF_GUI_Setup_Font::StylesStrToInt(const char *pc_Styles)
{
	ASSERT(pc_Styles);
	
	int iStyles=0;
		
	if(strstr(pc_Styles,"Bold")!=NULL) iStyles = iStyles | B_BOLD_FACE;
	if(strstr(pc_Styles,"Regular")!=NULL) iStyles = iStyles | B_REGULAR_FACE;
	if(strstr(pc_Styles,"Italic")!=NULL) iStyles = iStyles | B_ITALIC_FACE;
	
	return iStyles;
}

void					
BF_GUI_Setup_Font::MakeFont()
{
	oFont.SetFamilyAndFace(sName.String(),iStyles);	
	// check font //
	{
		font_family oFamily;
		font_style	oStyle;
		oFont.GetFamilyAndStyle(&oFamily,&oStyle);
		if(sName!=oFamily) sName = oFamily;		
		iStyles = StylesStrToInt(oStyle);
	}
	oFont.SetSize(fSize);
	oFont.SetFlags( iFlags );
	CalcVars();
}

void					
BF_GUI_Setup_Font::CalcVars()
{
	font_height	oFontHeight;

	oFont.GetHeight(&oFontHeight);

	fAscent = oFontHeight.ascent;
	fDescent = oFontHeight.descent;		
	fHeight = (float)(int)(oFontHeight.ascent + oFontHeight.descent);
	if(fHeight<(oFontHeight.ascent + oFontHeight.descent)) fHeight+=1.0;
}

void					
BF_GUI_Setup_Font::operator = (BF_GUI_Setup_Font & o_From)
{
	oFont = o_From.oFont;
	fSize  = o_From.fSize;
	iStyles  = o_From.iStyles;
	iFlags = o_From.iFlags;
	sName = o_From.sName;
	
	fHeight = o_From.fHeight;
	fAscent = o_From.fAscent;
	fDescent = o_From.fDescent;
}

void					
BF_GUI_Setup_Font::Load(BF_ConfigFile & o_File)
{
	BL_String 	s,sVarName,sVarValue;		
	iStyles = 0;
	iFlags = 0;
	while(B_OK==o_File.ReadString(&s)){		
		if(s=="}") break;
		if(B_OK!=o_File.VarUnpack(s,sVarName,sVarValue)) break;
		//
		if(sVarName=="name"){
			sName = sVarValue;	
		}else
		if(sVarName=="size"){
			fSize = sVarValue.Float();			
		}else
		if(sVarName=="style"){
			if(sVarValue=="bold") iStyles = iStyles | B_BOLD_FACE;else
			if(sVarValue=="regular") iStyles = iStyles | B_REGULAR_FACE;else
			if(sVarValue=="italic") iStyles = iStyles | B_ITALIC_FACE;else
			{}
		}else
		if(sVarName=="flag"){
			if(sVarValue=="force_antialising")	iFlags =  iFlags | B_FORCE_ANTIALIASING;else
			if(sVarValue=="disable_antialising") iFlags =  iFlags | B_DISABLE_ANTIALIASING;else
			{}
		}else
		{}
	}
	// post_settings 
	if( !(iFlags & B_FORCE_ANTIALIASING) && !(iFlags & B_DISABLE_ANTIALIASING)) iFlags =  iFlags | B_FORCE_ANTIALIASING;
	MakeFont();
}

void					
BF_GUI_Setup_Font::Save(const char *pc_Type,BF_ConfigFile & o_File)
{
	o_File.WriteChar("font{");
	o_File.WriteChar(pc_Type);
	
	BL_String s;	
	s="name=";
	s<<sName;
	o_File.WriteString(&s);
	s="size=";
	s<<fSize;
	o_File.WriteString(&s);
	
	printf("iStyles=%i\n",iStyles);
	if(iStyles & B_BOLD_FACE) o_File.WriteChar("style=bold");
	if(iStyles & B_REGULAR_FACE) o_File.WriteChar("style=regular");
	if(iStyles & B_ITALIC_FACE) o_File.WriteChar("style=italic");			
	
	if(iFlags & B_FORCE_ANTIALIASING) o_File.WriteChar("flag=force_antialising");
	if(iFlags & B_DISABLE_ANTIALIASING) o_File.WriteChar("flag=disable_antialising");
	
	
	o_File.WriteChar("}");
}

BF_GUI_Setup::BF_GUI_Setup()
{
	InitDefault();	
	if(!poSysSetup)	poSysSetup = this;
}

void					
BF_GUI_Setup::InitDefault()
{
	iCmdLineHistorySize = 20;
	
	sDictFile = "BeFar_Dict_Eng.dict";
	
	/* path */
	oPathSysTrash.SetTo("/boot/trash");
	/* vars */
	iPathHistorySize = 100;	
	iFPanelFullStyle = 0;
	iFPanelFullStyle = iFPanelFullStyle | BF_SETUP_FPANELMODE_FULL_DRAWLINES;
	iFPanelColsStyle = 0;
	iFPanelColsStyle = iFPanelColsStyle | BF_SETUP_FPANELMODE_COLS_DRAWLINES;
	iMainStyle = 0;
	iMainStyle = iMainStyle | BF_SETUP_MAIN_RETURN_REMEMBER;
	iMainStyle = iMainStyle | BF_SETUP_MAIN_ASK_DELETE;
	iMainStyle = iMainStyle | BF_SETUP_MAIN_SELECTVOL_SHOWSICON;
	iMainStyle = iMainStyle | BF_SETUP_MAIN_SELECTVOL_SHOWSPEC;	
	iMainStyle = iMainStyle | BF_SETUP_MAIN_SELECTVOL_SHOWSTACK;	
	iMainStyle = iMainStyle | BF_SETUP_MAIN_FP_HEADERICON;
	iMainStyle = iMainStyle | BF_SETUP_MAIN_ASK_EXIT;
	iMainStyle = iMainStyle | BF_SETUP_MAIN_ASK_COPY;
	iMainStyle = iMainStyle | BF_SETUP_NODEPANEL_DESELECT_AFTER_COPY;
	iMainStyle = iMainStyle | BF_SETUP_NODEPANEL_DESELECT_AFTER_MOVE;	
	iMainStyle = iMainStyle | BF_SETUP_OPERROSTER_DEFDIALOG;
	iMainStyle = iMainStyle | BF_SETUP_CMDLINE_SHOWPATH;
	iMainStyle = iMainStyle | BF_SETUP_FILESPANEL_TRACKERADDONS_SHOWICONS;
	iMainStyle = iMainStyle | BF_SETUP_SEARCH_SETUP_VOLICONS;
	iMainStyle = iMainStyle | BF_SETUP_AUTOSAVE;
		
	
	/* colors */
	SetColor(BF_COLOR_BACK,B_TRANSLATE(BF_DICT_SETUPCOLOR_PANELBACK),"back",0,0,200);
	SetColor(BF_COLOR_FRAME,B_TRANSLATE(BF_DICT_SETUPCOLOR_PANELFRAME),"frame",82,255,255);
	SetColor(BF_COLOR_NODE,B_TRANSLATE(BF_DICT_SETUPCOLOR_PANELNODE),"node",82,255,255);
	SetColor(BF_COLOR_NODE_LINK,B_TRANSLATE(BF_DICT_SETUPCOLOR_PANELNODELINK),"node_link",255,200,200);
	SetColor(BF_COLOR_NODE_DIR,B_TRANSLATE(BF_DICT_SETUPCOLOR_PANELNODEDIR),"node_dir",255,255,255);
	SetColor(BF_COLOR_NODE_DIR_LINK,B_TRANSLATE(BF_DICT_SETUPCOLOR_PANELNODEDIRLINK),"node_dir_link",255,200,200);
	SetColor(BF_COLOR_PANEL_INFO_TEXT,B_TRANSLATE(BF_DICT_SETUPCOLOR_PANELINFOTEXT),"panel_info_text",82,255,255);
	SetColor(BF_COLOR_CURSOR_BACK,B_TRANSLATE(BF_DICT_SETUPCOLOR_PANELCURSORBACK),"cursor_back",0,170,189);
	SetColor(BF_COLOR_CURSOR_TEXT,B_TRANSLATE(BF_DICT_SETUPCOLOR_PANELCURSORTEXT),"cursor_text",0,0,0);
	SetColor(BF_COLOR_HEADER_TEXT,B_TRANSLATE(BF_DICT_SETUPCOLOR_PANELHEADERSTEXT),"header_text",0,170,189);
	SetColor(BF_COLOR_PATH_FOCUSTEXT,B_TRANSLATE(BF_DICT_SETUPCOLOR_PANELFOCUSPATHTEXT),"focuspath_text",0,0,0);
	SetColor(BF_COLOR_PATH_FOCUSBACK,B_TRANSLATE(BF_DICT_SETUPCOLOR_PANELFOCUSPATHBACK),"focuspath_back",0,170,189);
	SetColor(BF_COLOR_PATH_TEXT,B_TRANSLATE(BF_DICT_SETUPCOLOR_PANELPATHTEXT),"path_text",82,255,255);
	SetColor(BF_COLOR_PATH_BACK,B_TRANSLATE(BF_DICT_SETUPCOLOR_PANELPATHBACK),"path_back",0,0,200);
	SetColor(BF_COLOR_NODE_SELECT,B_TRANSLATE(BF_DICT_SETUPCOLOR_PANELNODESELECTING),"node_select",255,255,0);
	SetColor(BF_COLOR_NODE_FOCUSSELECT,B_TRANSLATE(BF_DICT_SETUPCOLOR_PANELNODEFOCUSSELECTING),"node_focusselect",255,255,0);

	SetColor(BF_COLOR_DIALOG_VIEWBORDER_FOCUSED,B_TRANSLATE(BF_DICT_SETUPCOLOR_DIALOGFOCUSEDBORDER),"viewwborder_focused",200,0,0);
	SetColor(BF_COLOR_DIALOG_VIEWBORDER_UNFOCUSED,B_TRANSLATE(BF_DICT_SETUPCOLOR_DIALOGUNFOCUSEDBAR),"viewwborder_unfocused",0,0,0);
		
	SetColor(BF_COLOR_DIALOG_BACK,B_TRANSLATE(BF_DICT_SETUPCOLOR_DIALOGBACK),"dialog_back",170,170,170);
	SetColor(BF_COLOR_DIALOG_TEXT,B_TRANSLATE(BF_DICT_SETUPCOLOR_DIALOGTEXT),"dialog_text",0,0,0);
	SetColor(BF_COLOR_DIALOG_MENU_ITEMSELECTED,B_TRANSLATE(BF_DICT_SETUPCOLOR_DIALOGMENU),"dialog_menu_itemselected",0,255,0);
	SetColor(BF_COLOR_DIALOG_EDIT_TEXT,B_TRANSLATE(BF_DICT_SETUPCOLOR_DIALOGEDITTEXT),"dialog_edit_text",0,0,0);
	SetColor(BF_COLOR_DIALOG_EDIT_BACK,B_TRANSLATE(BF_DICT_SETUPCOLOR_DIALOGEDITBACK),"dialog_edit_back",0,170,189);
	SetColor(BF_COLOR_DIALOG_BUTTON_FOCUS,B_TRANSLATE(BF_DICT_SETUPCOLOR_DIALOGBUTTONFOCUS),"dialog_button_focus",0,170,189);
	SetColor(BF_COLOR_DIALOG_BUTTON_TEXT,B_TRANSLATE(BF_DICT_SETUPCOLOR_DIALOGBUTTONTEXT),"dialog_button_text",0,0,0);
	SetColor(BF_COLOR_DIALOG_PROGRESS_EMPTY,B_TRANSLATE(BF_DICT_SETUPCOLOR_DIALOGPROGRESSBARE),"dialog_progress_empty",150,150,150);
	SetColor(BF_COLOR_DIALOG_PROGRESS_FILLED,B_TRANSLATE(BF_DICT_SETUPCOLOR_DIALOGPROGRESSBARF),"dialog_progress_filled",100,100,100);

	SetColor(BF_COLOR_RED,B_TRANSLATE(BF_DICT_SETUPCOLOR_DIALOGRED),"red_color",255,0,0);

	SetColor(BF_COLOR_CMDLINE_BACK,B_TRANSLATE(BF_DICT_SETUPCOLOR_COMMANDLINE_BACK),"cmdline_back",0,0,0);
	SetColor(BF_COLOR_CMDLINE_TEXT,B_TRANSLATE(BF_DICT_SETUPCOLOR_COMMANDLINE_TEXT),"cmdline_text",82,255,255);

	SetColor(BF_COLOR_TEXTVIEWER_STATUS_TEXT,B_TRANSLATE(BF_DICT_SETUPCOLOR_TEXTVIEWER1),"textviewer_statusbar_text",0,0,0);
	SetColor(BF_COLOR_TEXTVIEWER_STATUS_BACK,B_TRANSLATE(BF_DICT_SETUPCOLOR_TEXTVIEWER2),"textviewer_statusbar_back",0,170,189);
	SetColor(BF_COLOR_TEXTVIEWER_TEXT,B_TRANSLATE(BF_DICT_SETUPCOLOR_TEXTVIEWER3),"textviewer_text",255,255,255);
	SetColor(BF_COLOR_TEXTVIEWER_BACK,B_TRANSLATE(BF_DICT_SETUPCOLOR_TEXTVIEWER4),"textviewer_back",0,0,200);
	
	/* fonts */
	{
		oFontNode.Set("Tahoma",B_REGULAR_FACE,B_FORCE_ANTIALIASING,11.0);
		oFontMono.Set("Courier New",B_REGULAR_FACE,B_FORCE_ANTIALIASING,11.0);		
		oFontToolView = oFontNode;		
		oFontCMDLine  = oFontNode;
	}
	/* finish init */
}

void					
BF_GUI_Setup::SetColor(int i_Index,const char* i_DictCode,const char *pc_Code,uint8 i_R,uint8 i_G,uint8 i_B,uint8 i_A)
{	
	RGBA_SET(aoColor[i_Index].rgb,i_R,i_G,i_B,i_A);
	aoColor[i_Index].iDictCode = i_DictCode;
	aoColor[i_Index].sCode = pc_Code;
}

uint32					
BF_GUI_Setup::MainStyle()
{	return iMainStyle; }

uint32					
BF_GUI_Setup::FPanelFullStyle()
{ return iFPanelFullStyle;}
uint32
BF_GUI_Setup::FPanelColsStyle()
{ return iFPanelColsStyle;}

void
BF_GUI_Setup::SetMainStyle(uint32 i_Style)
{
	iMainStyle = i_Style;
}

void					
BF_GUI_Setup::UpdateFrom(BF_GUI_Setup & o_Src)
{
	o_Src.CopyTo(*this);
}

void					
BF_GUI_Setup::CopyTo(BF_GUI_Setup & o_Res)
{
	// copy styles
	o_Res.iFPanelFullStyle = iFPanelFullStyle;
	o_Res.iFPanelColsStyle = iFPanelColsStyle;
	o_Res.iMainStyle = iMainStyle;
	// copy colors
	for(int iCol=0;iCol<BF_COLOR_END;iCol++){
		o_Res.aoColor[iCol] = aoColor[iCol];
	}
	// copy trash_path
	o_Res.oPathSysTrash = oPathSysTrash;
	// copy fonts
	o_Res.oFontNode = oFontNode;
	o_Res.oFontToolView = oFontToolView;
	o_Res.oFontMono = oFontMono;
	o_Res.oFontCMDLine = oFontCMDLine;
	// copy others
	o_Res.iPathHistorySize = iPathHistorySize;
}

BF_ConfigFile::BF_ConfigFile(const char *pc_Path, uint32 i_OpenMode)
:BL_File(pc_Path,i_OpenMode)
{}

status_t
BF_ConfigFile::WriteStyle(uint32 i_Styles,uint32 i_Style,const char *pc_Text)
{
	if(!(i_Styles & i_Style))  return B_OK;
	return WriteChar(pc_Text);	
}

bool					
BF_ConfigFile::IsLineVar(const char *pc_VarName)
{
	ASSERT(pc_VarName);
	return sLine.FindFirst(pc_VarName)==0;
}

status_t				
BF_ConfigFile::ReadLine()
{
	return ReadString(&sLine);
}

bool					
BF_ConfigFile::VarTo(BL_String & s_Var)
{
	int i=sLine.FindFirst("=");
	if(i<0) return false;
	sLine.CopyInto(s_Var,i+1,sLine.Length()-i);
	return true;
}

status_t				
BF_ConfigFile::ReadColor(BF_GUI_Setup_Color &o_Color)
{
	BL_String s,s1;
	status_t uRes = ReadString(&s);
	if(B_OK!=uRes) return uRes;
	if(s=="}") return B_ERROR;
	
	// code
	int i=s.FindFirst("=");
	if(i<0) return B_ERROR;
	s.CopyInto(o_Color.sCode,0,i);
	i++;
	
	// red
	int i1=s.FindFirst(",",i);
	if(i1<0) return B_ERROR;
	s.CopyInto(s1,i,i1-i);
	o_Color.rgb.red=s1.Int32();
	i = i1+1;
		
	// green
	i1=s.FindFirst(",",i);
	if(i1<0) return B_ERROR;
	s.CopyInto(s1,i,i1-i);
	o_Color.rgb.green=s1.Int32();
	i = i1+1;
	
	// blue	
	if(i1<0) return B_ERROR;
	s.CopyInto(s1,i,s.Length()-i);
	o_Color.rgb.blue=s1.Int32();
	
	return B_OK;
}

void
BF_GUI_Setup::Save_Colors(BF_ConfigFile & o_File)
{
	o_File.WriteChar("colors{");
	for(int iColor=0;iColor<BF_COLOR_END;iColor++){
		aoColor[iColor].Save(o_File);
	}
	o_File.WriteChar("}");
}

void
BF_GUI_Setup::Save_Fonts(BF_ConfigFile & o_File)
{
	oFontNode.Save("node",o_File);
	oFontToolView.Save("tool",o_File);
	oFontMono.Save("mono",o_File);
	oFontCMDLine.Save("cmdline",o_File);		
}

void
BF_GUI_Setup::Load(bool b_ShowErrors)
{	
	BL_String s,s1;	
	// load main setup
	
	s="";
	s<<BEFAR_SETTINGS_DIR;
	s<<"main.settings";
	
	BF_ConfigFile oFile(s.String(),B_READ_ONLY);
	status_t uErr = oFile.InitCheck();
		
	if(B_OK==uErr){	
		while(B_OK==oFile.ReadLine()){						
			// load setup_version //
			if(oFile.IsLineVar("settings_version")) continue;else
			// load dict_file_name //
			if(oFile.IsLineVar("dict_file=")){
				oFile.VarTo(sDictFile);
			}else
			// load setup //		
			if(oFile.IsLineVar("main_style")){
				iMainStyle=0;
				while(B_OK==oFile.ReadLine() && oFile.sLine!="}"){
					s = oFile.sLine;
					if(s=="RETURN_REMEMBER") iMainStyle = iMainStyle | BF_SETUP_MAIN_RETURN_REMEMBER;else
					if(s=="ASK_DELETE") iMainStyle = iMainStyle | BF_SETUP_MAIN_ASK_DELETE;else
					if(s=="ASK_COPY") iMainStyle = iMainStyle | BF_SETUP_MAIN_ASK_COPY;else				
					if(s=="ASK_EXIT") iMainStyle = iMainStyle | BF_SETUP_MAIN_ASK_EXIT;else
					if(s=="SELECTVOL_SHOWSICON") iMainStyle = iMainStyle | BF_SETUP_MAIN_SELECTVOL_SHOWSICON;else
					if(s=="SELECTVOL_SHOWSPEC") iMainStyle = iMainStyle | BF_SETUP_MAIN_SELECTVOL_SHOWSPEC;else	
					if(s=="SELECTVOL_SHOWSTACK") iMainStyle = iMainStyle | BF_SETUP_MAIN_SELECTVOL_SHOWSTACK;else	
					if(s=="FP_HEADERICON") iMainStyle = iMainStyle | BF_SETUP_MAIN_FP_HEADERICON;else	
					if(s=="NODEPANEL_DESELECT_AFTER_COPY") iMainStyle = iMainStyle | BF_SETUP_NODEPANEL_DESELECT_AFTER_COPY;else	
					if(s=="NODEPANEL_DESELECT_AFTER_MOVE") iMainStyle = iMainStyle | BF_SETUP_NODEPANEL_DESELECT_AFTER_MOVE;else	
					if(s=="NODEPANEL_USE_BACKSPACE") iMainStyle = iMainStyle | BF_SETUP_NODEPANEL_USE_BACKSPACE;else					
					if(s=="OPERROSTER_DEFDIALOG") 		iMainStyle = iMainStyle | BF_SETUP_OPERROSTER_DEFDIALOG;else	
					if(s=="CMDLINE_SHOWPATH")	iMainStyle = iMainStyle | BF_SETUP_CMDLINE_SHOWPATH;else	
					if(s=="FILESPANEL_TRACKERADDONS_SHOWICONS")	iMainStyle = iMainStyle | BF_SETUP_FILESPANEL_TRACKERADDONS_SHOWICONS;else
					if(s=="SETUP_SEARCH_SETUP_VOLICONS")	iMainStyle = iMainStyle | BF_SETUP_SEARCH_SETUP_VOLICONS;else
					if(s=="SETUP_AUTOSAVE")	iMainStyle = iMainStyle | BF_SETUP_AUTOSAVE;	else
					
					if(s=="SETUP_DEB_NOMONITORING")	iMainStyle = iMainStyle | BF_SETUP_DEB_NOMONITORING;	else
					if(s=="BF_SETUP_DEB_NOSORTING") iMainStyle = iMainStyle | BF_SETUP_DEB_NOSORTING;		else
					{}
				}
			}else
			// load colors
			if(oFile.IsLineVar("colors{")){
				LoadColors(oFile);
			}else
			// load fonts 
			if(oFile.IsLineVar("font{")){
				if(B_OK!=oFile.ReadString(&s)) break;
				if(s=="node")	oFontNode.Load(oFile);else
				if(s=="tool")	oFontToolView.Load(oFile);else
				if(s=="mono")	oFontMono.Load(oFile);else
				if(s=="cmdline")oFontCMDLine.Load(oFile);else
				{}
			}else
			{}
		}	
	}
	
	// load vars //
	oVars.Load();
}

void
BF_GUI_Setup::Save()
{
	if(B_OK!=BF_Roster_MakeDir(BEFAR_SETTINGS_DIR)){
		BF_Dialog_Alert_Sep(B_TRANSLATE(BF_DICT_SETUP_ERR2),"",NULL,poWin->CurrentFocus());
		return; 
	}
	/* create  file */
	BL_String 	s,s1;
	status_t	uErr;		
	s = BEFAR_SETTINGS_DIR;
	s<<"main.settings";
	
	BF_ConfigFile oFile(s.String(),B_READ_WRITE|B_CREATE_FILE|B_ERASE_FILE);
	uErr = oFile.InitCheck();
		
	if(uErr!=B_OK){
		s = B_TRANSLATE( BF_DICT_SETUP_ERR2 );
		BL_System_TranslError(uErr,s1);
		s<<s1;
		BF_Dialog_Alert_Sep(s1.String(),"",NULL,poWin->CurrentFocus());
		return;
	}
	// save setup_version //
	oFile.WriteChar("settings_version=1");
	// set current dictionary_name //
	oFile.WriteVar("dict_file",sDictFile.String());	
	
	/* save styles */
	oFile.WriteChar("main_style{");
	oFile.WriteStyle(iMainStyle,BF_SETUP_MAIN_RETURN_REMEMBER,"RETURN_REMEMBER");
	oFile.WriteStyle(iMainStyle,BF_SETUP_MAIN_ASK_DELETE,"ASK_DELETE");
	oFile.WriteStyle(iMainStyle,BF_SETUP_MAIN_ASK_EXIT,"ASK_EXIT");
	oFile.WriteStyle(iMainStyle,BF_SETUP_MAIN_ASK_COPY,"ASK_COPY");
	oFile.WriteStyle(iMainStyle,BF_SETUP_MAIN_SELECTVOL_SHOWSICON,"SELECTVOL_SHOWSICON");
	oFile.WriteStyle(iMainStyle,BF_SETUP_MAIN_SELECTVOL_SHOWSPEC,"SELECTVOL_SHOWSPEC");
	oFile.WriteStyle(iMainStyle,BF_SETUP_MAIN_SELECTVOL_SHOWSTACK,"SELECTVOL_SHOWSTACK");
	oFile.WriteStyle(iMainStyle,BF_SETUP_MAIN_FP_HEADERICON,"FP_HEADERICON");
	oFile.WriteStyle(iMainStyle,BF_SETUP_NODEPANEL_DESELECT_AFTER_COPY,"NODEPANEL_DESELECT_AFTER_COPY");			
	oFile.WriteStyle(iMainStyle,BF_SETUP_NODEPANEL_DESELECT_AFTER_MOVE,"NODEPANEL_DESELECT_AFTER_MOVE");
	oFile.WriteStyle(iMainStyle,BF_SETUP_NODEPANEL_USE_BACKSPACE,"NODEPANEL_USE_BACKSPACE");	
	oFile.WriteStyle(iMainStyle,BF_SETUP_OPERROSTER_DEFDIALOG,"OPERROSTER_DEFDIALOG");
	oFile.WriteStyle(iMainStyle,BF_SETUP_CMDLINE_SHOWPATH,"CMDLINE_SHOWPATH");	
	oFile.WriteStyle(iMainStyle,BF_SETUP_FILESPANEL_TRACKERADDONS_SHOWICONS,"FILESPANEL_TRACKERADDONS_SHOWICONS");		
	oFile.WriteStyle(iMainStyle,BF_SETUP_SEARCH_SETUP_VOLICONS,"SETUP_SEARCH_SETUP_VOLICONS");			
	oFile.WriteStyle(iMainStyle,BF_SETUP_AUTOSAVE,"SETUP_AUTOSAVE");			
	
	oFile.WriteStyle(iMainStyle,BF_SETUP_DEB_NOMONITORING,"SETUP_DEB_NOMONITORING");			
	oFile.WriteStyle(iMainStyle,BF_SETUP_DEB_NOSORTING,"SETUP_DEB_NOSORTING");				
	
	oFile.WriteChar("}");
	/* save colors */
	Save_Colors(oFile);
	/* save fonsts */
	Save_Fonts(oFile);
	/* save vars */
	oVars.Save();
}


void
BF_GUI_Setup::LoadPalete(const char *pc_FileName)
{
	ASSERT(pc_FileName);
	//
	BL_String 	s,s1;
	s = BEFAR_SETTINGS_DIR;
	s<<"paletes/";
	s<<pc_FileName;
	
	BF_ConfigFile oFile(s.String(),B_READ_WRITE);
	status_t uErr = oFile.InitCheck();
	
	if(uErr!=B_OK){
		s = B_TRANSLATE(BF_DICT_SETUP_ERR4);
		BL_System_TranslError(uErr,s1);
		s<<s1;
		BF_Dialog_Alert_Sep(s1.String(),"",NULL,poWin->CurrentFocus());
		return;
	}							
	//
	oFile.ReadString(&s);
	if(s!="colors{") return;	
	//	
	LoadColors(oFile);
	//
}

void
BF_GUI_Setup::LoadColors(BF_ConfigFile & o_File)
{
	BF_GUI_Setup_Color oCol;
	while(B_OK==o_File.ReadColor(oCol)){		
		SetColorByCode(oCol);
	}
}

int32	
BF_GUI_Setup::ColorIndexByCode(const char *pc_Code)
{
	ASSERT(pc_Code);
	for(int iCol=0;iCol<BF_COLOR_END;iCol++){
		if(aoColor[iCol].sCode==pc_Code) return iCol;
	}
	return -1;
}

void
BF_GUI_Setup::SetColorByCode(BF_GUI_Setup_Color & o_ColFrom)
{
	int32 iColIndex = ColorIndexByCode(o_ColFrom.sCode.String());
	if(iColIndex<0) return;
	aoColor[iColIndex] = o_ColFrom;
	
}

void
BF_GUI_Setup::SavePalete(const char *pc_FileName)
{
	// yes, save now //
	BL_String 	s,s1;
	s = BEFAR_SETTINGS_DIR;
	s<<"paletes";
	if(B_OK!=BF_Roster_MakeDir(s.String())){
		BF_Dialog_Alert_Sep(B_TRANSLATE(BF_DICT_SETUP_ERR2),"",NULL,poWin->CurrentFocus());
		return; 
	}
	/* create  file */		
	status_t	uErr;			
	s<<"/";
	s<<pc_FileName;
	
	BF_ConfigFile oFile(s.String(),B_READ_WRITE|B_CREATE_FILE|B_ERASE_FILE);
	uErr = oFile.InitCheck();
			
	if(uErr!=B_OK){
		s = B_TRANSLATE(BF_DICT_SETUP_ERR5);
		BL_System_TranslError(uErr,s1);
		s<<s1;
		BF_Dialog_Alert_Sep(s1.String(),"",NULL,poWin->CurrentFocus());
		return;
	}			

	Save_Colors(oFile);		
}
