#include "BL_Tools.h"
#include "BF_GUI_Resources.h"

#include <Autolock.h>
#include <Bitmap.h>
#include <Debug.h>
#include <DataIO.h>
#include <File.h>
#include <SupportDefs.h>

BF_GUI_Resources *poResources =  NULL;

BF_GUI_Resources::BF_GUI_Resources()
{
	image_id image = find_image(NULL);
	image_info info;
	if (get_image_info(image, &info) == B_OK) {
		BFile file;
		if (file.SetTo(&info.name[0], B_READ_ONLY) == B_OK) 
			fResources.SetTo(&file);
	}	
}

const void *
BF_GUI_Resources::LoadResource(type_code type, int32 id, size_t *out_size) const
{	
	// Serialize execution.
	// Looks like BResources is not really thread safe. We should
	// clean that up in the future and remove the locking from here.
	BAutolock lock(fLock);
	if (!lock.IsLocked())
		return 0;
	
	// Return the resource.  Because we never change the BResources
	// object, the returned data will not change until TTracker is
	// destroyed.
	return const_cast<BResources *>(&fResources)->LoadResource(type, id, out_size);
}

const void *
BF_GUI_Resources::LoadResource(type_code type, const char *name, size_t *out_size) const
{
	// Serialize execution.
	BAutolock lock(fLock);
	if (!lock.IsLocked())
		return NULL;
	
	// Return the resource.  Because we never change the BResources
	// object, the returned data will not change until TTracker is
	// destroyed.
	return const_cast<BResources *>(&fResources)->LoadResource(type, name, out_size);
}

status_t
BF_GUI_Resources::GetBitmapResource(type_code type, int32 id, BBitmap **out) const
{
	*out = NULL;
	
	size_t len = 0;
	const void *data = LoadResource(type, id, &len);

	if (data == NULL) {
		TRESPASS();
		return B_ERROR;
	}
	
	BMemoryIO stream(data, len);
	
	// Try to read as an archived bitmap.
	stream.Seek(0, SEEK_SET);
	BMessage archive;
	status_t err = archive.Unflatten(&stream);
	if (err != B_OK)
		return err;

	*out = new BBitmap(&archive);
	if (!*out)
		return B_ERROR;

	err = (*out)->InitCheck();
	if (err != B_OK) {
		delete *out;
		*out = NULL;
	}
	
	return err;
}

status_t
BF_GUI_Resources::GetIconResource(int32 id, icon_size size, BBitmap *dest) const
{
	if (size != B_LARGE_ICON && size != B_MINI_ICON )
		return B_ERROR;
	
	size_t len = 0;
	const void *data = LoadResource(size == B_LARGE_ICON ? 'ICON' : 'MICN',
		id, &len);

	if (data == 0 || len != (size_t)(size == B_LARGE_ICON ? 1024 : 256)) {
		TRESPASS();
		return B_ERROR;
	}
	
	dest->SetBits(data, (int32)len, 0, kDefaultIconDepth);
	return B_OK;
}




image_id
BF_GUI_Resources::find_image(void *memAddr) const
{
	image_info info; 
	int32 cookie = 0; 
	while (get_next_image_info(0, &cookie, &info) == B_OK) 
		if ((info.text <= memAddr && (((uint8 *)info.text)+info.text_size) > memAddr)
			||(info.data <= memAddr && (((uint8 *)info.data)+info.data_size) > memAddr)) 
			// Found the image.
			return info.id;
	
	return -1;
}