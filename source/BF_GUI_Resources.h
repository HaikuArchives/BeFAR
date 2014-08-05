#ifndef __BF_GUI_RESOURCES_H__
#define __BF_GUI_RESOURCES_H__

const color_space kDefaultIconDepth = B_COLOR_8_BIT;

enum {
	kResMoveStatusBitmap = 1039,
	kResCopyStatusBitmap = 1040,
	kResTrashStatusBitmap = 1041
};

#include <Locker.h>
#include <Resources.h>
#include <Mime.h>
#include <image.h>
#include <Bitmap.h>

#include "BL_Tools.h"

class BBitmap;


class BF_GUI_Resources:public BL_Object{
public:
						BF_GUI_Resources();	

	const void 			*LoadResource(type_code type, int32 id,
							size_t *outSize) const;
	const void 			*LoadResource(type_code type, const char *name,
							size_t *outSize) const;
	
	status_t 			GetIconResource(int32 id, icon_size size, BBitmap *dest) const;
 	status_t 			GetBitmapResource(type_code type, int32 id, BBitmap **out) const;
	
	
								
private:
	image_id 			find_image(void *memAddr) const;								
	
	mutable 			BLocker fLock;
	BResources 			fResources;

};

extern BF_GUI_Resources *poResources;


#endif