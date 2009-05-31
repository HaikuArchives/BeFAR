#include <Resources.h>
#include <Bitmap.h>
#include <stdlib.h>
#include "BF_GUI_Misc.h"
#include "Main.h"

void 
BF_GUI_GetBitmapFromResources( BBitmap*& themap, int32 resid)
{

	BResources res; 
   	status_t err; 
   	
   	size_t  reslength;
   	const char * resbuf;
   	BMessage archive;

	// 	Open resource file
	BFile file( poApp->AppFileName().String(), B_READ_ONLY );  

	// if the file loads OK, that is
   	if ( (file.InitCheck() || (err = res.SetTo(&file)) ) == B_NO_ERROR )  {

		// Find and load resource
   		resbuf = (char*)res.LoadResource( B_ANY_TYPE, resid, &reslength );
   							
   		if( resbuf ) {
   		
			// Inflate and unarchive BBitmap
	   		archive.Unflatten( resbuf );	
   			themap = new BBitmap( &archive );	

		} else {
	   		(new BAlert("", "Error while reading resource from file.", "OK"))->Go();
		}
		    		
   	} else {	// Error
   		(new BAlert("", "Error opening resource file.", "OK"))->Go();
   		exit(1);
   	}
}
