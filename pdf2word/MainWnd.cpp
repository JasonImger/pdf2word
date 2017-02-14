#include "stdafx.h"
#include "MainWnd.h"
#include "MenuWnd.h"
#include "MsgWnd.h"
#include "SetWnd.h"
#include "ManMakeWnd.h"
#include "VIPWnd.h"
#include "PageRangeWnd.h"
#include "InputPW.h"
#include "HttpTransByGet.h"
#include <UrlMon.h>


vector<Pdf_Info> g_vecPdfInfo;	//pdf�ļ�list
int g_nVecCursor = 0; //ÿ���������ļ�ˢ��list�� ��¼ g_vecPdfInfo λ��

vector<OrderInfo> g_vecManMakeInfo;	//�˹�ת�������б�

CString g_strSavePath;	// doc�ļ�����·��

Set_Connect g_tagConnectSetting;	//Ĭ����������

int g_nNowRow = 0;	//��ǰ����ת������

CIPdf2WordEvent g_IPdf2WordEvent;	// ת���¼�����

BOOL g_bIsSameAsPdf = FALSE;	//word�ļ���pdf�ļ���������ͬĿ¼

VIP_Info g_tagVipInfo;	//��Ա��Ϣ

//��ʼ��Ĭ��ת������
void InitConnectSetting()
{
	g_tagConnectSetting.nConversionMethod = 0; //��������

	g_tagConnectSetting.bAdjustFontNames = TRUE;	//У����������

	g_tagConnectSetting.bAdjustSpacing = TRUE;		//У�����

	g_tagConnectSetting.bExtractImages = TRUE;		//����ͼƬ

	g_tagConnectSetting.bExtractText = TRUE;		//��������

	g_tagConnectSetting.bShrinkFonts = TRUE;		//��С����

	g_tagConnectSetting.bDetectHeader = TRUE;		//���ͷ��

	g_tagConnectSetting.bSaveHiddenText = TRUE;	//���������ı�

	g_tagConnectSetting.nDocumentType = 0;		//�ĵ����ͣ� ���� ���� ����Ӧ

	g_tagConnectSetting.nPutoutExt = 1;			//�����ʽ: RTF�ļ� DOC�ļ� DOCX�ļ�

	g_tagConnectSetting.nBkmkOpt = 1;			//�����ǩ����ת����ǩ ÿҳ��ͷ������ǩ ��PDFһ��

	g_tagConnectSetting.nOcrOpt = 0;			//OCR�ĵ�������ͼƬ������ ���������� ������ͼƬ

	g_tagConnectSetting.nTimeout = 0;			//�ȴ�ת���ʱ�� ����

	g_tagConnectSetting.bOpenPath = FALSE;			//ת���ɹ����Զ���Word���ڵ�Ŀ¼
}

//����ת������
void SetConnectSetting(int nConversionMethod, BOOL bAdjustFontNames,
					   BOOL bAdjustSpacing, BOOL bExtractImages, BOOL bExtractText, BOOL bShrinkFonts,
					   BOOL bDetectHeader, BOOL bSaveHiddenText, int nDocumentType, int nPutoutExt,
					   int nBkmkOpt, int nOcrOpt, int nTimeout, int bOpenPath)
{
	g_tagConnectSetting.nConversionMethod = nConversionMethod; //��������

	g_tagConnectSetting.bAdjustFontNames = bAdjustFontNames;	//У����������

	g_tagConnectSetting.bAdjustSpacing = bAdjustSpacing;		//У�����

	g_tagConnectSetting.bExtractImages = bExtractImages;		//����ͼƬ

	g_tagConnectSetting.bExtractText = bExtractText;		//��������

	g_tagConnectSetting.bShrinkFonts = bShrinkFonts;		//��С����

	g_tagConnectSetting.bDetectHeader = bDetectHeader;		//���ͷ��

	g_tagConnectSetting.bSaveHiddenText = bSaveHiddenText;	//���������ı�

	g_tagConnectSetting.nDocumentType = nDocumentType;		//�ĵ����ͣ� ���� ���� ����Ӧ

	g_tagConnectSetting.nPutoutExt = nPutoutExt;			//�����ʽ: RTF�ļ� DOC�ļ� DOCX�ļ�

	g_tagConnectSetting.nBkmkOpt = nBkmkOpt;			//�����ǩ����ת����ǩ ÿҳ��ͷ������ǩ ��PDFһ��

	g_tagConnectSetting.nOcrOpt = nOcrOpt;			//OCR�ĵ�������ͼƬ������ ���������� ������ͼƬ

	g_tagConnectSetting.nTimeout = nTimeout;			//�ȴ�ת���ʱ�� ����

	g_tagConnectSetting.bOpenPath = bOpenPath;			//ת���ɹ����Զ���Word���ڵ�Ŀ¼
}

//ֻ��ʵ��OnProgress���������ʵ�֣�  
CBindCallback::CBindCallback()  
{  

}  

CBindCallback::~CBindCallback()  
{  

}  

//////��ʵ��OnProgress��Ա����  

LRESULT CBindCallback::OnProgress(ULONG ulProgress,  
								  ULONG ulProgressMax,  
								  ULONG ulSatusCode,  
								  LPCWSTR szStatusText)  
{  
	CProgressUI* m_prg = (CProgressUI*)m_lpRow->FindSubControl(_T("progress_down"));  
	m_prg->SetMinValue(0);
	m_prg->SetMaxValue(ulProgressMax);
	m_prg->SetValue(ulProgress);  

	return S_OK;  
}

//��ȡ�ļ���С (Mb)
CString M_GetFileSize(CString strFilePath)
{
	// ���ַ�����ȡ�ļ���С���ճ��ڴ�й©������ ԭ��δ֪ debug�� ��������û������ ֱ�����оͱ��� releaseû������ ����
	// 	struct _stat info;
	// 	_tstat(strFilePath.GetBuffer(0), &info);
	// 	_off_t filesize = info.st_size;

	WIN32_FIND_DATA fileInfo; 
	HANDLE hFind; 
	DWORD filesize; 
	hFind = FindFirstFile(strFilePath.GetBuffer(0) ,&fileInfo); 
	if(hFind != INVALID_HANDLE_VALUE) 
		filesize = fileInfo.nFileSizeLow; 

	float fSize = ((float)(filesize))/1024/1024;

	TCHAR szBuf[16] = _T("");
	_stprintf(szBuf, _T("%.2fMB"), fSize);

	CString strFileSize = szBuf;

	return strFileSize;
}

//��ȡg_nVecCursor ��Ԫ����Ч�ĸ���
int GetVecTrueNum()
{
	int nCount = 0;

	for (int i = 0; i < g_vecPdfInfo.size(); i++)
	{
		if (!g_vecPdfInfo[i].bIsDel)
		{
			nCount++;
		}
	}

	return nCount;
}

//����ת���õ�word�ļ�
unsigned __stdcall DownLoadWordFileFunc(LPVOID pM)
{
	CListContainerElementUI *lpRow = (CListContainerElementUI*) (((CButtonUI*)pM)->GetParent()->GetParent());
	int nIndex = lpRow->GetTag();

	CBindCallback cbc; 
	cbc.m_lpRow = lpRow;

	CButtonUI *lpBtnDownLoad = (CButtonUI*)pM;
	lpBtnDownLoad->SetVisible(FALSE);

	CString strOutputPath = g_strSavePath + g_vecManMakeInfo[nIndex].strFileName + _T(".doc");

	lpRow->FindSubControl(_T("progress_down"))->SetVisible(TRUE);	//��ʾ������

	if (S_OK == URLDownloadToFile(NULL, g_vecManMakeInfo[nIndex].strDownLoadUrl.GetBuffer(0), strOutputPath.GetBuffer(0), 0, &cbc))
	{
		lpRow->FindSubControl(_T("btnOpenFile_down"))->SetVisible(TRUE);	//��ʾ���ļ�
		lpRow->FindSubControl(_T("btnOpenFolder_down"))->SetVisible(TRUE);	//��ʾ��Ŀ¼
		g_vecManMakeInfo[nIndex].bIsDownLoadOK = TRUE;
		g_vecManMakeInfo[nIndex].strWordFile = strOutputPath;
	}
	else
	{
		lpBtnDownLoad->SetVisible(TRUE);
	}
	lpRow->FindSubControl(_T("progress_down"))->SetVisible(FALSE);	//���ؽ�����

	_endthreadex(0);
	return 0;
}

//����Ƿ���VIP�û�
unsigned __stdcall CheckVipUserFunc(LPVOID pM)
{
	CMyWnd *lpWnd = (CMyWnd*) pM;

	while (1)
	{
		BOOL bRet = ThisUserIsVIP(g_strMachineCode);

		if (bRet)
		{
			if (_T("2") == g_tagVipInfo.strType)
			{
				lpWnd->m_lpVipBtn->SetText(_T("����VIP"));
			}
			else
			{
				if (g_tagVipInfo.strNowDate > g_tagVipInfo.strEndDate)
				{
					lpWnd->m_lpVipBtn->SetText(_T("����VIP"));
				}
				else
				{
					lpWnd->m_lpVipBtn->SetText(_T("���VIP"));
				}
			}
			break;
		}
		Sleep(20 * 1000);
	}
	_endthreadex(0);
	return 0;
}

//���BCL�Ƿ���ȷ��װ
unsigned __stdcall CheckBCLIsOkFunc(LPVOID pM)
{
	EasyConverterLib::IPDF2WordPtr pConvObj = NULL;
	::CoInitialize(NULL);

	//û����ȷ��װ
	if (pConvObj.CreateInstance("EasyConverter.PDF2Word.5") != S_OK)
	{
		CString strBCL32 = g_strPath + _T("UninsFiles\\unins000.exe");
		ShellExecute(NULL, _T("open"), strBCL32.GetBuffer(0), _T("/repair"), NULL, SW_HIDE);
	}

	pConvObj = NULL; 
	::CoUninitialize();

	_endthreadex(0);
	return 0;
}

//���PDF�ļ��̺߳���
unsigned __stdcall WhenAddFilesFunc(LPVOID pM)
{
	CMyWnd *lpWnd = (CMyWnd*) pM;

	lpWnd->RefreshFileList();

	_endthreadex(0);
	return 0;
}

//��ʼת���߳�
unsigned __stdcall BeginConnectFunc(LPVOID pM)
{
	CMyWnd *lpWnd = (CMyWnd*)pM;
	CString strDocSavePath;

	lpWnd->m_lpbtnConvert->SetEnabled(FALSE);
	lpWnd->m_lpSelAllOpt->SetEnabled(FALSE);
// 	lpWnd->m_lpSelAllOpt1->SetEnabled(FALSE);
	lpWnd->m_lpbtnDelete->SetEnabled(FALSE);

	//�Ȼ�ȡ���Ŀ¼
	if (lpWnd->m_lpSavePathOpt->IsSelected())
	{
		g_bIsSameAsPdf = TRUE;
	}
	else
	{
		strDocSavePath = (lpWnd->m_lpSavePathEdt->GetText()).GetData();
		if (strDocSavePath.IsEmpty())
		{
			lpWnd->m_lpbtnConvert->SetEnabled(TRUE);
			lpWnd->m_lpSelAllOpt->SetEnabled(TRUE);
			lpWnd->m_lpbtnDelete->SetEnabled(TRUE);
			return 0;
		}
	}

	//�ж���û�п�ͨVIP
	UINT nRet = -1;

	//��ȡ�����
	int nRows = lpWnd->m_lpList->GetCount();

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

	BOOL bInitSdkRet = InitConvertSDK();

	for (int i = 0; i < nRows; i++)
	{
		CListContainerElementUI *lpRow = (CListContainerElementUI*)lpWnd->m_lpList->GetItemAt(i);
		if (!g_bIsVip)		
		{
			//û�п�ͨ����ʾֻת5ҳ
			CVIPWizardWnd *lpMsgWnd = new CVIPWizardWnd;
			lpMsgWnd->Init();
			nRet = lpMsgWnd->ShowModal();

			CLabelUI *lpFileState = (CLabelUI*)lpRow->FindSubControl(_T("statusText"));
			if (nRet == IDCANCEL)
			{
				lpFileState->SetVisible(TRUE);
				continue ;
			}
			else
			{
				lpFileState->SetVisible(FALSE);
			}
		}

		//�ж��Ƿ���ѡ����
		if (lpWnd->ChkRowIsSelect(i))
		{
			//��ȡ�ж�Ӧ��vector������
			int nIndex = lpWnd->GetRowInVectorIndex(i);

			//��ʾ������
			CProgressUI *lpProgress = (CProgressUI*)lpRow->FindSubControl(_T("progress"));
			lpProgress->SetVisible(TRUE);

			//����ɾ����ť�� combo
			CComboUI *lpCombo = (CComboUI*)lpRow->FindSubControl(_T("comboPageRange"));
			lpCombo->SetEnabled(FALSE);
			CButtonUI *lpDelBtn = (CButtonUI*)lpRow->FindSubControl(_T("btnDeleteFile"));
			lpDelBtn->SetEnabled(FALSE);

			//���� ���ļ��ʹ�Ŀ¼��ť
			CButtonUI *lpOpenFileBtn = (CButtonUI*)lpRow->FindSubControl(_T("btnOpenFile"));
			lpOpenFileBtn->SetVisible(FALSE);

			CButtonUI *lpOpenPathBtn = (CButtonUI*)lpRow->FindSubControl(_T("btnOpenFolder"));
			lpOpenPathBtn->SetVisible(FALSE);

			int nStart;
			int nEnd;
			if (!g_vecPdfInfo[nIndex].strPageRange.IsEmpty())
			{
				int nPos = g_vecPdfInfo[nIndex].strPageRange.Find(_T("-"));
				CString strStart = g_vecPdfInfo[nIndex].strPageRange.Mid(0, nPos);
				CString strEnd = g_vecPdfInfo[nIndex].strPageRange.Mid(nPos + 1);

				nStart = _ttoi(strStart.GetBuffer(0));
				nEnd = _ttoi(strEnd.GetBuffer(0));
			}
			else
			{
				nStart = 1;
				nEnd = g_vecPdfInfo[nIndex].nFilePages;
			}
			lpProgress->SetMinValue(0);
			lpProgress->SetMaxValue(nEnd - nStart);

			// ��ʼת��
			g_nNowRow = i;

			BeginConvert(nIndex);

			//ת����ȥ����ѡ
			COptionUI *lpCheck = (COptionUI*)lpRow->FindSubControl(_T("checkFile"));
			lpCheck->Selected(FALSE);
		}
	}

	g_IPdf2WordEvent.SetPDF2Word(NULL);

	lpWnd->m_lpbtnConvert->SetEnabled(TRUE);
	lpWnd->m_lpSelAllOpt->SetEnabled(TRUE);
// 	lpWnd->m_lpSelAllOpt1->SetEnabled(TRUE);
	lpWnd->m_lpbtnDelete->SetEnabled(TRUE);

	::CoUninitialize();		//

	_endthreadex(0);
	return 0;
}

//��������ת�����Դ���
unsigned __stdcall PopSetWndFunc(LPVOID pM)
{
	CMyWnd *lpWnd = (CMyWnd*)pM;

	CSetWnd *lpSetWnd = new CSetWnd;
	lpSetWnd->Init();
	lpSetWnd->ShowModal();

	//
	lpWnd->m_lpOutDocForma->SelectItem(g_tagConnectSetting.nPutoutExt);

	_endthreadex(0);
	return 0;
}


//NotVipWantToFunc
unsigned __stdcall NotVipWantToFunc(LPVOID pM)
{
	CMyWnd *lpWnd = (CMyWnd*)pM;

	CMsgWnd *lpMsg = new CMsgWnd;
	lpMsg->Init(lpWnd->GetHWND(), _T("��Ǹ��ֻ��VIP�û���������ת����ҳ����Χ"), 0);
	lpMsg->ShowModal();

	_endthreadex(0);
	return 0;
}

//��������ת��ҳ�淶Χ
unsigned __stdcall PopPageRangeWndFunc(LPVOID pM)
{
	CComboUI *lpCbo = (CComboUI*) pM;

	//��ȡpdf�ļ�ҳ��
	int nIndex = lpCbo->GetParent()->GetParent()->GetTag();
	int nPages = g_vecPdfInfo[nIndex].nFilePages;

	CPageRangeWnd *lpMsg = new CPageRangeWnd;
	lpMsg->Init(nPages);
	if (IDOK == lpMsg->ShowModal())
	{
		int nCount = lpCbo->GetCount();
		g_vecPdfInfo[nIndex].strPageRange = g_strPageRange;
		if (nCount > 2)
		{
			CString strText;
			strText.Format(_T("{f ΢���ź� 12}%s{/f}"), g_strPageRange.GetBuffer(0));
			lpCbo->GetItemAt(lpCbo->GetCount()-1)->SetText(strText.GetBuffer(0));
			lpCbo->SelectItem(lpCbo->GetCount()-1);
		}
		else
		{
			// ����һ��item����Combo
			CListLabelElementUI *lpItem = new CListLabelElementUI;
			CString strAtt;
			strAtt.Format(_T("height=\"18\" text=\"{f ΢���ź� 12}%s{/f}\" visible=\"true\" "), g_strPageRange);
			lpItem->ApplyAttributeList(strAtt.GetBuffer(0));
			lpCbo->Add(lpItem);
			lpCbo->SelectItem(lpCbo->GetCount()-1);
		}
	}
	else
	{
		lpCbo->SelectItem(0);
	}

	_endthreadex(0);
	return 0;
}

void CMyWnd::InitWindow()
{
	::SetWindowText(m_hWnd, _T("����PDFתWORD"));

	m_lpList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("listFiles")));
	m_lpTopLeft = static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("ListTopLeft")));
	m_lpListBottom = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("ListBottom")));
// 	m_lpListBottom->SetVisible(FALSE);

	m_lpList_man = static_cast<CListUI*>(m_PaintManager.FindControl(_T("listManMake")));
	m_lpDragFile = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnDragFile")));

	m_lpSelAllOpt = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("checkSelectAll2")));	//ȫѡ
// 	m_lpSelAllOpt1 = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("checkSelectAll1")));	//ȫѡ
	m_lpbtnDelete = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnDelete")));	//ɾ��
	
	m_lpSavePathOpt = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("Save_PDFPath")));	//���浽pdf�ļ���ͬ��Ŀ¼

	m_lpSelSavePathOpt = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("Save_CustomPath")));	//�Զ���Ŀ¼

	m_lpSavePathEdt = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editPath")));	//����·��edit
	m_lpSavePathEdt->SetText(g_strSavePath.GetBuffer(0));

	m_lpOpenPathBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnSetPath")));	//��Ŀ¼

	m_lpOutDocForma = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("comboOutDocFormat")));	

	m_lpVipBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("UserLogin")));	//��ͨvip

	m_lpbtnConvert = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnConvert")));	//��ʼת��

// 	m_lpFileState = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("statusText")));	//ҳ������5ҳ
// 
// 	m_lpbtnOpenFile  = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnOpenFile")));	//���ļ�
// 
// 	m_lpbtnOpenFolder = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnOpenFolder")));	//��Ŀ¼

	m_lpTabLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("Tab_main")));

	m_lpTabLayout->SelectItem(0);	//����

	m_lpWebTj = static_cast<CWebBrowserUI*>(m_PaintManager.FindControl(_T("webtj_main")));	//ͳ��

	if (NULL != m_lpWebTj)
	{
		m_lpWebTj->SetVisible();
		//��ȡÿ����װ�����ص�����ID
		CString strTjUrl = _T("http://update.bskrt.com/pdf/lc.html");
		m_lpWebTj->Navigate2(strTjUrl.GetBuffer(0));
		m_lpWebTj->SetVisible(FALSE);
	}

	//���ù�����ͼƬ
	CScrollBarUI* pShortListVer = m_lpList->GetVerticalScrollBar();
	if (NULL != pShortListVer)
	{
		pShortListVer->SetAttribute(_T("width"),_T("13"));

		pShortListVer->SetBkImage(_T("scrollbk.png"));
		pShortListVer->SetButton1NormalImage(_T("file='scroll_top_button.png' source='0,0,13,12'"));
		pShortListVer->SetButton1HotImage(_T("file='scroll_top_button.png' source='13,0,26,12'"));
		pShortListVer->SetButton1PushedImage(_T("file='scroll_top_button.png' source='26,0,39,12'"));
		pShortListVer->SetButton2NormalImage(_T("file='scroll_bottom_button.png' source='0,0,13,12'"));
		pShortListVer->SetButton2HotImage(_T("file='scroll_bottom_button.png' source='13,0,26,12'"));
		pShortListVer->SetButton2PushedImage(_T("file='scroll_bottom_button.png' source='26,0,39,12'"));
		pShortListVer->SetThumbNormalImage(_T("file='scroll.png' source='0,0,13,327'"));
		pShortListVer->SetThumbHotImage(_T("file='scroll.png' source='13,0,26,327'"));
		pShortListVer->SetThumbPushedImage(_T("file='scroll.png' source='26,0,39,327'"));
	}

	//��ʼ��ת������
	InitConnectSetting();

	//�����̼߳���Ƿ���VIP
	_beginthreadex(NULL, 0, CheckVipUserFunc, (LPVOID)this, 0, NULL);

	//�����̼߳��BCL�Ƿ���ȷ��װ
	_beginthreadex(NULL, 0, CheckBCLIsOkFunc, (LPVOID)this, 0, NULL);

	ChangeWindowMessageFilter(0x0049, MSGFLT_ADD);
	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
};

//ɾ���б��ˢ��
void CMyWnd::RefreshListWhenDel()
{
	if (GetVecTrueNum() <= 0)	// ����Ҫ��ȡ��Ч���ļ�����
	{
		m_lpList->SetVisible(FALSE);
		m_lpTopLeft->SetVisible(FALSE);
		m_lpListBottom->SetVisible(FALSE);
		m_lpDragFile->SetVisible();

		//��� vector �α��0
		g_vecPdfInfo.clear();
		g_nVecCursor = 0;
	}
}

//�����ļ���ˢ��Ҫ������ļ��б�
void CMyWnd::RefreshFileListWhenAddFiles()
{
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, WhenAddFilesFunc, (LPVOID)this, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
}

void CMyWnd::RefreshFileList()
{
	if (GetVecTrueNum() > 0)
	{
		m_lpList->SetVisible();
		m_lpTopLeft->SetVisible();
		m_lpListBottom->SetVisible(TRUE);
		m_lpDragFile->SetVisible(FALSE);
	}
// 	else
// 	{
// 		m_lpList->SetVisible(FALSE);
// 		m_lpTopLeft->SetVisible(FALSE);
// 		m_lpListBottom->SetVisible(FALSE);
// 		m_lpDragFile->SetVisible(TRUE);
// 		return ;
// 	}

	for (int i = g_nVecCursor; i < g_vecPdfInfo.size(); i++)
	{
		if (g_vecPdfInfo[i].bIsDel)
		{
			continue;
		}

		CDialogBuilder builder;
		CListContainerElementUI* pUI =  static_cast<CListContainerElementUI*>( builder.Create( _T("file_list_item.xml") , 0 , NULL , &m_PaintManager , NULL ) );
		pUI->FindSubControl(_T("fileName"))->SetText(g_vecPdfInfo[i].strFileName.GetBuffer(0));	//�ļ�����
		pUI->FindSubControl(_T("fileName"))->SetToolTip(g_vecPdfInfo[i].strFileName.GetBuffer(0));

		pUI->FindSubControl(_T("fileSize"))->SetText(g_vecPdfInfo[i].strFileSize.GetBuffer(0));	//�ļ���С

		if (!g_bIsVip)
		{
			//���ҳ������5ҳ ����״̬
			int nPage = _ttoi(g_vecPdfInfo[i].strFileSize.GetBuffer(0));
			if (nPage > 5)
			{
				pUI->FindSubControl(_T("statusText"))->SetVisible(TRUE);
				pUI->FindSubControl(_T("statusText"))->SetText(_T("ҳ������5ҳ")); //״̬
			}
		}

		CString strPages;
		strPages.Format(_T("%d"), g_vecPdfInfo[i].nFilePages);
		pUI->FindSubControl(_T("pageCount"))->SetText(strPages.GetBuffer(0));	//�ļ�ҳ��

		pUI->SetTag(i);	//��¼��vector�е�����

		m_lpList->Add(pUI);

		g_nVecCursor++;
	}

}

//��ȡ�ַ�����ָ��key��ֵ
CString GetVauleByKey(CString strList, CString strKey)
{
	CString strValue;

	int nPos = strList.Find(strKey.GetBuffer(0));
	if (-1 != nPos)
	{
		//�ҵ���β , �� }
		int nEnd = strList.Find(_T(','), nPos);

		if (-1 == nEnd)
		{
			nEnd = strList.Find(_T('}'), nPos);
		}

		strValue = strList.Mid(nPos + strKey.GetLength(), nEnd - nPos - strKey.GetLength());
		strValue.Replace(_T("\""), _T(""));
		strValue.Replace(_T(":"), _T(""));
		strValue.Replace(_T(","), _T(""));
		strValue.Replace(_T("}"), _T(""));
	}

	return strValue;
}

//ˢ���˹�ת�������б�
void CMyWnd::RefreshManMakeList()
{
	//��ȫ�����
	//�����ϵĺ�g_vecManMakeInfo�ļ�¼
	m_lpList_man->RemoveAll();
	g_vecManMakeInfo.clear();

	//�����»�ȡ���ж���
	SYSTEMTIME st;
	GetSystemTime(&st);
	CTime tUTC(st);
	CString strTime;
	strTime.Format(_T("%d"), tUTC.GetTime());

	CString strSign;
	strSign.Format(_T("%s|%s|quxiupdf2word"), g_strMachineCode, strTime);
	CString strSignMd5 = MakeCStringMD5W(strSign);

	CString strUrl = _T("http://www.pdf00.com/order/get_orders?mn=[mn]&time=[time]&sign=[sign]");
	strUrl.Replace(_T("[mn]"), g_strMachineCode);
	strUrl.Replace(_T("[time]"), strTime);
	strUrl.Replace(_T("[sign]"), strSignMd5);

	CString strListInfo = GetHtmlStr(strUrl);

	int nPos = strListInfo.Find(_T("\"order_num\""));

	while (nPos != -1)
	{
		int nEnd = strListInfo.Find(_T("}"), nPos);

		CString strOneOrder = strListInfo.Mid(nPos - 1, nEnd - nPos + 2);

		OrderInfo tagOrder;
		tagOrder.strOrderID = GetStrFormJson(strOneOrder, _T("order_num"), _T(""), 0);	//�������
		CString strFileName = GetVauleByKey(strOneOrder, _T("filename"));	//�ļ���
		CStringA strFileNameA = CStrW2CStrA(strFileName);
		char *lpFileName = strFileNameA.GetBuffer(0);
		CStringA strFileNameAA = UnEscape(lpFileName).c_str();
		tagOrder.strFileName = CStrA2CStrW(strFileNameAA);

		tagOrder.strPageCount = GetStrFormJson(strOneOrder, _T("fpage"), _T(""), 0);	//�ļ�ҳ��
		
		CString strOrderMoney;
		strOrderMoney.Format(_T("%sԪ"), GetStrFormJson(strOneOrder, _T("order_money"), _T(""), 0));	//�������
		tagOrder.strAmt = strOrderMoney;

		CString strOrder_state = GetStrFormJson(strOneOrder, _T("order_state"), _T(""), 0);	//δ����|�Ѹ���
		if (_T("0") == strOrder_state || strOrder_state.IsEmpty())
		{
			tagOrder.strOrderState = _T("δ����");
		}
		else
		{
			tagOrder.strOrderState = _T("�Ѹ���");
		}

		CString strViptype3_state = GetStrFormJson(strOneOrder, _T("viptype3_state"), _T(""), 0);	//������|�Ѵ���
		if (_T("0") == strViptype3_state || strViptype3_state.IsEmpty())
		{
			tagOrder.strJinDu = _T("������");
		}
		else
		{
			tagOrder.strJinDu = _T("�Ѵ���");
		}

		tagOrder.strDownLoadUrl = GetStrFormJson(strOneOrder, _T("fedit"), _T(""), 0);	//word�ļ����ص�ַ
		g_vecManMakeInfo.push_back(tagOrder);

		nPos = strListInfo.Find(_T("\"order_num\""), nEnd);
	}

	//�����б�
	for (int i = 0; i < g_vecManMakeInfo.size(); i++)
	{
		CDialogBuilder builder;
		CListContainerElementUI* pUI =  static_cast<CListContainerElementUI*>( builder.Create( _T("file_list_item_manmake.xml") , 0 , NULL , &m_PaintManager , NULL ) );
		pUI->FindSubControl(_T("fileName_man"))->SetText(g_vecManMakeInfo[i].strFileName.GetBuffer(0));	//�ļ�����
		pUI->FindSubControl(_T("fileName_man"))->SetToolTip(g_vecManMakeInfo[i].strFileName.GetBuffer(0));

		pUI->FindSubControl(_T("pageCount_man"))->SetText(g_vecManMakeInfo[i].strPageCount.GetBuffer(0));	//�ļ�ҳ��

		pUI->FindSubControl(_T("money_man"))->SetText(g_vecManMakeInfo[i].strAmt.GetBuffer(0));	//���

		pUI->FindSubControl(_T("PayState_man"))->SetText(g_vecManMakeInfo[i].strOrderState.GetBuffer(0));	//����״̬
		if (_T("δ����") == g_vecManMakeInfo[i].strOrderState)
		{
			pUI->FindSubControl(_T("btnpay_man"))->SetVisible(TRUE);
		}

		pUI->FindSubControl(_T("jingdu_man"))->SetText(g_vecManMakeInfo[i].strJinDu.GetBuffer(0));	//����״̬
		if (_T("�Ѵ���") == g_vecManMakeInfo[i].strJinDu)
		{
			pUI->FindSubControl(_T("btnDownFile_down"))->SetVisible(TRUE);
		}

// 		pUI->FindSubControl(_T("jingdu_man"))->SetText(g_vecManMakeInfo[i].strJinDu.GetBuffer(0));	//����״̬

		pUI->SetTag(i);	//��¼��vector�е�����

		m_lpList_man->Add(pUI);

	}
}

//������ѯ�ͷ�
void SayHelloOnLineByQQ()
{
	// ��ȡqq����·��
	HKEY hKey;
	TCHAR szProductType[MAX_PATH] = _T("");
	DWORD dwBufLen = MAX_PATH;
	LONG lRet;

	if(RegOpenKeyEx(HKEY_CURRENT_USER,
		_T("SOFTWARE\\Tencent\\PlatForm_Type_List\\1"),
		0,
		KEY_QUERY_VALUE,
		&hKey) == ERROR_SUCCESS)
	{
		lRet = RegQueryValueEx(hKey,
			_T("TypePath"),
			NULL,
			NULL,
			(LPBYTE)szProductType,
			&dwBufLen);
	}
	else
	{
		// û��qq
	}
	RegCloseKey(hKey);

	CString strQQTimvp = szProductType;
	if (strQQTimvp.IsEmpty())
	{
		//��ʾû�а�װqq http://crm2.qq.com/page/portalpage/wpa.php?uin=4000868286&aty=0&a=0&curl=&ty=1

		return ;
	}

	// �����ͷ�qq����
	strQQTimvp.MakeLower();
	strQQTimvp.Replace(_T("qq.exe"), _T("Timwp.exe"));

	ShellExecute(NULL, _T("open"), strQQTimvp.GetBuffer(0), _T("tencent://message/?uin=3321139384&site=qq&menu=yes"), NULL, SW_SHOWNORMAL);
}

void CMyWnd::Notify(TNotifyUI& msg)
{
	CString strName = msg.pSender->GetName();
	if(msg.sType == _T("click")) 
	{
		if (_T("btnMainMenu") == strName)		//�˵�
		{
			POINT pt = {msg.ptMouse.x, msg.ptMouse.y};
			CMainMenuWnd *pMenu = new CMainMenuWnd();

			pMenu->Init(*this, pt);
			pMenu->ShowWindow(TRUE);
		}
		else if (_T("btnQQLine") == strName)	//��ϵ�ͷ�
		{
			SayHelloOnLineByQQ();
		}
		else if (_T("btnMin") == strName)		//��С��
		{
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
		}
		else if (_T("btnClose") == strName)		//�ر�
		{
			Close();
// 			SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
		}
		else if (_T("btnDragFile") == strName || _T("btnAdd2") == strName)		// ���pdf�ļ�  
		{
			m_lpTabLayout->SelectItem(0);	
// 			if (g_vecPdfInfo.size() > 0)
// 			{
// 				m_lpListBottom->SetVisible(TRUE);
// 			}

			msg.pSender->SetEnabled(FALSE);
// 			_beginthreadex(NULL, 0, OpenAddPdfFileDlgFunc, (LPVOID)this, 0, NULL);
			OpenAddPdfFileDlg();
			msg.pSender->SetEnabled(TRUE);
		}
		else if (_T("btnConvertSetting") == strName)	// �������ת��Ч��
		{
			_beginthreadex(NULL, 0, PopSetWndFunc, (LPVOID)this, 0, NULL);
		}
		else if (_T("btnDeleteFile") == strName)	// ɾ��һ��
		{
			//��ȡ�ڼ���
			int nIndex = (msg.pSender->GetParent()->GetParent())->GetTag();
			int nRow = m_lpList->GetItemIndex(msg.pSender->GetParent()->GetParent());
			m_lpList->RemoveAt(nRow);

			//���Vector ����ɾ��
			if (nIndex >= 0)
			{
				g_vecPdfInfo[nIndex].bIsDel = TRUE;
			}

			// ɾ����ˢ������
			RefreshListWhenDel();
		}
		else if (_T("btnOpenFile") == strName)	//���ļ�
		{
			if (!g_bIsVip)
			{
				CMsgWnd *lpMsgWnd = new CMsgWnd;
				lpMsgWnd->Init(this->GetHWND(), _T("������VIP��Ա������ֻ��ת����ǰ5ҳ�ĵ�"), 3);
				if (IDOK == lpMsgWnd->ShowModal())
				{
					int nIndex = (msg.pSender->GetParent()->GetParent())->GetTag();
					if (!g_vecPdfInfo[nIndex].strWordFilePath.IsEmpty())
					{
						ShellExecute(NULL, _T("open"), g_vecPdfInfo[nIndex].strWordFilePath.GetBuffer(0) , NULL, NULL, SW_SHOWNORMAL);
					}
				}
			}
			else
			{
				int nIndex = (msg.pSender->GetParent()->GetParent())->GetTag();
				if (!g_vecPdfInfo[nIndex].strWordFilePath.IsEmpty())
				{
					ShellExecute(NULL, _T("open"), g_vecPdfInfo[nIndex].strWordFilePath.GetBuffer(0) , NULL, NULL, SW_SHOWNORMAL);
				}
			}

		}
		else if (_T("btn_qq") == strName)	//����ҳ�� �ͷ�qq
		{
			SayHelloOnLineByQQ();
		}
		else if (_T("btn_RefreshOrder") == strName)		//ˢ�¶���
		{
			RefreshManMakeList();
			ShowMsgTipWnd(this->GetHWND(), _T("����ˢ�³ɹ�"), 0);
		}
		else if (_T("btnDownFile_down") == strName)		//����word�ļ�
		{
			//����
			_beginthreadex(NULL, 0, DownLoadWordFileFunc, (LPVOID)(msg.pSender), 0, NULL);
		}
		else if (_T("btnOpenFile_down") == strName)	//��Word�ļ�
		{
			int nIndex = msg.pSender->GetParent()->GetParent()->GetTag();
			ShellExecute(NULL, _T("open"), g_vecManMakeInfo[nIndex].strWordFile.GetBuffer(0), NULL, NULL, SW_SHOWNORMAL);
		}
		else if (_T("btnOpenFolder_down") == strName)	//��Word�ļ�����Ŀ¼
		{
			int nIndex = msg.pSender->GetParent()->GetParent()->GetTag();
			int nPos = g_vecManMakeInfo[nIndex].strWordFile.ReverseFind(_T('\\'));
			if (-1 != nPos)
			{
				CString strFilePath = g_vecManMakeInfo[nIndex].strWordFile.Mid(0, nPos);
				ShellExecute(NULL, _T("open"), strFilePath.GetBuffer(0), NULL, NULL, SW_SHOWNORMAL);
			}
		}
		else if (_T("btnpay_man") == strName)	//����֧��
		{
			//������
			int nIndex = (msg.pSender->GetParent()->GetParent())->GetTag();
			CString strOrderID = g_vecManMakeInfo[nIndex].strOrderID;

			if (!strOrderID.IsEmpty())
			{
				SYSTEMTIME st;
				GetSystemTime(&st);
				CTime tUTC(st);
				CString strTime;
				strTime.Format(_T("%d"), tUTC.GetTime());

				CString strSign;
				strSign.Format(_T("%s|%s|quxiupdf2word"), strOrderID, strTime);
				CString strSignMd5 = MakeCStringMD5W(strSign);

				CString strUrl= _T("http://www.pdf00.com/order/re_order?ordernum=[ordernum]&time=[time]&sign=[sign]");
				strUrl.Replace(_T("[ordernum]"), strOrderID);
				strUrl.Replace(_T("[time]"), strTime);
				strUrl.Replace(_T("[sign]"), strSignMd5);

				ShellExecute(NULL, _T("open"), strUrl.GetBuffer(0), NULL, NULL, SW_SHOWNORMAL);
			}
		}
		else if (_T("btnDeleteFile_down") == strName)	//ɾ������
		{
			//������
			int nIndex = (msg.pSender->GetParent()->GetParent())->GetTag();
			CString strOrderID = g_vecManMakeInfo[nIndex].strOrderID;

			//�жϸö����ܲ���ɾ�� �Ѹ���Ķ�������ɾ
			BOOL bDel = FALSE;
			if (_T("�Ѹ���") == g_vecManMakeInfo[nIndex].strOrderState)
			{
				CMsgWnd *lpMsgWnd = new CMsgWnd;
				ShowMsgTipWnd(this->GetHWND(), _T("��֧���Ķ�������ɾ��"), 0);
				return ;
			}
			else
			{
				//��ʾȷ��ɾ��
				CMsgWnd *lpMsgWnd = new CMsgWnd;
				lpMsgWnd->Init(this->GetHWND(), _T("��ȷ��ɾ�����Ķ���"), 4);
				if (IDOK == lpMsgWnd->ShowModal())
				{
					bDel = TRUE;
				}
			}

			if (!strOrderID.IsEmpty() && bDel)
			{
				SYSTEMTIME st;
				GetSystemTime(&st);
				CTime tUTC(st);
				CString strTime;
				strTime.Format(_T("%d"), tUTC.GetTime());

				CString strSign;
				strSign.Format(_T("%s|%s|%s|quxiupdf2word"), strOrderID,g_strMachineCode, strTime);
				CString strSignMd5 = MakeCStringMD5W(strSign);

				CString strUrl= _T("http://www.pdf00.com/order/hide_order?ordernum=[ordernum]&mn=[mn]&time=[time]&sign=[sign]");
				strUrl.Replace(_T("[ordernum]"), strOrderID);
				strUrl.Replace(_T("[mn]"), g_strMachineCode);
				strUrl.Replace(_T("[time]"), strTime);
				strUrl.Replace(_T("[sign]"), strSignMd5);

				//ִ��ɾ��
				CString strUrlInfo = GetHtmlStr(strUrl);
				CString strCode = GetStrFormJson(strUrlInfo, _T("code"), _T(""), 1);
				if (_T("200") == strCode)
				{
					//�ɹ� ˢ�½���
					RefreshManMakeList();
				}

			}
		}
		else if (_T("btnOpenFolder") == strName)	//���ļ�����Ŀ¼
		{
			if (!g_bIsVip)
			{
				CMsgWnd *lpMsgWnd = new CMsgWnd;
				lpMsgWnd->Init(this->GetHWND(), _T("������VIP��Ա������ֻ��ת����ǰ5ҳ�ĵ�"), 3);
				if (IDOK == lpMsgWnd->ShowModal())
				{
					int nIndex = (msg.pSender->GetParent()->GetParent())->GetTag();
					CString strFilePath = g_vecPdfInfo[nIndex].strWordFilePath;
					int nPos = strFilePath.ReverseFind(_T('\\'));
					CString strPath = strFilePath.Mid(0, nPos);
					ShellExecute(NULL, _T("open"), strPath.GetBuffer(0) , NULL, NULL, SW_SHOWNORMAL);
				}
			}
			else
			{
				int nIndex = (msg.pSender->GetParent()->GetParent())->GetTag();
				CString strFilePath = g_vecPdfInfo[nIndex].strWordFilePath;
				int nPos = strFilePath.ReverseFind(_T('\\'));
				CString strPath = strFilePath.Mid(0, nPos);
				ShellExecute(NULL, _T("open"), strPath.GetBuffer(0) , NULL, NULL, SW_SHOWNORMAL);
			}
		}
		else if (_T("btnOpenFolder2") == strName)	//�Ҳ��Ŀ¼
		{
			ShellExecute(NULL, _T("open"), g_strSavePath.GetBuffer(0) , NULL, NULL, SW_SHOWNORMAL);
		}
		else if (_T("btnDelete") == strName)	//ɾ��
		{
			//
			for (int i = m_lpList->GetCount() - 1; i >= 0; i--)
			{
				//�ж���û��ѡ��
				COptionUI *lpSel = ((COptionUI*)(((CListContainerElementUI*)(m_lpList->GetItemAt(i)))->FindSubControl(_T("checkFile"))));
				if (lpSel->IsSelected())
				{
					int nIndex = (lpSel->GetParent()->GetParent())->GetTag();
					m_lpList->RemoveAt(i);

					//���Vector ����ɾ��
					if (nIndex >= 0)
					{
						g_vecPdfInfo[nIndex].bIsDel = TRUE;
					}
				}
			}

			//ɾ����ˢ��
			RefreshListWhenDel();
		}
		else if (_T("btnSetPath") == strName)	//��ѡ�񱣴�doc�ļ�Ŀ¼
		{
			TCHAR chPath[255];  //�����洢·�����ַ���
			CString strPath;
			BROWSEINFO bInfo;
			GetModuleFileName(NULL,chPath,MAX_PATH);
			strPath=chPath;
			ZeroMemory(&bInfo, sizeof(bInfo));
			bInfo.hwndOwner = m_hWnd;
			bInfo.lpszTitle = _T("��ѡ��·��: ");    
			bInfo.ulFlags   = BIF_RETURNONLYFSDIRS|BIF_EDITBOX;
// 			bInfo.lpfn      = BrowseCallbackProc;
			bInfo.lParam    = (LPARAM)strPath.GetBuffer(strPath.GetLength());

			LPITEMIDLIST lpDlist;  //�������淵����Ϣ��IDList
			lpDlist = SHBrowseForFolder(&bInfo) ; //��ʾѡ��Ի���
			if(lpDlist != NULL)  //�û�����ȷ����ť
			{
				SHGetPathFromIDList(lpDlist, chPath);//����Ŀ��ʶ�б�ת�����ַ���
				strPath = chPath; //��TCHAR���͵��ַ���ת��ΪCString���͵��ַ���
				if (strPath.GetLength() > 3)
				{
					g_strSavePath = strPath + _T("\\");
				}
				else
				{
					g_strSavePath = strPath;
				}
				m_lpSavePathEdt->SetText(g_strSavePath.GetBuffer(0));
			}
		}
		else if (_T("btnManMake") == strName)	// �˹�ת��
		{
			m_lpTabLayout->SelectItem(1);	//
			RefreshManMakeList();

			CManMakeWnd *lpWnd = new CManMakeWnd;
			lpWnd->Init();
			lpWnd->ShowModal();
		}
		else if (_T("UserLogin") == strName)	//����VIP
		{
			if (!g_bIsVip)
			{
				CVIPWizardWnd *lpWnd = new CVIPWizardWnd;
				lpWnd->Init();
				lpWnd->m_lpCloseBtn->SetVisible(FALSE);
				lpWnd->m_lpExitBtn->SetVisible(TRUE);
				lpWnd->ShowModal();
			}
			else
			{
				CString strMsg;
				if (_T("2") == g_tagVipInfo.strType)
				{
					strMsg = _T("���Ѿ������û�Ա����������ʹ��");
					ShowMsgTipWnd(this->GetHWND(), strMsg.GetBuffer(0), 0);
// 					CMsgWnd *lpMsg = new CMsgWnd;
// 					lpMsg->Init(strMsg.GetBuffer(0), 0);
				}
				else
				{
					if (g_tagVipInfo.strNowDate > g_tagVipInfo.strEndDate)
					{
						strMsg.Format(_T("������ѻ�Ա������ʱ�� %s �ѹ��ڣ���Ҫ���Ѳſ��Լ���ʹ�ã�"), g_tagVipInfo.strEndDate);
						ShowMsgTipWnd(this->GetHWND(), strMsg.GetBuffer(0), 2);
// 						CMsgWnd *lpMsg = new CMsgWnd;
// 						lpMsg->Init(strMsg.GetBuffer(0), 2);
					}
					else
					{
						strMsg.Format(_T("������ѻ�Ա������ʱ�� %s"), g_tagVipInfo.strEndDate);
						ShowMsgTipWnd(this->GetHWND(), strMsg.GetBuffer(0), 0);
// 						CMsgWnd *lpMsg = new CMsgWnd;
// 						lpMsg->Init(strMsg.GetBuffer(0), 0);
					}
				}
			}
		}
		else if (_T("btnConvert") == strName)	//��ʼת��
		{
			m_lpTabLayout->SelectItem(0);
			_beginthreadex(NULL, 0, BeginConnectFunc, (LPVOID)this, 0, NULL);
		}
	}
	else if(msg.sType == _T("itemselect"))
	{
		if (_T("comboPageRange") == strName)	//���ת�����е�ҳ��ʱ��
		{
			CComboUI *lpCombo = static_cast<CComboUI*>(msg.pSender);

			if (1 == lpCombo->GetCurSel())	// ... ����ѡ�з�Χ
			{
				//�ж���û�п�ͨvip
				if (g_bIsVip)	//�ѿ�ͨ
				{
					_beginthreadex(NULL, 0, PopPageRangeWndFunc, (LPVOID)lpCombo, 0, NULL);
				}
				else
				{
					lpCombo->SelectItem(0);
					_beginthreadex(NULL, 0, NotVipWantToFunc, (LPVOID)this, 0, NULL);
				}
			}
		}
		else if (_T("comboOutDocFormat") == strName)
		{
			CComboUI *lpCombo = static_cast<CComboUI*>(msg.pSender);
			int nIndex = lpCombo->GetCurSel();
			g_tagConnectSetting.nPutoutExt = nIndex;
		}
	}
	else if(msg.sType == _T("selectchanged"))
	{
		// ȫѡѡ��
		if (_T("checkSelectAll2") == strName)
		{
			if (((COptionUI*)msg.pSender)->IsSelected())
			{
				for (int i = 0; i < m_lpList->GetCount(); i++)
				{
					((COptionUI*)(((CListContainerElementUI*)(m_lpList->GetItemAt(i)))->FindSubControl(_T("checkFile"))))->Selected(TRUE);
				}
			}
			else
			{
				for (int i = 0; i < m_lpList->GetCount(); i++)
				{
					((COptionUI*)(((CListContainerElementUI*)(m_lpList->GetItemAt(i)))->FindSubControl(_T("checkFile"))))->Selected(FALSE);
				}
			}
		}
		else if (_T("Save_PDFPath") == strName)
		{
			if (m_lpSavePathOpt->IsSelected())	//���浽pdfĿ¼
			{
				m_lpSavePathEdt->SetEnabled(FALSE);
				m_lpOpenPathBtn->SetEnabled(FALSE);
			}
		}
		else if (_T("Save_CustomPath") == strName)	
		{
			if (m_lpSelSavePathOpt->IsSelected())	//���浽�Զ���Ŀ¼
			{
				m_lpSavePathEdt->SetEnabled(TRUE);
				m_lpOpenPathBtn->SetEnabled(TRUE);
			}
		}
	}

		__super::Notify(msg);
};

//��ѡ��pdf�ļ��Ի���
void CMyWnd::OpenAddPdfFileDlg()
{
// 	AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);	//ʹ��mfc CFIledlg	�޷�����������
// 
// 	//����һ������ѡ�����ļ���CFileDialog  
// 	TCHAR* filters = _T("PDF�ļ�(*.pdf)|*.pdf||");
// 	CFileDialog fileDlg(true,NULL,_T("*.pdf"),OFN_ALLOWMULTISELECT | OFN_ENABLESIZING | OFN_HIDEREADONLY, filters);
// 
// 	TCHAR szLargeBuf[1024*10] = _T("");
// 
// 	fileDlg.m_ofn.lpstrTitle = _T("��ѡ��Ҫת����PDF�ļ���");
// 	fileDlg.m_ofn.lpstrFile = szLargeBuf;			//��ѡ�ļ�ʱ Ĭ�ϳ���ֻ��260 ѡ���˼��� �����Զ��峤�� 
// 	fileDlg.m_ofn.nMaxFile = 1024*10;
// 
// 	if (fileDlg.DoModal () == IDOK)
// 	{
// 		CString strFilePath;
// 		CString strFileName;
// 
// 		//��ȡ��һ���ļ���λ��  
// 		POSITION pos_file;  
// 		pos_file = fileDlg.GetStartPosition(); 
// 
// 		while(NULL != pos_file)
// 		{
// 			//ȫ·��
// 			strFilePath = fileDlg.GetNextPathName(pos_file);
// 			AddPdfFile2Vec(strFilePath);
// 		}
// 	}

	TCHAR szOpenFileNames[200*MAX_PATH] = _T("");
	TCHAR szPath[MAX_PATH] = _T("");
	TCHAR* p; 
	TCHAR szFileName[MAX_PATH] = _T("");

	OPENFILENAME file = {0};
	file.lpstrTitle = _T("��ѡ��Ҫת����PDF�ļ���");
	file.lStructSize = sizeof(file);

	file.hwndOwner = this->GetHWND();
	file.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT;  
	file.lpstrFile = szOpenFileNames;
	file.nMaxFile = 200*MAX_PATH;

	file.lpstrFilter = _T("PDF�ļ�(*.pdf)\0*.pdf");
	file.nFilterIndex = 1;//Ĭ��ѡ���һ��
	// �������ļ��ĶԻ���
	if(::GetOpenFileName(&file))
	{
		// ��ȡ�ļ���·��szPath  
		lstrcpyn(szPath, szOpenFileNames, file.nFileOffset );  
		lstrcat(szPath, _T("\\"));   // ĩβ���Ϸ�б��  

		p = szOpenFileNames + file.nFileOffset; //��ָ���Ƶ���һ���ļ�  

		while(*p)  
		{
			lstrcat(szFileName, szPath);  //���ļ�������·��    
			lstrcat(szFileName, p);       //�����ļ���       

			AddPdfFile2Vec(szFileName);

			p += lstrlen(p) +1;           //������һ���ļ� 
			memset(szFileName, 0, sizeof(TCHAR) * MAX_PATH);
		}
	}

}

//����ȫ·�������ļ���Ϣ��vec
void CMyWnd::AddPdfFile2Vec(CString strFilePath)
{
	CString strExt = strFilePath.Right(4);
	strExt.MakeLower();

	if (_T(".pdf") != strExt)
	{
		return ;
	}

	//�ж��ļ��Ƿ��Ѿ�����
	for (int i = 0; i < g_vecPdfInfo.size(); i++)
	{
		CString strPath = g_vecPdfInfo[i].strFilePath;
		if ((strFilePath == strPath))
		{
			if (!(g_vecPdfInfo[i].bIsDel)) //������û��ɾ��
			{
				return ;
			}
		}
	}

	//�ļ���
	CString strFileName  = strFilePath.Mid(strFilePath.ReverseFind(_T('\\'))+1);

	//��ȡ�ļ���С
	CString strFileSize = M_GetFileSize(strFilePath);

	//��ȡ�ļ�ҳ��
	int nPages = 0;

	//��¼�ļ���Ϣ
	Pdf_Info pdfinfo;
	pdfinfo.strFilePath = strFilePath;
	pdfinfo.strFileName = strFileName;
	pdfinfo.strFileSize = strFileSize;
	pdfinfo.nFilePages = nPages;
	pdfinfo.bIsDel = FALSE;	// �����϶�ûɾ��
	pdfinfo.nTag = 0;	//�����ɶ���δת��

	g_vecPdfInfo.push_back(pdfinfo);

	//��ȡ�ļ�
	::CoInitialize(NULL);
	EasyConverterLib::IPDF2WordPtr pConvObj = NULL;
	try
	{
		pConvObj.CreateInstance ("EasyConverter.PDF2Word.5"); 
		pConvObj->LicenseKey = GetBCLKey().GetBuffer(0);	//��ʽ��OEM license key 20161221 14����

		nPages = pConvObj->GetNumberOfPages(strFilePath.GetBuffer(0));	
		g_vecPdfInfo[g_vecPdfInfo.size() - 1].nFilePages = nPages;
	}
	catch(_com_error &e) //��ʾ��������ļ���
	{
		//�˴�ȫ������������
		{
			CInputPWWnd *lpWnd = new CInputPWWnd;
			lpWnd->Init(g_vecPdfInfo.size() - 1);
			if (IDCANCEL == lpWnd->ShowModal())
			{
				vector<Pdf_Info>::iterator it = g_vecPdfInfo.begin() + g_vecPdfInfo.size() - 1;
				g_vecPdfInfo.erase(it);
			}
		}
	}

	pConvObj = NULL; 
	::CoUninitialize();

	RefreshFileListWhenAddFiles();
};

//�϶��ļ����Ի���
void CMyWnd::DropFileOnDlg(HDROP hDrop)
{
	int DropCount = DragQueryFile(hDrop, -1, NULL, 0);
	TCHAR szDropFilePath[MAX_PATH] = _T("");

	m_lpTabLayout->SelectItem(0);

	for (int i = 0; i < DropCount; i++)
	{
		DragQueryFile(hDrop, i, szDropFilePath, MAX_PATH);

		AddPdfFile2Vec(szDropFilePath);
	}

	DragFinish(hDrop);  
};

LRESULT CMyWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DROPFILES:	//�϶��ļ����Ի���
		{
			DropFileOnDlg((HDROP)wParam);
			RefreshFileListWhenAddFiles();
		}
		break;

	default:
		break;
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
};

//�ж����Ƿ���ѡ�е�
BOOL CMyWnd::ChkRowIsSelect(int nRow)
{
	CListContainerElementUI *lpRow = (CListContainerElementUI*)m_lpList->GetItemAt(nRow);
	CCheckBoxUI *lpCheckBox =  (CCheckBoxUI*)lpRow->FindSubControl(_T("checkFile"));

	return lpCheckBox->IsSelected();
}

//��ȡ������Ӧvector �е�����
int CMyWnd::GetRowInVectorIndex(int nRow)
{
	int nIndex = -1;

	CListContainerElementUI *lpRow = (CListContainerElementUI*)m_lpList->GetItemAt(nRow);

	nIndex = lpRow->GetTag();

	return nIndex;
}


