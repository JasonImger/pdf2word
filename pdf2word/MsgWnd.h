#pragma once

// ������Ϣ��ʾ��
class CMsgWnd: public WindowImplBase
{
public:
	int m_nType;

	explicit CMsgWnd();

protected:
	virtual ~CMsgWnd();   // ˽�л����������������˶���ֻ��ͨ��new�����ɣ�������ֱ�Ӷ���������ͱ�֤��delete this�������

public:
	virtual LPCTSTR    GetWindowClassName() const   {   return _T("Update_Msg_wnd");  }
	virtual CDuiString GetSkinFile()                {   return _T("dlg_message.xml");  }
	virtual CDuiString GetSkinFolder()              {   return _T("xx?");  }	

	void Notify(TNotifyUI& msg);
	void Init(HWND hParent, CString strMsg, int nTag);
	virtual void    OnFinalMessage(HWND hWnd);
// 	virtual LRESULT HandleMessage (UINT uMsg, WPARAM wParam, LPARAM lParam);
};
