#include "stdafx.h"
#include "MsgWnd.h"

CMsgWnd::CMsgWnd() 
{
	m_nType = -1;
}

CMsgWnd::~CMsgWnd()
{

}

void CMsgWnd::Init(HWND hParent, CString strMsg, int nTag)
{
	Create(hParent, _T(""), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
	CenterWindow();

	CControlUI *lpPng;
	CTextUI *lpMsg;
	CButtonUI *lpLeftBtn;
	CButtonUI *lpOkBtn;
	CButtonUI *lpNoBtn;
	RECT rc = {300, 4, 370, 33};
	lpLeftBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnLeft")));
	lpOkBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnYes")));
	lpNoBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnClose")));

	m_nType = nTag;
	if (0 == nTag)
	{
		lpPng = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("infoLogo")));
		lpOkBtn->SetPos(rc);
		lpNoBtn->SetVisible(FALSE);
	}
	else if (1 == nTag)
	{
		lpPng = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("errorLogo")));
	}
	else if (2 == nTag)	//vip ����
	{
		lpPng = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("warnLogo")));
		lpOkBtn->SetText(_T("����"));
	}
	else if (3 == nTag)	//ת���ú� ���ļ����Ŀ¼
	{
		lpPng = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("warnLogo")));
		lpLeftBtn->SetVisible(TRUE);
		lpLeftBtn->SetText(_T("������"));
		lpOkBtn->SetVisible(TRUE);
		lpOkBtn->SetNormalImage(_T("file='button4.png' source='0,0,42,29' corner='10,4,10,4'"));
		lpOkBtn->SetHotImage(_T("file='button4.png' source='42,0,84,29' corner='10,4,10,4'"));
		lpOkBtn->SetPushedImage(_T("file='button4.png' source='84,0,126,29' corner='10,4,10,4'"));
		lpOkBtn->SetText(_T("��ͨVIP"));
		lpNoBtn->SetVisible(TRUE);
	}
	else if (4 == nTag)	//ȷ����ȡ��
	{
		lpPng = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("warnLogo")));
		lpOkBtn->SetVisible(TRUE);
		lpNoBtn->SetVisible(TRUE);
	}
	else	// -1
	{
		lpOkBtn->SetPos(rc);
		lpNoBtn->SetVisible(FALSE);
		lpPng = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("warnLogo")));
	}
	lpMsg = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("context")));

	lpPng->SetVisible(TRUE);
	lpMsg->SetText(strMsg.GetBuffer(0));
}

void CMsgWnd::OnFinalMessage( HWND /*hWnd*/ )
{
	delete this;
}

// LRESULT CMsgWnd::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
// {
// 	LRESULT lRes = 0;
// 	BOOL bHandled = TRUE;
// 
// 	switch( uMsg )
// 	{
// 	case WM_KILLFOCUS:    
// // 		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); 
// 		break; 
// 
// 	default:
// 		bHandled = FALSE;
// 	}
// 
// 	if(bHandled || m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) 
// 	{
// 		return lRes;
// 	}
// 
// 	return __super::HandleMessage(uMsg, wParam, lParam);
// }

void CMsgWnd::Notify(TNotifyUI& msg)
{
	if(msg.sType == _T("click")) 
	{
		CString strName = msg.pSender->GetName();
		if (_T("btnYes") == strName)		//�رնԻ���
		{
			if (2 == m_nType || 3 == m_nType)	//vip ���� //ת����� ���ļ� �� ��Ŀ¼
			{
				PayForVip(1);
			}
			
			if (4 == m_nType)	//ȷ��ɾ��
			{
				Close(IDOK);
				return ;
			}
// 			if ();	//ת����� ���ļ� �� ��Ŀ¼
// 			{
// 				PayForVip(1);
// 			}
			Close(IDCANCEL);
		}
		else if (_T("btnExit") == strName)
		{
			Close(IDCANCEL);
		}
		else if (_T("btnClose") == strName)
		{
			Close(IDCANCEL);
		}
		else if (_T("btnLeft") == strName)
		{
			Close(IDOK);
		}
	}

	__super::Notify(msg);
};
