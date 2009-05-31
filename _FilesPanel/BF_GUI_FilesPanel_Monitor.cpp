#include <stdio.h>
#include "BF_GUI_FilesPanel.h"
#include "BF_Roster.h"
//#include "BF_GUI_CmdLine.h"
//#include "BF_GUI_KeysMenu.h"
//#include "BF_GUI_FilesPanel_Tasks.h"
//#include "BF_GUI_WinMain.h"

#include <NodeMonitor.h>


void					
BF_GUI_FilesPanel::HandlingMonitor(BMessage &o_Message)
{
	int32 iOpCode=0;
	if(B_OK!=o_Message.FindInt32("opcode",&iOpCode)) return;		
	
	/* get node_ref */ 
	node_ref uRef;
	o_Message.FindInt32("device", &uRef.device); 
	o_Message.FindInt64("node", &uRef.node); 	
	switch(iOpCode){
	case B_ENTRY_REMOVED:
		HandlingMonitor_Removed(o_Message,uRef);
		break;
	case B_ENTRY_MOVED:
		HandlingMonitor_Moved(o_Message,uRef);
		break;
	case B_ENTRY_CREATED:
		HandlingMonitor_Created(o_Message,uRef);
		break;
	case B_STAT_CHANGED:
		HandlingMonitor_Changed(o_Message,uRef);
		break;
	};
	
}


void					
BF_GUI_FilesPanel::HandlingMonitor_Removed(BMessage &o_Message,node_ref & u_Ref)
{
	/* seek this node in current node_list */
	BF_Node *poNode = loNode.NodeByRef(u_Ref);
	if(!poNode) return;		

	Nodes_Remove(poNode,NULL);
	DELETE(poNode);
}		


void					
BF_GUI_FilesPanel::HandlingMonitor_Moved(BMessage &o_Message,node_ref & u_Ref)
{		
	/* get source and destination dirs*/
	node_ref uRefSrcDir,uRefDestDir;
	uRefSrcDir.device = u_Ref.device;
	if(B_OK!=o_Message.FindInt64("from directory",&uRefSrcDir.node)) return;
	if(B_OK!=o_Message.FindInt64("to directory",&uRefDestDir.node)) return;
	if(B_OK!=o_Message.FindInt32("device",&uRefDestDir.device)) return;
	
	/* if renamed */
	if(oPath.CheckNodeRef(uRefSrcDir) && oPath.CheckNodeRef(uRefDestDir)){
		const char *pcNodeName=NULL;
		ASSERT(B_OK==o_Message.FindString("name",&pcNodeName) && pcNodeName);
		BF_Node *poNode = loNode.NodeByRef(u_Ref);
		if(!poNode) return;		
		poNode->sName = pcNodeName;
		
		Nodes_Update(poNode,NULL);		
	}else
	/* if moved from this panel */
	if(oPath.CheckNodeRef(uRefSrcDir)){
		/* seek this node in current node_list */
		BF_Node *poNode = loNode.NodeByRef(u_Ref);
		if(!poNode) return;		
		
		Nodes_Remove(poNode,NULL);
		DELETE(poNode);		
	}else
	/* if moved to this panel */
	if(oPath.CheckNodeRef(uRefDestDir)){
		const char *pcNodeName=NULL;
		ASSERT(B_OK==o_Message.FindString("name",&pcNodeName) && pcNodeName);
		BF_Node *poNode = BF_Roster_LoadNode(oPath.Path(),pcNodeName,LoadStyles());	
		if(!poNode)	 return;
		Nodes_Add(poNode,NULL);	
	}else
	/* curent dir moved */
	if(oPath.CheckNodeRef(u_Ref)){
		NavGoParentDir();
	}	
}

void					
BF_GUI_FilesPanel::HandlingMonitor_Created(BMessage &o_Message,node_ref & u_Ref)
{
	const char *pcNodeName=NULL;
	ASSERT(B_OK==o_Message.FindString("name",&pcNodeName) && pcNodeName);
	BF_Node *poNode = BF_Roster_LoadNode(oPath.Path(),pcNodeName,LoadStyles());			
	if(!poNode)	 return;
	Nodes_Add(poNode,NULL);		
}

void					
BF_GUI_FilesPanel::HandlingMonitor_Changed(BMessage &o_Message,node_ref & u_Ref)
{
	BF_Node *poNode = loNode.NodeByRef(u_Ref);
	if(!poNode) return;
	
	BF_Roster_ReloadNode(oPath.Path(),poNode,LoadStyles());
	
	Nodes_Update(poNode,NULL);
}