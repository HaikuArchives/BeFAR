#ifndef __TEXTFILE_H
#define __TEXTFILE_H

#include <Be.h>

class KTextFile : public BFile {
public:
	KTextFile(void);
	KTextFile(const KTextFile &file);
	KTextFile(const entry_ref *ref, uint32 openMode);
	KTextFile(const BEntry *entry, uint32 openMode);
	KTextFile(const char *path, uint32 openMode);
	KTextFile(BDirectory *dir, const char *path, uint32 openMode);
	status_t ReadString(BString *str);
	status_t WriteString(const BString *str);
	status_t ReadStringAt(off_t location, BString str);
	status_t WriteStringAt(off_t location, BString str);
	virtual ~KTextFile();
private:
	off_t	line;
};

#endif