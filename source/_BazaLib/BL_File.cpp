/*
===============================================
Project:	BeFar
File:		BL_File.cpp   ( was TextFile.cpp )
Desc:		Provides textfile services.
Author:		Baza, [based on Nexus sources ]
Created:	05 Nov 1999
Modified:	09 Jul 2000
===============================================
*/
#include "stdio.h"
#include <stdlib.h>
#include <fs_attr.h>
#include "BL_File.h"

BL_File::BL_File(void) : BFile(){
}

BL_File::BL_File(const BL_File &file) : BFile(file) {
}

BL_File::BL_File(const entry_ref *ref, uint32 openMode) : BFile(ref, openMode){
}

BL_File::BL_File(const BEntry *entry, uint32 openMode) : BFile(entry, openMode){
}
	
BL_File::BL_File(const char *path, uint32 openMode) : BFile(path, openMode) {
}

BL_File::BL_File(BDirectory *dir, const char *path, uint32 openMode) : BFile(dir, path, openMode) {
}

// Destructor
BL_File::~BL_File() {
}

// Reads strings from file, line by line.
status_t BL_File::ReadString(BString *str) 
{
	if(!str)	return B_ERROR;
	off_t 		pos = BFile::Position();
	char 		buf[0x100];
	ssize_t		size = BFile::Read(buf, 0x100);

	if (!size)
		size = B_ERROR;

	if (size < 0)
		return status_t(size);

	BString bstr(buf);
	int32 line_end = bstr.FindFirst('\n');
	if(line_end > size) { // there was data read, but the end of file reached
		off_t size;
		BFile::GetSize(&size);
		line_end =  size - pos;
	}
	bstr.CopyInto(*str, 0, line_end);
	int32 bytes = str->Length();
	line += 1; 
	if(BFile::Seek(pos+bytes+1, SEEK_SET)<0) return B_ERROR;
	return B_OK;
}

// Reads strings from file, line by line , back direction
status_t
BL_File::FindPrevEOL(off_t & i_PosResult) 
{
	i_PosResult = -1;
	
	off_t 		iPos = BFile::Position(),iOldPos = iPos;
	char 		cBuf[0x1000+1];	
	
	if(iPos<=0){
		i_PosResult = 0;
		return B_OK;		
	}
	
	while(true){
		iPos -= 0x1000;
		if(iPos<0) iPos=0;				
		
		iPos = BFile::Seek(iPos,SEEK_SET);
		if(iPos<0) return B_ERROR;
		
		size_t size = BFile::Read(cBuf, iOldPos - iPos+1);		
		if(0==size) return B_ERROR;

		cBuf[size]='\0';
		
		BString bstr(cBuf);
		int32 line_end = bstr.FindLast('\n');
		
		if(line_end<0 && iPos==0){
			i_PosResult = 0;
			return B_OK;
		}else			
		if(line_end<0){
			line_end = bstr.Length()-1;
		}
		if(line_end>size) return B_ERROR;else	
		if(line_end<=size){
			i_PosResult = iPos+line_end;
			return B_OK;
		}		
		iOldPos = iPos;
		continue;				
	}		
}


status_t 	
BL_File::WriteString(const BString &s)
{	
	BFile::Seek(0, SEEK_END);		
	BFile::Write(s.String(), s.Length());
	if(s.FindLast('\n') != (s.Length()-1)) 	BFile::Write("\n",1);
	return  B_OK;	
}

// Appends a string to file, string being forced to end by \n
status_t BL_File::WriteString(const BString *po_Str)   
{
	ASSERT(po_Str);		
	return WriteString(*po_Str);
}

status_t 
BL_File::WriteChar(const char *pc_Text)
{
	ASSERT(pc_Text);
	BString s(pc_Text);	
	return WriteString(&s);
}


// TODO
status_t BL_File::ReadStringAt(off_t location, BString str) {
	const off_t pos = BFile::Position();
	return B_OK;
}

// TODO
status_t BL_File::WriteStringAt(off_t location, BString str) {
	const off_t pos = BFile::Position();
	return B_OK;
}

status_t	BL_File::CopyAttributeFrom(const char*pc_AttrName,BL_File & o_SrcFile)
{
	// check files 
	status_t uRes = InitCheck();	if(B_OK!=uRes) return uRes;
	uRes = o_SrcFile.InitCheck();	if(B_OK!=uRes) return uRes;
	// check name
	if(!pc_AttrName || (0==strlen(pc_AttrName))) return B_ERROR;

	{
		Debug_Info("start");
		char 		pcName[B_ATTR_NAME_LENGTH];
		attr_info	uAttrInfo;
		o_SrcFile.RewindAttrs();
		while(B_OK==o_SrcFile.GetNextAttrName(pcName)){
			Debug_Info(pcName);
		}
	}

	// try to find req.attribute //
	attr_info	uAttrInfo;
	uRes = o_SrcFile.GetAttrInfo(pc_AttrName,&uAttrInfo);
	if(B_OK!=uRes) return uRes;
	// alloc buffer //
	char *pcBuf = (char*)malloc(uAttrInfo.size)+1;
	// read data //
	if(0==o_SrcFile.ReadAttr(pc_AttrName,uAttrInfo.type,0,(void*)pcBuf,uAttrInfo.size)) return B_ERROR;
	// write data //
	return 0==WriteAttr(pc_AttrName,uAttrInfo.type,0,(void*)pcBuf,uAttrInfo.size)?B_ERROR:B_OK;
}
