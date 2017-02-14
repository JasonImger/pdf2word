#include "stdafx.h"
#include "MenuWnd.h"
#include "MsgWnd.h"
#include "SetWnd.h"
#include "FeedBackWnd.h"
#include "AboutWnd.h"

//��������ת�����Դ���
extern unsigned __stdcall PopSetWndFunc(LPVOID pM);

CMainMenuWnd::CMainMenuWnd() 
{

}

CMainMenuWnd::~CMainMenuWnd()
{

}

void CMainMenuWnd::Init(HWND hWndParent, POINT ptPos)
{
    Create(hWndParent, _T(""), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
    ::ClientToScreen(hWndParent, &ptPos);
    ::SetWindowPos(*this, NULL, ptPos.x, ptPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void CMainMenuWnd::OnFinalMessage( HWND /*hWnd*/ )
{
    delete this;
}

LRESULT CMainMenuWnd::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;

    switch( uMsg )
    {
    case WM_KILLFOCUS:    
        lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); 
        break; 

    default:
        bHandled = FALSE;
    }

    if(bHandled || m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) 
    {
        return lRes;
    }

    return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CMainMenuWnd::OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    Close();
    bHandled = FALSE;
    return __super::OnKillFocus(uMsg, wParam, lParam, bHandled); 
}

/*
�������ܣ���ָ���ļ���ָ����Ŀ¼�´������ݷ�ʽ
����������
lpszFileName    ָ���ļ���ΪNULL��ʾ��ǰ���̵�EXE�ļ���
lpszLnkFileDir  ָ��Ŀ¼������ΪNULL��
lpszLnkFileName ��ݷ�ʽ���ƣ�ΪNULL��ʾEXE�ļ�����
wHotkey         Ϊ0��ʾ�����ÿ�ݼ�
lpszComLine		����
pszDescription  ��ע
iShowCmd        ���з�ʽ��Ĭ��Ϊ���洰��
*/
BOOL CreateFileShortcut(LPTSTR lpszFileName, LPTSTR lpszLnkFileDir, LPTSTR lpszLnkFileName, LPTSTR lpszWorkDir, 
						WORD wHotkey, LPTSTR lpszIco, LPTSTR lpszComLine, LPTSTR lpszDescription, int iShowCmd = SW_SHOWNORMAL)
{
	if (lpszLnkFileDir == NULL)
		return FALSE;

	HRESULT hr;
	IShellLink     *pLink;  //IShellLink����ָ��
	IPersistFile   *ppf; //IPersisFil����ָ��

	CoInitialize(NULL);

	//����IShellLink����
	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pLink);
	if (FAILED(hr))
		return FALSE;

	//��IShellLink�����л�ȡIPersistFile�ӿ�
	hr = pLink->QueryInterface(IID_IPersistFile, (void**)&ppf);
	if (FAILED(hr))
	{
		pLink->Release();
		return FALSE;
	}

	//Ŀ��
	if (lpszFileName == NULL)
		pLink->SetPath(_tpgmptr);
	else
	{
		pLink->SetPath(lpszFileName);
	}

	if (lpszIco != NULL)
	{
		pLink->SetIconLocation(lpszIco, 0);
	}

	//����Ŀ¼
	if (lpszWorkDir != NULL)
		pLink->SetPath(lpszWorkDir);

	//��ݼ�
	if (wHotkey != NULL)
		pLink->SetHotkey(wHotkey);

	//����
	if (lpszComLine != NULL)
	{
		pLink->SetArguments(lpszComLine);
	}
	//��ע
	if (lpszDescription != NULL)
		pLink->SetDescription(lpszDescription);

	//��ʾ��ʽ
	pLink->SetShowCmd(iShowCmd);

	//��ݷ�ʽ��·�� + ����
	TCHAR szBuffer[MAX_PATH];
	if (lpszLnkFileName != NULL) //ָ���˿�ݷ�ʽ������
		_stprintf(szBuffer, _T("%s\\%s"), lpszLnkFileDir, lpszLnkFileName);
	else  
	{
		//û��ָ�����ƣ��ʹ�ȡָ���ļ����ļ�����Ϊ��ݷ�ʽ���ơ�
		TCHAR *pstr;
		if (lpszFileName != NULL)
			pstr = _tcsrchr(lpszFileName, _T('\\'));
		else
			pstr = _tcsrchr(_tpgmptr, _T('\\'));

		if (pstr == NULL)
		{    
			ppf->Release();
			pLink->Release();
			return FALSE;
		}
		//ע���׺��Ҫ��.exe��Ϊ.lnk
		_stprintf(szBuffer, _T("%s\\%s"), lpszLnkFileDir, pstr);
		int nLen = _tcsclen(szBuffer);
		szBuffer[nLen - 3] = _T('l');
		szBuffer[nLen - 2] = _T('n');
		szBuffer[nLen - 1] = _T('k');
	}

	//�����ݷ�ʽ��ָ��Ŀ¼��
#ifdef _UNICODE
	hr = ppf->Save(szBuffer, TRUE);
#else
	WCHAR  wsz[MAX_PATH];  //����Unicode�ַ���
	MultiByteToWideChar(CP_ACP, 0, szBuffer, -1, wsz, MAX_PATH);
	hr = ppf->Save(wsz, TRUE);
#endif

	ppf->Release();
	pLink->Release();

	CoUninitialize();

	return SUCCEEDED(hr);
}

//���������ݷ�ʽ
void SetLnkOnDesk()
{
	TCHAR szPath[MAX_PATH] = _T("");
	SHGetSpecialFolderPath(0,szPath, CSIDL_COMMON_DESKTOPDIRECTORY, FALSE);
	CString strDeskPath = szPath;

	CreateFileShortcut(NULL, strDeskPath.GetBuffer(0),
					_T("����PDFתWord.lnk"), NULL, 0, NULL, NULL, _T("����PDFתWord"));
}

unsigned __stdcall PopFeedBackWndFunc(LPVOID pM)
{
	CMyWnd *lpWnd = (CMyWnd*)pM;

	CFeedBackWnd *lpFeedback = new CFeedBackWnd;
	lpFeedback->Init();
	lpFeedback->ShowModal();

	_endthreadex(0);
	return 0;
}

unsigned __stdcall PopAboutWndFunc(LPVOID pM)
{
	CMyWnd *lpWnd = (CMyWnd*)pM;

	CAboutWnd *lpAboutWnd = new CAboutWnd;
	lpAboutWnd->Init();
	lpAboutWnd->ShowModal();

	_endthreadex(0);
	return 0;
}

//
void CMainMenuWnd::Notify(TNotifyUI& msg)
{
	if(msg.sType == _T("itemclick")) 
	{
		CString strName = msg.pSender->GetName();
		if (_T("menuCreateSC") == strName)		//���������ݷ�ʽ
		{
			SetLnkOnDesk();
			Close();
		}
		else if (_T("menuUpdate") == strName)	//������
		{
// 			Close();

			ShowMsgTipWnd(g_hMainWnd, _T("�Ѿ������°汾"), 0);
// 			CMsgWnd *lpMsgWnd = new CMsgWnd;
// 			lpMsgWnd->Init(g_hMainWnd, _T("�Ѿ������°汾"), 0);
// 			lpMsgWnd->ShowWindow();
		}
		else if (_T("menuSetting") == strName)		//ת��Ч������
		{
			_beginthreadex(NULL, 0, PopSetWndFunc, (LPVOID)g_MainWnd, 0, NULL);
		}
		else if (_T("menuFeedback") == strName)		//�������
		{
// 			Close();
			_beginthreadex(NULL, 0, PopFeedBackWndFunc, (LPVOID)g_MainWnd, 0, NULL);
		}
		else if (_T("menuAbout") == strName)		//��������
		{
// 			Close();
			_beginthreadex(NULL, 0, PopAboutWndFunc, (LPVOID)g_MainWnd, 0, NULL);
		}
	}

	__super::Notify(msg);
};
