#ifndef __BF_ROSTER_H__
#define __BF_ROSTER_H__

#include "BL_Tools.h"
#include "BF_Node.h"

///////////////////////////////////////////////////////////////////////
BF_Node*	BF_Roster_LoadNode(const char *pc_Path,const char *pc_NodeName,int i_Style);
bool 		BF_Roster_LoadNodeList(BF_Path &o_Path,BF_NodeList & lo_Node,int i_Style);
bool 		BF_Roster_LoadNodeList_Ext(BF_Path &o_Path,int i_Style,BView *po_View,int i_MessageIter,int i_MessageFinal);
status_t	BF_Roster_LoadNodeRef(const char *pc_NodeFullPath,node_ref & u_Ref);
status_t	BF_Roster_LoadEntryRef(const char *pc_NodeFullPath,entry_ref & u_Ref);
bool		BF_Roster_ReloadNode(const char *pc_PathToNode,BF_Node*po_Node,int i_Style,bool b_PathIncludeName=false);

status_t	BF_Roster_TraverseLink(const char *pc_Path,BL_String & s_Name);
///////////////////////////////////////////////////////////////////////
#define BF_NODEINFO_NAME	1
status_t	BF_Roster_GetNodeInfoByRef(BF_Node*po_Node,int i_Style);
///////////////////////////////////////////////////////////////////////
status_t	BF_Roster_RunEntry(const BEntry & o_Entry,const char *pc_Args=NULL);
int 		BF_Roster_RunCommand(const BL_String s_Command);
int 		BF_Roster_RunFile(const char *pc_Path,const char *pc_File,const char *pc_Args=NULL);
///////////////////////////////////////////////////////////////////////
bool 		BF_Roster_RemoveNode(BF_Path & o_Path,BF_Node & o_Node);  
status_t	BF_Roster_MakeLink(const char *pc_SrcFile,const char *pc_DestFile);
///////////////////////////////////////////////////////////////////////

#define 	BF_ROSTER_LOADVOLS_DEFAULT		0
#define 	BF_ROSTER_LOADVOLS_SICON		1
#define 	BF_ROSTER_LOADVOLS_LICON		2
#define 	BF_ROSTER_LOADVOLS_SPEC			4

void		BF_Roster_VolumeList_AddSpec(BF_NodeCollection &lo_NodeResult,int i_Styles);
void		BF_Roster_VolumeList(BF_NodeList &lo_NodeResult,int i_Styles);

///////////////////////////////////////////////////////////////////////

status_t	BF_Roster_MakeDir(const char *pc_Name);
BF_Node* 	BF_Roster_MakeDirNode(BF_Path &o_Path,const char *pc_Name=NULL,int i_Style=BF_ROSTER_LOAD_NORMAL);

bool		BF_Roster_RemoveNode(const char *pc_Path);
bool		BF_Roster_RemoveNode(BF_Path &o_Path,const char *pc_Node); 
bool		BF_Roster_RemoveNode(BF_Path &o_Path,BF_Node* po_Node);
status_t	BF_Roster_MoveNode(BF_Path &o_Path,BF_Node* po_Node,const char *pc_NewPath,const char *pc_NewName=NULL);
status_t	BF_Roster_RenameNode(const BF_Path & o_Path,BF_Node* po_Node,const char *pc_NewName);

bool		BF_Roster_NodeExists(const char *pc_Name);

bool		BF_Roster_StartMonitor_Dir(BF_FilesPath &o_Path,BHandler *po_Handler);
void		BF_Roster_StartMonitor_Node(BF_Node *po_Node,BHandler *po_Handler,int32 i_Flags=-1);
void		BF_Roster_StartMonitor_Nodes(BF_NodeCollection &lo_Node,BHandler *po_Handler);

void		BF_Roster_StopMonitor_Node(BF_Node *po_Node,BHandler *po_Handler);
void		BF_Roster_StopMonitor_Nodes(BF_NodeCollection &lo_Node,BHandler *po_Handler);

void		BF_Roster_StopAllMonitors(BHandler *po_Handler);
///////////////////////////////////////////////////////////////////////

bool		BF_Roster_MIME_by_Name(BString & s_Name,BString & s_MIME);

///////////////////////////////////////////////////////////////////////

#endif