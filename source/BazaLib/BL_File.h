#ifndef __BL_FILE_H
#define __BL_FILE_H

#include <String.h>
#include <File.h>
#include "BL_Tools.h"

class BL_File : public BFile {
public:
					BL_File(void);
					BL_File(const BL_File &file);
					BL_File(const entry_ref *ref, uint32 openMode);
					BL_File(const BEntry *entry, uint32 openMode);
					BL_File(const char *path, uint32 openMode);
					BL_File(BDirectory *dir, const char *path, uint32 openMode);
		
virtual 			~BL_File();
		
		status_t 	ReadString(BString *po_Str);
		status_t 	WriteString(const BString *str);
		status_t 	WriteString(const BString &s);
		status_t 	WriteChar(const char *pc_Text);
		
		status_t 	ReadStringAt(off_t location, BString str);
		status_t 	WriteStringAt(off_t location, BString str);
		status_t	FindPrevEOL(off_t & i_PosResult);
		
		status_t	CopyAttributeFrom(const char*pc_AttrName,BL_File & o_SrcFile);
	
private:
	off_t	line;
};

#endif