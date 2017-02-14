#pragma once

// �˹�ת������ ��
class CManMakeWnd: public WindowImplBase
{
public:
	explicit CManMakeWnd();

protected:
	virtual ~CManMakeWnd();   // ˽�л����������������˶���ֻ��ͨ��new�����ɣ�������ֱ�Ӷ���������ͱ�֤��delete this�������

public:
	virtual LPCTSTR    GetWindowClassName() const   {   return _T("Man_Make_wnd");  }
	virtual CDuiString GetSkinFile()                {   return _T("dlg_manmake.xml");  }
	virtual CDuiString GetSkinFolder()              {   return _T("xx?");  }	

	void Notify(TNotifyUI& msg);
	void Init();
	virtual void    OnFinalMessage(HWND hWnd);
	// 	virtual LRESULT HandleMessage (UINT uMsg, WPARAM wParam, LPARAM lParam);

	//����ļ��ڷ������Ƿ����
	BOOL CheckFileInSvr(CString strFile);

public:
	CString m_strFilePath;
	CTabLayoutUI *m_lpStepTab;
	CTextUI *m_lpmoney;
	CTextUI *m_lpprogress;
	CButtonUI *m_lpbtnUpload;
	CButtonUI *m_lpbtnOK;
	CButtonUI *m_lpbtnPayOK;
	CEditUI *m_lpEmailEdt;
	CEditUI *m_lpMobileEdt;
	CEditUI *m_lpMachineCode;

	CString m_strFileMd5;
	int m_nFilePages;
	CString m_strFileName;
	CString m_strSign;
};
