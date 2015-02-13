#ifndef __BF_GUI_SETUP_H__
#define __BF_GUI_SETUP_H__
/* includes */
#include <Font.h>
#include "BF_Node.h"
#include "BL_File.h"
//#include "BF_GUI_NodePanel.h"
/* */

class BF_GUI_Setup_Color;

#define BEFAR_SETTINGS_DIR	(const char*)"/boot/home/config/settings/BeFAR/"

class BF_ConfigFile:public BL_File{
public:
		BL_String				sLine;
		
								BF_ConfigFile(const char *pc_Path, uint32 i_OpenMode);
		status_t				WriteStyle(uint32 i_Styles,uint32 i_Style,const char *pc_Text);		
		status_t				ReadColor(BF_GUI_Setup_Color &o_Color);
		status_t				VarUnpack(BL_String & s_Var,BL_String & s_Name,BL_String & s_Value);
		
		status_t				WriteVar(const char *pc_Name,const char *pc_Value);
		status_t				WriteVar(const char *pc_Name,float f_Value);	
		
		status_t				ReadLine();

		bool					IsLineVar(const char *pc_VarName);
		bool					VarTo(BL_String & s_Var);
};

class BF_GUI_Setup_Vars_Panel:public BL_Object{
public:
		BL_String				sPath;
		uint32					iModeFullStyles,iModeColsStyles,iMode;		
				
								BF_GUI_Setup_Vars_Panel();
		void					Clear();
		void					Load(BF_ConfigFile &o_File);
};


class BF_GUI_Setup_Vars:public BL_Object{
public:			
		BF_GUI_Setup_Vars_Panel	oLPanel,oRPanel;
		BRect					oWinFrame;		
						
								BF_GUI_Setup_Vars();
								
		void					Save();
		status_t				Load();
		bool					Loaded(){return bLoaded;};
		
private:
		void					SavePanel(BF_ConfigFile &o_File,const BF_GUI_Setup_Vars_Panel & o_Panel,bool b_LeftPanel);
		bool					bLoaded;
};

class BF_GUI_Setup_Font:public BL_Object{
public:
		BL_String				sName;
		int						iStyles;
		int						iFlags;
		float 					fSize;
		
		float					fHeight;
		float					fAscent,fDescent;		
		BFont					oFont;
								
		void					operator = (BF_GUI_Setup_Font & o_From);
		void					Set(const char *pc_Name,int i_Styles,int i_Flags,float f_Size);		
		void					Save(const char *pc_Type,BF_ConfigFile & o_File);
		void					Load(BF_ConfigFile & o_File);		
		
		int						StylesStrToInt(const char *pc_Styles);
private:		
		void					CalcVars();
		void					MakeFont();
};

class BF_GUI_Setup_Color:public BL_Object{
public:
		rgb_color				rgb;
		BL_String				sCode;
		const char*				iDictCode;
								BF_GUI_Setup_Color(){RGB_SET(rgb,0,0,0);iDictCode=NULL;};								
		void					operator = (BF_GUI_Setup_Color & o_From);
		void					Save(BF_ConfigFile & o_File);
};

class BF_GUI_Setup;

enum{
	BF_COLOR_BACK=0,
	BF_COLOR_FRAME,	
	BF_COLOR_NODE,
	BF_COLOR_NODE_LINK,
	BF_COLOR_NODE_DIR,	
	BF_COLOR_NODE_DIR_LINK,		
	BF_COLOR_CURSOR_BACK,
	BF_COLOR_CURSOR_TEXT,
	BF_COLOR_HEADER_TEXT,
	BF_COLOR_PANEL_INFO_TEXT,
	BF_COLOR_PATH_FOCUSTEXT,
	BF_COLOR_PATH_FOCUSBACK,
	BF_COLOR_PATH_TEXT,
	BF_COLOR_PATH_BACK,
	BF_COLOR_NODE_SELECT,
	BF_COLOR_NODE_FOCUSSELECT,
	BF_COLOR_DIALOG_BACK,	
	BF_COLOR_DIALOG_TEXT,	
	BF_COLOR_DIALOG_MENU_ITEMSELECTED,	
	BF_COLOR_DIALOG_EDIT_TEXT,	
	BF_COLOR_DIALOG_EDIT_BACK,	
	BF_COLOR_DIALOG_BUTTON_FOCUS,	
	BF_COLOR_DIALOG_BUTTON_TEXT,	
	BF_COLOR_DIALOG_PROGRESS_EMPTY,
	BF_COLOR_DIALOG_PROGRESS_FILLED,
	BF_COLOR_DIALOG_VIEWBORDER_FOCUSED,
	BF_COLOR_DIALOG_VIEWBORDER_UNFOCUSED,
	BF_COLOR_RED,  
	BF_COLOR_CMDLINE_BACK,  
	BF_COLOR_CMDLINE_TEXT,
	BF_COLOR_TEXTVIEWER_STATUS_BACK,
	BF_COLOR_TEXTVIEWER_STATUS_TEXT,
	BF_COLOR_TEXTVIEWER_TEXT,	
	BF_COLOR_TEXTVIEWER_BACK,	
	BF_COLOR_END
};

#define BF_SETUP_FPANELMODE_FULL_DRAWLINES 	1

#define BF_SETUP_FPANELMODE_COLS_DRAWLINES 	1

#define BF_SETUP_MAIN_RETURN_REMEMBER 				0x0000001
#define BF_SETUP_MAIN_ASK_DELETE 					0x0000002
#define BF_SETUP_MAIN_ASK_EXIT	 					0x0000004
#define BF_SETUP_MAIN_ASK_COPY	 					0x0000008
#define BF_SETUP_MAIN_SELECTVOL_SHOWSICON 			0x0000010
#define BF_SETUP_MAIN_SELECTVOL_SHOWSPEC 			0x0000020
#define BF_SETUP_MAIN_SELECTVOL_SHOWSTACK 			0x0000040
#define BF_SETUP_SEARCH_SETUP_VOLICONS				0x0000080
#define BF_SETUP_MAIN_FP_HEADERICON					0x0000100
#define BF_SETUP_NODEPANEL_DESELECT_AFTER_COPY		0x0000200
#define BF_SETUP_NODEPANEL_DESELECT_AFTER_MOVE		0x0000400
#define BF_SETUP_NODEPANEL_USE_BACKSPACE			0x0000800
#define BF_SETUP_OPERROSTER_DEFDIALOG				0x0001000
#define BF_SETUP_CMDLINE_SHOWPATH					0x0002000
#define BF_SETUP_FILESPANEL_TRACKERADDONS_SHOWICONS	0x0004000
#define BF_SETUP_AUTOSAVE							0x0008000

#define BF_SETUP_DEB_NOMONITORING					0x0100000
#define BF_SETUP_DEB_NOSORTING						0x0200000

class BF_GUI_Setup_FilePanel{	
public:
		
		
		BF_GUI_Setup			*poSetup;
		
								BF_GUI_Setup_FilePanel();
		void					Calc(float f_Width);
};

class BF_GUI_Setup:BArchivable{
public:
								BF_GUI_Setup();
		
		void					InitDefault();		
		
		uint32					MainStyle();		
		uint32					FPanelFullStyle();
		uint32					FPanelColsStyle();
		
		void					SetMainStyle(uint32 i_Style);
		
		void					SetColor(int i_Index,const char*	i_DictCode,const char *pc_Code,uint8 i_R,uint8 i_G,uint8 i_B,uint8 i_A=255);
		int32					ColorIndexByCode(const char *pc_Code);
		void					SetColorByCode(BF_GUI_Setup_Color & o_ColFrom);
		
		void					CopyTo(BF_GUI_Setup & o_Res);
		void					UpdateFrom(BF_GUI_Setup & o_Src);		
		
		void					Save();
		void					Load(bool b_ShowErrors=true);		
		void					SavePalete(const char *pc_FileName);
		void					LoadPalete(const char *pc_FileName);
		void					LoadColors(BF_ConfigFile & o_File);		
		void					Save_Fonts(BF_ConfigFile & o_File);
		
		
		BF_GUI_Setup_Font		oFontNode,oFontToolView,oFontMono,oFontCMDLine;		
		BF_GUI_Setup_Color		aoColor[BF_COLOR_END];
		BF_GUI_Setup_Vars 		oVars;
		
		int32					iPathHistorySize;
		int32					iCmdLineHistorySize;		
		
		BF_Path					oPathSysTrash;
		
		BL_String				sDictFile;
private:
		uint32					iFPanelFullStyle,iFPanelColsStyle,iMainStyle;		
		
		void					Save_Colors(BF_ConfigFile & o_File);
			
};

#define SYS_COLOR(index) poSysSetup->aoColor[index].rgb

extern BF_GUI_Setup *poSysSetup;


#endif
