#pragma once

// �������Ǵ�����
class CAboutWnd: public WindowImplBase
{
public:
	explicit CAboutWnd();

protected:
	virtual ~CAboutWnd();   // ˽�л����������������˶���ֻ��ͨ��new�����ɣ�������ֱ�Ӷ���������ͱ�֤��delete this�������

public:
	virtual LPCTSTR    GetWindowClassName() const   {   return _T("About_wnd");  }
	virtual CDuiString GetSkinFile()                {   return _T("dlg_about.xml");  }
	virtual CDuiString GetSkinFolder()              {   return _T("xx?");  }	

	void Notify(TNotifyUI& msg);
	void Init();
	virtual void    OnFinalMessage(HWND hWnd);
	// 	virtual LRESULT HandleMessage (UINT uMsg, WPARAM wParam, LPARAM lParam);

};
