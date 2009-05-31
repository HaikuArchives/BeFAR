#ifndef __BF_GUI_TEXTVIEWER__
#define __BF_GUI_TEXTVIEWER__

#include <InterfaceKit.h>
#include <List.h>
#include "BL_Tools.h"
#include "BF_GUI_Setup.h"

enum {
	CP_UTF8 = 0,
	CP_DOS,
	CP_WIN,
	CP_KOI,
	CP_END,
};

class BF_TextFile_Line:public BL_Object{
public:
							BF_TextFile_Line(const BL_String & s_Src,off_t i_Pos,int32 i_Bytes);
		void				SetBytesTo(int32 i_Bytes)	{iBytes = i_Bytes;};
		int32				Bytes()const				{return iBytes;};
		off_t				Pos()const					{return iPos;};
		
		const char* 		String()const				{return s.String();};
		
		BL_String			s;
private:
		int32				iBytes;		
		off_t				iPos;
};

class BF_TextFile_Buffer:public BL_Object{
public:
							BF_TextFile_Buffer();
							
		void				SetFileNameTo(const char *pc_FileName);
		
		int32				LoadText(off_t i_StartPos,int32 i_LinesCount,BL_List & lo_Data,bool b_DeleteData=true,off_t i_LastPos=-1);
		int32				LoadText_FromNextLine(off_t & i_StartPos,int32 i_LinesCount,BL_List & lo_Data,bool b_DeleteFirstLines=true);
		int32 				LoadText_BeforePos(off_t & i_LastPos,int32 i_LinesCount,BL_List & lo_Data);
		
		off_t				FileSize();
private:	
		BL_String			sFileName;		
		BL_File 			oFile;				
		off_t				iFileSize;
		
		status_t			OpenFile();
		void				CloseFile();
};

class BF_GUI_TextViewer_StatusBar : public BView{
public:
							BF_GUI_TextViewer_StatusBar(BRect & o_Rect,const char *pc_FileName);
		void				SetVarsTo(off_t i_FilePos,off_t i_PageSize,off_t i_FileSize,int32 i_CodePage,int32 i_StartColumn);
virtual void				Draw(BRect o_Rect);		
private:		
		off_t				iFilePos,iPageSize,iFileSize;		
		BL_String			sFileName;
		int32				iCodePage,iStartColumn;
};


class BF_GUI_TextViewer : public BView
{
public:

							BF_GUI_TextViewer(BRect o_frame,const char *pcFileForView);
							~BF_GUI_TextViewer();
							
		
						
virtual void				Draw(BRect o_Rect);
virtual	void				MouseDown(BPoint point);
virtual void 				MakeFocus(bool b_Focused = true);
virtual void 				MessageReceived(BMessage* po_Message);
virtual void 				KeyDown(const char *bytes, int32 numBytes);

private:
		BF_TextFile_Buffer	oBuffer;

		BView				*poLastFocus;
		BL_List				loData;
		BL_String			sFileName;
		int32				iCodePage,iLinesCount,iStartColumn;
		off_t				iFilePos,iFileSize;
		float 				fFontHeight;
		bool				bWrapped,bShowSpec;
		
		int32				iSearchPos,iSearchLine,iSearchPosInLine;
		BL_String			sSearchText;
		
		char				pcConvertBuffer[0x1000];
		
		void				CalcVars();
		void				UpdateStatusBar();
		
		status_t			OpenFile(BL_File & o_File);
		void				LoadPage();
		
		void				PrepareKeysMenu();
		void				CloseViewer();
		
		bool				Nav_LineUp();
		bool				Nav_LineDown();
		bool				Nav_PageDown();
		bool				Nav_PageUp();
		bool				Nav_End();
		void				Nav_GoPos(off_t i_NewFilePos);
		bool				Nav_Right();
		bool				Nav_Left();

virtual	void				Search_Step0();
virtual	void				Search_Step1(BMessage & o_Message);
virtual	void				Search_Run();		

		void				ChangeCodePage();
virtual void				ToUtf8(const char *pc_Source,char *pc_Dest);
virtual void				FromUtf8(const char *pc_Source,char *pc_Dest);
virtual uint32				GetConversion( void );
		
		void				DrawLine(BView *po_Render,int i_Line,bool b_InitRender,bool b_DrawBackground,BL_String & s);
		
};


void  BF_GUI_TextViewer_Run(const char *pc_File);


#endif
