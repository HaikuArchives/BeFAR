#ifndef _BF_TOOLSVIEW_
#define _BF_TOOLSVIEW_

#include <InterfaceKit.h>
#include <List.h>
#include "BF_GUI_Setup.h"
/*
enum {
	CP_UTF8 = 0,
	CP_DOS,
	CP_WIN,
	CP_KOI
};
*/
class BF_GUI_Status : public BView
{
public:
	
						BF_GUI_Status(BRect o_frame,BF_GUI_Setup *po_Setup, char *pc_FileForView);
		
virtual void 			SetPos(float f_Pos);
virtual void 			SetCodePage(uint32 i_CodePage);
virtual void 			MessageReceived(BMessage* po_Message);
		
private:
	
		BF_GUI_Setup	*poSetup;
		BStringView		*poPos;
		BStringView		*poCode;
};

class BF_GUI_Text : public BView
{
public:
		uint32			iKeysModifiers;
	
						BF_GUI_Text(BRect o_frame,
								BList *plo_String, BF_GUI_Status *po_Status, BHandler *po_Handler);
						~BF_GUI_Text();
						
virtual void			DrawPage();
virtual void			Draw(BRect oRect);
		void			MouseDown(BPoint point);

virtual void			Nav_LineDown();
virtual void			Nav_LineUp();
virtual void			Nav_LineLeft();
virtual void			Nav_LineRight();
virtual void			Nav_PageUp();
virtual void			Nav_PageDown();

virtual void			NextCodePage();

		void			PrepareKeysMenu();

virtual void			FindFirst(char *pc_Sample);
virtual void			FindNext(void);
virtual void			ClearMatch( void );
virtual char			*GetMatch( void );

private:
		BList			*ploString;
		BF_GUI_Status	*poStatus;
		bool 			bFullPage;
		int				iCodePage; 
		BHandler		*poHandler;
		int32			iMatchLine,iStartSel,iStopSel;
		bool			bDrawSel;
		BString			*poMatch;
		
virtual void			ToUtf8(const char *pc_Source,char *pc_Dest);
virtual void			FromUtf8(const char *pc_Source,char *pc_Dest);
virtual uint32			GetConversion( void );
virtual void			Find(char *pc_Sample, uint32 i_StartLine = 0, uint32 i_StartCol = 0);
virtual void			ClearView( void ); 
};


#endif
