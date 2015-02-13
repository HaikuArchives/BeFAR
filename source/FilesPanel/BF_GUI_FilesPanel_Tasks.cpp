#include <stdio.h>
#include <File.h>
#include <fs_attr.h>
#include <stdlib.h>

#include "BF_GUI_FilesPanel_Tasks.h"
#include "BF_Dict.h"
#include "BF_Roster.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Tasks"

BF_GUI_FilesPanel_Task::BF_GUI_FilesPanel_Task(const char *pc_Name,int32 i_Styles)
:BF_GUI_OperTask(pc_Name,i_Styles)
{
}

void
BF_GUI_FilesPanel_Task::RenameNodes(
	const BF_NodeCollection & lo_Node,
	const BF_FilesPath & o_SrcPath,
	const BL_String &s_NewName)
{
	BF_Node 	*poNode=NULL;
	BL_String 	s;
	const char  *pcNewName = NULL;
	BL_String 	sAfterMask;
	
	
		// check drive for read_only //
	if(o_SrcPath.IsVolumeReadOnly()){
		Ask_Error(B_TRANSLATE(BF_DICT_TASKS_READONLYVOL),false);
		return;
	}

	
	// check mask 
	if(s_NewName.FindFirst("*")<0)
	{// has not mask_chars
		// check count 
		if(lo_Node.CountItems()>1){
			Ask_Error(B_TRANSLATE(BF_DICT_TASKS_ONENAMEMANYFILES),false,s_NewName.String());
			return;
		}
	}	
	
	SetProgress(0,lo_Node.CountItems());
	
	for(int i=0;i<lo_Node.CountItems();i++){
		poNode = lo_Node.NodeAt(i);
		
		pcNewName = s_NewName.String();
		
		if(poNode->ConvertNameByMask(s_NewName,sAfterMask)){
			pcNewName = sAfterMask.String();
		}
		
		ASSERT(pcNewName);
		
		status_t uRes = BF_Roster_RenameNode(o_SrcPath,poNode,pcNewName);
		if(uRes!=B_OK){
			BL_String s;
			BL_System_TranslError(uRes,s);
			if(BF_GUI_OPERTASK_ERROR_OK!=Ask_Error(s.String())) return;
		}
				
		SetProgress(i,lo_Node.CountItems());
		if(Canceled()) return;
	}
}

bool
BF_GUI_FilesPanel_Task::Check_DestFolder(BF_FilesPath &oPathDest)
{
	// check dest_path for normal rules 
	if(!oPathDest.IsStorageKit()){
		Ask_Error(B_TRANSLATE(BF_DICT_TASKS_DESTFOLDERNAMEWRONG),false,oPathDest.Path());
		return false;		
	}
	
	// check dest_path on existing  	
	if(!oPathDest.IsExisting())
	{// not exist,create  it ? //
		BL_List	*ploMenu = new BL_List();
		ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_TASKS_CREATEITNOW),""));
		ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_CANCEL),""));
		
		// ask question 		
		if(1==Ask_Message(B_TRANSLATE(BF_DICT_TASKS_DESTFOLDERNOTEXIST),ploMenu)) return false;
		// try to create 
		status_t uRes = oPathDest.Create();
		// check result of creating
		if(uRes!=B_OK){
			Ask_Error(B_TRANSLATE(BF_DICT_TASKS_CANTMAKEFOLDER),false,oPathDest.Path());
			return false;
		}				
	}
		
	// check drive for read_only //
	if(oPathDest.IsVolumeReadOnly()){
		Ask_Error(B_TRANSLATE(BF_DICT_TASKS_DESTVOLREADONLY),false);
		return false;
	}
	
	return true;
}


/////////////////////////////////////////////////////////////////////////


BF_GUI_FilesPanel_DeleteTask::BF_GUI_FilesPanel_DeleteTask(
	BF_FilesPath o_Path,
	BF_NodeCollection & lo_Node,
	BF_GUI_FilesPanel_DeleteTask_Type i_DeleteType
):BF_GUI_OperTask("")
{
	iOperIndex = 0;
	iOperCount = lo_Node.CountItems();
	
	oPath = o_Path;		
	lo_Node.CopyTo(loNode,false);
	loNode.SetSelfPath(&oPath);
	iDeleteType = i_DeleteType;	
	
	// prepare sys_paths //	
	lsSysPath.AddItem(new BL_String("/boot/home"));
	lsSysPath.AddItem(new BL_String("/boot"));
	lsSysPath.AddItem(new BL_String("/boot/home/config"));
	lsSysPath.AddItem(new BL_String("/boot/home/mail"));
	lsSysPath.AddItem(new BL_String("/boot/home/apps"));
	//lsSysPath.AddItem(new BL_String("/boot/home/test"));
	
	// set new name //
	{
		BL_String s;
		s = "deleting files from /";
		s << oPath.LastDir();
		sName = s;
	}	
}


bool
BF_GUI_FilesPanel_DeleteTask::PrepareTrashPath()
{
	BL_String s(oPath.GetVolume());
	if(s=="") return false;
	sTrashPath = s;
	sTrashPath<<"/home/Desktop/Trash";
	if(!BF_Roster_NodeExists(sTrashPath.String())){
		sTrashPath = s;
		sTrashPath<<"/RECYCLED/_BEOS_";
		if(!BF_Roster_NodeExists(sTrashPath.String())){
			if(B_OK!=BF_Roster_MakeDir(sTrashPath.String())){
				s=B_TRANSLATE(BF_DICT_TASKS_CANTMAKETRASH);
				s<<sTrashPath.String();
				ASSERT(false,s.String());
			}
		}
	}
	return true;
}

bool
BF_GUI_FilesPanel_DeleteTask::DeleteList(BF_NodeCollection & lo_Node)
{
	BF_Node  	*poNode;
	status_t	uRes;
	BL_String   s;
	
	for(int i=0;i<lo_Node.CountItems();i++){
		poNode = (BF_Node*)lo_Node.ItemAt(i);
		
		if(poNode->iType==BF_NODE_TYPE_DIR){
			// check folder_path+name //
			ASSERT(lo_Node.poPath);
			s = lo_Node.poPath->Path();
			s << "/";
			s << poNode->sName;
			if(!CheckFolder_ForSys(s.String())){
				return false;
			}					
		}
		
		if(poNode->ploChild){						
			DeleteList(*poNode->ploChild);
		}
		// check, remove file or move it to trash
		if(BF_FILESPANEL_DELETE_TO_VOID==iDeleteType){
			BF_Roster_RemoveNode(*lo_Node.poPath,poNode);
		}else
		if(BF_FILESPANEL_DELETE_TO_TRASH==iDeleteType){			
			int64		iIndex = 1;	
			char		*pcName=NULL;
			while(TRUE){
				uRes = BF_Roster_MoveNode(*lo_Node.poPath,poNode,sTrashPath.String(),pcName);
				if(B_OK==uRes) break;
				/* try to move to new name */
				if(B_FILE_EXISTS==uRes){
					s=poNode->sName;
					s<<" ";
					s<<iIndex++;
					pcName = (char*)s.String();
					continue;
				}
				/* ups....some error */				
				BL_System_TranslError(uRes,s);

				BL_List	*ploMenu = new BL_List();
				ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_TRYAGAIN),"try"));
				ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_SKIP),"skip"));
				
				if(1==Ask_Message(s.String(),ploMenu)) break;
			}
		}
		//
		SetProgress(++iOperIndex,iOperCount);
		/* check for cancel_process */		
		if(Canceled()) return false;				
		
	}
	return true;
}

bool
BF_GUI_FilesPanel_DeleteTask::CheckFolder_ForSys(const char *pc_Folder)
{	
	
	// check  current path //
	BL_String *ps=NULL,s;
	
	for(int i=0;i<lsSysPath.CountItems();i++){
		ps = (BL_String*)lsSysPath.ItemAt(i);
		if(ps->ICompare(pc_Folder)!=0) continue;
		
		// prepare menu_buttons //
		BL_List *ploMenu = new BL_List();
		ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_YES),"yes"));
		ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_NO),"no"));
		
		// ask //
		int32 iResult = Ask_Message(B_TRANSLATE(BF_DICT_TASKS_SYSFOLDERDELETE),ploMenu,pc_Folder);
		if(iResult!=0) return false;
		
	}
	return true;	
}

void
BF_GUI_FilesPanel_DeleteTask::Run()
{
	// check drive for read_only //
	if(oPath.IsVolumeReadOnly()){
		Ask_Error(B_TRANSLATE(BF_DICT_TASKS_READONLYVOL),false);
		return;
	}	

	/// prepare ///
	if(BF_FILESPANEL_DELETE_TO_TRASH==iDeleteType) if(!PrepareTrashPath()) return;		
	
	// load childs for void_deleting
	if(BF_FILESPANEL_DELETE_TO_VOID==iDeleteType){			
		loNode.LoadChilds(BF_ROSTER_LOAD_NODE_ALL_REAL|BF_ROSTER_LOAD_NORMAL);
		iOperCount = loNode.CountAll(true);
		SetProgress(iOperIndex,iOperCount);
	}	
				
	/// run ///
	DeleteList(loNode);
}
/////////////////////////////////////////////////////////////////////////

BF_GUI_FilesPanel_CopyTask::BF_GUI_FilesPanel_CopyTask(
	BF_FilesPath& o_PathSrc,
	BF_FilesPath& o_PathDest,	
	BF_NodeCollection & lo_Node,
	bool b_FilesMove
):BF_GUI_FilesPanel_Task(B_TRANSLATE(BF_DICT_TASKS_COPYINGFILES))
{	bFilesMove = b_FilesMove;
	oPathSrc = o_PathSrc;
	oPathDest = o_PathDest;
	lo_Node.CopyTo(loNode,false);
	loNode.SetSelfPath(&oPathSrc);
	
	iOperIndex = 0;
	iOperCount = loNode.CountItems();	
	
	// set new name //
	{
		BL_String s;
		s = B_TRANSLATE(BF_DICT_TASKS_COPYINGFILESFROM);
		s << oPathSrc.LastDir();
		s << B_TRANSLATE(BF_DICT_TASKS_COPYINGFILESTO);
		s << oPathDest.LastDir();
		sName = s;
	}	
}

/* copy atribytes */
bool 
BF_GUI_FilesPanel_CopyTask::Copy_Atributes(BNode & o_NodeSrc,BNode & o_NodeDest)
{
	char 		pcName[B_ATTR_NAME_LENGTH];
	attr_info	uAttrInfo;
	uint32		iBufMaxSize=255;
	char		*pcBuf = (char*)malloc(iBufMaxSize);
	
	o_NodeSrc.RewindAttrs();
	//
	while(B_OK==o_NodeSrc.GetNextAttrName(pcName)){
		if(B_OK==o_NodeSrc.GetAttrInfo(pcName,&uAttrInfo)){
			/* check buffer size  */
			if(uAttrInfo.size>iBufMaxSize){
				DELETE(pcBuf);
				iBufMaxSize = uAttrInfo.size;
				pcBuf = (char*)malloc(iBufMaxSize);									
			}
			/* read attr */
			o_NodeSrc.ReadAttr(pcName,uAttrInfo.type,0,
				(void*)pcBuf,uAttrInfo.size);
			/* write attr */
			o_NodeDest.WriteAttr(pcName,uAttrInfo.type,
					0,(void*)pcBuf,uAttrInfo.size);
			/* check for cancel_process */		
			if(Canceled()) return false;				
		}
	}						
	DELETE(pcBuf);
	return true;
}/* end of atributes */					


bool
BF_GUI_FilesPanel_CopyTask::Copy_Nodes(
	BF_NodeCollection *plo_Node,
	BString		& s_NewPath,
	bool		b_DeleteAfterCopy)
{
	if(!plo_Node) return true;

	BF_Node 	*poNode;
	BL_String	s,s1;
	const char	*pcNewName;
	status_t 	uRes;
	BString		sNewName;
		
	for(int i=0;i<plo_Node->CountItems();i++){
		poNode = (BF_Node*)plo_Node->ItemAt(i);
		ASSERT(plo_Node->poPath);		
		/* convert name for mask */
		/*
		if(poNode->ConvertNameByMask(s_NewPath,sNewName)) pcNewName = sNewName.String();
		else */pcNewName = poNode->sName.String();
		/* make new link */		
		if(poNode->bLink){
			// make link
			s=plo_Node->poPath->Path();
			s<<"/";
			s<<poNode->sName;
			s1=s_NewPath;
			s1<<"/";
			s1<<pcNewName;
			while(true){
				uRes = BF_Roster_MakeLink(s.String(),s1.String());
				if(B_FILE_EXISTS==uRes){
					BF_GUI_OperTask_Copy_Answer iResult = Ask_Copy_FileExist(poNode->Name(),false);
					
					if(BF_GUI_OPERTASK_COPY_SKIP == iResult ){						
						break;
					}else
					if(BF_GUI_OPERTASK_COPY_OVERWRITE == iResult){
						BF_Roster_RemoveNode(s1.String());					
						continue;
					}else
					if(BF_GUI_OPERTASK_COPY_CANCEL == iResult){
						return false;
					}else
					{}								
				}else
				if(B_OK!=uRes){
					// show message about uknown error //					
					BL_System_TranslError(uRes,s);
					if(BF_GUI_OPERTASK_ERROR_CANCEL_ALL==Ask_Error(s.String())) 
						return false; // cancel all //
				}else
				{}
				break;
			}
			//iOperIndex += poNode->iSize;
			//SetProgress(iOperIndex,iOperCount);			
		}else
		// make new dir //
		if(BF_NODE_TYPE_DIR==poNode->iType){
			// make new dir //
			s=s_NewPath;
			s<<"/";
			s<<pcNewName;
			BF_Roster_MakeDir(s.String());			
			
			//iOperIndex += poNode->iSize;
			//SetProgress(iOperIndex,iOperCount);
			
			// copy atributes //
			{
				// open src and dest nodes //
				BNode oDirDest(s.String());
				
				s1=plo_Node->poPath->Path();
				s1<<"/";
				s1<<poNode->sName;			
				
				BNode oDirSrc(s1.String());				
				
				if(!Copy_Atributes(oDirSrc,oDirDest)) return false;
			}						
			// copy childs  //
			BString  sNewPath(s);
			if(!Copy_Nodes(poNode->ploChild,sNewPath,b_DeleteAfterCopy)) return false;						
		}else					
		// copy file //
		{		
			// open source file
			s=plo_Node->poPath->Path();
			s<<"/";
			s<<poNode->sName;			
			
			BFile oSrcFile(s.String(),B_READ_ONLY);
			if(B_OK==oSrcFile.InitCheck()){				
				// open dest file
				s="";
				if(!strchr(pcNewName,'/')) 
					if(s_NewPath.FindFirst("/")==0)	s<<s_NewPath; else	  s<<plo_Node->poPath->Path();
				s<<"/";
				s<<pcNewName;
				BFile oDestFile;	
				
				/*uRes = oDestFile.SetTo(s.String(),B_READ_WRITE | B_CREATE_FILE | B_FAIL_IF_EXISTS);				
				if(B_FILE_EXISTS==uRes){*/
				
				uRes = oDestFile.SetTo(s.String(),B_READ_ONLY);				
				
				if(B_OK==uRes){
					BF_GUI_OperTask_Copy_Answer iResult = Ask_Copy_FileExist(poNode->Name());
					if(BF_GUI_OPERTASK_COPY_SKIP == iResult ){
						iOperIndex+=poNode->iSize;
						SetProgress(iOperIndex,iOperCount);
						continue;
					}else
					if(BF_GUI_OPERTASK_COPY_APPEND == iResult){
						uRes = oDestFile.SetTo(s.String(),B_READ_WRITE | B_CREATE_FILE | B_OPEN_AT_END);				
					}else
					if(BF_GUI_OPERTASK_COPY_OVERWRITE == iResult){
						uRes = oDestFile.SetTo(s.String(),B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);				
					}else
					if(BF_GUI_OPERTASK_COPY_CANCEL == iResult){
						return false;
					}else
					{}
				}
				else{
					uRes = oDestFile.SetTo(s.String(),B_READ_WRITE | B_CREATE_FILE | B_FAIL_IF_EXISTS);					
				}										
										
				if(B_OK == uRes)
				{
					//oDestFile.Unset();
					//oDestFile.SetTo(s.String(),
					/// copy data
					uint64 	iStart=0,iCount=0;					
					while(iStart<poNode->iSize){
						// calc buffer_size
						iCount = BF_GUI_FILESPANEL_COPY_BUFFER_SIZE;
						if((iStart+iCount)>poNode->iSize)	iCount = poNode->iSize-iStart;
						// read
						oSrcFile.Read((void*)pcBuffer,iCount);
						// write
						oDestFile.Write((void*)pcBuffer,iCount);												
						// step one_progress
						iStart+=iCount;
						iOperIndex+=iCount;
						//printf("iOperIndex=%i  iCount=%i\n",iOperIndex,iCount);
						SetProgress(iOperIndex,iOperCount);												
						/* check for cancel_process */		
						if(Canceled()) return false;				
					}													
				}else /* end of work with oDestFile */
				{ // skip this file
					BL_String sError;
					BL_System_TranslError(uRes,sError);
					if(BF_GUI_OPERTASK_ERROR_CANCEL_ALL==Ask_Error(sError.String())) return false;
					iOperIndex+=poNode->iSize;
					SetProgress(iOperIndex,iOperCount);								
				}				
				if(!Copy_Atributes(oSrcFile,oDestFile)) return false;
				oDestFile.Unset();				
			}
		} /* end of file_node */
		/* check for cancel_process */		
		if(Canceled()) return false;				
		/* delete, if removing */
		if(b_DeleteAfterCopy){
			BF_Roster_RemoveNode(*plo_Node->poPath,*poNode);
		}
	}				
	return true;
}

void								
BF_GUI_FilesPanel_CopyTask::Move_Nodes()
{
	iOperCount = loNode.CountItems();
	SetProgress(iOperIndex,iOperCount);
	
	BF_Node  	*poNode=NULL;
	status_t	uRes=B_ERROR;	
	
	
	for(int i=0;i<loNode.CountItems();i++){
		poNode = loNode.NodeAt(i);
		while(true){
			uRes = BF_Roster_MoveNode(oPathSrc,poNode,oPathDest.Path());
			if(uRes==B_OK) break;
			if(B_FILE_EXISTS == uRes){
				BL_String s(B_TRANSLATE(BF_DICT_TASKS_DESTFILE));
				s<<poNode->sName;
				s<<B_TRANSLATE(BF_DICT_TASKS_DESTFILEEXIST);
				BL_List *ploMenu= new BL_List();
				ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_OWERWRITE),"") );
				ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_SKIP),"") );
				ploMenu->AddItem(new BF_GUI_ViewMenu_Item(B_TRANSLATE(BF_DICT_CANCELALL),"") );
				int iRes = Ask_Message(s.String(),ploMenu);
				if(iRes==0){
					// delete dest_file //
					BF_Roster_RemoveNode(oPathDest,poNode->sName.String());
					continue;
				}else
				if(iRes==2){
					// cancel all //
					i = loNode.CountItems()+1;													
				}
			}else{
				BL_String s(B_TRANSLATE(BF_DICT_TASKS_CANTMOVE)),s1;
				BL_System_TranslError(uRes,s1);	
				s<<poNode->sName;							
				if(BF_GUI_OPERTASK_ERROR_CANCEL_ALL==Ask_Error(s.String(),true,s1.String()) ){				
					// cancel all //
					i = loNode.CountItems()+1;				
				}				
			}	
			break;		
		}
		SetProgress(++iOperIndex,iOperCount);
	}
}

void								
BF_GUI_FilesPanel_CopyTask::Run()
{	
	if(bFilesMove)
	{// prepare to move 
	
		BL_String s(oPathDest.Path());
		
		// check first slash 
		if(s.FindFirst("/")==0){
			// begin from slash
			if(s.FindFirst("*")>=0){
				Ask_Error("Sorry, but this release can`t move and rename by mask at one time",false);
				return;
			}
		}else{
			// begin from non-slash
			// rename files
			RenameNodes(loNode,oPathSrc,s);
			return;			
		}
	
		BL_String  sSrcVol(oPathSrc.GetVolume()),sDestVol(oPathDest.GetVolume());
		if(sSrcVol==sDestVol){
			Move_Nodes();
			return;	
		}		
	}else
	{// prepare to copy 
		
	}
	
	if(!Check_DestFolder(oPathDest)) return;
	
	// load childs for 
	{			
		loNode.LoadChilds(BF_ROSTER_LOAD_NODE_ALL_REAL|BF_ROSTER_LOAD_NORMAL);
		iOperCount = loNode.SizeAll(true);
		SetProgress(iOperIndex,iOperCount);
	}
	BL_String sNewPath(oPathDest.Path());
	Copy_Nodes(&loNode,sNewPath,bFilesMove);
}


/////////////////////////////////////////////////////////////////////////

BF_GUI_FilesPanel_ChangeCaseTask::BF_GUI_FilesPanel_ChangeCaseTask(
	BF_FilesPath& o_Path,
	BF_NodeCollection & lo_Node,
	int32 i_CaseType
):BF_GUI_OperTask("change case")
{
	lo_Node.CopyTo(loNode,false);
	iCaseType = i_CaseType;
	iOperIndex = 0;
	iOperCount = loNode.CountItems();

	oPath = o_Path;
}

void								
BF_GUI_FilesPanel_ChangeCaseTask::Run()
{
	// check drive for read_only //
	if(oPath.IsVolumeReadOnly()){
		Ask_Error(B_TRANSLATE(BF_DICT_TASKS_READONLYVOL),false);
		return;
	}	

	BF_Node *poNode=NULL;
	BL_String s;
	
	SetProgress(iOperIndex,iOperCount);
	
	for(int i=0;i<loNode.CountItems();i++){
		poNode = loNode.NodeAt(i);
		s = poNode->sName;
		switch(iCaseType){
		case 0:
			s.ToLower();
			break;
		case 1:
			s.ToUpper();
			break;
		case 2:
			s.Capitalize();
			break;
		case 3:
			s.CapitalizeEachWord();
			break;			
		}
		BF_Roster_RenameNode(oPath,poNode,s.String());
		SetProgress(++iOperIndex,iOperCount);
		if(Canceled()) return;
	}
}
/////////////////////////////////////////////////////////////////////////
BF_GUI_FilesPanel_Rename::BF_GUI_FilesPanel_Rename(
	BF_FilesPath& o_Path,
	BF_NodeCollection & lo_Node,
	BL_String & s_NewName
):BF_GUI_FilesPanel_Task("rename files")
{
	lo_Node.CopyTo(loNode,false);
	oPath = o_Path;
	sNewName = s_NewName;
}

void
BF_GUI_FilesPanel_Rename::Run()
{
	RenameNodes(loNode,oPath,sNewName);
}

/////////////////////////////////////////////////////////////////////////
BF_GUI_FilesPanel_MakeLinksTask::BF_GUI_FilesPanel_MakeLinksTask(
	BF_FilesPath& o_PathSrc,
	BF_FilesPath& o_PathDest,
	BF_NodeCollection & lo_Node
):BF_GUI_FilesPanel_Task("make links")
{
	oPathSrc = o_PathSrc;
	oPathDest = o_PathDest;
	lo_Node.CopyTo(loNode,false);
	loNode.SetSelfPath(&oPathSrc);		
}
	
void
BF_GUI_FilesPanel_MakeLinksTask::Run()
{
	// check dest_path
	{
		BL_String s(oPathDest.Path());
				
		if(s.FindFirst("/")==0)
		{// path begin from /
			if(s.FindFirst("*")>=0)
			{// path include * 
				Ask_Error(B_TRANSLATE(BF_DICT_TASKS_CANTMAKERENAME),false);
				return;		
			}						
		}else
		{// path dosn`t begin from /
			if(s.FindFirst("/")>0)
			{// path include /
				Ask_Error(B_TRANSLATE(BF_DICT_TASKS_CANTMAKEINFOLDER),false,s.String());
				return;
			}			
			// ? mask with * 
			if(s.FindFirst("*")>=0){	
				// ok mask with *, mask = dest_path and dest_path = oPathSrc
				sMask = s;
				oPathDest = oPathSrc;
			}else{
				// fuck,mask without *				
				if(loNode.CountItems()>1){
					Ask_Error(B_TRANSLATE(BF_DICT_TASKS_CANTMAKELINKONENAME),false,s.String());
					return;	
				}else{
					// ok, we must make only one link
					sMask = s;
					oPathDest = oPathSrc;
				}
			}			
		}
	}

	// check dest_path
	if(!Check_DestFolder(oPathDest)) return;	

	// start //
	
	SetProgress(0,loNode.CountItems());	
	
	BL_String 	sSrc,sDest,s,s1;
	status_t	uRes;
	BF_Node		*poNode=NULL;
	
	
	for(int i=0;i<loNode.CountItems();i++){
		poNode = loNode.NodeAt(i);
	
		sSrc = oPathSrc.Path();
		sSrc<<"/";
		sSrc<<poNode->Name();
		
		sDest = oPathDest.Path();
		sDest<<"/";
		sDest<<*poNode->ConvertNameByMaskExt(sMask,s1);
		
		uRes = BF_Roster_MakeLink(sSrc.String(),sDest.String());
		if(B_FILE_EXISTS==uRes){
			s=B_TRANSLATE(BF_DICT_TASKS_DESTFILE);
			s<<poNode->sName;
			s<<B_TRANSLATE(BF_DICT_TASKS_DESTFILEEXIST);
			if(BF_GUI_OPERTASK_ERROR_CANCEL_ALL==Ask_Error(s.String())) return;
		}else
		if(uRes!=B_OK){
			BL_System_TranslError(uRes,s);
			if(BF_GUI_OPERTASK_ERROR_CANCEL_ALL==Ask_Error(s.String())) return;
		}		
		SetProgress(i,loNode.CountItems());
		if(Canceled()) return;
	}
}

/////////////////////////////////////////////////////////////////////////																
