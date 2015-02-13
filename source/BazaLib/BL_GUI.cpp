#include "BL_GUI.h"  
#include "stdio.h"  
  
BL_GUI_ListView::BL_GUI_ListView(
	BRect o_Frame,
	const char *pc_Name,
	list_view_type u_Type,
	uint32 i_ResizingMode,
	uint32 i_Flags
)
:BListView(o_Frame,pc_Name,u_Type,i_ResizingMode,i_Flags)
{
}


void BL_GUI_ListView::List(BL_Collection &o_List)
{
	BListItem **po = (BListItem**)Items();	
	for(int i=0;i<CountItems();i++){	
		o_List.AddItem(*po);
		po++;
	}
}

void		
BL_GUI_ListView::MakeEmpty(void)
{
	BL_List oList;
	List(oList);	
	BListView::MakeEmpty();
}

BL_GUI_ListView::~BL_GUI_ListView()
{
	printf("try to ~BL_GUI_ListView()\n");
	MakeEmpty();
	printf("finished ~BL_GUI_ListView()\n");
}
