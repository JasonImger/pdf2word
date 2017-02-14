#pragma once

// �û�����������
class CFeedBackWnd: public WindowImplBase
{
public:
	explicit CFeedBackWnd();

protected:
	virtual ~CFeedBackWnd();   // ˽�л����������������˶���ֻ��ͨ��new�����ɣ�������ֱ�Ӷ���������ͱ�֤��delete this�������

public:
	virtual LPCTSTR    GetWindowClassName() const   {   return _T("FeedBack_wnd");  }
	virtual CDuiString GetSkinFile()                {   return _T("dlg_feedback.xml");  }
	virtual CDuiString GetSkinFolder()              {   return _T("xx?");  }	

	void Notify(TNotifyUI& msg);
	void Init();
	virtual void    OnFinalMessage(HWND hWnd);
	// 	virtual LRESULT HandleMessage (UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	CRichEditUI *m_edtFeedback;
	CEditUI *m_edtContact;
};
