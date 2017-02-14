// pdf2word.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "pdf2word.h"
#include "MainWnd.h"

#include "MyDiskInfo.h" 
#include "MsgWnd.h"
#include "ping.h"
#include <fcntl.h>
#include <sys/stat.h>


#import "progid:EasyConverter.PDF2Word.5"

HWND g_hMainWnd = NULL;
HINSTANCE g_hInstance;

CMyWnd *g_MainWnd = NULL;

CString g_strMachineCode; //������
BOOL g_bIsVip = FALSE;

int g_nVIPType = 0;	// 0:�ǻ�Ա 1:��� 2:����

CString g_strPath;
CString g_strMainIni;

EasyConverterLib::IPDF2WordPtr g_pConvObj = NULL;

//��ȡBCL SDK key V5 key
CString g_BCLkey1 = _T("4645");
CString g_BCLkey2 = _T("F1FD");
CString g_BCLkey3 = _T("B3F9");
CString g_BCLkey4 = _T("DD7D");
CString g_BCLkey5 = _T("0CAD");
CString g_BCLkey6 = _T("F34D");


//V4 key 464D-F1FD-B3F9-DD7D-3F42-BD2A
// CString g_BCLkey1 = _T("464D");
// CString g_BCLkey2 = _T("F1FD");
// CString g_BCLkey3 = _T("B3F9");
// CString g_BCLkey4 = _T("DD7D");
// CString g_BCLkey5 = _T("3F42");
// CString g_BCLkey6 = _T("BD2A");

CString GetBCLKey()
{
	CString strKey;
	strKey = g_BCLkey1 + _T("-") + g_BCLkey2 + _T("-") + g_BCLkey3 + _T("-") + g_BCLkey4 + _T("-") + g_BCLkey5 + _T("-") + g_BCLkey6;

	return strKey;
}

//��ȡ�µ�GUID
CString GetNewGUID()
{
	CString strGUID;
	TCHAR szGUID[64] = _T("");

	::CoInitialize(NULL);

	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		_sntprintf(szGUID, sizeof(szGUID)
			, _T("{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}")
			, guid.Data1
			, guid.Data2
			, guid.Data3
			, guid.Data4[0], guid.Data4[1]
		, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
		, guid.Data4[6], guid.Data4[7]
		);
	}
	::CoUninitialize();

	strGUID = szGUID;
	return strGUID;
}

//��ȡ��̨������󶨵�GUID
CString GetThisComputerGUID()
{
	CString strGUID;

	//�ȴ�ע����ȡ���Ƿ��Ѿ�����GUID��
	CReg reg;
	DWORD dwDisposition;

	if (reg.Create(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\CGPdf2Word"), &dwDisposition))
	{
		if(reg.Open(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\CGPdf2Word"), KEY_READ| KEY_WRITE))
		{
			if (reg.GetValueSZ(_T("CGPDFGUID"), strGUID))
			{
				if (strGUID.IsEmpty())
				{
					strGUID = GetNewGUID();
					reg.SetValueSZ(_T("CGPDFGUID"), strGUID.GetBuffer());
				}
			}
			else
			{
				strGUID = GetNewGUID();

				//д��ע���
				reg.SetValueSZ(_T("CGPDFGUID"), strGUID);
			}
		}
	}
	
	return strGUID;
}

//��ȡCPUID
// CString cpu_id()
// {
// 	int s1,s2;
// 	CString CPUID_1,CPUID_2; 
// 	__asm{   
// 		mov eax,01h   
// 			xor edx,edx   
// 			cpuid   
// 			mov s1,edx   
// 			mov s2,eax   
// 	}   
// 	CPUID_1.Format(_T("%08X%08X"),s1,s2);   
// 	__asm{   
// 		mov eax,03h   
// 			xor ecx,ecx   
// 			xor edx,edx   
// 			cpuid   
// 			mov s1,edx   
// 			mov s2,ecx   
// 	}   
// 	CPUID_2.Format(_T("%08X%08X"),s1,s2);  
// 
// 	return CPUID_1+CPUID_2;
// }

CString cpu_id()
{
	CString CPUID;
	unsigned long s1,s2;
	unsigned char vendor_id[]="------------";
	char sel;
	sel='1';
	CString VernderID;
	CString MyCpuID,CPUID1,CPUID2;
	switch(sel)
	{
	case '1':
		__asm{
			xor eax,eax      //eax=0:ȡVendor��Ϣ
				cpuid    //ȡcpu idָ�����Ring3��ʹ��
				mov dword ptr vendor_id,ebx
				mov dword ptr vendor_id[+4],edx
				mov dword ptr vendor_id[+8],ecx
		}
		VernderID.Format(_T("%s-"), vendor_id);
		__asm{
			mov eax,01h   //eax=1:ȡCPU���к�
				xor edx,edx
				cpuid
				mov s1,edx
				mov s2,eax
		}
		CPUID1.Format(_T("%08X%08X"),s1,s2);
		__asm{
			mov eax,03h
				xor ecx,ecx
				xor edx,edx
				cpuid
				mov s1,edx
				mov s2,ecx
		}
		CPUID2.Format(_T("%08X%08X"),s1,s2);
		break;
	case '2':
		{
			__asm{
				mov ecx,119h
					rdmsr
					or eax,00200000h
					wrmsr
			}
		}
		break;
	}
	MyCpuID = CPUID1+CPUID2;
	CPUID = MyCpuID;
	return CPUID;
}

//��CPUID
CString GetComputerCpuID()
{
	CString strCPUID;

	//�ȴ�ע����ȡ���Ƿ��Ѿ�����GUID��
	CReg reg;
	DWORD dwDisposition;

	if (reg.Create(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\CGPdf2Word"), &dwDisposition))
	{
		if(reg.Open(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\CGPdf2Word"), KEY_READ| KEY_WRITE))
		{
			if (reg.GetValueSZ(_T("CGPDFCPU"), strCPUID))
			{
				if (strCPUID.IsEmpty())
				{
					strCPUID = cpu_id();
					reg.SetValueSZ(_T("CGPDFCPU"), strCPUID.GetBuffer());
				}
			}
			else
			{
				strCPUID = cpu_id();

				//д��ע���
				reg.SetValueSZ(_T("CGPDFCPU"), strCPUID);
			}
		}
	}

	return strCPUID;
}

// CString disk_id()	����ֻ�ǻ�ȡ��
// {
// 	CString   id; 
// 	TCHAR   Name[MAX_PATH]; 
// 	DWORD   serno; 
// 	DWORD   length; 
// 	DWORD   FileFlag; 
// 	TCHAR   FileName[MAX_PATH]; 
// 	BOOL   Ret; 
// 	Ret   =   GetVolumeInformation(_T("c:\\"),   Name,   MAX_PATH,   &serno,   &length,   &FileFlag,   FileName,   MAX_PATH); 
// 	if (Ret) 
// 		id.Format(_T("%x"), serno); 
// 	return id;
// }

//��ȡӲ��ID
CString GetNewDiskID()
{
	CString strDiskID;

	MyDiskInfo DiskInfo;
	if (0 == DiskInfo.GetDiskInfo())
	{
		strDiskID = DiskInfo.GetDiskID();
	}

	return strDiskID;
}

//��ȡӲ�̵�ID
CString GetDiskID()
{
	CString strDiskID;

	//�ȴ�ע����ȡ���Ƿ��Ѿ�����GUID��
	CReg reg;
	DWORD dwDisposition;

	if (reg.Create(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\CGPdf2Word"), &dwDisposition))
	{
		if(reg.Open(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\CGPdf2Word"), KEY_READ| KEY_WRITE))
		{
			if (reg.GetValueSZ(_T("CGPDFDISK"), strDiskID))
			{
				if (strDiskID.IsEmpty())
				{
					strDiskID = GetNewDiskID();
					reg.SetValueSZ(_T("CGPDFDISK"), strDiskID.GetBuffer());
				}
			}
			else
			{
				strDiskID = GetNewDiskID();

				//д��ע���
				reg.SetValueSZ(_T("CGPDFDISK"), strDiskID);
			}
		}
	}

	return strDiskID;
}

//��ȡ������
CString GetMachineCode(CString strGUID, CString strCPUID, CString strDiskID)
{
	CString strMachineCode;

	CString strAll = strGUID + strCPUID + strDiskID + _T("quxiupdf2word");

	//��ȡ����ֶε�MD5��Ϊ�û����Ļ�����
	strMachineCode = MakeCStringMD5W(strAll);

	return strMachineCode;
}

//��ȡ������
CString GetMachineCode_L(CString strGUID, CString strCPUID, CString strDiskID)
{
	CString strMachineCode;

	//�ȴ�ע����ȡ���Ƿ��Ѿ�����GUID��
	CReg reg;
	DWORD dwDisposition;

	if (reg.Create(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\CGPdf2Word"), &dwDisposition))
	{
		if(reg.Open(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\CGPdf2Word"), KEY_READ| KEY_WRITE))
		{
			if (reg.GetValueSZ(_T("MACHINECODE"), strMachineCode))
			{
				if (strMachineCode.IsEmpty())
				{
					strMachineCode = GetMachineCode(strGUID, strCPUID, strDiskID);
					reg.SetValueSZ(_T("MACHINECODE"), strMachineCode.GetBuffer());
				}
			}
			else
			{
				strMachineCode = GetMachineCode(strGUID, strCPUID, strDiskID);

				//д��ע���
				reg.SetValueSZ(_T("MACHINECODE"), strMachineCode);
			}
		}
	}

	return strMachineCode;
}

//���´�Ӳ����ȡ������
CString GetMachineCodeInfo(CString &strGUID, CString &strCPUID, CString &strDiskID)
{
	CString strMachineCode;

	CString strGUID_n = GetThisComputerGUID();
	CString strCPUID_n = cpu_id();
	CString strDiskID_n = GetNewDiskID();

	strMachineCode = GetMachineCode(strGUID_n, strCPUID_n, strDiskID_n);
// 	if (strGUID != NULL)
	{
		strGUID = strGUID_n;
	}
// 	if (strCPUID != NULL)
	{
		strCPUID = strCPUID_n;
	}
// 	if (strDiskID != NULL)
	{
		strDiskID = strDiskID_n;
	}

	return strMachineCode;
}

LPWSTR ANSIConvertUNCOIDE(char* aText,int sourceCodePage)
{

	int len=MultiByteToWideChar(sourceCodePage,NULL,aText,-1,NULL,0);
	wchar_t *psText=new wchar_t[len+1];
	memset(psText,0,len);
	MultiByteToWideChar(sourceCodePage,NULL,aText,-1,psText,len+1);
	//psText[len]='\0';
	return psText;
}

CStringA CStrW2CStrA(const CStringW &cstrSrcW)
{
	int len = WideCharToMultiByte(CP_ACP, 0, LPCWSTR(cstrSrcW), -1, NULL, 0, NULL, NULL);
	char *str = new char[len];
	memset(str, 0, len);
	WideCharToMultiByte(CP_ACP, 0, LPCWSTR(cstrSrcW), -1, str, len, NULL, NULL);
	CStringA cstrDestA = str;
	delete[] str;

	return cstrDestA;
}

CStringW CStrA2CStrW(const CStringA &cstrSrcA)
{
	int len = MultiByteToWideChar(CP_ACP, 0, LPCSTR(cstrSrcA), -1, NULL, 0);
	wchar_t *wstr = new wchar_t[len];
	memset(wstr, 0, len*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, LPCSTR(cstrSrcA), -1, wstr, len);
	CStringW cstrDestW = wstr;
	delete[] wstr;

	return cstrDestW;
}

void UTF_8ToUnicode(wchar_t* pOut,char *pText)
{
	char* uchar = (char *)pOut;

	uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
	uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

	return;
}

void UnicodeToGB2312(char* pOut,wchar_t uData)
{
	WideCharToMultiByte(CP_ACP,NULL,&uData,1,pOut,sizeof(wchar_t),NULL,NULL);
	return;
}     

char* UTF_8ToGB2312(char *pText, int pLen)
{
	char * newBuf = new char[pLen+10];
	char Ctemp[4];
	memset(Ctemp,0,4);
	int i =0;
	int j = 0;
	while(i < pLen)
	{
		if(pText[i] > 0)
			newBuf[j++] = pText[i++];                       
		else                 
		{
			WCHAR Wtemp;
			UTF_8ToUnicode(&Wtemp,pText + i);
			UnicodeToGB2312(Ctemp,Wtemp);
			newBuf[j] = Ctemp[0];
			newBuf[j + 1] = Ctemp[1];
			i += 3;    
			j += 2;   
		}
	}
	newBuf[j] = '\0';
	return newBuf; 
}

// #include <afxinet.h>	//��ȡ��ҳ����
// 
//��ȡ��ҳ��Ϣ
// CString GetHtmlStr(CString listurl)
// {
// 	try 
// 	{
// 		CInternetSession mySession(NULL,0);   //��ʼץȡ��ҳ��֤��½
// 		CHttpFile* myHttpFile=NULL;
// 
// 		char strBuff[1024] = {0}; 
// 		CStringA strHtmlA;
// 
// 		myHttpFile=(CHttpFile*)mySession.OpenURL(listurl.GetBuffer(0),1,INTERNET_FLAG_TRANSFER_ASCII|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD);
// 
// 		int nReadLength=0; 
// 		while(nReadLength = myHttpFile->Read((void*)strBuff, 1023))
// 		{
// 			strBuff[nReadLength]=('\0');
// 			strHtmlA += strBuff;
// 		}
// 
// 		myHttpFile->Close();
// 		mySession.Close();
// 
// 		CStringA strHtml_G = UTF_8ToGB2312(strHtmlA.GetBuffer(0), strHtmlA.GetLength());
// 		CString strHtml = CStrA2CStrW(strHtml_G);
// 		return strHtml;
// 	}
// 	catch(CInternetException *e)
// 		//catch(CException *e) 
// 	{
// 		TCHAR err[1024];  
// 		e->GetErrorMessage(err,1024); 
// 		return "";
// 	}
// }

#include "HttpTransByGet.h"
CString GetHtmlStr(CString listurl)
{
	CHttpTransByGet httpGet;
	VecStParam vecParam;
	char szBuf[1024] = "";
	DWORD dwInfoLen = 1024;
	httpGet.TransDataToServer(listurl.GetBuffer(0), 3000, vecParam, szBuf, dwInfoLen);

	char *lp = UTF_8ToGB2312(szBuf, dwInfoLen);
	CStringA strHtml_G = lp;
	delete [] lp;
	CString strHtml = CStrA2CStrW(strHtml_G);

	return strHtml;
}

//��ȡjson�ַ���ֵ nType = 0|�ַ��� 1|int
CString GetStrFormJson(CString strJson, CString strKey1, CString strKey2, int nType)
{
	CString strJsonW;
	CStringA strJsonA = CStrW2CStrA(strJson);
	CStringA strKey1A = CStrW2CStrA(strKey1);
	CStringA strKey2A = CStrW2CStrA(strKey2);
	CStringA strTestA;

	const char* str = strJsonA.GetBuffer(0);  

	Json::Reader reader;  
	Json::Value root;  
	if (reader.parse(str, root))  // reader��Json�ַ���������root��root������Json��������Ԫ��  
	{  
		if (strKey2A.IsEmpty())
		{
			if (nType == 0)
			{
				std::string strVaule = root[strKey1A].asString();
				strTestA = strVaule.c_str();
				strJsonW = CStrA2CStrW(strTestA);
			}
			else
			{
				int nVaule = root[strKey1A].asInt();
				strJsonW.Format(_T("%d"), nVaule);
			}
		}
		else
		{
			if (nType == 0)
			{
				std::string strVaule = root[strKey1A][strKey2A].asString();
				strTestA = strVaule.c_str();
				strJsonW = CStrA2CStrW(strTestA);
			}
			else
			{
				int nVaule = root[strKey1A][strKey2A].asInt();
				strJsonW.Format(_T("%d"), nVaule);
			}
		}
	}  

	return strJsonW;
}

//�ж�����û��Ƿ���VIP strThisMachineCode ��ʾ���ر���Ļ�����
BOOL ThisUserIsVIP(CString strThisMachineCode)
{
	BOOL bRet = FALSE;
	CString strGUID;
	CString strCPUID;
	CString strDiskID;

	//���ж���û������
// 	CPing ping;
// 	bool bResult = FALSE;
// 	int nTimes = 0;
// 
// 	while (1)
// 	{
// 		WORD wVersionRequested;
// 		WSADATA wsaData;
// 		int err;
// 
// 		wVersionRequested = MAKEWORD(1, 1);
// 		err = WSAStartup( wVersionRequested, &wsaData );
// 
// 		bResult = ping.Ping("www.baidu.com");
// 
// 		if (bResult)
// 		{
// 			break;
// 		}
// 
// 		if (0 == nTimes)
// 		{
// 			//��ʾû������
// 			ShowMsgTipWnd(g_hMainWnd, _T("�޷���������"), -1);
// 		}
// 		nTimes ++;
// 		Sleep(30* 1000);		
// 	}

	//�ٴλ�ȡ������
	CString strMachineCode = GetMachineCodeInfo(strGUID, strCPUID, strDiskID);

	if (strThisMachineCode != strMachineCode)
	{
		//��һ����ʾ����Ӳ���б䶯	xx?
		g_strMachineCode = strMachineCode;

		//����ע���
	}

	SYSTEMTIME st;
	GetSystemTime(&st);
	CTime tUTC(st);
	CString strTime;
	strTime.Format(_T("%d"), tUTC.GetTime());

	CString strSign;
	strSign.Format(_T("%s|%s|quxiupdf2word"), g_strMachineCode, strTime);
	CString strSignMd5 = MakeCStringMD5W(strSign);

	//637175DFFE85D71E4154A890F74402FF
// 	g_strMachineCode = _T("F74CF275786A49CDE0E79A62C0FF04F2");
	CString strUrl = _T("http://www.pdf00.com/key/checkkey?mn=[mn]&time=[time]&sign=[sign]");
	strUrl.Replace(_T("[mn]"), g_strMachineCode);
	strUrl.Replace(_T("[time]"), strTime);
	strUrl.Replace(_T("[sign]"), strSignMd5);

	CString strUrlInfo = GetHtmlStr(strUrl);
	if (strUrlInfo.IsEmpty() || (-1 == strUrlInfo.Find(_T("code"))))
	{
		ShowMsgTipWnd(g_hMainWnd, _T("�޷����ʷ�����������������ϵ�ͷ�"), -1);
		g_bIsVip = FALSE;

		bRet = FALSE;
		g_tagVipInfo.strType = _T("0");
		g_tagVipInfo.strPayDate = _T("");
		g_tagVipInfo.strEndDate = _T("");
		g_tagVipInfo.strNowDate = _T("");
		WritePrivateProfileString(_T("Info"), _T("checkinfoNULL"), strUrlInfo.GetBuffer(0), g_strMainIni.GetBuffer(0));	//��¼��ȡ����Ϣ
		return bRet;
	}

	//�����ַ���
	CString strCode = GetStrFormJson(strUrlInfo, _T("code"), _T(""), 1);

	if (_T("500") == strCode)	//δ��ͨ����Ա
	{
		g_bIsVip = FALSE;
		g_nVIPType = 0;
		bRet = FALSE;
	}
	else	//��ͨ����Ա
	{
		//����Ա����
		CString strCipType = GetStrFormJson(strUrlInfo, _T("data"), _T("viptype"), 1);
		if (_T("2") == strCipType)	//�����û�
		{
			g_bIsVip = TRUE;
			g_nVIPType = 2;
			g_tagVipInfo.strType = _T("2");
			g_tagVipInfo.strPayDate = GetStrFormJson(strUrlInfo, _T("pay_date"), _T(""), 0);
			g_tagVipInfo.strEndDate = GetStrFormJson(strUrlInfo, _T("end_date"), _T(""), 0);
			g_tagVipInfo.strNowDate = GetStrFormJson(strUrlInfo, _T("now_date"), _T(""), 0);
			bRet = TRUE;
		}
		else	//����û�
		{
			g_nVIPType = 1;
			//����ʱ��
			CString strEndDate = GetStrFormJson(strUrlInfo, _T("data"), _T("end_date"), 0);
			CString strNowDate = GetStrFormJson(strUrlInfo, _T("data"), _T("now_date"), 0);
			if (strNowDate > strEndDate)
			{
				g_bIsVip = FALSE;
				g_tagVipInfo.strType = _T("1");
				g_tagVipInfo.strPayDate = GetStrFormJson(strUrlInfo, _T("pay_date"), _T(""), 0);
				g_tagVipInfo.strEndDate = strEndDate;
				g_tagVipInfo.strNowDate = strNowDate;
				bRet = FALSE;
			}
			else
			{
				g_bIsVip = TRUE;
				g_tagVipInfo.strType = _T("1");
				g_tagVipInfo.strPayDate = GetStrFormJson(strUrlInfo, _T("pay_date"), _T(""), 0);
				g_tagVipInfo.strEndDate = strEndDate;
				g_tagVipInfo.strNowDate = strNowDate;
				bRet = TRUE;
			}
		}
	}

	WritePrivateProfileString(_T("Info"), _T("UserMcode"), g_strMachineCode.GetBuffer(0), g_strMainIni.GetBuffer(0));	//��¼������
	WritePrivateProfileString(_T("Info"), _T("checkurl"), strUrl.GetBuffer(0), g_strMainIni.GetBuffer(0));	//��¼����
	WritePrivateProfileString(_T("Info"), _T("checkinfo"), strUrlInfo.GetBuffer(0), g_strMainIni.GetBuffer(0));	//��¼��ȡ����Ϣ
	WritePrivateProfileString(_T("Info"), _T("strCode"), strCode.GetBuffer(0), g_strMainIni.GetBuffer(0));	//��¼��ȡ����Ϣ

	return bRet;
}

//��������Ȩ��
// BOOL UpgradeProcessPrivilege(HANDLE hProcess, LPCTSTR lpPrivilegeName ) 
// { 
// 	HANDLE hToken = NULL; 
// 	if(OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken)) 
// 	{ 
// 		LUID Luid; 
// 		if(LookupPrivilegeValue(NULL, lpPrivilegeName, &Luid)) 
// 		{ 
// 			TOKEN_PRIVILEGES tp; 
// 			tp.PrivilegeCount = 1; 
// 			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
// 			tp.Privileges[0].Luid = Luid; 
// 			return( AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL) ); 
// 		} 
// 	} 
// 	return FALSE; 
// } 

//��ȡ��ǰ����·��
CString Get_ProcessPath(void)
{
	CString path;
	GetModuleFileName(NULL, path.GetBufferSetLength(MAX_PATH+1), MAX_PATH);
	path.ReleaseBuffer();
	int pos = path.ReverseFind('\\');
	path = path.Left(pos + 1);

	return path;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CWinApp app(lpCmdLine);		//win32 ��ʹ��MFC����
	app.InitApplication();
	AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);

// 	UpgradeProcessPrivilege(::GetModuleHandle(NULL), SE_DEBUG_NAME);

	// �ҵ��ĵ�
	TCHAR szPath[MAX_PATH] = _T("");
	SHGetSpecialFolderPath(NULL, szPath, CSIDL_PERSONAL, FALSE);
	g_strSavePath.Format(_T("%s\\CGPdf2Word\\"), szPath);

	if(PathIsDirectory(g_strSavePath.GetBuffer(0)) != FILE_ATTRIBUTE_DIRECTORY)
		CreateDirectory(g_strSavePath.GetBuffer(0),NULL);

	g_hInstance = hInstance;

	g_strPath = Get_ProcessPath();
	g_strMainIni = g_strPath + _T("cguoif.db");

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;


	// ��ȡGUID ��ΪΨһ��ʶ��
	CString strGUID = GetThisComputerGUID();	// {08FA0B6A-B62F-44f2-8DE7-8AAA848B39A1}

	CString strCPU = GetComputerCpuID();	// BFEBFBFF000506E30000000000000000

	CString strDiskID = GetDiskID();	// S2R7NX0HB02022WSamsungSSD850EVO250GB

	//��ȡ������
	g_strMachineCode = GetMachineCode_L(strGUID, strCPU, strDiskID);	// 0DB78455E814AD983418355A0390D22F

	//����Ƿ�ͨ��Ա
// 	ThisUserIsVIP(g_strMachineCode);

	HINSTANCE hInst = NULL;
	hInst = GetModuleHandle(NULL);

	if (NULL == hInst)
	{
		return FALSE;
	}

	//������Դ
	HRSRC hRes = FindResource(hInst, MAKEINTRESOURCE(IDR_DATA2), _T("DATA"));
	if (NULL == hRes)
	{
		return FALSE;
	}

	//��ȡ��Դ��С
	DWORD len = SizeofResource(hInst, hRes);

	//������Դ
	HGLOBAL hg = LoadResource(hInst, hRes);

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourceZip((void*)hg, len);
// 	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin\\pdf2word"));
	// 	CPaintManagerUI::SetResourceZip(CPaintManagerUI::GetInstancePath() + _T("skin.zip"));

	CMyWnd *pFrame = new CMyWnd();
	pFrame->Create(NULL, _T(""), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE| WS_EX_ACCEPTFILES);

	g_hMainWnd = pFrame->GetHWND();
	g_MainWnd = pFrame;

	pFrame->CenterWindow();
	pFrame->ShowModal();

	delete pFrame;

	//�ͷż��ص��ڴ���Դ
	FreeResource(hg);

	::CoUninitialize();

	return 0;
}

//��ʼ��SDK
BOOL InitConvertSDK()
{
	try
	{
		if (g_pConvObj.CreateInstance("EasyConverter.PDF2Word.5") == S_OK)
		{
			g_pConvObj->LicenseKey = GetBCLKey().GetBuffer(0);	//��ʽ��OEM license key 20161221 14����
			g_pConvObj->PutJpegQuality(75);
			if (g_tagConnectSetting.nTimeout == 0)
			{
				g_tagConnectSetting.nTimeout = 5;
			}
			g_pConvObj->PutFileConversionTimeout(g_tagConnectSetting.nTimeout * 60 * 1000);
			g_pConvObj->PutWordpadImages(VARIANT_FALSE);
			g_pConvObj->PutConnectHyphens(VARIANT_FALSE);

			//�����ʽ
			if (g_tagConnectSetting.nPutoutExt == 0)
			{
				g_pConvObj->PutOutputDocumentFormat(EasyConverterLib::OPT_OUTPUT_RTF);
			}
			else if (g_tagConnectSetting.nPutoutExt == 1)
			{
				g_pConvObj->PutOutputDocumentFormat(EasyConverterLib::OPT_OUTPUT_DOC);
			}
			else
			{
				g_pConvObj->PutOutputDocumentFormat(EasyConverterLib::OPT_OUTPUT_DOCX);
			}

			//ȥ�������ı�
			g_pConvObj->PutRemoveHiddenText(g_tagConnectSetting.bSaveHiddenText ? VARIANT_FALSE:VARIANT_TRUE);

			//OCR�ĵ�������ͼƬ������ ���������� ������ͼƬ
			long nOpt = 0;
			switch (g_tagConnectSetting.nOcrOpt) 
			{
				case 0:
					nOpt = EasyConverterLib::OPT_OCR_TEXT_IMAGE;
					break;
				case 1:
					nOpt = EasyConverterLib::OPT_OCR_TEXTONLY;
					break;
				case 2:
					nOpt = EasyConverterLib::OPT_OCR_IMAGEONLY;
					break;
			}
			g_pConvObj->PutOcrOption(nOpt);

			//�����ǩ����ת����ǩ ÿҳ��ͷ������ǩ ��PDFһ��
			nOpt = 0;
			switch (g_tagConnectSetting.nBkmkOpt) 
			{
				case 0:
					nOpt = EasyConverterLib::OPT_BKMK_NONE;
					break;
				case 1:
					nOpt = EasyConverterLib::OPT_BKMK_PAGE_START;
					break;
				case 2:
					nOpt = EasyConverterLib::OPT_BKMK_PDF_EXTRACT;
					break;
			}
			g_pConvObj->PutConvertBookmarks(nOpt);

			//��������
			nOpt = 0;
			switch (g_tagConnectSetting.nConversionMethod) 
			{
				case 1:
					nOpt = EasyConverterLib::CNV_METHOD_USE_SPACES_AND_TABS;
					break;
				case 2:
					nOpt = EasyConverterLib::CNV_METHOD_USE_TEXTBOXES;
					break;
				case 3:
					nOpt = EasyConverterLib::CNV_METHOD_TEXT_FLOW;
					break;
				default:
					nOpt = EasyConverterLib::CNV_METHOD_DEFAULT;
			}
			g_pConvObj->PutConversionMethod(nOpt);

			//�ĵ����ͣ� ���� ���� ����Ӧ
			nOpt = 0;
			switch (g_tagConnectSetting.nDocumentType) 
			{
				case 0:
					nOpt = EasyConverterLib::DOCTYPE_SINGLE_COLUMN;
					break;
				case 1:
					nOpt = EasyConverterLib::DOCTYPE_MULTI_COLUMN;
					break;
				case 2:
					nOpt = EasyConverterLib::DOCTYPE_PLEADING;
					break;
				default:
					nOpt = EasyConverterLib::DOCTYPE_SINGLE_COLUMN;
			}
			g_pConvObj->PutDocumentType(nOpt);

			//�ϲ�����
			g_pConvObj->PutMergeParagraphs(VARIANT_TRUE);
			//�ϲ��ı�
			g_pConvObj->PutMergeText(VARIANT_TRUE);
			//�������� ͼƬ
			g_pConvObj->PutExtractText(g_tagConnectSetting.bExtractText?VARIANT_TRUE:VARIANT_FALSE);
			g_pConvObj->PutExtractImages(g_tagConnectSetting.bExtractImages?VARIANT_TRUE:VARIANT_FALSE);

			g_pConvObj->PutExtractHyperlinks(VARIANT_FALSE);	//������
			g_pConvObj->PutDetectHeader(g_tagConnectSetting.bDetectHeader?VARIANT_TRUE:VARIANT_FALSE); //���ͷ��
			g_pConvObj->PutAdjustSpacing(g_tagConnectSetting.bAdjustSpacing?VARIANT_TRUE:VARIANT_FALSE); //У�����
			g_pConvObj->PutAdjustFontNames(g_tagConnectSetting.bAdjustFontNames?VARIANT_TRUE:VARIANT_FALSE); //У����������
			g_pConvObj->PutShrinkFonts(g_tagConnectSetting.bShrinkFonts?VARIANT_TRUE:VARIANT_FALSE); //��С����

			g_IPdf2WordEvent.SetPDF2Word(g_pConvObj);
		}
	}
	catch (_com_error &err)
	{
		CString strError = (char *)err.Description();
		return FALSE;
	}
// 	::CoUninitialize();

	return TRUE;
}

//ת��һ���ļ�����
void BeginConvert(int nIndex)
{
	int nPages; // = g_pConvObj->GetNumberOfPages(g_vecPdfInfo[nIndex].strFilePath.GetBuffer(0), g_vecPdfInfo[nIndex].strPassWord.GetBuffer(0));

	//��ȡ���·��
	CString strOutputPath = g_strSavePath;

	//��ȡ�ļ���
	CString strPdfName = g_vecPdfInfo[nIndex].strFilePath.Mid(g_vecPdfInfo[nIndex].strFilePath.ReverseFind(_T('\\')) + 1);

	CString strWordName = strPdfName;

	CString strExt;
	if (g_tagConnectSetting.nPutoutExt == 0)
	{
		strExt = _T(".rtf");
	}
	else if (g_tagConnectSetting.nPutoutExt == 1)
	{
		strExt = _T(".doc");
	}
	else
	{
		strExt = _T(".docx");
	}

	if (g_bIsSameAsPdf)
	{
		strOutputPath = g_vecPdfInfo[nIndex].strFilePath;
		strOutputPath += strExt;
	}
	else
	{
		strOutputPath += strWordName;
		strOutputPath += strExt;
	}

	g_vecPdfInfo[nIndex].strWordFilePath = strOutputPath;

	//��ȡת��ҳ����Χ
	if (g_vecPdfInfo[nIndex].strPageRange.IsEmpty())	//ȫ��ת��
	{
		if (!g_bIsVip)
		{
			nPages = g_vecPdfInfo[nIndex].nFilePages > 5 ? 5 : g_vecPdfInfo[nIndex].nFilePages;
		}
		else
		{
			nPages = g_vecPdfInfo[nIndex].nFilePages;
		}
		g_pConvObj->ConvertToWord(g_vecPdfInfo[nIndex].strFilePath.GetBuffer(0), strOutputPath.GetBuffer(0), g_vecPdfInfo[nIndex].strPassWord.GetBuffer(0), 1, nPages);
	}
	else
	{
		if (!g_bIsVip)
		{
			nPages = g_vecPdfInfo[nIndex].nFilePages > 5 ? 5 : g_vecPdfInfo[nIndex].nFilePages;
			g_pConvObj->ConvertToWord(g_vecPdfInfo[nIndex].strFilePath.GetBuffer(0), strOutputPath.GetBuffer(0), g_vecPdfInfo[nIndex].strPassWord.GetBuffer(0), 1, nPages);
		}
		else
		{
			//�ֽ�ҳ��
			int nPos = g_vecPdfInfo[nIndex].strPageRange.Find(_T("-"));
			CString strStart = g_vecPdfInfo[nIndex].strPageRange.Mid(0, nPos);
			CString strEnd = g_vecPdfInfo[nIndex].strPageRange.Mid(nPos + 1);

			int nStart = _ttoi(strStart.GetBuffer(0));
			int nEnd = _ttoi(strEnd.GetBuffer(0));

			g_pConvObj->ConvertToWord(g_vecPdfInfo[nIndex].strFilePath.GetBuffer(0), strOutputPath.GetBuffer(0), g_vecPdfInfo[nIndex].strPassWord.GetBuffer(0), nStart, nEnd);
		}
	}
}



//////////////////////////////////////////////////////
// �ϴ��ļ�
//////////////////////////////////////////////////////
// CString MakeRequestHeaders(CString &strBoundary)    
// {    
// 	CString strData;    
// 
// 	strData += _T("Content-Type: multipart/form-data; boundary=");    //�������ļ�����Content-Type����Ϊ: multipart/form-data    
// 	strData += strBoundary;    
// 	strData +="/r/n";    
// 	return strData;    
// }    
// CString MakePreFileData(CString &strBoundary, CString &strFileName, int iRecordID)    
// {    
// 	//////////////////////////////////////////////////////////////////////////////////    
// 	CString strData;    
// 
// 	strData += _T("--");    
// 	strData += strBoundary;    
// 	strData += _T("\r\n");    
// 	strData += _T("Content-Disposition: form-data; name=\"file1\"; filename=\"");  //�ر�1:Path    
// 		strData += strFileName;                                             
// 	strData += _T("\"");    
// 		strData += _T("\r\n");    
// 	strData += _T("Content-Type: Text/pdf");                                     //�ر�2:Type    
// 
// 	strData += _T("\r\n\r\n");    
// 
// 	return strData;    
// }    
// CString MakePostFileData(CString &strBoundary)    
// {    
// 	CString strData;    
// 
// 	strData = _T("\r\n");       
// 	strData += _T("--");    
// 	strData += strBoundary;    
// 	strData += _T("\r\n");    
// 	strData += _T("Content-Disposition: form-data; name=\"submitted\"");    
// 	strData += _T("\r\n\r\n");    
// 	strData += _T("hello");    
// 	strData += _T("\r\n");    
// 	strData += _T("--");    
// 	strData += strBoundary;    
// 	strData += _T("--");    
// 	strData += _T("\r\n");    
// 	return strData;    
// }    
// //�ļ��������ļ�����     
// typedef struct _FileInfo    
// {    
// 	DWORD  nFileLen;    
// 	DWORD  nFileHighLen;                
// 	LPSTR FileBuf;    
// }FileInfo,*pFileInfo;    
// BOOL GetFileInfo(OUT FileInfo &BinaryInfo, IN CString FilePath)    
// {    
// 	BOOL   nRet = FALSE;    
// 	HANDLE hFile;     
// 	DWORD nBytesRead;    
// 	hFile = CreateFile(FilePath.GetBuffer(0),              
// 		GENERIC_ALL,              // open for reading     
// 		FILE_SHARE_READ,           // share for reading     
// 		NULL,                      // no security     
// 		OPEN_EXISTING,             // existing file only     
// 		FILE_ATTRIBUTE_NORMAL,     // normal file     
// 		NULL);                     // no attr. template %    
// 
// 	if (hFile == INVALID_HANDLE_VALUE)     
// 	{     
// 		return nRet;    
// 	}     
// 
// 	BinaryInfo.nFileLen = GetFileSize(hFile,&BinaryInfo.nFileHighLen);    
// 	BinaryInfo.FileBuf = new char[BinaryInfo.nFileLen];    
// 	if (!BinaryInfo.FileBuf)    
// 	{    
// 		CloseHandle(hFile);    
// 		return nRet;    
// 	}    
// 	ZeroMemory(BinaryInfo.FileBuf,BinaryInfo.nFileLen);    
// 	if (!ReadFile(hFile, BinaryInfo.FileBuf, BinaryInfo.nFileLen, &nBytesRead, NULL))    
// 	{    
// 		CloseHandle(hFile);    
// 		return nRet;    
// 	}    
// 
// 	CloseHandle(hFile);    
// 	nRet = TRUE;    
// 	return nRet;    
// }    
// /*  
// * ����·�� ��������ַ ������·�� ���ݷָ���Ϣ �˿�  
// * ͨ�����ϴ�����Ϣ �����������ݴ���web������  
// *  
// */    
// BOOL Upload(IN CString& FilePath,IN CString& ServerName,IN CString& ObjectName, IN CString& HTTPBoundary,IN INTERNET_PORT &nPort)    
// {    
// 	BOOL      nRet = FALSE;    
// 	HINTERNET hInternet;                //by   InternetOpen    
// 	HINTERNET hHttpSession;             //by   InternetConnect    
// 	HINTERNET hHttpRequest;             //by   HttpOpenRequest    
// 	int       iRecordID = 1;    
// 	DWORD     dwTotalLen;               //���ݰ����ܳ���    
// 	//׼������    
// 	int    startp   = FilePath.Find('\\');    
// 	int    namelen  = FilePath.GetLength()-startp-1;    
// 	CString FileName = FilePath;    
// 
// 	CString strHeaders   = MakeRequestHeaders(HTTPBoundary);    
// 	CString PreFileData  = MakePreFileData(HTTPBoundary, FileName, iRecordID);    
// 	CString PostFileData = MakePostFileData(HTTPBoundary);    
// 	//////////////////////////////////////////////////////////////////////////    
// 	//1:getFileInfo    
// 	FileInfo localJpg;    
// 	if (!GetFileInfo(localJpg,FilePath))    
// 	{    
// 		return FALSE;    
// 	}    
// 	dwTotalLen = localJpg.nFileLen + PreFileData.GetLength() + PostFileData.GetLength();    
// 	//2:init www    
// 	hInternet = InternetOpen(_T("upfile"),    
// 		INTERNET_OPEN_TYPE_DIRECT,     
// 		NULL,    
// 		NULL,    
// 		0);    
// 
// 	if (!hInternet)    
// 	{    
// 		return nRet;    
// 	}    
// 
// 	hHttpSession = InternetConnect( hInternet,    
// 		ServerName.GetBuffer(0),    
// 		INTERNET_DEFAULT_HTTP_PORT,    
// 		NULL,    
// 		NULL,    
// 		INTERNET_SERVICE_HTTP,    
// 		INTERNET_FLAG_RELOAD,0);         
// 
// 	if (hHttpSession == NULL)    
// 	{    
// 
// 		InternetCloseHandle(hInternet);    
// 		return nRet;    
// 	}    
// 	//3:Opening a Request    
// 	hHttpRequest = HttpOpenRequest(hHttpSession, _T("POST"),     
// 		ObjectName.GetBuffer(0),    
// 		HTTP_VERSIONW,     
// 		NULL,NULL, INTERNET_FLAG_MAKE_PERSISTENT,1);     
// 	if (hInternet == NULL)    
// 	{    
// 		InternetCloseHandle(hHttpSession);    
// 		InternetCloseHandle(hInternet);    
// 		return nRet;    
// 	}    
// 	//4:HttpAddRequestHeaders    
// 	if (!HttpAddRequestHeaders(hHttpRequest,strHeaders.GetBuffer(0),strHeaders.GetLength(),HTTP_ADDREQ_FLAG_ADD))    
// 	{       
// 		goto END;    
// 	}    
// 
// 	//5:HttpSendRequestEx    
// 	INTERNET_BUFFERSW buffer;    
// 	memset(&buffer, 0, sizeof(buffer));    
// 	buffer.dwStructSize  = sizeof(buffer);    
// 	buffer.dwBufferTotal = dwTotalLen;    
// 	if (!HttpSendRequestEx(hHttpRequest,&buffer,NULL,HSR_SYNC | HSR_INITIATE,1))    
// 	{    
// 		goto END;    
// 	}    
// 
// 	//6:_A_send httpͷ    
// 	DWORD dwNumberOfBytesWritten;       
// 	if(!InternetWriteFile(hHttpRequest,PreFileData.GetBuffer(),PreFileData.GetLength(),&dwNumberOfBytesWritten))    
// 	{    
// 		goto END;    
// 	}    
// 	if (dwNumberOfBytesWritten != PreFileData.GetLength())    
// 	{    
// 		goto END;    
// 	}    
// 
// 	//6:_B_send filedata    
// 	if(!InternetWriteFile(hHttpRequest,localJpg.FileBuf,localJpg.nFileLen,&dwNumberOfBytesWritten))    
// 	{    
// 		goto END;    
// 	}
// 	else
// 	{
// 		//��ȡ������Ϣ
// 		TCHAR szBuf[1024] =_T("");
// 		DWORD readed;
// 
// 		BOOL bRet = InternetReadFile(hHttpRequest, szBuf, sizeof(TCHAR) * 1024, &readed);
// 
// 		int a;
// 		a = 3;
// 	}
// 
// 	if (dwNumberOfBytesWritten != localJpg.nFileLen)    
// 	{    
// 		goto END;    
// 	}    
// 	//6:_C_send Httpβ    
// 	if(!InternetWriteFile(hHttpRequest,PostFileData.GetBuffer(0),PostFileData.GetLength(),&dwNumberOfBytesWritten))    
// 	{    
// 		goto END;    
// 	}    
// 
// 	if (dwNumberOfBytesWritten != PostFileData.GetLength())    
// 	{    
// 		goto END;    
// 	}    
// 	//7:����ύ �ز�����    
// 	HttpEndRequest(hHttpRequest,NULL,HSR_SYNC,1);    
// END:    
// 	InternetCloseHandle(hHttpRequest);    
// 	InternetCloseHandle(hHttpSession);    
// 	InternetCloseHandle(hInternet);    
// 	return nRet;    
// }    

// int main(int argc, char* argv[])    
// {    
// 	CString          FilePath     = _T("C://test//16.jpg");;                             //�����ļ�·��    
// 	CString          ServerName   = _T("www.baidu.com");                                 //��������ַ    
// 	CString          ObjectName   = _T("//test//upload//upload.asp");                    //�������ļ�����    
// 	CString          HTTPBoundary = _T("---------------------------7db29f2140360");      //�߽�ֵ:Ҫ���ϸ�    
// 	INTERNET_PORT   nPort        = 80;                                                  //�˿�    
// 	Upload(FilePath,ServerName,ObjectName,HTTPBoundary,nPort);    
// 	return 0;    
// }    


// int get_file_size(TCHAR *filename)
// {
// 	FILE* fp = NULL;
// 	int nFileLen = 0;
// 
// 	fp = _tfopen(filename, _T("rb"));
// 
// 	if (fp == NULL)
// 	{
// 		return 0;
// 	}
// 
// 	fseek(fp,0,SEEK_END); //��λ���ļ�ĩ 
// 	nFileLen = ftell(fp); //�ļ�����
// 	return nFileLen;
// }
// 
// int http_post_file(const TCHAR *url, const TCHAR *filename)
// {
// 	CURL *curl = NULL;
// 	CURLcode res;
// 
// 	struct curl_httppost *post=NULL;
// 	struct curl_httppost *last=NULL;
// 	struct curl_slist *headerlist=NULL;
// 
// 	if(filename == NULL || url == NULL)
// 		return -1;
// 
// 	/* Add simple file section */
// 	if( curl_formadd(&post, &last, CURLFORM_COPYNAME, _T("file"),
// 		CURLFORM_FILE, filename, CURLFORM_END) != 0)
// 	{
// 		return -1;
// 	}
// 
// 	/* Fill in the submit field too, even if this is rarely needed */
// 	curl_formadd(&post, &last,
// 		CURLFORM_COPYNAME, _T("upfile"),
// 		CURLFORM_COPYCONTENTS, _T("OK"),
// 		CURLFORM_END);
// 
// 	//curl_global_init(CURL_GLOBAL_ALL);
// 	curl = curl_easy_init();
// 	if(curl == NULL)
// 	{
// 		curl_formfree(post);
// 		return -1;
// 	}
// 
// 	curl_easy_setopt(curl, CURLOPT_HEADER, 0);
// 	curl_easy_setopt(curl, CURLOPT_URL, url); /*Set URL*/
// 	curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
// 	int timeout = 5;
// 	curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
// 	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
// 	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1);
// 
// 	res = curl_easy_perform(curl);
// 	if(res != CURLE_OK)
// 	{
// 		curl_formfree(post);
// 		return -1;
// 	}
// 
// 	curl_easy_cleanup(curl);    
// 
// 	return 0;
// }


enum
{
	UNICODE_CALC_SIZE = 1,
	UNICODE_GET_BYTES = 2
};

//��unicodeת���ַ�����ת��Ϊ�ڴ��е�unicode�ַ���
int unicode_bytes(char* p_unicode_escape_chars,wchar_t *bytes,int flag)
{
	/*
	char* p_unicode_escape_chars="pp\\u4fddp\\u5b58\\u6210pp\\u529f0a12";

	//ͨ���˺�����֪ת������Ҫ���ֽ���
	int n_length=unicode_bytes(p_unicode_escape_chars,NULL,UNICODE_CALC_SIZE);

	//�ٴε��ô˺�����ȡ���ֽ�����
	wchar_t *bytes=new wchar_t[n_length+sizeof(wchar_t)];
	unicode_bytes(p_unicode_escape_chars,bytes,UNICODE_GET_BYTES);
	bytes[n_length]=0;

	//��ʱ��bytes����ת������ֽ�����
	delete[] bytes;
	*/

	int unicode_count=0;
	int length=strlen(p_unicode_escape_chars);
	for (int char_index=0;char_index<length;char_index++)
	{
		char unicode_hex[5];
		memset(unicode_hex,0,5);

		char ascii[2];
		memset(ascii,0,2);

		if (*(p_unicode_escape_chars+char_index)=='\\')
		{
			char_index++;
			if (char_index<length)
			{
				if (*(p_unicode_escape_chars+char_index)=='u')
				{
					if (flag==UNICODE_GET_BYTES)
					{
						memcpy(unicode_hex,p_unicode_escape_chars+char_index+1,4);

						//sscanf������ʹ��unsigned short����
						//����Run-Time Check Failure #2 - Stack around the variable 'a' was corrupted.
						unsigned int a=0;
						sscanf_s(unicode_hex,"%04x",&a);
						bytes[unicode_count++]=a;
					}
					else if(flag==UNICODE_CALC_SIZE)
					{
						unicode_count++;
					}
					char_index+=4;
				}
			}
		}
		else
		{
			if (flag==UNICODE_GET_BYTES)
			{
				memcpy(ascii,p_unicode_escape_chars+char_index,1);
				unsigned int a=0;
				sscanf_s(ascii,"%c",&a);
				bytes[unicode_count++]=a;
			}
			else if(flag==UNICODE_CALC_SIZE)
			{
				unicode_count++;
			}
		}
	}

	return unicode_count;
}

string UnEscape(char* p_unicode_escape_chars)
{
	int nBytes=unicode_bytes(p_unicode_escape_chars,NULL,UNICODE_CALC_SIZE);

	wchar_t *p_bytes=new wchar_t[nBytes+sizeof(wchar_t)];
	unicode_bytes(p_unicode_escape_chars,p_bytes,UNICODE_GET_BYTES);
	p_bytes[nBytes]=0;

	USES_CONVERSION;
	string cs_return=W2A((wchar_t*)p_bytes);

	delete[] p_bytes;

	return cs_return;
}
