#include "Directory.h"

#include "BF_Dict.h"
#include "BF_Roster.h"

#include "BF_GUI_FtpPanel.h"
#include "BF_GUI_KeysMenu.h"
#include "BF_GUI_WinMain.h"
#include "BF_GUI_FtpPanel_Tasks.h"


BF_GUI_FtpPanel_UploadTask::BF_GUI_FtpPanel_UploadTask(
	BL_String & s_PathSrc,
	BF_GUI_FtpPanel *po_Panel,
	BF_NodeCollection & lo_Node,
	bool b_FilesMove
):BF_GUI_OperTask("upload to ftp",BF_GUI_OPERTASK_STYLE_DEFAULT)
{			
	iOperIndex = 0;
	iOperCount = 0;
	bFilesMove = b_FilesMove;

	ASSERT(po_Panel);
	poPanel = po_Panel;
	poFtp = po_Panel->poClient;;
	poBookmark  =  po_Panel->poBookmark;
	ASSERT(poFtp && poBookmark);	
	
	oPathSrc.SetTo(s_PathSrc.String());
	lo_Node.CopyTo(loNode,false);
	loNode.SetSelfPath(&oPathSrc);
}

void								
BF_GUI_FtpPanel_UploadTask::Put_File(BF_Node *po_Node)
{
	ASSERT(po_Node);

	bool rc = false;
	string cmd, replystr;
	int code, codetype, i;
	ssize_t rlen, slen;
	
	BL_String s;
	
	s=oPathSrc.Path();
	s<<"/";
	s<<po_Node->Name();
	BFile oFileSrc(s.String(), B_READ_ONLY);		

	if(oFileSrc.InitCheck() != B_NO_ERROR){		
		// !!!!!!!!!!!!!!
		return;
	}

	char buf[8192], sbuf[16384], *stmp;
		
	if(!poBookmark->bAscii_Mode)
		cmd = "TYPE I";
	else
		cmd = "TYPE A";

	if(poFtp->p_sendRequest(cmd))
		poFtp->p_getReply(replystr, code, codetype);

	try
	{
		cmd = "STOR ";
		cmd += po_Node->Name();
		if(	poFtp->p_openDataConnection() && 
			poFtp->p_sendRequest(cmd) 	&& 
			poFtp->p_getReply(replystr, code, codetype) &&
			codetype <= 2 && 
			poFtp->p_acceptDataConnection()) {
			rlen = 1;
			while(rlen > 0)
			{
				memset(buf, 0, sizeof(buf));
				memset(sbuf, 0, sizeof(sbuf));
				rlen = oFileSrc.Read((void *) buf, sizeof(buf));
				slen = rlen;
				stmp = buf;
				if(poBookmark->bAscii_Mode)
				{
					stmp = sbuf;
					slen = 0;
					for(i=0;i<rlen;i++)
					{
						if(buf[i] == '\n')
						{
							*stmp = '\r';
							stmp++;
							slen++;
						}
						*stmp = buf[i];
						stmp++;
						slen++;
					}
					stmp = sbuf;
				}
				if(slen > 0)
				{
					size_t amount = poFtp->m_data->Send(stmp, (size_t) slen);
					if(amount < 0)	throw "bail";
					if(amount > 0){
						iOperIndex += amount;
						SetProgress(iOperIndex,iOperCount);
					}		
				}
			}

			rc = true;
		}
	}

	catch(const char *errstr)
	{

	}

	DELETE(poFtp->m_data);
	poFtp->m_data = 0;

	if(rc == true)
	{
		poFtp->p_getReply(replystr, code, codetype);
		rc = (bool) codetype <= 2;
	}
	
	///	
	
	return;
}

void								
BF_GUI_FtpPanel_UploadTask::Put_List(BF_NodeCollection *plo_Node)
{
	ASSERT(plo_Node);
	
	BF_Node *poNode;
	for(int i=0;i<plo_Node->CountItems();i++){
		poNode = plo_Node->NodeAt(i);
		Put_File(poNode);		
	}
}

void								
BF_GUI_FtpPanel_UploadTask::Run()
{
	iOperIndex  = 0;
	iOperCount = loNode.SizeAll(true);	
	SetProgress(iOperIndex,iOperCount);
	Put_List(&loNode);	
	
	// reload panel_nodes 
	BMessenger 	oMessenger(poPanel);
	BMessage	oMessage(BF_FTP_MSG_DIR_RELOAD_START);
	oMessenger.SendMessage(&oMessage);
}

////////////////////////////////////////////////////////////////////////////////////////////////

BF_GUI_FtpPanel_DownloadTask::BF_GUI_FtpPanel_DownloadTask(
	BL_String & s_PathDest,
	BF_GUI_FtpPanel *po_Panel,
	BF_NodeCollection & lo_Node,
	bool b_FilesMove
):BF_GUI_OperTask(BF_DictAt(BF_DICT_FTP_TASKDOWNLOAD),BF_GUI_OPERTASK_STYLE_DEFAULT)
{			
	iOperIndex = 0;
	iOperCount = 0;
	bFilesMove = b_FilesMove;

	ASSERT(po_Panel);
	poPanel = po_Panel;
	poFtp = po_Panel->poClient;;
	poBookmark  =  po_Panel->poBookmark;
	ASSERT(poFtp && poBookmark);	
	
	oPathDest.SetTo(s_PathDest.String());
	lo_Node.CopyTo(loNode,false);
}

void								
BF_GUI_FtpPanel_DownloadTask::Get_File(BF_Node *po_Node)
{
	ASSERT(po_Node);
	
	bool 	rc = false;
	string 	cmd, replystr;
	int 	code, codetype, i;
	ssize_t rlen, slen;
	
	BL_String sFileDest(oPathDest.Path());
	sFileDest << "/";
	sFileDest << po_Node->Name();
		
	BFile oDestFile(sFileDest.String(), B_READ_WRITE | B_CREATE_FILE);
	
	char buf[8192], sbuf[16384], *stmp;
	bool writeerr = false;
	
	if(oDestFile.InitCheck() != B_NO_ERROR){
		// !!!!!!!!!!!!!!
		return ;
	}

	if(!poBookmark->bAscii_Mode)
		cmd = "TYPE I";
	else
		cmd = "TYPE A";

	if(poFtp->p_sendRequest(cmd))	poFtp->p_getReply(replystr, code, codetype);


	cmd = "RETR ";
	cmd += po_Node->Name();
	if(	poFtp->p_openDataConnection() &&
		poFtp->p_sendRequest(cmd) && 
		poFtp->p_getReply(replystr, code, codetype)	&& 
		codetype <= 2 && 
		poFtp->p_acceptDataConnection())
	{
		rlen = 1;
		rc = true;
		while(rlen > 0)
		{
			memset(buf, 0, sizeof(buf));
			memset(sbuf, 0, sizeof(sbuf));
			rlen = poFtp->m_data->Receive(buf, sizeof(buf));

			if(rlen > 0)
			{

				slen = rlen;
				stmp = buf;
				if(poBookmark->bAscii_Mode)
				{
					stmp = sbuf;
					slen = 0;
					for(i=0;i<rlen;i++)
					{
						if(buf[i] == '\r')
						{
							i++;
						}
						*stmp = buf[i];
						stmp++;
						slen++;
					}
					stmp = sbuf;
				}

				if(slen > 0)
				{
					slen = oDestFile.Write(stmp, (size_t) slen);
					if(slen<0){
						writeerr = true;
					}else{
						iOperIndex += slen;
						SetProgress(iOperIndex,iOperCount);
					}
				}
			}
		}
	}

	DELETE(poFtp->m_data);
	poFtp->m_data = 0;

	if(rc == true)
	{
		poFtp->p_getReply(replystr, code, codetype);
		rc = (bool) ((codetype <= 2) && (writeerr == false));
	}
	

}

void								
BF_GUI_FtpPanel_DownloadTask::Get_List(BF_NodeCollection *plo_Node)
{
	ASSERT(plo_Node);
	
	BF_Node *poNode;
	for(int i=0;i<plo_Node->CountItems();i++){
		poNode = plo_Node->NodeAt(i);
		Get_File(poNode);		
	}
}

void								
BF_GUI_FtpPanel_DownloadTask::Run()
{
	iOperIndex  = 0;
	iOperCount = loNode.SizeAll(true);	
	SetProgress(iOperIndex,iOperCount);
	Get_List(&loNode);	
	
	// reload panel_nodes 
	BMessenger 	oMessenger(poPanel);
	BMessage	oMessage(BF_FTP_MSG_DIR_RELOAD_START);
	oMessenger.SendMessage(&oMessage);
}
////////////////////////////////////////////////////////////////////////////////////////////////

BF_GUI_FtpPanel_DeleteTask::BF_GUI_FtpPanel_DeleteTask(
	BF_GUI_FtpPanel *po_Panel,
	BF_NodeCollection & lo_Node
):BF_GUI_OperTask(BF_DictAt(BF_DICT_FTP_TASKDEL),BF_GUI_OPERTASK_STYLE_DEFAULT)
{			
	iOperIndex = 0;
	iOperCount = 0;

	ASSERT(po_Panel);
	poPanel = po_Panel;
	poFtp = po_Panel->poClient;;
	poBookmark  =  po_Panel->poBookmark;
	ASSERT(poFtp && poBookmark);	
	
	lo_Node.CopyTo(loNode,false);
}

void								
BF_GUI_FtpPanel_DeleteTask::Delete_File(BF_Node *po_Node)
{
	ASSERT(po_Node);
	
	bool rc = false;
	string request = "DELE ";
	string  replystr;
	int code, codetype;

	request += po_Node->Name();
	//request += "\"";

	if(poFtp->p_sendRequest(request)&& poFtp->p_getReply(replystr, code, codetype)&& codetype == 2)	rc = true;
	
	return;
}

void								
BF_GUI_FtpPanel_DeleteTask::Delete_List(BF_NodeCollection *plo_Node)
{
	ASSERT(plo_Node);
	
	BF_Node *poNode;
	for(int i=0;i<plo_Node->CountItems();i++){
		poNode = plo_Node->NodeAt(i);
		switch(poNode->iType){
		case BF_NODE_TYPE_FILE:
			Delete_File(poNode);
			break;
		}
		SetProgress(++iOperIndex,iOperCount);		
	}
}

void								
BF_GUI_FtpPanel_DeleteTask::Run()
{
	iOperIndex  = 0;
	iOperCount = loNode.CountAll(true);	
	SetProgress(iOperIndex,iOperCount);
	Delete_List(&loNode);	
	
	// reload panel_nodes 
	BMessenger 	oMessenger(poPanel);
	BMessage	oMessage(BF_FTP_MSG_DIR_RELOAD_START);
	oMessenger.SendMessage(&oMessage);
}