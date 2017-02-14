// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
//Ϊ��ʹ��CString 
#include <afx.h>

// Windows ͷ�ļ�:
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include <shellapi.h>
#include <vector>
#include <process.h>
#include <shlobj.h>
#include <shlwapi.h>

#include <afxdlgs.h>	//ʹ��CFileDialog
using namespace std;


//duilib ��
#include "..\\..\\..\\duilib\\DuiLib\\UIlib.h"
#pragma comment(lib, "..\\..\\..\\Lib\\DuiLib_d.lib")
using namespace DuiLib;

//BCL sdk
// #import "progid:EasyConverter.PDF2Word.5"
// #import "progid:EasyConverter.PDF2Word.5"

#include <atlbase.h>
extern CComModule _Module;
#include <atlcom.h>
#include <comdef.h>
#include <regex>	//Ϊ��ʹ��������ʽ

#include "Reg.h"
#include "MD5_MBCS.h"

//Json�ַ������� jsoncpp�� d:\xwork\jsoncpp\include\json\json.h
#include "..\\..\\..\\jsoncpp\\include\\json\\json.h"

#ifdef _DEBUG
#pragma comment(lib, "..\\..\\..\\Lib\\json_d.lib")
#else
#pragma comment(lib, "..\\..\\..\\Lib\\json.lib")
#endif

////////////////////////////////
extern HWND g_hMainWnd;		//�����ھ��
extern HINSTANCE g_hInstance;

extern CString g_strPath;
extern CString g_strMainIni;

#include "MainWnd.h"
extern CMyWnd *g_MainWnd;


typedef struct tag_PdfFile_info 
{
	CString strFilePath;	//ȫ·��

	CString strFileName;	//�ļ���

	CString strFileSize;	//�ļ���С (Mb)

	int nFilePages; //�ļ�ҳ��

	BOOL bIsDel;	//�Ƿ��Ѿ��ӱ�����Ƴ���

	int nTag;		// ��ת���Ǵ�ת 0��ʾ��ת, 1��ʾ��ת

	CString strPageRange;	// ��ʽ: 1-100

	CString strPassWord;	//�ļ�����

	CString strWordFilePath;	//doc�ļ�ȫ·��

} Pdf_Info;
extern vector<Pdf_Info> g_vecPdfInfo;

extern CString g_strSavePath;	// doc�ļ�����·��

extern CString g_strPageRange;	//����ҳ����Χ

//ת���������Խṹ
typedef struct tag_Set_Connect 
{
	int nConversionMethod;	//��������

	BOOL bAdjustFontNames;	//У����������

	BOOL bAdjustSpacing;	//У�����

	BOOL bExtractImages;	//����ͼƬ

	BOOL bExtractText;		//��������

	BOOL bShrinkFonts;		//��С����

	BOOL bDetectHeader;		//���ͷ��

	BOOL bSaveHiddenText;	//���������ı�

	int nDocumentType;		//�ĵ����ͣ� ���� ���� ����Ӧ

	int nPutoutExt;			//�����ʽ: RTF�ļ� DOC�ļ� DOCX�ļ�

	int nBkmkOpt;			//�����ǩ����ת����ǩ ÿҳ��ͷ������ǩ ��PDFһ��

	int nOcrOpt;			//OCR�ĵ�������ͼƬ������ ���������� ������ͼƬ

	int nTimeout;			//�ȴ�ת���ʱ�� ����

	BOOL bOpenPath;			//ת���ɹ����Զ���Word���ڵ�Ŀ¼

} Set_Connect;

extern Set_Connect g_tagConnectSetting;	//Ĭ����������

//�˹�ת�����������б�
typedef struct tag_order_Info
{
	CString strOrderID;			//�������

	CString strFileName;		//�ļ���

	CString strPageCount;		//�ļ�ҳ��

	CString strAmt;				//���

	CString strOrderState;		//����״̬	//��֧��|δ֧��

	CString strJinDu;			//�������	//������

	CString strDownLoadUrl;		//ת�����ļ����ص�ַ

	BOOL bIsDownLoadOK;			//�Ѿ�����Word�ļ���

	CString strWordFile;		//����������word�ļ�Ⱥ·��

} OrderInfo;

extern vector<OrderInfo> g_vecManMakeInfo;	//�˹�ת�������б�

//��ʼ��Ĭ��ת������
extern void InitConnectSetting();

//����ת������
extern void SetConnectSetting(int nConversionMethod, BOOL bAdjustFontNames,
					   BOOL bAdjustSpacing, BOOL bExtractImages, BOOL bExtractText, BOOL bShrinkFonts,
					   BOOL bDetectHeader, BOOL bSaveHiddenText, int nDocumentType, int nPutoutExt,
					   int nBkmkOpt, int nOcrOpt, int nTimeout, int bOpenPath);


//��Ա��Ϣ
typedef struct tag_Vip_Info
{
	CString strType;	//0�ǻ�Ա 1��� 2����

	CString strPayDate;	//֧��ʱ��

	CString strEndDate;	//����ʱ��

	CString strNowDate;	//��ǰʱ��

} VIP_Info;
extern VIP_Info g_tagVipInfo;	//��Ա��Ϣ

//sdk
// extern EasyConverterLib::IPDF2WordPtr g_pConvObj;

//ת��һ���ļ�����
extern void BeginConvert(int nIndex);

//��ʼ��SDK
extern BOOL InitConvertSDK();

//��ȡPDF�ļ�ҳ�� ����-1��ʾ������
extern int GetPdfFilePages(CString strFilePath);

//��ȡ�ļ���С (Mb)
extern CString M_GetFileSize(CString strFilePath);

extern CProgressUI *g_lpProcess;	//��ǰת��������

extern int g_nNowRow;	//��ǰ����ת������

extern CIPdf2WordEvent g_IPdf2WordEvent;	// ת���¼�����

extern BOOL g_bOnlyFilePages;	//ֻת5ҳ

extern BOOL g_bIsSameAsPdf;	//word�ļ���pdf�ļ���������ͬĿ¼

extern BOOL g_bIsVip;

extern int g_nVIPType;	// 0:�ǻ�Ա 1:��� 2:����

extern EasyConverterLib::IPDF2WordPtr g_pConvObj;

// #include <Wininet.h>
// #pragma comment(lib,"Wininet.lib")
// 
// /*  
// * ����·�� ��������ַ ������·�� ���ݷָ���Ϣ �˿�  
// * ͨ�����ϴ�����Ϣ �����������ݴ���web������  
// *  
// */    
// extern BOOL Upload(IN CString& FilePath,IN CString& ServerName,IN CString& ObjectName, IN CString& HTTPBoundary,IN INTERNET_PORT &nPort);

// libcurl	��Ҫ Ws2_32.lib Wldap32.lib
// #include "..\\..\\..\\curl-7.26.0\\include\\curl\\curl.h"
// #pragma comment(lib, "..\\..\\..\\Lib\\libcurl_imp.lib")


// ����Url���� UTF-8 
extern CString UrlEncode(CString strUnicode);

//��ȡ��ҳ��Ϣ
extern CString GetHtmlStr(CString listurl);

extern char* UTF_8ToGB2312(char *pText, int pLen);

extern CStringA CStrW2CStrA(const CStringW &cstrSrcW);

extern CStringW CStrA2CStrW(const CStringA &cstrSrcA);

//�ж�����û��Ƿ���VIP strThisMachineCode ��ʾ���ر���Ļ�����
extern BOOL ThisUserIsVIP(CString strThisMachineCode);

extern CString g_strMachineCode; //������

//��ȡjson�ַ���ֵ nType = 0|�ַ��� 1|int
extern CString GetStrFormJson(CString strJson, CString strKey1, CString strKey2, int nType);

//������ѯ�ͷ�
extern void SayHelloOnLineByQQ();

//֧������
extern void PayForVip(int nPayType);

extern CString GetBCLKey();

extern void ShowMsgTipWnd(HWND hwnd, CString strMsg, int nTag);

extern string UnEscape(char* p_unicode_escape_chars);
