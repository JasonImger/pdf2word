#pragma once

// �������봰����
class CInputPWWnd: public WindowImplBase
{
public:
	explicit CInputPWWnd();

protected:
	virtual ~CInputPWWnd();   // ˽�л����������������˶���ֻ��ͨ��new�����ɣ�������ֱ�Ӷ���������ͱ�֤��delete this�������

public:
	virtual LPCTSTR    GetWindowClassName() const   {   return _T("InputPW_wnd");  }
	virtual CDuiString GetSkinFile()                {   return _T("dlg_inputpassword.xml");  }
	virtual CDuiString GetSkinFolder()              {   return _T("xx?");  }	

	void Notify(TNotifyUI& msg);
	void Init(int nIndex);
	virtual void    OnFinalMessage(HWND hWnd);
	// 	virtual LRESULT HandleMessage (UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	CTextUI *m_lpfilename;
	CTextUI *m_lptitle3;
	CEditUI *m_lppassword;
	int m_nIndex;
};
