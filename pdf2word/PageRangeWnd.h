#pragma once

// ����ת��ҳ��������
class CPageRangeWnd: public WindowImplBase
{
public:
	explicit CPageRangeWnd();

protected:
	virtual ~CPageRangeWnd();   // ˽�л����������������˶���ֻ��ͨ��new�����ɣ�������ֱ�Ӷ���������ͱ�֤��delete this�������

public:
	virtual LPCTSTR    GetWindowClassName() const   {   return _T("PageRange_wnd");  }
	virtual CDuiString GetSkinFile()                {   return _T("dlg_pagerange.xml");  }
	virtual CDuiString GetSkinFolder()              {   return _T("xx?");  }	

	void Notify(TNotifyUI& msg);
	void Init(int nFilePages);
	virtual void    OnFinalMessage(HWND hWnd);
	// 	virtual LRESULT HandleMessage (UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	int m_nFilePages;
	CEditUI *m_lpMinEdit;
	CEditUI *m_lpMaxEdit;
};
