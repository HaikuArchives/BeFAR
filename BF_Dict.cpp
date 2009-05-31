#include <stdio.h>
#include <stdlib.h>
#include "BF_Dict.h"
#include "BL_File.h"

//////////////////////////////////////////////

class BF_Dict:public BL_Object{
public:				
									BF_Dict();
			const char* At(uint32 i_Index);			
			void				Load(const char *pc_Name);
			const char* CurrentFile() const {return sFile.String();};
private:
			BL_List			lsData;					
			BL_String		sFile;				
};

BF_Dict *poDict=NULL;

//////////////////////////////////

BF_Dict::BF_Dict()
{
}

#include <Roster.h>
#include <Application.h>
#include <Path.h>

void 
BF_Dict::Load(const char *pc_Name)
{
	ASSERT(pc_Name);	
		
	sFile = pc_Name;		
		
	BL_String s;
	{
		app_info uInfo;
		be_app->GetAppInfo(&uInfo);
		BEntry oEntry(&uInfo.ref);

		BPath oPath;
		oEntry.GetPath(&oPath);
		BPath oPath1;
		oPath.GetParent(&oPath1);
		s<<oPath1.Path();
	}
	
	s<<"/";	
	s<<pc_Name;
	printf(s.String());
	printf("-------\n");
		
	BL_File oFile(s.String(),B_READ_ONLY);
	if(B_OK!=oFile.InitCheck()){
		BL_String sError("Can`t open ");
		sError<<s.String();
		Debug_Error(sError.String(),oFile.InitCheck());
		exit(-1);		
	}
	// clear data //
	lsData.DeleteItems();
	// init data //
	for(int i=0;i<BF_DICT_COUNT;i++) lsData.AddItem(new BL_String());
	// load data //
	s="";
	BL_String	s1;
	BL_String *ps;
	int32			i1,i2;
	uint32 		iCode;
	
	while(B_OK==oFile.ReadString(&s)){
		if(s.FindFirst("//")==0) continue;
		if(s.FindFirst("`")==0) continue;
		if(s=="") continue;

		i2=s.FindFirst("|");
		if(i2<=0) continue;
		//		
		s1= "";
		s1.Insert(s,0,i2,0);
		//
		iCode = s1.Int32();
		//
		i1=s.FindFirst("|",i2+1);
		if(i1<0) continue;
		//
		s1 = "";
		s1.Insert(s,i1+1,s.Length()-i1,0);
		//
		ps = lsData.StringAt(iCode);
		if(!ps) continue;
		ps->SetTo(s1);
	}
}

const char* 
BF_Dict::At(uint32 i_Index)
{
	const BL_String *ps = lsData.StringAt(i_Index);
	if(!ps){
		BL_String s("Can`t find dictionary frase #");
		s<<i_Index;
		ASSERT(FALSE,s.String());
	}
	return ps->String();
}
/////////////////////////////////////////

void BF_Dict_Load(const char *pc_DictName)
{
	if(poDict) delete poDict;
	poDict = new BF_Dict();
	poDict->Load(pc_DictName);		
}

const char* BF_DictAt(uint32 i_Index)
{
	ASSERT(poDict);
	return poDict->At(i_Index);
}
/////////////////////////////////////////

const char* BF_DictCurrentFile()
{
	ASSERT(poDict);
	return poDict->CurrentFile();
}

