#ifndef __BL_GUI_H__
#define __BL_GUI_H__

#include <ListView.h>
#include "BL_Tools.h"

class BL_GUI_ListView:public BListView{
public:
					BL_GUI_ListView(
							BRect o_Frame,
							const char *pc_Name,
							list_view_type u_Type = B_SINGLE_SELECTION_LIST,
							uint32 i_ResizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
							uint32 i_Flags = B_WILL_DRAW | B_NAVIGABLE | B_FRAME_EVENTS
							);

virtual void		MakeEmpty(void);
		void		List(BL_Collection &o_List);
					~BL_GUI_ListView();
};


#endif