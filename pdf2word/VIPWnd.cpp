#include "stdafx.h"
#include "VIPWnd.h"

BOOL g_bOnlyFilePages = FALSE;	//ֻת5ҳ

CVIPWizardWnd::CVIPWizardWnd() 
{
	m_nPayType = 1;
}

CVIPWizardWnd::~CVIPWizardWnd()
{
}

void CVIPWizardWnd::Init()
{
	Create(g_hMainWnd, _T(""), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
	CenterWindow();

	m_lpTab = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("tab")));	
	m_lpCloseBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnClose")));	//ֻת5ҳ
	m_lpExitBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnExit2")));	//�ر�
	m_lpPay = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnPay")));	//֧��
	m_lpPayOK = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnPayOK")));	//֧���ɹ�
	m_lpOneYearOpt = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("ForeverType")));	//һ��
	m_lpMoreYearOpt = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("ForeverType2")));	//����
	m_lpMat = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("price")));	//���
}

void CVIPWizardWnd::OnFinalMessage( HWND /*hWnd*/ )
{
	delete this;
}

// �����Ƿ�֧���ɹ�
BOOL CheckPayOk()
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	CTime tUTC(st);
	CString strTime;
	strTime.Format(_T("%d"), tUTC.GetTime());

	CString strSign;
	strSign.Format(_T("%s|%s|quxiupdf2word"), g_strMachineCode, strTime);
	CString strSignMd5 = MakeCStringMD5W(strSign);

	CString strUrl = _T("http://www.pdf00.com/key/checkkey?mn=[mn]&time=[time]&sign=[sign]");
	strUrl.Replace(_T("[mn]"), g_strMachineCode);
	strUrl.Replace(_T("[time]"), strTime);
	strUrl.Replace(_T("[sign]"), strSignMd5);

	CString strUrlInfo = GetHtmlStr(strUrl);
	if (strUrlInfo.IsEmpty())
	{
		g_bIsVip = FALSE;
		return FALSE;
	}

	BOOL bRet = FALSE;
	//�����ַ���
	CString strCode = GetStrFormJson(strUrlInfo, _T("code"), _T(""), 1);

	if (_T("500") == strCode)	//δ��ͨ����Ա
	{
		g_nVIPType = 0;
		g_bIsVip = FALSE;
		bRet = FALSE;
	}
	else	//��ͨ����Ա
	{
		//����Ա����
		CString strCipType = GetStrFormJson(strUrlInfo, _T("data"), _T("viptype"), 1);
		if (_T("2") == strCipType)	//�����û�
		{
			g_tagVipInfo.strType = _T("2");
			g_tagVipInfo.strPayDate = GetStrFormJson(strUrlInfo, _T("pay_date"), _T(""), 0);
			g_tagVipInfo.strEndDate = GetStrFormJson(strUrlInfo, _T("end_date"), _T(""), 0);
			g_tagVipInfo.strNowDate = GetStrFormJson(strUrlInfo, _T("now_date"), _T(""), 0);

			g_nVIPType = 2;
			g_bIsVip = TRUE;
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
				bRet = FALSE;
			}
			else
			{
				g_bIsVip = TRUE;
				bRet = TRUE;
			}
			g_tagVipInfo.strType = _T("1");
			g_tagVipInfo.strPayDate = GetStrFormJson(strUrlInfo, _T("pay_date"), _T(""), 0);
			g_tagVipInfo.strEndDate = strEndDate;
			g_tagVipInfo.strNowDate = strNowDate;
		}
	}

	if (g_bIsVip)
	{
		if (_T("2") == g_tagVipInfo.strType)
		{
			g_MainWnd->m_lpVipBtn->SetText(_T("����VIP"));
		}
		else
		{
			if (g_tagVipInfo.strNowDate > g_tagVipInfo.strEndDate)
			{
				g_MainWnd->m_lpVipBtn->SetText(_T("����VIP"));
			}
			else
			{
				g_MainWnd->m_lpVipBtn->SetText(_T("���VIP"));
			}
		}
	}

	WritePrivateProfileString(_T("Info"), _T("YUserMcode"), g_strMachineCode.GetBuffer(0), g_strMainIni.GetBuffer(0));	//��¼������
	WritePrivateProfileString(_T("Info"), _T("Ycheckurl"), strUrl.GetBuffer(0), g_strMainIni.GetBuffer(0));	//��¼����
	WritePrivateProfileString(_T("Info"), _T("Ycheckinfo"), strUrlInfo.GetBuffer(0), g_strMainIni.GetBuffer(0));	//��¼��ȡ����Ϣ
	WritePrivateProfileString(_T("Info"), _T("YstrCode"), strCode.GetBuffer(0), g_strMainIni.GetBuffer(0));	//��¼��ȡ����Ϣ

	return bRet;
}

//����Ƿ񸶿�ɹ�
unsigned __stdcall CheckPayOkFunc(LPVOID pM)
{
// 	CVIPWizardWnd *lpWnd = (CVIPWizardWnd*) pM;

	while(1)
	{
		Sleep(20 * 1000);
		BOOL bRet = CheckPayOk();

		if (bRet)
		{
			break;
		}
	}

	_endthreadex(0);
	return 0;
}

//֧������
void PayForVip(int nPayType)
{
	CString strVipType;
	strVipType.Format(_T("%d"), nPayType);

	SYSTEMTIME st;
	GetSystemTime(&st);
	CTime tUTC(st);
	CString strTime;
	strTime.Format(_T("%d"), tUTC.GetTime());

	CString strSign;
	strSign.Format(_T("%s|%s|quxiupdf2word"), g_strMachineCode, strTime);
	CString strSignMd5 = MakeCStringMD5W(strSign);

	CString strUrl = _T("http://www.pdf00.com/pay?viptype=[viptype]&mn=[mn]&time=[time]&sign=[sign]");
	strUrl.Replace(_T("[viptype]"), strVipType);
	strUrl.Replace(_T("[mn]"), g_strMachineCode);
	strUrl.Replace(_T("[time]"), strTime);
	strUrl.Replace(_T("[sign]"), strSignMd5);

	ShellExecute(NULL, _T("open"), strUrl.GetBuffer(0), NULL, NULL, SW_SHOWNORMAL);

	//�����̼߳��
	_beginthreadex(NULL, 0, CheckPayOkFunc, NULL, 0, NULL);

}
void CVIPWizardWnd::Notify(TNotifyUI& msg)
{
	if(msg.sType == _T("click")) 
	{
		CString strName = msg.pSender->GetName();
		if (_T("btnClose") == strName)		//ֻת5ҳ
		{
			g_bOnlyFilePages = TRUE;
			Close(IDOK);
		}
		else if (_T("btnPay") == strName)	//֧��
		{
			m_lpTab->SelectItem(1);
			m_lpPay->SetVisible(FALSE);
			m_lpPayOK->SetVisible(TRUE);
			m_lpCloseBtn->SetVisible(FALSE);
			m_lpExitBtn->SetVisible(TRUE);

			//
			PayForVip(m_nPayType);
		}
		else if (_T("btnPayOK") == strName)	//֧���ɹ�
		{
			g_bOnlyFilePages = FALSE;
			// �����Ƿ�֧���ɹ�
			CheckPayOk();

			Close(IDCANCEL);
		}
		else if (_T("btnQQ") == strName)	//��ѯQQ�ͷ�
		{
			SayHelloOnLineByQQ();
		}
		else if (_T("btnExit2") == strName || _T("btnExit") == strName)	//�ر�
		{
			Close(IDCANCEL);
		}
	}
	else if (msg.sType == _T("selectchanged"))
	{
		if (m_lpOneYearOpt->IsSelected())
		{
			m_lpMat->SetText(_T("39Ԫ"));
			m_nPayType = 1;
		}

		if (m_lpMoreYearOpt->IsSelected())
		{
			m_lpMat->SetText(_T("49Ԫ"));
			m_nPayType = 2;
		}
	}

	__super::Notify(msg);
};
