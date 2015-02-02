#include "BF_Node.h"
#include "BF_Roster.h"
#include "stdio.h"

#include <Application.h>
#include <Path.h>
#include <Volume.h>

BF_Node::BF_Node(const char *pc_Name)
{
	if(pc_Name)	sName << pc_Name;
	iSize = 0;
	iType = BF_NODE_TYPE_UNKNOWN;
	bSelect = false;
	bLink = false;
	poSIcon = NULL;
	poLIcon = NULL;	
	ploChild = NULL;
	iStyles = BF_NODE_STYLE_DEFAULT;	
}

BF_Node::BF_Node(BF_Node *po_FromNode,bool b_SetIcon)
{
	ASSERT(po_FromNode);
	bSelect = false;	
	poSIcon = NULL;		
	poLIcon = NULL;
	ploChild = NULL;
	Set(po_FromNode,b_SetIcon);
	
}


BF_Node::BF_Node(const BF_Node &o_FromNode,bool b_SetIcon)
{
	bSelect = false;	
	poSIcon = NULL;		
	poLIcon = NULL;
	ploChild = NULL;
	Set(o_FromNode,b_SetIcon);
	
}

void
BF_Node::AllocLIcon()
{
	if(!poLIcon) poLIcon = new BBitmap(BRect(0,0,31,31),B_RGB32);
}

void
BF_Node::AllocSIcon()
{
	if(!poSIcon) poSIcon = new BBitmap(BRect(0,0,15,15),B_RGB32);
}

void
BF_Node::FreeSIcon()
{
	if(!poSIcon) return;
	DELETE(poSIcon);
}

const char*
BF_Node::Name() const 
{
	return sName.String();
}

int
BF_Node::Type() const 
{ return iType; }

void				
BF_Node::Set( BF_Node *po_FromNode,bool b_SetIcon)
{
	ASSERT(po_FromNode && po_FromNode!=this);
	Set(*po_FromNode,b_SetIcon);
}

void				
BF_Node::Set(const BF_Node &o_FromNode,bool b_SetIcon)
{
	sName = o_FromNode.sName;
	sType = o_FromNode.sType;
	iSize = o_FromNode.iSize;	
	uCreateTime = o_FromNode.uCreateTime;		
	iType = o_FromNode.iType;			
	bLink = o_FromNode.bLink;			
	uNodeRef = o_FromNode.uNodeRef;		
	iStyles = o_FromNode.iStyles;
	/* copy icon */
	if(b_SetIcon && o_FromNode.poSIcon) CopySIconFrom(o_FromNode.poSIcon);
	//uEntryRef = po_FromNode->uEntryRef;					
}

void				
BF_Node::CopySIconFrom(BBitmap *po_SIcon)
{
	ASSERT(po_SIcon);
	if(poSIcon) DELETE(poSIcon);
	poSIcon = new BBitmap(po_SIcon);
}

BF_Node::~BF_Node()
{
	if(poSIcon) DELETE(poSIcon);
	if(poLIcon) DELETE(poLIcon);	
	if(ploChild) DELETE(ploChild);
}

void
BF_Node::PermsTo(BL_String & s_Res) const
{
	//read/write/execute * owner/group/others 
	s_Res = "";
	s_Res<<((uPerms & S_IRUSR)?"r":"-");		
	s_Res<<((uPerms & S_IWUSR)?"w":"-");		
	s_Res<<((uPerms & S_IXUSR)?"x":"-");		
	s_Res<<" ";
	s_Res<<((uPerms & S_IRGRP)?"r":"-");		
	s_Res<<((uPerms & S_IWGRP)?"w":"-");		
	s_Res<<((uPerms & S_IXGRP)?"x":"-");		
	s_Res<<" ";
	s_Res<<((uPerms & S_IROTH)?"r":"-");		
	s_Res<<((uPerms & S_IWOTH)?"w":"-");		
	s_Res<<((uPerms & S_IXOTH)?"x":"-");	
}

const BString*			
BF_Node::ConvertNameByMaskExt(const BString & s_Mask,BString & s_NewName) const
{
	if(!ConvertNameByMask(s_Mask,s_NewName)) return &sName;else return &s_NewName;
}

bool
BF_Node::ConvertNameByMask(const BString & s_Mask,BString & s_NewName) const
{
	if(s_Mask=="") return false;
	/* check for mask */
	if(s_Mask.FindFirst("*")<0){
		s_NewName = s_Mask;
		return true;
	}else	
	if(s_Mask.FindFirst(".*")>0){
		s_NewName = "";		
		
		int iMaskPos = s_Mask.FindFirst(".*");
		s_NewName.Append(s_Mask,iMaskPos);		

		int iDotPos = sName.FindFirst(".");
		if(iDotPos<0) return false;
		BL_String s;
		sName.CopyInto(s,iDotPos,sName.Length()-iDotPos);
		s_NewName << s;				
		
		return true;	
	}else		
	if(s_Mask.FindFirst("*.")==0){
		s_NewName = "";		
		
		int iDotPos = sName.FindFirst(".");
		if(iDotPos<0) return false;
		sName.CopyInto(s_NewName,0,iDotPos+1);
		
		BL_String s;
		s_Mask.CopyInto(s,2,s_Mask.Length()-2);
		s_NewName<<s;				
		
		return true;			
	}else
	{
		int i=s_Mask.FindFirst("*");
		s_NewName="";
		if(i>0) s_NewName.Append(s_Mask,i);
		i++;
		s_NewName.Append(sName);
		const char *pc = s_Mask.String()+i;
		s_NewName.Append(pc,strlen(pc));
			
		return true;
	}
}


int8				
BF_Node::CompareCreateTime(const BF_Node &o_CompNode) const
{
	if(o_CompNode.uCreateTime > uCreateTime) return 1;
	if(o_CompNode.uCreateTime < uCreateTime) return -1;	
	return 0;
}

void				
BF_Node::SetSelect(uint i_SelectAction)
{
	if(BF_NODE_TYPE_PARENTDIR==iType) return;
	switch(i_SelectAction){
	case BF_NODE_SELECT_INVERT:
		bSelect = !bSelect;break;
	default:
		bSelect = i_SelectAction;		
	}
}

bool 
BF_Node::CheckByMask(const char *pc_MaskLine) const
{
	ASSERT(pc_MaskLine);
	if(BF_NODE_TYPE_PARENTDIR==iType) return false;
	if(strlen(pc_MaskLine)==0) return false;
		

/*	
	BL_String sMask(pc_MaskLine);
	// check *.lala 
	if(sMask.FindFirst("*")==0 && sMask.FindFirst("*",1)<0){
		BL_String 	s;	
		int32		iOffset;
		sMask.CopyInto(s,1,sMask.Length()-2);
		if(sName.FindFirst(s.String())>0) return true;
	}else
	// check *.*
	if(sMask=="*.*" || sMask=="*"){
		return true;
	}else
	// check *.lala 
	if(sMask.FindFirst("*")==(sMask.Length()-1)){
		BL_String s;	
		sMask.CopyInto(s,1,sMask.Length()-2);
		if(sName.FindFirst(s.String())>0) return true;
	}else		
	{}
	
	return false;

*/				
	char pcSelf[sName.Length()+1];	
	strcpy(pcSelf,sName.String());
	//
	char *pc,*pc1 = pcSelf,c1,*pc3;
	char  pcMask[strlen(pc_MaskLine)+1];
	char *pc2 = pcMask;
	strcpy(pcMask,pc_MaskLine);
	//
	while('\0'!=pc2[0] || '\0'!=pc1[0]){
		// if current char '*'   
		if('*'==pc2[0]){
			pc2++;
			if('\0'==pc2[0]) return true;
		}else
		// if current char '?'   
		if('?'==pc2[0]){			
			pc2++;pc1++;
		}else{
			// any other char 
			pc = pc2;
			while(pc[0]!='\0' && pc[0]!='*' && pc[0]!='?') pc++;
			c1 = pc[0];
			pc[0]='\0';
			//printf("pc1<%s> pc2<%s>\n",pc1,pc2);
			pc3 = strstr(pc1,pc2);
			if(!pc3){
				pc[0] = c1;
				if(strcmp(pc2,".*")==0) return true; 
				return false;			
			}
			pc1=pc3+strlen(pc2);
			pc[0] = c1;
			if('\0'==pc[0]) return '\0' == pc1[0];
			pc2 = pc;
		}		
	}	
	return '\0'==pc1[0] && '\0'==pc2[0];
}

/*=====================================================*/
BF_Volume::BF_Volume():BF_Node((const char*)NULL)
{
	bBootVolume = false;
	bReadOnly = false;
	bRemovable = false;
	bPersistent = false;
	bShared = false;
	bKnowsQuery = false;
	
	iCapacity = 0;
	iFree = 0;
	
	bLoadedOk = false;
}

bool
BF_Volume::Load(const char *pc_Name,int i_NodeLoadStyles)
{
	bLoadedOk = false;
	if(pc_Name) sName = pc_Name;
	if(!BF_Roster_ReloadNode(NULL,this,i_NodeLoadStyles)) return false;
	
	LoadInfo();
		 
	return true;
}

void				
BF_Volume::LoadInfo()
{	
	bLoadedOk = false; 
	
	BVolume oVol(uNodeRef.device);	
	bLoadedOk = B_OK==oVol.InitCheck();
	if(!bLoadedOk) return;
	
	iCapacity = oVol.Capacity();
	iFree = 	oVol.FreeBytes();
	bReadOnly = oVol.IsReadOnly();
	bRemovable = oVol.IsRemovable();
	bPersistent = oVol.IsPersistent();
	bShared	 = oVol.IsShared();
	bKnowsQuery = oVol.KnowsQuery();
}
/*=====================================================*/
BF_Path::BF_Path(const char *pc_Value)
{
	SetTo(pc_Value?pc_Value:"");
}

BF_Path::~BF_Path()
{
}

const BL_String     
BF_Path::PathWFS() const
{
	BL_String s;
	sValue.CopyInto(s,1,sValue.Length()-1);
	return s;
}

const BL_String		
BF_Path::String() const
{
	return BL_String(Path());
}

const char*				
BF_Path::Path() const
{
	return sValue.String();
}

void				
BF_Path::operator = (const BL_String & s_From)
{
	SetTo(s_From.String());
}

void				
BF_Path::SetTo(const char *pc_Path)
{
	sValue = pc_Path;
}

bool
BF_Path::operator	== (const char *pc_ComparePath) const
{
	return sValue==pc_ComparePath;
}

bool				
BF_Path::InFolder(const char *pc_ParentPath) const
{
	return sValue.FindFirst(pc_ParentPath)==0;
}

// sValue="/boot/home/test",s_Result = b_CleanVolume?"boot":"/boot"

const BString 		
BF_Path::GetVolume(bool b_CleanVolume) const
{
	BString s;
	
	if(sValue.FindFirst('/')<0 || sValue.Length()==1) return s;

	int i2=sValue.FindFirst('/',1);
	if(i2<=0) i2 = sValue.Length();
	//
	sValue.CopyInto(s,(b_CleanVolume?1:0),i2-(b_CleanVolume?1:0));
	//	
	return s;
}

bool				
BF_Path::IsStorageKit() const
{
	return sValue.FindFirst("//")<0 &&  sValue.FindFirst("/")==0;
}

bool				
BF_Path::LastDir(BString & s) const
{	
	int i=sValue.FindLast('/');
	if(i<0) return false;
	sValue.CopyInto(s,i+1,sValue.Length()-1-i);
	printf("LastDir %s\n",s.String());
	return true;
}

const BL_String		
BF_Path::LastDir() const
{
	BL_String s;
	LastDir(s);
	return s;
}

bool
BF_Path::IsVirtualRoot() const
{
	return sValue=="/";	
}

#include <Roster.h>

bool
BF_Path::GetPathForCurrentApp(BL_String & s_Path)
{
	app_info uAppInfo;
	be_app->GetAppInfo(&uAppInfo);
	BEntry 	oEntry(&uAppInfo.ref);
	oEntry.GetParent(&oEntry);
	BPath	oPath;
	oEntry.GetPath(&oPath);

	s_Path	= oPath.Path();
	return true;
}

bool				
BF_Path::GoParent()
{
	BPath oPath(Path());
	if(B_OK!=oPath.GetParent(&oPath)) return false;
	
	sValue = oPath.Path();	
	
	return(true);
}

bool				
BF_Path::GoLocalDir(const BString & s_LocalDir)
{	
	if(sValue!="/")	sValue<<"/";
	sValue<<s_LocalDir;		
	
	return(true);
}

bool				
BF_Path::Compare(const char *pc_Path)
{
	ASSERT(pc_Path);
	return sValue==pc_Path;
}




bool
BF_Path::IsEmpty() const
{
	return sValue=="";
}

/*=====================================================*/

BF_FilesPath::BF_FilesPath(const char *pc_Value)
:BF_Path(pc_Value)
{			
	SetTo(pc_Value,NULL);
}

BF_FilesPath::BF_FilesPath(const BF_Path & o_SrcPath)
{	
	SetTo(o_SrcPath);
}


BF_FilesPath::BF_FilesPath(const BF_FilesPath & o_SrcPath)
{	
	SetTo(o_SrcPath);
}

bool				
BF_FilesPath::IsExisting() const
{
	return BF_Roster_NodeExists(Path());
}

status_t			
BF_FilesPath::Create()
{
	return BF_Roster_MakeDir(Path());
}

bool
BF_FilesPath::IsVolumeReadOnly() const
{
	// check drive for read_only //
	BF_Volume 	oVol;
	BL_String	s(GetVolume(true));
	oVol.Load(s.String(),BF_ROSTER_LOAD_NORMAL);
	return oVol.bReadOnly;
}



bool				
BF_FilesPath::GoLocalDir(const BString & s_LocalDir)
{
	if(!BF_Path::GoLocalDir(s_LocalDir)) return false;
	
	BF_Roster_LoadNodeRef(sValue.String(),uNodeRef);
	
	return true;
}

bool				
BF_FilesPath::GoParent()
{
	if(!BF_Path::GoParent()) return true;
	
	BF_Roster_LoadNodeRef(sValue.String(),uNodeRef);
	return true;
}

BBitmap*		
BF_FilesPath::LoadSIcon()
{	
	BBitmap* poSIcon;
	
	BF_Node *poNode = BF_Roster_LoadNode("",Path(),BF_ROSTER_LOAD_SICON);
	if(!poNode) return NULL;
	poSIcon = poNode->poSIcon;
	poNode->poSIcon = NULL;
	DELETE(poNode);
			
	return poSIcon;
}

void 				
BF_FilesPath::operator = (const BF_FilesPath &o_Path)
{
	BF_FilesPath::SetTo(o_Path);
}

void 				
BF_FilesPath::operator = (const BF_Path &o_Path)
{
	BF_FilesPath::SetTo(o_Path.sValue.String(),NULL);
}

void 				
BF_FilesPath::SetTo(const BF_FilesPath &o_Path)
{
	BF_FilesPath::SetTo(o_Path.sValue.String(),&o_Path.uNodeRef);
}

bool				
BF_FilesPath::Compare(const BF_FilesPath &o_SomePath)
{
	return uNodeRef==o_SomePath.uNodeRef;
}

void				
BF_FilesPath::SetTo(const char *pc_Path,const node_ref *pu_NodeRef)
{
	BF_Path::SetTo(pc_Path); 
	/* set node_ref for this path */
	if(pu_NodeRef) 
		uNodeRef = *pu_NodeRef;
	else{
		BF_Roster_LoadNodeRef(pc_Path,uNodeRef); 
	}
}

node_ref&			
BF_FilesPath::NodeRef() const
{
	node_ref *puResultRef = new node_ref(uNodeRef);
	return *puResultRef;
}

bool				
BF_FilesPath::CheckNodeRef(const node_ref & u_NodeRef) const
{
	return uNodeRef==u_NodeRef;
}


/*=====================================================*/
BF_NodeCollection::BF_NodeCollection(bool b_DeleteItems)
:BL_Collection(b_DeleteItems)
{
	poPath = NULL;
	bSelfPath = FALSE;
}

BF_NodeCollection::~BF_NodeCollection()
{
}

void				
BF_NodeCollection::MakePath(const char *pc_Path)
{
	if(poPath && !bSelfPath) poPath = NULL;
	bSelfPath = true;
	if(!poPath) poPath = new BF_Path();
	poPath->SetTo(pc_Path);
}

void				
BF_NodeCollection::SetSelfPath(BF_Path* po_NewPath)
{
	ASSERT(po_NewPath);
	if(poPath && bSelfPath) DELETE(poPath);
	bSelfPath = true;
	poPath = po_NewPath;	
}

void				
BF_NodeCollection::CopyTo(BF_NodeCollection & lo_Dest,bool b_CopyIcons) const
{
	BF_Node *poNodeSrc,*poNodeDest;
	for(int i=0;i<CountItems();i++){
		poNodeSrc = NodeAt(i);
		poNodeDest = new BF_Node(*poNodeSrc,b_CopyIcons);
		lo_Dest.AddItem(poNodeDest);
	}
}


BF_Node*
BF_NodeCollection::NodeByName(const char *pc_Name) const
{
	ASSERT(pc_Name);
	BF_Node *poNode;
	for(int i=0;i<CountItems();i++){
		poNode =(BF_Node*)ItemAt(i);
		if(poNode->sName==pc_Name){			
			return poNode;
		}
	}
	return NULL;
}

int32
BF_NodeCollection::IndexByName(const char *pc_Name) const
{
	ASSERT(pc_Name);
	BF_Node *poNode;
	for(int i=0;i<CountItems();i++){
		poNode =(BF_Node*)ItemAt(i);
		if(poNode->sName==pc_Name)	return i;
	}
	return 0;
}
 


BF_Node*			
BF_NodeCollection::NodeByRef(node_ref & u_NodeRef) const
{
	BF_Node *poNode;
	for(int i=0;i<CountItems();i++){
		poNode =(BF_Node*)ItemAt(i);
		if(BF_NODE_TYPE_PARENTDIR == poNode->iType) continue;
		/*printf("checking node name=%s device=%i node=%i\n",
			poNode->sName.String(),	poNode->uRef.device,poNode->uRef.node);*/
		if(poNode->uNodeRef==u_NodeRef) return poNode;
	}	
	return NULL;
}

BF_Node*			
BF_NodeCollection::NodeAt(int32 i_Index) const
{
	return( (BF_Node*)ItemAt(i_Index) );
}

void				
BF_NodeCollection::SetSelectAll(uint i_SelectAction,const char *pc_Mask,int i_What)
{
	if(pc_Mask && strlen(pc_Mask)==0) return;
	BF_Node *poNode;	
	for(int i=0;i<CountItems();i++){
		poNode = (BF_Node*)ItemAt(i);
		if(pc_Mask && !poNode->CheckByMask(pc_Mask)) continue;
		if(poNode->iType==BF_NODE_TYPE_DIR && !(i_What & BF_NODELIST_NODETYPE_DIRS)) continue;
		if(poNode->iType==BF_NODE_TYPE_FILE && !(i_What & BF_NODELIST_NODETYPE_FILES)) continue;		
		if(poNode->iType==BF_NODE_TYPE_LINK && !(i_What & BF_NODELIST_NODETYPE_LINKS)) continue;		
		poNode->SetSelect(i_SelectAction);
	}
}
uint64				
BF_NodeCollection::GetSelecting(BF_NodeCollection & lo_Result) const
{
	BF_Node *poNode;	
	lo_Result.MakeEmpty();
	for(int i=0;i<CountItems();i++){
		poNode = (BF_Node*)ItemAt(i);
		if(poNode->bSelect) lo_Result.AddItem(poNode);
	}
	lo_Result.poPath = poPath;
	return lo_Result.CountItems();
}

uint64				
BF_NodeCollection::CountRealNodes() const
{
	uint64 iCount = CountItems();
	BF_Node *poNode = NodeAt(0);
	if(poNode && BF_NODE_TYPE_PARENTDIR==poNode->iType) iCount--;
	return iCount;
}

uint64				
BF_NodeCollection::SizeAll(bool b_WithChilds) const
{	
	uint64 iSize = 0;
	BF_Node *poNode;
	for(int i=0;i<CountItems();i++){
		poNode = NodeAt(i);				
		
		if( poNode->bLink ) continue;
		if( BF_NODE_TYPE_FILE==poNode->iType ) iSize+=poNode->iSize;
		if( BF_NODE_TYPE_DIR!=poNode->iType ) continue;
		
		if(!b_WithChilds && !poNode->ploChild) continue;
		/* size of childs this node */
		iSize+=poNode->ploChild->SizeAll(true);
	}
	return iSize;
}

uint64				
BF_NodeCollection::CountAll(bool b_WithChilds) const
{
	uint64 iCount = CountItems();
	/* count of all childs */
	if(b_WithChilds){
		BF_Node *poNode;
		for(int i=0;i<CountItems();i++){
			poNode  = NodeAt(i);			
			if(poNode->bLink || BF_NODE_TYPE_DIR!=poNode->iType) continue;
			if(!poNode->ploChild) continue;
			iCount+=poNode->ploChild->CountAll(true);
		}
	}
	return iCount;
}

void				
BF_NodeCollection::SelectedInfo(uint64 & i_Bytes,uint64 & i_Count) const
{
	i_Bytes = 0;
	i_Count = 0;
	BF_Node *poNode;	
	for(int i=0;i<CountItems();i++){
		poNode = (BF_Node*)ItemAt(i);
		if(BF_NODE_TYPE_PARENTDIR==poNode->iType) continue;
		if(!poNode->bSelect) continue;
		i_Bytes+=poNode->iSize;
		i_Count++;
	}	
}

uint64				
BF_NodeCollection::CountSelected() const
{
	uint64 	iCount = 0;
	BF_Node *poNode=NULL;
	for(int i=0;i<CountItems();i++){
		poNode = (BF_Node*)ItemAt(i);
		if(BF_NODE_TYPE_PARENTDIR==poNode->iType) continue;
		if(poNode->bSelect) iCount++;
	}
	return iCount;
}

void				
BF_NodeCollection::LoadChilds(int i_WhatRosterLoad)
{
	BF_Node *poNode;
	BString	s;
	for(int i=0;i<CountItems();i++){
		poNode = NodeAt(i);
		if(poNode->ploChild) poNode->ploChild->DeleteItems();
		if(poNode->bLink || poNode->iType!=BF_NODE_TYPE_DIR) continue;
		/* load childs */
		if(!poNode->ploChild) poNode->ploChild = new  BF_NodeList();		
		//
		ASSERT(poPath);
		/* make path for childs */
		s=poPath->Path();
		s<<"/";
		s<<poNode->sName;
		poNode->ploChild->MakePath(s.String());		
		/* load path */
		BF_Roster_LoadNodeList(*poNode->ploChild->poPath
			,*poNode->ploChild,i_WhatRosterLoad);
		//µprintf(">>>>>to_deep{%s},childs_count{%i}\n",s.String(),poNode->ploChild->CountItems());		
		//
		poNode->ploChild->LoadChilds(i_WhatRosterLoad);		
	}	
	
}

void				
BF_NodeCollection::MakeSelfNodes()
{	
	BF_Node *poNode;
	for(int i=0;i<CountItems();i++){
		poNode = NodeAt(i);
		RemoveItem(i);
		AddItem( new BF_Node(*poNode),i);
	}
	bDeleteItems = true;
}

uint32 	BF_NodeCollection::iSortStyle=BF_SORT_STYLE_DIRFIRST;


#define PREPARE_SORT() BF_Node	*po1 = *((BF_Node**)p_1),*po2 = *((BF_Node**)p_2); \
	ASSERT(po1  && po2); \
	if(po1->iType==BF_NODE_TYPE_PARENTDIR)	return(-1); \
	if(po2->iType==BF_NODE_TYPE_PARENTDIR)	return(+1);	\
	if((BF_NodeList::iSortStyle & BF_SORT_STYLE_DIRFIRST) &&  po2->iType==BF_NODE_TYPE_DIR && po1->iType!=BF_NODE_TYPE_DIR) \
	return(1)

int
BF_NodeCollection_Sort_Ext(const void * p_1, const void *p_2)
{
	PREPARE_SORT();		
		
	if(po2->iType!=BF_NODE_TYPE_DIR && po1->iType!=BF_NODE_TYPE_DIR){
		int i1,i2;
		i1 = po1->sName.FindLast('.');
		i2 = po2->sName.FindLast('.');
		if(!i1 && i2) return(-1);
		if(i1 && !i2) return(1);
		if(!i1 && !i2) return(0);						
		int iResult=po1->sName.ComparePos(po2->sName,i1+1,i2+1);
		if(iResult!=0) return iResult;
		//printf("{%s},{%s}\n",po1->sName.String(),po2->sName.String());
		return po1->sName.Compare(po2->sName);
	}
	return 0;	
}

int
BF_NodeCollection_Sort_Type(const void * p_1, const void *p_2)
{
	PREPARE_SORT();		

	if((BF_NodeList::iSortStyle & BF_SORT_STYLE_DIRFIRST) &&
		po2->iType==BF_NODE_TYPE_DIR && po1->iType!=BF_NODE_TYPE_DIR)
	{
		return(1);
	}
	if(po2->iType!=BF_NODE_TYPE_DIR && po1->iType!=BF_NODE_TYPE_DIR){
		return(po1->sType.Compare(po2->sType));
	}			
	return 0;
}

int
BF_NodeCollection_Sort_Size(const void * p_1, const void *p_2)
{
	PREPARE_SORT();		
	
	if((BF_NodeList::iSortStyle & BF_SORT_STYLE_DIRFIRST) &&
		po2->iType==BF_NODE_TYPE_DIR && po1->iType!=BF_NODE_TYPE_DIR)
	{
		return(1);
	}
	if(po2->iType!=BF_NODE_TYPE_DIR && po1->iType!=BF_NODE_TYPE_DIR){
		return(po1->iSize>po2->iSize);
	}
	return 0;
}

int
BF_NodeCollection_Sort_Name(const void * p_1, const void *p_2)
{
	PREPARE_SORT();		
	if((BF_NodeList::iSortStyle & BF_SORT_STYLE_DIRFIRST) &&
		po2->iType==BF_NODE_TYPE_DIR && po1->iType!=BF_NODE_TYPE_DIR)
	{
		return(1);
	}
	if(po2->iType!=BF_NODE_TYPE_DIR && po1->iType!=BF_NODE_TYPE_DIR){
		return(po1->sName.ICompare(po2->sName));
	}
	if(po2->iType==BF_NODE_TYPE_DIR && po1->iType==BF_NODE_TYPE_DIR){
		return(po1->sName.ICompare(po2->sName));
	}			
	return 0;
}

int
BF_NodeCollection_Sort_Date(const void * p_1, const void *p_2)
{
	PREPARE_SORT();		
	
	if((BF_NodeList::iSortStyle & BF_SORT_STYLE_DIRFIRST) &&
		po2->iType==BF_NODE_TYPE_DIR && po1->iType!=BF_NODE_TYPE_DIR)
	{
		return(1);
	}
	if(po2->iType==po1->iType){
		return(po1->uCreateTime>po2->uCreateTime);
	}			
	return 0;
}

void				
BF_NodeCollection::Sort(BF_NodeList_SortType i_SortType)
{
	switch(i_SortType){
	case BF_NODELIST_SORT_EXT:{
		SortItems(BF_NodeCollection_Sort_Ext);
		break;}
	case BF_NODELIST_SORT_TYPE:
		SortItems(BF_NodeCollection_Sort_Type);
		break;
	case BF_NODELIST_SORT_SIZE:
		SortItems(BF_NodeCollection_Sort_Size);
		break;
	case BF_NODELIST_SORT_NAME:
		SortItems(BF_NodeCollection_Sort_Name);
		break;
	case BF_NODELIST_SORT_DATE:
		SortItems(BF_NodeCollection_Sort_Date);
		break;
	default:
		break;
	}
}

/*=====================================================*/	
BF_NodeList::BF_NodeList(bool b_DeleteItems)
:BF_NodeCollection(b_DeleteItems)
{
	
}

/*=====================================================*/




















