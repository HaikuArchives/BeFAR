/*
===============================================
Project:	BeFar
File:		TextFile.cpp
Desc:		Provides textfile services.
Author:	Nexus
Created:	05.11.99
Modified:	05.11.99
===============================================
*/
#include <Be.h>
#include "TextFile.h"

KTextFile::KTextFile(void) : BFile(){
}

KTextFile::KTextFile(const KTextFile &file) : BFile(file) {
}

KTextFile::KTextFile(const entry_ref *ref, uint32 openMode) : BFile(ref, openMode){
}

KTextFile::KTextFile(const BEntry *entry, uint32 openMode) : BFile(entry, openMode){
}
	
KTextFile::KTextFile(const char *path, uint32 openMode) : BFile(path, openMode) {
}

KTextFile::KTextFile(BDirectory *dir, const char *path, uint32 openMode) : BFile(dir, path, openMode) {
}

// Destructor
KTextFile::~KTextFile() {
}

// Reads strings from file, line by line.
status_t KTextFile::ReadString(BString *str) {
	if(!str)
		return B_ERROR;
	off_t pos = BFile::Position();
	status_t status = B_OK;
	char buf[0x100];
	size_t size = BFile::Read(buf, 0x100);
	BString bstr(buf);
	uint32 line_end = bstr.FindFirst('\n');
	if(0 == size) // no data and hit the end of file
		return B_ERROR;
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

// Appends a string to file, string being forced to end by \n
status_t KTextFile::WriteString(const BString *str) {
	if(!str)
		return B_ERROR;
	BFile::Seek(0, SEEK_END);
	if(str->FindLast('\n') != (str->Length()-1)){
		BString s;
		s<<*str;
		s<<"\n";
		return BFile::Write(s.String(), s.Length());
	}else{
		return BFile::Write(str->String(), str->Length());
	}
}

// TODO
status_t KTextFile::ReadStringAt(off_t location, BString str) {
	const off_t pos = BFile::Position();
	return B_OK;
}

// TODO
status_t KTextFile::WriteStringAt(off_t location, BString str) {
	const off_t pos = BFile::Position();
	return B_OK;
}
