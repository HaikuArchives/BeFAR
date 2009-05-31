#ifndef __BL_TOOLS_H__
#define __BL_TOOLS_H__

#include <GraphicsDefs.h>
#include <SupportDefs.h>
#include <Message.h>

void
Debug_Error(const char * pc_Message, status_t err=B_ERROR);

void
Debug_Info(const char * pc_Message);

void
Debug_Alert(const char * pc_Message);

#define DPRINT(c) Debug_Info(c)


void
ASSERT(bool b_True,const char *pc_Message=NULL);

#define DEBUG_INFO(message) Debug_Info(message)


#define RGB_SET(c,r,g,b) {c.red=r;c.green=g;c.blue=b;c.alpha=255;}
#define RGBA_SET(c,r,g,b,a) {c.red=r;c.green=g;c.blue=b;c.alpha=a;}

// Nexus 16.11.99 - Safe delete macro
#define DELETE(_p)	{if(_p) delete _p; _p = NULL;}

class BL_String;

class BL_Object{
public:
					BL_Object(){};
virtual				~BL_Object(){};
};

#include <List.h>

#define BL_COLLECTION_ITEMS_IS_VOID			0
#define BL_COLLECTION_ITEMS_IS_STRING 		1
#define BL_COLLECTION_ITEMS_IS_LIST		 	2
#define BL_COLLECTION_ITEMS_IS_BLOBJECT	 	3

class BL_Collection:public BList{
public:
		 			BL_Collection(bool b_DeleteItems=false);
virtual	 			~BL_Collection();
		void		DeleteItems();		
		void		RemoveList(BL_Collection &lo_Item);
		void		RemoveList(BL_Collection *plo_Item);
		void		DeleteItem(void *pu_Item);
		void		DeleteItemAt(int i_Index);
		
virtual	bool		AddItem(void *pu_Item);
		bool		AddItem(BString * ps_Item);
		bool		AddItem(BList * pl_Item);
		bool		AddItem(BL_Object * po_Item);
virtual bool		AddList(BList *pl_List);
virtual	bool		AddList(BL_Collection *pl_List);
virtual	bool		AddList(BL_Collection *pl_List,int i_Index);

virtual bool		AddItem(void *pu_Item, int32 atIndex);
virtual bool		AddItem(BL_Object *po_Item, int32 atIndex);
virtual bool		AddItem(BString *ps_Item, int32 atIndex);

		BL_String*	StringAt(int i_Index);

		void*		LastItem();
		void*		FirstItem();
		
protected:
		bool		bDeleteItems;		 			
		int			iItemsType;
};

class BL_List:	public BL_Collection{
public:
		 			BL_List(bool b_DeleteItems=true);
		 			
};

#include <String.h>

class BL_String:public BString{
public:
					BL_String(const char *pc_Text);
					BL_String(const BString & s_Source);
					BL_String();
virtual				~BL_String();

		int			ComparePos(BL_String & s_Friend,int i_SelfPos,int i_FriendPos);
virtual	void		operator=(const char* pc_Value);
virtual	void		operator=(int64 i_Value);
virtual	void		operator=(BString & s_Value);
		int32		CountChar(char c_Value);
		BL_String*	SetDigits(char c_Digit=',',char c_DecPointChar='.');
		
		int32		Int32();
		float		Float();
		
		void		RTrim(char c_Byte=' ');
		void		LTrim(char c_Byte=' ');		
			
		status_t	GetFromMessage(BMessage *po_Message,const char *pc_Name);
		
		int32		LengthUTF8() const; 
		void		RemoveUTF8(int32 i_FromChar, int32 i_CharsCount);
		void		AppendUTF8(const char *source, int32 i_CharsCount);
		const char* StringUTF8(int32 i_FromChar)const;
		void		InsertUTF8(const char *pc_Value, int32 i_CharsCount,int32 i_CharPos);
		
};

int32   PCharCharPosInBytes_UTF8(const char *pc_Source,int32 i_Char);
int32 	PCharCharsInBytes_UTF8(const char *pc_Source,int32 i_Chars);
char* 	PCharFindLastChar(char *pc_Line,char c_Char);


void BL_System_TranslError(status_t u_Error,BL_String & s_Text); 
void BL_System_TypeToString(uint32 i_Type,BL_String & s);
void BL_Int_Swap(void *pi);

#define BL_KEY_PLUS 	58
#define BL_KEY_MINUS 	37
#define BL_KEY_MUL 		36


#include <Bitmap.h>

#define BL_MIME_FOLDER 		"application/x-vnd.Be-directory"
#define BL_MIME_BOOKMARK	"application/x-vnd.Be-bookmark"
#define BL_MIME_FILE		"application/octet-stream"
#define BL_MIME_FONT		"application/x-vnd.Be-FDEM"

BBitmap*	BL_Load_SIconFromMIME(const char *pc_MIME);

void		BL_Get_SList_FromMessage(BMessage & o_Message,const char *pc_Name,BL_List & ls_Result);


#include <Autolock.h>

#define LOCK_WINDOW() BAutolock oLock(Window())

#endif
