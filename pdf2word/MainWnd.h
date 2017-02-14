#pragma once

#include "PDF2WordWrapper.h"


// ��������
class CMyWnd : public WindowImplBase
{
public:
	CListUI *m_lpList;
	CListUI *m_lpList_man;
	CContainerUI *m_lpTopLeft;
	CButtonUI *m_lpDragFile;
	CVerticalLayoutUI *m_lpListBottom;
	COptionUI *m_lpSelAllOpt;
// 	COptionUI *m_lpSelAllOpt1;
	CButtonUI *m_lpbtnDelete;
	COptionUI *m_lpSavePathOpt;
	COptionUI *m_lpSelSavePathOpt;
	CEditUI *m_lpSavePathEdt;
	CButtonUI *m_lpOpenPathBtn;
	CComboUI *m_lpOutDocForma;
	CButtonUI *m_lpVipBtn;
	CButtonUI *m_lpbtnConvert;
	CLabelUI *m_lpFileState;
	CButtonUI *m_lpbtnOpenFile;
	CButtonUI *m_lpbtnOpenFolder;
	CTabLayoutUI *m_lpTabLayout;

	CWebBrowserUI *m_lpWebTj;

	int m_nPageNumber;
	int m_nPageCount;

public:
	virtual LPCTSTR    GetWindowClassName() const   {   return _T("My_Wnd_Frame");  }
	virtual CDuiString GetSkinFile()                {   return _T("main.xml");  }
	virtual CDuiString GetSkinFolder()              {   return _T("xx?");  }	

	virtual void InitWindow();

	//��ȡ������Ӧvector �е�����
	int GetRowInVectorIndex(int nRow);

	//�ж����Ƿ���ѡ�е�
	BOOL ChkRowIsSelect(int nRow);

	//ɾ���б��ˢ��
	void RefreshListWhenDel();

	//�����ļ���ˢ��Ҫ������ļ��б�
	void RefreshFileList();
	void RefreshFileListWhenAddFiles();

	//ˢ���˹�ת�������б�
	void RefreshManMakeList();

	virtual void Notify(TNotifyUI& msg);

	//��ѡ��pdf�ļ��Ի���
	void OpenAddPdfFileDlg();

	//����ȫ·�������ļ���Ϣ��vec
	void AddPdfFile2Vec(CString strFilePath);

	//�϶��ļ����Ի���
	void DropFileOnDlg(HDROP hDrop);

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);


};

unsigned __stdcall ProgressFunc(LPVOID pM)
{
	CMyWnd *lpWnd = (CMyWnd*) pM;

// 	CListContainerElementUI *lpRow = (CListContainerElementUI*)lpWnd->m_lpList->GetItemAt(g_nNowRow);
// 	CProgressUI *lpProgress = (CProgressUI*)lpRow->FindSubControl(_T("progress"));
// 	lpProgress->SetMinValue(1);
// 	lpProgress->SetMinValue(lpWnd->m_nPageCount);
// 	lpProgress->SetValue(lpWnd->m_nPageNumber + 1);

	_endthreadex(0);
	return 0;
}

extern CMyWnd *g_MainWnd;
extern int g_nNowRow;	//��ǰ����ת������


class CIPdf2WordEvent : public CPDF2WordWrapper
{
public:
	EasyConverterLib::cnvResponse OnPageStart(long nPageNumber, long nPageCount, BSTR bstrFileName)
	{

		CListContainerElementUI *lpRow = (CListContainerElementUI*)g_MainWnd->m_lpList->GetItemAt(g_nNowRow);
		CProgressUI *lpProgress = (CProgressUI*)lpRow->FindSubControl(_T("progress"));
		lpProgress->SetValue(nPageNumber + 1);

		if (0)
			return EasyConverterLib::CNV_CANCEL;
		else
			return EasyConverterLib::CNV_CONTINUE;
	}

	void OnConversionFinished(long nResult)
	{
		CListContainerElementUI *lpRow = (CListContainerElementUI*)g_MainWnd->m_lpList->GetItemAt(g_nNowRow);
		CProgressUI *lpProgress = (CProgressUI*)lpRow->FindSubControl(_T("progress"));
		lpProgress->SetVisible(FALSE);

		CButtonUI *lpbtnOpenFile = (CButtonUI*)lpRow->FindSubControl(_T("btnOpenFile"));
		CButtonUI *lpbtnOpenFolder = (CButtonUI*)lpRow->FindSubControl(_T("btnOpenFolder"));
		lpbtnOpenFile->SetVisible(TRUE);
		lpbtnOpenFolder->SetVisible(TRUE);

		//����ɾ����ť�� combo
		CComboUI *lpCombo = (CComboUI*)lpRow->FindSubControl(_T("comboPageRange"));
		lpCombo->SetEnabled(TRUE);
		CButtonUI *lpDelBtn = (CButtonUI*)lpRow->FindSubControl(_T("btnDeleteFile"));
		lpDelBtn->SetEnabled(TRUE);

		if (nResult)
		{
		}

	}

};

class CBindCallback : public IBindStatusCallback    
{  
public:  
	CBindCallback();  
	virtual ~CBindCallback();  

	//������ʾ���ȴ��ڵľ��  
	CListContainerElementUI* m_lpRow;  

	//IBindStatusCallback�ķ���������OnProgress     �����������������E_NOTIMPL   

	STDMETHOD(OnStartBinding)  
		( DWORD dwReserved,  
		IBinding __RPC_FAR *pib)  
	{ return E_NOTIMPL; }  

	STDMETHOD(GetPriority)  
		( LONG __RPC_FAR *pnPriority)  
	{ return E_NOTIMPL; }  

	STDMETHOD(OnLowResource)  
		( DWORD reserved)  
	{ return E_NOTIMPL; }  

	//OnProgress������  
	STDMETHOD(OnProgress)  
		( ULONG ulProgress,      
		ULONG ulProgressMax,  
		ULONG ulStatusCode,  
		LPCWSTR wszStatusText);  

	STDMETHOD(OnStopBinding)  
		( HRESULT hresult,  
		LPCWSTR szError)  
	{ return E_NOTIMPL; }  

	STDMETHOD(GetBindInfo)  
		( DWORD __RPC_FAR *grfBINDF,  
		BINDINFO __RPC_FAR *pbindinfo)  
	{ return E_NOTIMPL; }  

	STDMETHOD(OnDataAvailable)  
		( DWORD grfBSCF,  
		DWORD dwSize,  
		FORMATETC __RPC_FAR *pformatetc,  
		STGMEDIUM __RPC_FAR *pstgmed)  
	{ return E_NOTIMPL; }  

	STDMETHOD(OnObjectAvailable)  
		( REFIID riid,  
		IUnknown __RPC_FAR *punk)  
	{ return E_NOTIMPL; }  

	// IUnknown����.IE ���������Щ������  

	STDMETHOD_(ULONG,AddRef)()  
	{ return 0; }  

	STDMETHOD_(ULONG,Release)()  
	{ return 0; }  

	STDMETHOD(QueryInterface)  
		( REFIID riid,  
		void __RPC_FAR *__RPC_FAR *ppvObject)  
	{ return E_NOTIMPL; }  
};  
