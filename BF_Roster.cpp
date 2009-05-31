#include <StorageKit.h>
#include <sys/stat.h>
#include <stdio.h>
#include <Roster.h>
#include <Messenger.h>
#include "BF_Roster.h"

#include <stdlib.h>

#define BF_ROSTER_MIMETYPE_SYMLINK "application/x-vnd.Be-symlink"
#define BF_ROSTER_MIMETYPE_DIRECTORY "application/x-vnd.Be-directory"


bool 
BF_Roster_RemoveNode(BF_Path & o_Path,BF_Node & o_Node)
{
	BString s;
	s=o_Path.Path();
	s<<"/";
	s<<o_Node.sName;
	//	
	BEntry	oEntry(s.String(),true);
	if(B_OK==oEntry.InitCheck()){
		oEntry.Remove();
		return true;
	}	
	return false;
}

status_t
BF_Roster_RunEntry(const BEntry & o_Entry,const char *pc_Args)
{
	BRoster 	oRoster;
	entry_ref 	oRef;
	team_id		uTeam=0;
	status_t	uRes;
	
	// prepare args //
	BL_List		lsArg;			
	if(pc_Args){
		BL_String s(pc_Args),*ps;
		int	i=0,i1;
		
		while(i<s.Length()){
			i1 = s.FindFirst(' ',i);
			if(i1<0){
				i1 = s.Length();
				if(i1<=1) break;
			}
			ps = new BL_String();
			lsArg.AddItem(ps);
			s.CopyInto(*ps,i,i1-i);
			i = i1+1;
		}		
	}	
	char		*pc1[lsArg.CountItems()>0?lsArg.CountItems():1];
	for(int i=0;i<lsArg.CountItems();i++){
		BL_String *ps = (BL_String*)lsArg.ItemAt(i);
		pc1[i] = (char*)ps->String();
	}
			
	// launch entry //
	
	uRes = o_Entry.InitCheck();
	if(B_OK!=uRes) return uRes;
	uRes = o_Entry.GetRef(&oRef);
	if(B_OK!=uRes) return uRes;
	uRes = oRoster.Launch(&oRef,lsArg.CountItems(),lsArg.CountItems()>0?(&pc1[0]):NULL,&uTeam);
	if(uTeam>0) uRes = B_OK;
	return uRes;
}

int
BF_Roster_RunCommand(const BL_String s_Command)
{
	int iResult = system(s_Command.String());
	return iResult;
}

int 
BF_Roster_RunFile(const char *pc_Path,const char *pc_File,const char *pc_Args)
{	
	BL_String s;
	if(pc_Path || strlen(pc_Path)>0){
		s<<pc_Path;
		s<<"/";
	}
	if(pc_File){
		s<<pc_File;
	}
	if(pc_Args){
		s<<" ";
		s<<pc_Args;
	}
	
	return BF_Roster_RunCommand(s);
	
}

status_t
BF_Roster_TraverseLink(const char *pc_Path,BL_String & s_Name)
{
	BEntry 		oEntry(pc_Path,true);
//	char		pcName[B_FILE_NAME_LENGTH];
	BPath	 	oPath;
	status_t	uRes;
		
	uRes = oEntry.InitCheck();
	if(B_OK!=uRes) return uRes;
	/*uRes = oEntry.GetName(pcName);
	if(B_OK!=uRes) return uRes;*/
	uRes = oEntry.GetPath(&oPath);
	if(B_OK!=uRes) return uRes;
	s_Name = oPath.Path();
	/*s_Name<<pcName;*/
	
	return B_OK;	
}

BF_Node*
BF_Roster_LoadNodeList_Entry(
	BEntry &oEntry,
	BFile &oFile,
	char *pc, 
	struct stat *st,
	int i_Style , 
	BNodeInfo	&oInfo,
	BF_Node* po_ToBFNode=NULL
	)
{
	oEntry.GetName(pc);
	BF_Node* poBFNode = po_ToBFNode?po_ToBFNode:new BF_Node(pc);	
	/**/
	oEntry.GetStat(st);
	poBFNode->uCreateTime = st->st_ctime;
	poBFNode->uAccessTime = st->st_atime;	
	poBFNode->uModifyTime = st->st_mtime;		
	if(i_Style & BF_ROSTER_LOAD_PERMS){
		oEntry.GetPermissions(&poBFNode->uPerms);
	}
	if(S_ISLNK(st->st_mode)) poBFNode->bLink = true;
	/* try to open file */
	status_t uRes = oFile.SetTo(&oEntry,B_READ_ONLY);	
	if(B_OK==uRes){
		/* get mime type */
		if(i_Style & BF_ROSTER_LOAD_TYPE){
			oInfo.SetTo(&oFile);
			oInfo.GetType(pc);
			poBFNode->sType = pc;
		}
		/* get small icon */
		if(i_Style & BF_ROSTER_LOAD_SICON){
			BNodeInfo oNodeInfo(&oFile);
			poBFNode->AllocSIcon();
			if(B_OK!=oNodeInfo.GetIcon(poBFNode->poSIcon,B_MINI_ICON)){
				if(B_OK!=oNodeInfo.GetTrackerIcon(poBFNode->poSIcon,B_MINI_ICON)){
					DELETE(poBFNode->poSIcon);
				}		
			}
		}
		/* get large icon */
		if(i_Style & BF_ROSTER_LOAD_LICON){
			BNodeInfo oNodeInfo(&oFile);
			poBFNode->AllocLIcon();		
			if(B_OK!=oNodeInfo.GetTrackerIcon(poBFNode->poLIcon,B_LARGE_ICON)){
				if(B_OK!=oNodeInfo.GetIcon(poBFNode->poLIcon,B_LARGE_ICON)){
					DELETE(poBFNode->poLIcon);
				}		
			}
		}		
		/* get node_Ref */
		oEntry.GetNodeRef(&poBFNode->uNodeRef);
		/* get size */
		oFile.GetSize(&poBFNode->iSize);
		/* set type */
		if(oFile.IsSymLink()) poBFNode->iType = BF_NODE_TYPE_LINK;else	
		if(oFile.IsFile()) poBFNode->iType = BF_NODE_TYPE_FILE;else
		if(oFile.IsDirectory()) poBFNode->iType = BF_NODE_TYPE_DIR;else			
		{};
	}else{	
		/* try to open Dir */
		BDirectory oDir(&oEntry);
		uRes = oDir.InitCheck();
		if(B_OK==uRes){
			poBFNode->iType = BF_NODE_TYPE_DIR;			
			oEntry.GetNodeRef(&poBFNode->uNodeRef);
			/* get mime type */
			if(i_Style & BF_ROSTER_LOAD_TYPE){
				oInfo.SetTo(&oFile);
				oInfo.GetType(pc);
				poBFNode->sType = pc;
			}
			/* get small icon */
			if(i_Style & BF_ROSTER_LOAD_SICON){
				BNodeInfo oNodeInfo(&oFile);
				if(!poBFNode->poSIcon) poBFNode->poSIcon = new BBitmap(BRect(0,0,15,15),B_CMAP8);
				if(B_OK!=oNodeInfo.GetTrackerIcon(poBFNode->poSIcon,B_MINI_ICON)){
					if(B_OK!=oNodeInfo.GetIcon(poBFNode->poSIcon,B_MINI_ICON)){
						/* try to get  mime_icon */
						BMimeType oType(BF_ROSTER_MIMETYPE_DIRECTORY);
						uRes = oType.InitCheck();
						if(B_OK==uRes) uRes = oType.GetIcon(poBFNode->poSIcon,B_MINI_ICON);
						if(B_OK!=uRes)
						{						
							DELETE(poBFNode->poSIcon);										
						}
					}		
				}
			}
		}else{	
			/* print ERROR */
			BL_String s;
			BL_System_TranslError(uRes,s);
			printf("GENERAL ERROR : ");
			s+="\n";
			printf(s.String());
			/* try to init BSymLink */
			BSymLink oLink(&oEntry);
			if(oLink.InitCheck()==B_OK){
				BNodeInfo oNodeInfo(&oFile);
				if(!poBFNode->poSIcon) poBFNode->poSIcon = new BBitmap(BRect(0,0,15,15),B_CMAP8);
				/* attempt to get icon from Link */
				if(B_OK!=oNodeInfo.GetTrackerIcon(poBFNode->poSIcon,B_MINI_ICON)){
				    /* error.....ok..attempt to get icon from Link.MimeType */
					oInfo.SetTo(&oLink);
					oInfo.GetType(pc);
					BMimeType oType(pc);
					if(B_OK!=oType.GetIcon(poBFNode->poSIcon,B_MINI_ICON)){
					    /* error.....ok..attempt to get icon from Sys::MimeType */
						if(B_OK!=oType.SetTo(BF_ROSTER_MIMETYPE_SYMLINK)) ASSERT(false);
						if(B_OK!=oType.GetIcon(poBFNode->poSIcon,B_MINI_ICON)){
							poBFNode->FreeSIcon();							
						}
					}
				}else{				
				}	
			}else{
				/* ups...can`t init link*/
				return NULL;
			}		
		}
	}
	return poBFNode;	
}

bool
BF_Roster_ReloadNode(const char *pc_PathToNode,BF_Node*po_Node,int i_Style,bool b_PathIncludeName)
{	
	ASSERT(po_Node);	
	
	char		pc[500];
	BString		s(pc_PathToNode?pc_PathToNode:"");
		
	if(!b_PathIncludeName){		
		if(pc_PathToNode && s.FindLast("/")!=s.Length()-1) s<<"/";
		s<<po_Node->sName;
	}
	
	BEntry 		oEntry(s.String());	
	BFile 		oFile;
	BNodeInfo	oInfo;
	struct stat st;
	
	return BF_Roster_LoadNodeList_Entry(oEntry,oFile,pc,&st,i_Style,oInfo,po_Node)!=NULL;
}

status_t
BF_Roster_GetNodeInfoByRef(BF_Node*po_Node,int i_Style)
{
	ASSERT(po_Node);
//	status_t uResult;
	/*
	BEntry oEntry(&po_Node->uEntryRef);
	uResult = oEntry.InitCheck();
	if(B_OK!=uResult) return uResult;
	
	if(i_Style & BF_NODEINFO_NAME){
		char		pc[500];
		oEntry.GetName(pc);
		po_Node->sName = pc;
	}*/
	
	return B_OK;
}

status_t
BF_Roster_LoadNodeRef(const char *pc_NodeFullPath,node_ref & u_Ref)
{
	status_t uRes;
	BEntry 	oEntry(pc_NodeFullPath);	
	uRes = oEntry.InitCheck();
	if(B_OK!=uRes) return uRes;
	return oEntry.GetNodeRef(&u_Ref);
}

status_t
BF_Roster_LoadEntryRef(const char *pc_NodeFullPath,entry_ref & u_Ref)
{
	status_t uRes;
	BEntry 	oEntry(pc_NodeFullPath);	
	uRes = oEntry.InitCheck();
	if(B_OK!=uRes) return uRes;
	return oEntry.GetRef(&u_Ref);
}


BF_Node*
BF_Roster_LoadNode(const char *pc_Path,const char *pc_NodeName,int i_Style)
{
	char		pc[500];
	BString		s;
	
	s<<pc_Path;
	s<<"/";
	s<<pc_NodeName;
	
	BEntry 		oEntry(s.String());	
	BFile 		oFile;
	BNodeInfo	oInfo;
	struct stat st;
	
	BF_Node *poNode = BF_Roster_LoadNodeList_Entry(oEntry,oFile,pc,&st,i_Style,oInfo);	
	if(poNode) poNode->sName=pc_NodeName;
	return poNode;
}

bool BF_Roster_LoadNodeList(BF_Path &o_Path,BF_NodeList & lo_Node,int i_Style)
{
	lo_Node.DeleteItems();	
	lo_Node.poPath = &o_Path;
	/**/
	BDirectory  oDir(o_Path.Path());
	/**/
	BEntry 		oEntry;
	char		pc[500];
	BF_Node		*poBFNode;
	BFile 		oFile;
	BNodeInfo	oInfo;
	struct stat st;
	/**/
	BL_String s;
	s= o_Path.Path();
	if((BF_ROSTER_LOAD_NODE_PARENTDIR & i_Style) && s.CountChars()>1){
		poBFNode = new BF_Node("..");
		poBFNode->iType = BF_NODE_TYPE_PARENTDIR;
		lo_Node.AddItem(poBFNode);
	}
	/**/
	if(B_OK!=oDir.Rewind()) return(false);	
	while(B_OK==oDir.GetNextEntry(&oEntry)){
		poBFNode = BF_Roster_LoadNodeList_Entry(oEntry,oFile,pc,&st,i_Style,oInfo);
		if(poBFNode) lo_Node.AddItem(poBFNode);
		/**/		
	}
	return(true);
}

bool 
BF_Roster_LoadNodeList_Ext(BF_Path &o_Path,int i_Style, BView *po_View,int i_MessageIter,int i_MessageFinal)
{	
	printf("BF_Roster_LoadNodeList_Ext started \n");
	ASSERT(po_View);
	status_t	uRes;
	
	/////// preparea message and messenger ////
	BMessage oMessage(i_MessageIter);
	BMessenger oMessenger((const BHandler*)po_View,(const BLooper*)NULL,&uRes);
	if(B_OK!=uRes){
		BL_String s;
		BL_System_TranslError(uRes,s);
		s<<"\n";
		printf(s.String());
	}			
	//////////
	BDirectory  oDir(o_Path.Path());
	int iCount = oDir.CountEntries(),iIndex=0;
	//////////
	BF_NodeList *ploNode = new BF_NodeList();	
	//////////
	BEntry 		oEntry;
	char		pc[500];
	BF_Node		*poBFNode;
	BFile 		oFile;
	BNodeInfo	oInfo;
	struct stat st;	
	/**/
	BL_String s;
	s= o_Path.Path();
	if((BF_ROSTER_LOAD_NODE_PARENTDIR & i_Style) && s.CountChars()>1){
		poBFNode = new BF_Node("..");
		poBFNode->iType = BF_NODE_TYPE_PARENTDIR;
		ploNode->AddItem(poBFNode);
		oMessage.AddInt32("bf_Count",iCount);
		oMessage.AddInt32("bf_Index",iIndex++);		
		printf("BF_Roster_LoadNodeList_Ext: send message \n");
		//oMessenger.SendMessage(&oMessage);	
	}
	////
	if(B_OK==oDir.Rewind()){
		while(B_OK==oDir.GetNextEntry(&oEntry)){
			//
			poBFNode = BF_Roster_LoadNodeList_Entry(oEntry,oFile,pc,&st,i_Style,oInfo);
			if(!poBFNode) continue;
			ploNode->AddItem(poBFNode);
			iIndex++;
			//
		
			printf("%i %i \n",iIndex,(int(iIndex/10))*10);
			if( (int(iIndex/10))*10==iIndex){
				oMessage.MakeEmpty();
				oMessage.AddInt32("bf_Count",iCount);
				oMessage.AddInt32("bf_Index",iIndex);		
				printf("BF_Roster_LoadNodeList_Ext: send message ,%i %i \n",iIndex,(int(iIndex/10))*10);
				oMessenger.SendMessage(&oMessage);			
			}
			/**/		
		}
	}
	////
	oMessage.MakeEmpty();
	oMessage.what = i_MessageFinal;
	oMessage.AddPointer("bf_Nodes",ploNode);
	oMessenger.SendMessage(&oMessage);
	////
	return(true);
}

void BF_Roster_VolumeList_AddSpec(BF_NodeCollection &lo_NodeResult,int i_Styles)
{	
	/**/
	int iNodeStyles = ( (i_Styles & BF_ROSTER_LOADVOLS_SICON)?BF_ROSTER_LOAD_SICON:0 )
				|	( (i_Styles & BF_ROSTER_LOADVOLS_LICON)?BF_ROSTER_LOAD_LICON:0 );
				
	lo_NodeResult.AddItem( BF_Roster_LoadNode("","boot",iNodeStyles));
	lo_NodeResult.AddItem( BF_Roster_LoadNode("","boot/home",iNodeStyles));
	lo_NodeResult.AddItem( BF_Roster_LoadNode("","boot/home/Desktop",iNodeStyles));
}

void BF_Roster_VolumeList(BF_NodeList &lo_NodeResult,int i_Styles)
{	
	//
	lo_NodeResult.DeleteItems();
	//	
	if(i_Styles & BF_ROSTER_LOADVOLS_SPEC){
		BF_Roster_VolumeList_AddSpec(lo_NodeResult,i_Styles);
		lo_NodeResult.AddItem(new BF_Node());
	}		
	//
	{
		BVolumeRoster oRoster;
		BVolume		oVol;
		char		pc[400];
		BF_Volume	*poVol;
		//
		char		pcBoot[400]="";
		if(B_OK==oRoster.GetBootVolume(&oVol)) oVol.GetName(pcBoot);
		//
		oRoster.Rewind();
		while(oRoster.GetNextVolume(&oVol)==B_OK){				
			//if(oVol.Capacity()==0) continue;
			oVol.GetName(pc);
			if(!pc || strlen(pc)==0) continue;
		
			// init new node 
			poVol = new BF_Volume;	
			poVol->sName = pc;
			poVol->iType = BF_NODE_TYPE_DIR;
			
			// set icons 
			if(i_Styles & BF_ROSTER_LOADVOLS_SICON){
				poVol->AllocSIcon();
				oVol.GetIcon(poVol->poSIcon,B_MINI_ICON);
			}
			if(i_Styles & BF_ROSTER_LOADVOLS_LICON){
				poVol->AllocLIcon();
				oVol.GetIcon(poVol->poSIcon,B_LARGE_ICON);
			}
			// check for boot_volume //
			poVol->bBootVolume = poVol->sName==pcBoot;			
			lo_NodeResult.AddItem(poVol);
		}
	}	
	/*
	if(!b_SpecVolumsFirst){
		ls_Result.AddItem(new BL_String(""));
		BF_Roster_VolumeList_AddSpec(ls_Result);
	}
	*/
}


status_t
BF_Roster_MakeDir(const char *pc_Name)
{
	ASSERT(pc_Name);
	return create_directory(pc_Name,777);
}

BF_Node* 
BF_Roster_MakeDirNode(BF_Path &o_Path,const char *pc_Name,int i_Style)
{	
	BString s(o_Path.Path());
	if(pc_Name){
		s<<"/";
		s<<pc_Name;
	}
	if(B_OK==BF_Roster_MakeDir(s.String())){
		if(pc_Name){
			BF_Node *poNode = BF_Roster_LoadNode(o_Path.Path(),pc_Name,i_Style);
			return(poNode);
		}		
	}
	return(NULL);
}

bool
BF_Roster_RemoveNode(const char *pc_Path) 
{
	ASSERT(pc_Path);
	
	BEntry oEntry;
	if(B_OK!=oEntry.SetTo(pc_Path) ) return false;
	return B_OK==oEntry.Remove();	
}

bool		
BF_Roster_RemoveNode(BF_Path &o_Path,const char *pc_Node)
{
	ASSERT(pc_Node);
	BL_String s;
	s<<o_Path.Path(); 
	s<<"/";	
	s<<pc_Node;
	
	return BF_Roster_RemoveNode(s.String());
}

bool
BF_Roster_RemoveNode(BF_Path &o_Path,BF_Node* po_Node)
{
	ASSERT(po_Node);
	if(BF_NODE_TYPE_PARENTDIR==po_Node->iType) return false;
	BString s;
	s<<o_Path.Path();
	s<<"/";
	s<<po_Node->sName;
	
	return BF_Roster_RemoveNode(s.String());
}


status_t
BF_Roster_RenameNode(const BF_Path & o_Path,BF_Node* po_Node,const char *pc_NewName)
{
	ASSERT(po_Node && pc_NewName);
	if(BF_NODE_TYPE_PARENTDIR==po_Node->iType || po_Node->sName==pc_NewName) return B_OK;	
	BString s;
	s<<o_Path.Path();
	s<<"/";
	s<<po_Node->sName;
	//
	BEntry oEntry;
	status_t uRes=B_OK;
	uRes = oEntry.SetTo(s.String());
	if(B_OK!=uRes) return uRes;	
	//
	return oEntry.Rename(pc_NewName,false);
}

bool		
BF_Roster_NodeExists(const char *pc_Name)
{
	BNode oNode(pc_Name);
	return(oNode.InitCheck()==B_OK);
}

status_t
BF_Roster_MoveNode(BF_Path &o_Path,BF_Node* po_Node,const char *pc_NewPath,
	const char *pc_NewName)
{
	ASSERT(po_Node && pc_NewPath);
	if(BF_NODE_TYPE_PARENTDIR==po_Node->iType) return B_ERROR;
	//
	status_t uRes;
	BString s;
	s<<o_Path.Path();
	s<<"/";
	s<<po_Node->sName;
	//
	BEntry oEntry(s.String());
	uRes = oEntry.InitCheck();
	if(B_OK!=uRes) return uRes;
	//
	BDirectory oNewDir(pc_NewPath);
	uRes = oNewDir.InitCheck();
	if(B_OK!=uRes) return uRes;
	
	
	return oEntry.MoveTo(&oNewDir,pc_NewName);
}

status_t
BF_Roster_MakeLink(const char *pc_SrcFile,const char *pc_DestFile)
{
	ASSERT(pc_SrcFile && pc_DestFile);
	BDirectory oDir;	
	return oDir.CreateSymLink(pc_DestFile,pc_SrcFile,NULL);
}


void
BF_Roster_StartMonitor_Node(BF_Node *po_Node,BHandler *po_Handler,int32 i_Flags)
{
	ASSERT(po_Node);
	if(-1==i_Flags) i_Flags = B_WATCH_STAT | B_WATCH_NAME;
	watch_node(&po_Node->uNodeRef,i_Flags,po_Handler);	
	
	/*printf("Start_Monitor_Node flags=%i\n",i_Flags);*/
}

void
BF_Roster_StartMonitor_Nodes(BF_NodeCollection &lo_Node,BHandler *po_Handler)
{
	BF_Node *poNode;
	for(int i=0;i<lo_Node.CountItems();i++){
		poNode = lo_Node.NodeAt(i);
		if(BF_NODE_TYPE_PARENTDIR == poNode->iType) continue;
		BF_Roster_StartMonitor_Node(poNode,po_Handler,B_WATCH_STAT | B_WATCH_NAME);
	}
}

void
BF_Roster_StopMonitor_Node(BF_Node *po_Node,BHandler *po_Handler)
{
	ASSERT(po_Node);
	if(BF_NODE_TYPE_PARENTDIR == po_Node->iType) return;
	BF_Roster_StartMonitor_Node(po_Node,po_Handler,B_STOP_WATCHING);	
}

void
BF_Roster_StopMonitor_Nodes(BF_NodeCollection &lo_Node,BHandler *po_Handler)
{
	BF_Node *poNode;
	for(int i=0;i<lo_Node.CountItems();i++){
		poNode = lo_Node.NodeAt(i);
		if(BF_NODE_TYPE_PARENTDIR == poNode->iType) continue;
		BF_Roster_StartMonitor_Node(poNode,po_Handler,B_STOP_WATCHING);
	}
}

void
BF_Roster_StopAllMonitors(BHandler *po_Handler)
{
	stop_watching(po_Handler);
}

bool		
BF_Roster_StartMonitor_Dir(BF_FilesPath &o_Path,BHandler *po_Handler)
{
	node_ref uNodeRef(o_Path.NodeRef());
	status_t uRes = watch_node(&uNodeRef,
			B_WATCH_DIRECTORY | B_WATCH_NAME,po_Handler);
/*	printf("start dir {%s} monitoring device=%i node=%i result=%i \n",
		o_Path.String(),uNodeRef.device,uNodeRef.node,(int32)uRes);	
	printf("start dir %s\n",uRes==B_OK?"OK":"NO");*/
	return B_OK==uRes;
	
}

bool
BF_Roster_MIME_by_Name(BString & s_Name,BString & s_MIME)
{

	BMessage oMessage,oMessage1; 
	uint32 i=0,i1=0;
	char *pc,*pc1;
	BMimeType oMime;
	BL_String	sExt;
	
	i = s_Name.FindLast(".");
	if(i>0){
		s_Name.CopyInto(sExt,i+1,s_Name.Length()-i-1);
		
		if(B_OK!=BMimeType::GetInstalledTypes(&oMessage)) return false;
		i = 0;	
		
		while(true){
			if(oMessage.FindString("types", i++, (const char**)&pc) != B_OK) break; 
			//printf("type=%s\n",pc);	
		
			if(B_OK!=oMime.SetTo(pc)) continue;
			
			if(oMime.GetFileExtensions(&oMessage1) != B_OK) continue;
			
			i1 = 0;			
			while(true){
				if(oMessage1.FindString("extensions", i1++, (const char**)&pc1) != B_OK) break; 
				//sprintf("> Extension:  %s \n", pc1);
				if(sExt==pc1){
					s_MIME = pc;
					return true;
				}
			}				
		}				
	};
	return false;
}
	