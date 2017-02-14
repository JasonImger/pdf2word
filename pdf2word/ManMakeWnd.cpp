#include "stdafx.h"
#include "ManMakeWnd.h"
#include "MsgWnd.h"
#include "HttpUploadFiles.h"

#import "progid:EasyConverter.PDF2Word.5"

//���������ַ�Ƿ�Ϸ�
BOOL IsValidEmail(TCHAR *lpszEmail)
{
	if(lpszEmail == NULL)
	{
		return FALSE;
	}

	int iAtPos = 0;
	int iLastDotPos = 0;
	int i = 0;
	int iAtTimes = 0;

	while(*(lpszEmail + i) != _T('\0'))
	{
		TCHAR ch = *(lpszEmail + i);
		if(!_istprint(ch) || _istspace(ch))  //�ո�Ϳ����ַ��ǷǷ��ģ����Ƶû��ȽϿ���
		{
			iAtTimes = 0;
			break;
		}
		if(ch == _T('@'))
		{
			iAtPos = i;
			iAtTimes++;
		}
		else if(ch == _T('.'))
		{
			iLastDotPos = i;
		}
		i++;
	}

	if(i > 64 || iAtPos < 1 || (iLastDotPos - 2) < iAtPos ||
		(i - iLastDotPos) < 3 || (i - iLastDotPos) > 5 || iAtTimes > 1 || iAtTimes == 0) //��@�Լ���������λ�����жϣ����Ƴ���Ϊ64
	{
		return FALSE;
	}
	return TRUE;
}

CManMakeWnd::CManMakeWnd() 
{
	m_strFileMd5 = _T("");
	m_nFilePages = 0;
	m_strFileName = _T("");
	m_strSign = _T("");
}

CManMakeWnd::~CManMakeWnd()
{
}

void CManMakeWnd::Init()
{
	Create(g_hMainWnd, _T(""), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
	CenterWindow();

	m_lpStepTab = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("tabStep")));
	m_lpmoney = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("money")));
	m_lpmoney->SetText(_T("�������ϴ�Ҫ�˹�ת����PDF�ĵ�"));
	m_lpprogress = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("progress")));
	m_lpbtnUpload = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnUpload")));
	m_lpbtnOK = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnOK")));
	m_lpbtnPayOK = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnPayOK")));

	m_lpEmailEdt = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("contact")));
	m_lpMobileEdt = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("mobile")));
	m_lpMachineCode = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("MachineCode")));

	TCHAR szBuf[1024] = _T("");
	GetPrivateProfileString(_T("INFO"), _T("UserEmail"), _T(""), szBuf, 1024, g_strMainIni.GetBuffer(0));
	m_lpEmailEdt->SetText(szBuf);
	GetPrivateProfileString(_T("INFO"), _T("UserPhone"), _T(""), szBuf, 1024, g_strMainIni.GetBuffer(0));
	m_lpMobileEdt->SetText(szBuf);

	m_lpMachineCode->SetText(g_strMachineCode.GetBuffer(0));
}

void CManMakeWnd::OnFinalMessage( HWND /*hWnd*/ )
{
	delete this;
}

// LRESULT CManMakeWnd::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
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

//��ȡPDF�ļ�ҳ�� ����-1��ʾ������
int GetPdfFilePages(CString strFilePath)
{
	int nPages = 0;
	//��ȡ�ļ�
	::CoInitialize(NULL);
	EasyConverterLib::IPDF2WordPtr pConvObj = NULL;
	try
	{
		pConvObj.CreateInstance("EasyConverter.PDF2Word.5"); 
		pConvObj->LicenseKey = GetBCLKey().GetBuffer(0);	//��ʽ��OEM license key 20161221 14����

		nPages = pConvObj->GetNumberOfPages(strFilePath.GetBuffer(0));	
	}
	catch(_com_error &e) //��ʾ��������ļ���
	{
		//�˴�ȫ������������
		{
			nPages = -1;
		}
	}

	pConvObj = NULL; 
	::CoUninitialize();

	return nPages;
}

//�����ļ��շ�
int GetPDFPrice(CString strFilePath)
{
	int nPages = 0;
	int nPrice = 0;

	nPages = GetPdfFilePages(strFilePath);

	if (nPages < 0)
	{
		return nPrice;
	}

	if (nPages > 0 && nPages <= 100)
	{
		nPrice = 10;
	}
	else if (nPages > 100 && nPages <= 200)
	{
		nPrice = 20;
	}
	else if (nPages > 200 && nPages <= 300)
	{
		nPrice = 30;
	}
	else
	{
		nPrice = 50;
	}

	return nPrice;
}

//�ļ��ϴ��ɹ�����д���ݿ�
BOOL UpdataWhenUpFileSuccess(CString strFileMd5, int nPages, CString strFileDownLoadUrl)
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	CTime tUTC(st);
	CString strTime;
	strTime.Format(_T("%d"), tUTC.GetTime());

	CString strSign;
	strSign.Format(_T("%s|%d|%s|quxiupdf2word"), strFileMd5, nPages, strTime);
	CString strSignMd5 = MakeCStringMD5W(strSign);

	CString strPage;
	strPage.Format(_T("%d"), nPages);
	CString strUrl = _T("http://www.pdf00.com/files/add?filemd5=[filemd5]&page=[page]&url=[url]&time=[time]&sign=[sign]");
	strUrl.Replace(_T("[filemd5]"), strFileMd5);
	strUrl.Replace(_T("[page]"), strPage);
	strUrl.Replace(_T("[url]"), strFileDownLoadUrl);
	strUrl.Replace(_T("[time]"), strTime);
	strUrl.Replace(_T("[sign]"), strSignMd5);

// 	WritePrivateProfileString(_T("Info"), _T("Updata"), strUrl.GetBuffer(0), _T("C:\\1.ini")); //xx? ����
	CString strUrlInfo = GetHtmlStr(strUrl);
	if (strUrlInfo.IsEmpty())
	{
		return FALSE;
	}

	//��ȡ����ֵ
	CString strCode = GetStrFormJson(strUrlInfo, _T("code"), _T(""), 1);
	if (_T("200") == strCode)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

//�ϴ��ļ��߳�
unsigned __stdcall UploadFile2SvrFunc(LPVOID pM)
{
	CManMakeWnd *lpWnd = (CManMakeWnd*)pM;

	CString strFile = lpWnd->m_strFilePath;
	CString strFileMd5 = MakeFileMD5W(strFile);

	lpWnd->m_lpbtnOK->SetEnabled(FALSE);
	lpWnd->m_lpbtnUpload->SetEnabled(FALSE);

	//��ȡ�ļ���
	CString strFileName;
	strFileName = strFile.Mid(strFile.ReverseFind(_T('\\')) + 1);
	lpWnd->m_lpbtnUpload->SetText(strFileName.GetBuffer(0));
	lpWnd->m_strFileName = strFileName;

	SYSTEMTIME st;
	GetSystemTime(&st);
	CTime tUTC(st);
	CString strTime;
	strTime.Format(_T("%d"), tUTC.GetTime());

	CString strMd5;
	strMd5.Format(_T("%s|%s|quxiupdf2word"), strFileMd5, strTime);
	CString md5 = MakeCStringMD5W(strMd5);

	CString strUrl = _T("http://upload.pdf00.com/php/controller.php?action=uploadfile&encode=utf-8&filemd5=[filemd5]&time=[time]&sign=[md5]");
	strUrl.Replace(_T("[filemd5]"), strFileMd5);
	strUrl.Replace(_T("[time]"), strTime);
	strUrl.Replace(_T("[md5]"), md5);

	lpWnd->m_lpprogress->SetText(_T("�����ϴ�PDF (0%%)"));
	CHttpUploadFiles httpUpFile;
	httpUpFile.SetUserData(strFile, (void*)lpWnd);

	VecStParam vecParam;

	CString strKey = _T("upfile");
	BOOL bUpFile = httpUpFile.TransDataToServer(strUrl.GetBuffer(0),
							vecParam, strFile.GetBuffer(0), strKey.GetBuffer(0));

	CString strReceive;
	BYTE szBuf[1024] = {0};
	DWORD dwReceive = 1024;
	httpUpFile.ReceiveData(szBuf, dwReceive);
	char *lp = UTF_8ToGB2312((char*)szBuf, dwReceive);
	CStringA strReceiveA = lp;
	delete []lp;
	CString strReceiveW = CStrA2CStrW(strReceiveA);

	//�ֽ�����
	CString strState = GetStrFormJson(strReceiveW, _T("state"), _T(""), 0);
	CString strDownUrl = GetStrFormJson(strReceiveW, _T("url"), _T(""), 0);

	if (bUpFile && (_T("SUCCESS") == strState))
	{
		CString strMsg;
		strMsg.Format(_T("�ϴ���� (100%%)"));
		lpWnd->m_lpprogress->SetText(strMsg.GetBuffer(0));
		lpWnd->m_lpbtnOK->SetEnabled(TRUE);

		//�����ļ��Ľ��
		int nPrice = GetPDFPrice(strFile);
		if (nPrice <= 0)
		{
			lpWnd->m_lpbtnOK->SetEnabled(TRUE);
			lpWnd->m_lpbtnUpload->SetEnabled(TRUE);
			lpWnd->m_lpbtnUpload->SetText(_T("�ϴ�Ҫת�����ļ�"));
			lpWnd->m_lpprogress->SetText(_T("�����򳬹�20M��PDF������ͷ���ϵ"));
		}
		CString strPrice;
		strPrice.Format(_T("%dԪ"), nPrice);
		lpWnd->m_lpmoney->SetText(strPrice.GetBuffer(0));

		//�ϴ��ɹ����������ݿ�
		int nPages = GetPdfFilePages(strFile.GetBuffer(0));

		if (!UpdataWhenUpFileSuccess(strFileMd5, nPages, strDownUrl))
		{
			//д�����ݿ�ʧ��
			CMsgWnd *lpMsg = new CMsgWnd;
			lpMsg->Init(lpWnd->GetHWND(), _T("�ļ��ϴ�ʧ�ܣ����ٴ��ϴ��������Ա��ϵ"), -1);
			lpMsg->ShowModal();

			lpWnd->m_lpbtnOK->SetEnabled(TRUE);
			lpWnd->m_lpbtnUpload->SetEnabled(TRUE);
			lpWnd->m_lpbtnUpload->SetText(_T("�ϴ�Ҫת�����ļ�"));
			lpWnd->m_lpprogress->SetText(_T("�����򳬹�20M��PDF������ͷ���ϵ"));
		}
		
	}
	else
	{
		lpWnd->m_lpbtnOK->SetEnabled(TRUE);
		lpWnd->m_lpbtnUpload->SetEnabled(TRUE);
		lpWnd->m_lpbtnUpload->SetText(_T("�ϴ�Ҫת�����ļ�"));
		lpWnd->m_lpprogress->SetText(_T("�����򳬹�20M��PDF������ͷ���ϵ"));
	}

	lpWnd->m_strFileMd5 = strFileMd5;

	_endthreadex(0);
	return 0;
}

//����ļ��ڷ������Ƿ����
BOOL CManMakeWnd::CheckFileInSvr(CString strFile)
{
	CString strfileMd5 = MakeFileMD5W(strFile);

	SYSTEMTIME st;
	GetSystemTime(&st);
	CTime tUTC(st);
	CString strTime;
	strTime.Format(_T("%d"), tUTC.GetTime());

	CString strSign;
	strSign.Format(_T("%s|%s|quxiupdf2word"), strfileMd5, strTime);
	CString strSignMd5 = MakeCStringMD5W(strSign);

	CString strUrl = _T("http://www.pdf00.com/files/check_fileext?filemd5=[filemd5]&time=[time]&sign=[sign]");
	strUrl.Replace(_T("[filemd5]"), strfileMd5);
	strUrl.Replace(_T("[time]"), strTime);
	strUrl.Replace(_T("[sign]"), strSignMd5);

	CString strUrlInfo = GetHtmlStr(strUrl);
	if (strUrlInfo.IsEmpty())
	{
		return FALSE;
	}

	//��ȡ����ֵ
	CString strCode = GetStrFormJson(strUrlInfo, _T("code"), _T(""), 1);
	if (_T("500") == strCode)
	{
		m_strFileMd5 = strfileMd5;
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

void CManMakeWnd::Notify(TNotifyUI& msg)
{
	if(msg.sType == _T("click")) 
	{
		CString strName = msg.pSender->GetName();
		if (_T("btnClose") == strName)
		{
			Close();
		}
		else if (_T("btnQQ") == strName)	//���߿ͷ���ѯ
		{
			SayHelloOnLineByQQ();
		}
		else if (_T("btnNext") == strName)	// ��һ��
		{
			//�л�
			m_lpStepTab->SelectItem(1);

			//����
			msg.pSender->SetVisible(FALSE);

			//��ʾ֧��
			static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnOK")))->SetVisible(TRUE);
		}
		else if (_T("btnOK") == strName)	// ֧��
		{
			//���ϴ��ļ�
			if (m_strFileMd5.IsEmpty())
			{
				ShowMsgTipWnd(this->GetHWND(), _T("�������ϴ�Ҫ�˹�ת����PDF�ĵ�"), -1);
// 				CMsgWnd *lpMsg = new CMsgWnd;
// 				lpMsg->Init(_T("�������ϴ�Ҫ�˹�ת����PDF�ĵ�"), -1);
// 				lpMsg->ShowModal();
				return ;
			}

			//������������ʽ�Ϸ���
			CString strEmail = m_lpEmailEdt->GetText();
			CString strPhone = m_lpMobileEdt->GetText();
			if (strEmail.IsEmpty() || strPhone.IsEmpty())
			{
				ShowMsgTipWnd(this->GetHWND(), _T("������д����������ֻ�"), -1);
// 				CMsgWnd *lpMsg = new CMsgWnd;
// 				lpMsg->Init(_T("������д����������ֻ�"), -1);
// 				lpMsg->ShowModal();
				return ;
			}

			//�������Ϸ���
			if (!IsValidEmail(strEmail.GetBuffer(0)))
			{
				ShowMsgTipWnd(this->GetHWND(), _T("�ܱ�Ǹ�����������ַ����Ч�ģ�����������"), -1);
// 				CMsgWnd *lpMsg = new CMsgWnd;
// 				lpMsg->Init(_T("�ܱ�Ǹ�����������ַ����Ч�ģ�����������"), -1);
// 				lpMsg->ShowModal();
				return ;
			}
			
			//�ֻ�
			if (strPhone.GetLength() < 11)
			{
				ShowMsgTipWnd(this->GetHWND(), _T("�ܱ�Ǹ�������ֻ���������Ч�ģ�����������"), -1);
// 				CMsgWnd *lpMsg = new CMsgWnd;
// 				lpMsg->Init(_T("�ܱ�Ǹ�������ֻ���������Ч�ģ�����������"), -1);
// 				lpMsg->ShowModal();
				return ;
			}

			CString strPayUrl = _T("http://www.pdf00.com/pay/?viptype=3&mn=[mn]&filemd5=[FileMd5]&email=[email]&phone=[phone]&time=[time]&filename=[filename]&sign=[sign]");

			CString strFileName = UrlEncode(m_strFileName);

			SYSTEMTIME st;
			GetSystemTime(&st);
			CTime tUTC(st);
			CString strTime;
			strTime.Format(_T("%d"), tUTC.GetTime());

			CString strSign;
			strSign.Format(_T("%s|%s|quxiupdf2word"), m_strFileMd5, strTime);
			CString strSignMd5 = MakeCStringMD5W(strSign);
			
			strPayUrl.Replace(_T("[mn]"), g_strMachineCode);
			strPayUrl.Replace(_T("[FileMd5]"), m_strFileMd5);
			strPayUrl.Replace(_T("[email]"), strEmail);
			strPayUrl.Replace(_T("[phone]"), strPhone);
			strPayUrl.Replace(_T("[time]"), strTime);
			strPayUrl.Replace(_T("[filename]"), strFileName);
			strPayUrl.Replace(_T("[sign]"), strSignMd5);

// 			WritePrivateProfileString(_T("Info"), m_strFileName.GetBuffer(0), strPayUrl.GetBuffer(0), _T("C:\\1.ini"));
			ShellExecute(NULL, _T("open"), strPayUrl.GetBuffer(0), NULL, NULL, SW_SHOWNORMAL);

			//��¼�ֻ�������
			WritePrivateProfileString(_T("Info"), _T("UserEmail"), strEmail.GetBuffer(0), g_strMainIni.GetBuffer(0));
			WritePrivateProfileString(_T("Info"), _T("UserPhone"), strPhone.GetBuffer(0), g_strMainIni.GetBuffer(0));

			m_lpbtnOK->SetText(_T("����֧��"));
			m_lpbtnPayOK->SetVisible(TRUE);
		}
		else if (_T("btnPayOK") == strName)	//����֧���ɹ�
		{
			//ˢ�¶����б�
			g_MainWnd->RefreshManMakeList();

			Close();
		}
		else if (_T("btnUpload") == strName)	//�ϴ��ļ�
		{
			// ���ļ��򿪶Ի������ѡ���ļ�����NewGame  
			OPENFILENAME ofn;      // �����Ի���ṹ��  
			TCHAR szFile[MAX_PATH]; // �����ȡ�ļ����ƵĻ�������            

			// ��ʼ��ѡ���ļ��Ի���  
			ZeroMemory(&ofn, sizeof(ofn));  
			ofn.lStructSize = sizeof(ofn);  
			ofn.hwndOwner = this->GetHWND();  
			ofn.lpstrFile = szFile;  
			ofn.lpstrTitle = _T("��ѡ��Ҫת����PDF�ļ���");
			//  
			//  
			ofn.lpstrFile[0] = _T('\0');  
			ofn.nMaxFile = sizeof(szFile);  
			ofn.lpstrFilter = _T("PDF�ļ�(*.pdf)\0*.pdf");  
			ofn.nFilterIndex = 1;  
			ofn.lpstrFileTitle = NULL;  
			ofn.nMaxFileTitle = 0;  
// 			ofn.lpstrInitialDir = _T("Record//");  
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;  

			// ��ʾ��ѡ���ļ��Ի���  
			if (GetOpenFileName(&ofn))  
			{  
				CString strFile = szFile;
				m_strFilePath = strFile;

				//�ж��ļ���û�г���20M
				CString strFileSize = M_GetFileSize(strFile.GetBuffer(0));
				float fFileSize = _tstof(strFileSize.GetBuffer(0));
				if (fFileSize >= 20.0)
				{
					//��ʾ����20M
					ShowMsgTipWnd(this->GetHWND(), _T("���ã�����20M��PDF�ļ��޷�ֱ���ϴ�������ϵ���߿ͷ�����"), -1);
// 					CMsgWnd *lpMsg = new CMsgWnd;
// 					lpMsg->Init(this->GetHWND(), _T("���ã�����20M��PDF�ļ��޷�ֱ���ϴ�������ϵ���߿ͷ�����"), -1);
// 					lpMsg->ShowModal();
					return ;
				}

				//�ж��ļ���û������
				int nPages = GetPdfFilePages(strFile.GetBuffer(0));
				if (nPages < 0)
				{
					//������
					ShowMsgTipWnd(this->GetHWND(), _T("�ܱ�Ǹ����PDF�Ǽ��ܱ����ļ����޷�ֱ���˹�ת����������ѯ�ͷ������ύ�˹��ƽ����"), -1);
// 					CMsgWnd *lpMsg = new CMsgWnd;
// 					lpMsg->Init(this->GetHWND(), _T("�ܱ�Ǹ����PDF�Ǽ��ܱ����ļ����޷�ֱ���˹�ת����������ѯ�ͷ������ύ�˹��ƽ����"), -1);
// 					lpMsg->ShowModal();
					return ;
				}
				m_nFilePages = nPages;

				//����ļ��Ƿ���ڷ�����
				if (!CheckFileInSvr(strFile))
				{
					//�ϴ��ļ�
					_beginthreadex(NULL, 0, UploadFile2SvrFunc, (LPVOID)this, 0, NULL);
				}
				else
				{
					//�����������ļ�
					CString strFileName;
					strFileName = strFile.Mid(strFile.ReverseFind(_T('\\')) + 1);
					m_lpbtnUpload->SetText(strFileName.GetBuffer(0));
					m_lpbtnUpload->SetEnabled(FALSE);
					m_strFileName = strFileName;

					CString strMsg;
					strMsg.Format(_T("�ϴ���� (100%%)"));
					m_lpprogress->SetText(strMsg.GetBuffer(0));
					m_lpbtnOK->SetEnabled(TRUE);

					//�����ļ��Ľ��
					int nPrice = GetPDFPrice(strFile);
					if (nPrice <= 0)
					{
						m_lpbtnOK->SetEnabled(TRUE);
						m_lpbtnUpload->SetEnabled(TRUE);
						m_lpbtnUpload->SetText(_T("�ϴ�Ҫת�����ļ�"));
						m_lpprogress->SetText(_T("�����򳬹�20M��PDF������ͷ���ϵ"));
					}
					CString strPrice;
					strPrice.Format(_T("%dԪ"), nPrice);
					m_lpmoney->SetText(strPrice.GetBuffer(0));

				}

			} 

		}
		
	}

	__super::Notify(msg);
};
