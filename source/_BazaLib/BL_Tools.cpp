/*
===============================================
Project:	BeFar
File:		BL_Tools.cpp
Desc:		Provides some usefull functions
Author:		Baza,some minor changes by Nexus
Created:	20.11.99
Modified:	07.01.2000
===============================================
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <Application.h>
#include <Alert.h>

#include "../_BazaLib/BL_Tools.h"
#include "../_BazaLib/BL_File.h"


void
ASSERT(bool b_True,const char *pc_Message)
{
	if(b_True) return;
	Debug_Error(pc_Message?pc_Message:"ASSERT!");
	exit(-1);
}


void
Debug_Error(const char * pc_Message, status_t err)
{
	ASSERT(pc_Message);
	BL_String s(pc_Message),s1;
	BL_System_TranslError(err,s1);

	//	write log //
	{
		BL_File oFile("errors.log",B_WRITE_ONLY|B_OPEN_AT_END|B_CREATE_FILE);		
		BL_String s2(s);
		s2<<"  ";
		s2<<s1;
		oFile.WriteString(&s2);
	}
	//
	s<<" \n error : ";	
	s<<s1;	
	printf(s.String());
	printf("\n");	
	(new BAlert("", s.String(), "Quit"))->Go();
	be_app->PostMessage(B_QUIT_REQUESTED);
}

void
Debug_Alert(const char * message)
{
	(new BAlert("", message, "Quit"))->Go();
}

void
Debug_Info(const char * message) 
{
	printf("%s\n", message);
}

BL_Collection::BL_Collection(bool b_DeleteItems)
{
	bDeleteItems = b_DeleteItems;	
	iItemsType = BL_COLLECTION_ITEMS_IS_VOID;
}

BL_Collection::~BL_Collection()
{
	if(bDeleteItems) DeleteItems();
}

void		
BL_Collection::RemoveList(BL_Collection *plo_Item)
{
	ASSERT(plo_Item);
	RemoveList(*plo_Item);	
}
void		
BL_Collection::RemoveList(BL_Collection &lo_Item)
{
	for(int32 i=0;i<lo_Item.CountItems();i++){
		RemoveItem(lo_Item.ItemAt(i) );
	}
}	

void		
BL_Collection::DeleteItemAt(int i_Index)
{
	void *puItem = ItemAt(i_Index);
	DeleteItem(puItem);
}

void*		
BL_Collection::LastItem()
{
	ASSERT(CountItems()!=0);
	return ItemAt(CountItems()-1);
}

void*		
BL_Collection::FirstItem()
{
	ASSERT(CountItems()!=0);
	return ItemAt(0);
}

void		
BL_Collection::DeleteItem(void *pu_Item)
{
	ASSERT(pu_Item);
	RemoveItem(pu_Item);

/*	
	if(is_kind_of(pu_Item,BString)){
		BString* ps = cast_as(pu_Item,BString);
		delete ps;	
	}else
		ASSERT(false);
*/
		
	switch(iItemsType){
	case BL_COLLECTION_ITEMS_IS_STRING:{
		BL_String* ps = (BL_String*)pu_Item;
		printf("\t\tDeleteItem:BL_String:%s\n", ps->String());
		printf("TATA:%s\n", ps->String());
		ps->SetTo("");
		DELETE(ps);
		break;}
	case BL_COLLECTION_ITEMS_IS_LIST:{
		BList* pl = (BList*)pu_Item;
		DELETE(pl);
		break;}	
	case BL_COLLECTION_ITEMS_IS_BLOBJECT:{
		delete (BL_Object*)pu_Item;
		break;}
	case BL_COLLECTION_ITEMS_IS_VOID:{
		delete (void*)pu_Item;
		break;}
	};	
}

bool		
BL_Collection::AddItem(BString *ps_Item, int32 atIndex)
{	
	ASSERT(ps_Item);
	ASSERT(BL_COLLECTION_ITEMS_IS_VOID==iItemsType
		|| BL_COLLECTION_ITEMS_IS_STRING==iItemsType);
		
	iItemsType = BL_COLLECTION_ITEMS_IS_STRING;
	return BList::AddItem((void*)ps_Item,atIndex);
}

bool		
BL_Collection::AddItem(void *pu_Item, int32 atIndex)
{
	ASSERT(BL_COLLECTION_ITEMS_IS_VOID==iItemsType);
	return BList::AddItem(pu_Item,atIndex);	
}

bool		
BL_Collection::AddItem(BL_Object *po_Item, int32 atIndex)
{
	ASSERT(BL_COLLECTION_ITEMS_IS_VOID==iItemsType
		|| BL_COLLECTION_ITEMS_IS_BLOBJECT==iItemsType);				
	iItemsType = BL_COLLECTION_ITEMS_IS_BLOBJECT;	
	
	return BList::AddItem((void*)po_Item,atIndex);
}


void		
BL_Collection::DeleteItems()
{ 
	void *pu;
	while(true){
		pu = ItemAt(0);
		if(!pu) break;				
		DeleteItem(pu);
	}
}


bool		
BL_Collection::AddList(BL_Collection *pl_List,int i_Index)
{
	ASSERT(pl_List);
	if(pl_List->CountItems()==0) return true;	
	ASSERT(BL_COLLECTION_ITEMS_IS_VOID==iItemsType || iItemsType==pl_List->iItemsType);
	
	iItemsType = pl_List->iItemsType;
	return BList::AddList(pl_List,i_Index);
}

bool		
BL_Collection::AddList(BList *pl_List)
{
	ASSERT(pl_List);
	ASSERT(BL_COLLECTION_ITEMS_IS_VOID==iItemsType);
	
	return BList::AddList(pl_List);
}

bool		
BL_Collection::AddList(BL_Collection *pl_List)
{
	ASSERT(pl_List);
	if(pl_List->CountItems()==0) return true;	
	ASSERT(BL_COLLECTION_ITEMS_IS_VOID==iItemsType || iItemsType==pl_List->iItemsType);
	
	iItemsType = pl_List->iItemsType;
	return BList::AddList(pl_List);
}

bool	
BL_Collection::AddItem(void *pu_Item)
{	
	ASSERT(BL_COLLECTION_ITEMS_IS_VOID==iItemsType);
	return BList::AddItem(pu_Item);
}

bool
BL_Collection::AddItem(BString * ps_Item)
{
	ASSERT(BL_COLLECTION_ITEMS_IS_VOID==iItemsType
		|| BL_COLLECTION_ITEMS_IS_STRING==iItemsType);
	BString* ps = (BString*)ps_Item;
	printf("\t\tAddItem:BString:%s\n", ps->String());
	
	iItemsType = BL_COLLECTION_ITEMS_IS_STRING;
	return BList::AddItem((void*)ps_Item);
}

bool
BL_Collection::AddItem(BL_String * ps_Item)
{
	ASSERT(BL_COLLECTION_ITEMS_IS_VOID==iItemsType
		|| BL_COLLECTION_ITEMS_IS_STRING==iItemsType);
	BL_String* ps = (BL_String*)ps_Item;
	printf("\t\tAddItem:BL_String:%s\n", ps->String());
	
	iItemsType = BL_COLLECTION_ITEMS_IS_STRING;
	return BList::AddItem((void*)ps_Item);
}

bool
BL_Collection::AddItem(BList * pl_Item)
{
	ASSERT(BL_COLLECTION_ITEMS_IS_VOID==iItemsType
		|| BL_COLLECTION_ITEMS_IS_LIST==iItemsType);
		
	iItemsType = BL_COLLECTION_ITEMS_IS_LIST;
	return BList::AddItem((void*)pl_Item);
}

bool
BL_Collection::AddItem(BL_Object * po_Item)
{
	ASSERT(BL_COLLECTION_ITEMS_IS_VOID==iItemsType
		|| BL_COLLECTION_ITEMS_IS_BLOBJECT==iItemsType);		

	iItemsType = BL_COLLECTION_ITEMS_IS_BLOBJECT;
	return BList::AddItem((void*)po_Item);
}

BL_String*
BL_Collection::StringAt(int i_Index)
{
	return (BL_String*)ItemAt(i_Index);
}

BL_List::BL_List(bool b_DeleteItems)
	:BL_Collection(b_DeleteItems)
{
}

////////////////////////////////////////////////////////////////////////////

BL_String::BL_String()
{
}

BL_String::BL_String(const BString & s_Source)
:BString(s_Source)
{
}

BL_String::BL_String(const char *pc_Text)
:BString(pc_Text)
{
}

BL_String::~BL_String()
{
}

int32
BL_String::Int32()
{
	return atoi(String());
}

float
BL_String::Float()
{
	return atof(String());
}

void		
BL_String::RTrim(char c_Byte)
{
	while(ByteAt(Length()-1)==c_Byte){
		Remove(Length()-1,1);
	}
}

void		
BL_String::LTrim(char c_Byte)
{
	while(ByteAt(0)==c_Byte){
		Remove(0,1);
	}
}

int			
BL_String::ComparePos(BL_String & s_Friend,int i_SelfPos,int i_FriendPos)
{
	const char *pc1,*pc2;
// sz: 09.05.2009 - incompatibility with Haiku API	
//	pc1 = _privateData+i_SelfPos;
//	pc2 = s_Friend._privateData+i_FriendPos;
	pc1 = String()+i_SelfPos;
	pc2 = s_Friend.String()+i_FriendPos;
	return ( strcmp(pc1,pc2) );
}

void		
BL_String::operator=(int64 i_Value)
{
	SetTo("");
	*this<<i_Value;
}

void		
BL_String::operator=(const char* pc_Value)
{
	SetTo(pc_Value);
}

int32 
BL_String::CountChar(char c_Value)
{
	int32 iCount=0;
	// sz: 09.05.2009 - incompat with Haiku API
	//char *pc = _privateData;
	const char *pc = String();
	while(pc[0]!='\0'){
		if(pc[0]==c_Value) iCount++;
		pc++;
	} 	
	return(iCount);
}

void		
BL_String::operator=(BString & s_Value)
{
	SetTo(s_Value);
}

BL_String*
BL_String::SetDigits(char c_Digit,char c_DecPointChar)
{
	BString sResult;
	/* check for zero  */
	if(Length()==0){
		SetTo("0");
		return this;
	}
	/* check for dec_point */
	// sz: 09.05.2009 - incompat with Haiku API!
	//char *pc,*pcZero = strchr(_privateData,c_DecPointChar);
	char *pcZero = strchr(String(),c_DecPointChar);
	if(pcZero) pcZero[0]='\0';
	// sz: 09.05.2009 - incompat with Haiku API!
	//pc = _privateData;
	char* pc = (char*)String();
	/* set digits */
	int 	iLen = strlen(pc);
	char	c1;
	for(int i = (iLen % 3), i1=0; i<=iLen; i+=3){
		if(i==0) continue;
		c1 = pc[i];
		pc[i] =	'\0';
		sResult << (pc+i1);
		pc[i] =	c1;
		i1 = i;
		if(i!=iLen) sResult << c_Digit;
	}
	/* set dec */
	if(pcZero){
		pcZero[0] = c_DecPointChar;
		sResult << (pcZero+1);
	}
	/* finish */
	SetTo(sResult);
	
	return this;
}

status_t	
BL_String::GetFromMessage(BMessage *po_Message,const char *pc_Name)
{
	if(!po_Message || !pc_Name) return B_ERROR;
	
	const char *pc=NULL;
	status_t uRes = po_Message->FindString(pc_Name,&pc);
	if(uRes!=B_OK) return uRes;
	if(!pc) return B_ERROR;
	
	SetTo(pc);
	return B_OK;
}

int32		
BL_String::LengthUTF8() const	
{

	int32 iLen=0;
	for(int i=0;i<BString::Length();i++){
		if(ByteAt(i)&128)	i++;
		iLen++;
	}
	return iLen;
}

void
BL_String::RemoveUTF8(int32 i_FromChar, int32 i_CharsCount)
{
	//
	int32 iFromChar=0;
	int32 iFromByte=0;
	for(int32 i=0;i<BString::Length();i++){				
		if(iFromChar==i_FromChar) break;
		
		if(ByteAt(iFromByte)&128){
			i++;			
			iFromByte++;
		}
		iFromChar++;
		iFromByte++;
	}	
	//
	int32 iCharsCount=0;	
	int32 iBytes = 0;
	for(int32 i = iFromByte;i<BString::Length();i++){				
		if(iCharsCount==i_CharsCount) break;				
		iCharsCount++;
		if(ByteAt(i)&128){
			i++;
			iBytes++;
		}
		iBytes++;
		printf("          i=%i iBytes=%i\n",i,iBytes);				
	}
	printf("RemoveUTF8(%i,%i)  >  Remove(%i,%i)  OldBytesCount=%i\n",i_FromChar,i_CharsCount,iFromByte,iBytes,Length());
	BString::Remove(iFromByte,iBytes);	
}

void		
BL_String::AppendUTF8(const char *source, int32 i_CharsCount)
{
	int32 iBytes=PCharCharsInBytes_UTF8(source,i_CharsCount);
//	printf("AppendUTF8(%i)  >  Append(%i)\n",i_CharsCount,iBytes,source);
	Append(source,iBytes);			
}

const char* 
BL_String::StringUTF8(int32 i_FromChar)const
{	
	int32 iByte = PCharCharPosInBytes_UTF8(String(),i_FromChar);
//	printf("StringUTF8(%i)  >  String(%i)\n",i_FromChar,iByte);
	return String() + iByte;
}	

void
BL_String::InsertUTF8(const char *pc_Value, int32 i_CharsCount,int32 i_CharPos)
{
	int32 iValueBytes = PCharCharsInBytes_UTF8(pc_Value,i_CharsCount);
	int32 iCharPos = PCharCharPosInBytes_UTF8(String(),i_CharPos);
	BString::Insert(pc_Value,iValueBytes,iCharPos);
}

////////////////////////////////////////////////////////////////////////

int32 
PCharCharPosInBytes_UTF8(const char *pc_Source,int32 i_Char)
{
	ASSERT(pc_Source && (i_Char>=0));
	
	int32 iChar=0;
	int32 iByte=0;
	for(uint32 i=0;i<strlen(pc_Source);i++){
		if(iChar==i_Char) break;
		
		if(pc_Source[i]&128){
			i++;
			iByte++;
			uint8 c=pc_Source[i-1];
			printf("> %i\n",c);
		}
		iChar++;		
		iByte++;
	}	
	return iByte;
}

int32 
PCharCharsInBytes_UTF8(const char *pc_Source,int32 i_Chars)
{
	ASSERT(pc_Source && (i_Chars>=0));
	
	int32 iChars=0;
	int32 iBytes=0;
	for(uint32 i=0;i<strlen(pc_Source);i++){		
		
		if(pc_Source[i]&128){
			i++;
			iBytes++;
		}
		iChars++;		
		iBytes++;
		
		if(iChars==i_Chars) break;
	}	
	return iBytes;
}
//////////////////////////////////////////////////

char* 
PCharFindLastChar(char *pc_Line,char c_Char)
{
	char *pcLast = NULL,*pc = pc_Line;	
	while(true){
		pc = strchr(pc,c_Char);
		if(!pc) break;
		pcLast = pc++;
	}
	return pcLast;
}

void
BL_System_TranslError(status_t u_Error,BL_String & s_Text)
{
	switch(u_Error){
	/*--------- GENERAL ERRORS---------*/
	case B_OK:
		s_Text = "B_OK";
		break;
	case B_TIMED_OUT:
		s_Text = "B_TIMED_OUT";
		break;
	case B_BAD_VALUE:
		s_Text = "B_BAD_VALUE";
		break;
	case  B_IO_ERROR:
		s_Text = "B_IO_ERROR";
		break;
	case  B_BAD_INDEX:
		s_Text = "B_BAD_INDEX";
		break;		
	case  B_BAD_TYPE:
		s_Text = " B_BAD_TYPE";
		break;				
	case B_MISMATCHED_VALUES:
		s_Text = "B_MISMATCHED_VALUES";
		break;								
	case B_NAME_NOT_FOUND:
		s_Text = "B_NAME_NOT_FOUND";
		break;																						
	case B_INTERRUPTED:
		s_Text = "B_INTERRUPTED";
		break;														
	case B_CANCELED:
		s_Text = "B_CANCELED";
		break;																
	case B_NO_INIT:
		s_Text = "B_NO_INIT";
		break;																		
	case B_BUSY:
		s_Text = "B_BUSY";
		break;	
	case B_NOT_ALLOWED:
		s_Text = "B_NOT_ALLOWED";
		break;																								
	case B_WOULD_BLOCK:
		s_Text = "B_INTERRUPTED";
		break;																
	case B_NAME_IN_USE:
		s_Text = "B_NAME_IN_USE";
		break;
	case B_FILE_EXISTS:
		s_Text = "B_FILE_EXISTS";
		break;		
	case B_ENTRY_NOT_FOUND:
		s_Text = "B_ENTRY_NOT_FOUND";
		break;
	case B_PERMISSION_DENIED:
		s_Text = "B_PERMISSION_DENIED";
		break;				
	case B_NO_MEMORY:
		s_Text = "B_NO_MEMORY";
		break;						
	case B_BAD_PORT_ID:
		s_Text = "B_BAD_PORT_ID";
		break;
	case B_NO_MORE_PORTS:
		s_Text = "B_NO_MORE_PORTS";
		break;
	case B_ERROR:
		s_Text = "B_ERROR";
		break;								
	/*----------------*/
	default:
		s_Text = "uknown error";
	};
}

void
BL_System_TypeToString(uint32 i_Type,BL_String & s)
{
		switch(i_Type){
		case B_ANY_TYPE:
			s="B_ANY_TYPE";
			break;
		case B_ASCII_TYPE:
			s="B_ASCII_TYPE";
			break;
		case B_CHAR_TYPE:
			s="B_CHAR_TYPE";			
			break;		
		case B_COLOR_8_BIT_TYPE:
			s="B_COLOR_8_BIT_TYPE";
			break;
		case B_GRAYSCALE_8_BIT_TYPE:
			s="B_GRAYSCALE_8_BIT_TYPE";
			break;
		case B_MIME_TYPE:
			s="B_MIME_TYPE";
			break;
		case B_MONOCHROME_1_BIT_TYPE:
			s="B_MONOCHROME_1_BIT_TYPE";
			break;
		case B_INT8_TYPE:
			s="B_INT8_TYPE";
			break;					
		case B_INT16_TYPE:
			s="B_INT16_TYPE";			
			break;			
		case B_INT32_TYPE:
			s="B_INT32_TYPE";
			break;	
		case B_INT64_TYPE:
			s="B_INT64_TYPE";
			break;	
		case B_UINT8_TYPE:
			s="B_UINT8_TYPE";
			break;	
		case B_UINT16_TYPE:
			s="B_UINT16_TYPE";			
			break;			
		case B_UINT32_TYPE:
			s="B_UINT32_TYPE";
			break;	
		case B_UINT64_TYPE:
			s="B_UINT64_TYPE";
			break;	
		case B_FLOAT_TYPE:
			s="B_FLOAT_TYPE";
			break;		
		case B_DOUBLE_TYPE:
			s="B_DOUBLE_TYPE";
			break;		
		case B_BOOL_TYPE:
			s="B_BOOL_TYPE";
			break;	
		case B_OFF_T_TYPE:
			s="B_OFF_T_TYPE";
			break;	
		case B_SIZE_T_TYPE:
			s="B_SIZE_T_TYPE";
			break;
		case B_SSIZE_T_TYPE:
			s="B_SSIZE_T_TYPE";
			break;		
		case B_POINTER_TYPE:
			s="B_POINTER_TYPE";
			break;
		case B_OBJECT_TYPE:
			s="B_OBJECT_TYPE";
			break;		
		case B_RAW_TYPE:
			s="B_RAW_TYPE";
		case B_MESSAGE_TYPE:
			s="B_MESSAGE_TYPE";
			break;		
		case B_MESSENGER_TYPE:
			s="B_MESSENGER_TYPE";
			break;
		case B_POINT_TYPE:
			s="B_POINT_TYPE";
			break;	
		case B_RECT_TYPE:
			s="B_RECT_TYPE";
			break;
/*		case B_PATH_TYPE:
			s="B_PATH_TYPE";
			break;*/
		case B_REF_TYPE:
			s="B_REF_TYPE";
			break;
		case B_RGB_COLOR_TYPE:
			s="B_RGB_COLOR_TYPE";
			break;
		case B_RGB_32_BIT_TYPE:
			s="B_RGB_32_BIT_TYPE";
			break;			
		case B_PATTERN_TYPE:
			s="B_PATTERN_TYPE";
			break;
		case B_STRING_TYPE:
			s="B_STRING_TYPE";
			break;
		case B_TIME_TYPE:
			s="B_TIME_TYPE";
			break;
		default:{
			char c[5];
			uint32	*pi;
			pi = (uint32*)c;
			*pi=i_Type;	
		 	BL_Int_Swap(pi);
			c[4] = '\0';						
			s=c;	
			break;}
		};
}

void BL_Int_Swap(void *pi)
{
	int8 *pb,b;
	pb = (int8*)pi;
	b = pb[0];
	pb[0] = pb[1];
	pb[1] = b;
}

//////////////////////////////////////////////////////////////
#include <Bitmap.h>
#include <Mime.h>

BBitmap*
BL_Load_SIconFromMIME(const char *pc_MIME)
{
	ASSERT(pc_MIME);
	BBitmap* poSIcon = NULL ;
	BMimeType	oMimeType;
	if(B_OK==oMimeType.SetTo(pc_MIME)){
		poSIcon = new BBitmap(BRect(0,0,15,15),B_CMAP8);
		if(B_OK!=oMimeType.GetIcon(poSIcon,B_MINI_ICON)){
			DELETE(poSIcon);
		}
	}
	return poSIcon;
}
//////////////////////////////////////////////////////////////
void
BL_Get_SList_FromMessage(BMessage & o_Message,const char *pc_Name,BL_List & ls_Result)
{
	ASSERT(pc_Name);
	ls_Result.DeleteItems();
	//
	int i=0;
	const char *pc=NULL;
	while(B_OK==o_Message.FindString(pc_Name,i++,&pc)){
		ls_Result.AddItem(new BL_String(pc));
	}
}
//////////////////////////////////////////////////////////////
