#include "stdafx.h"
#include "SetWnd.h"



CSetWnd::CSetWnd() 
{
}

CSetWnd::~CSetWnd()
{
}

void CSetWnd::SetConnectSetting()
{
	switch (g_tagConnectSetting.nConversionMethod)  //��������
	{
	case 1:
		m_lpCNV_USE_SAT->Selected(TRUE);
		break;
	case 2:
		m_lpCNV_TEXTBOX->Selected(TRUE);
		break;
	case 3:
		m_lpCNV_TEXTFLOW->Selected(TRUE);
		break;
	default:
		m_lpCNV_Default->Selected(TRUE);
		break;
	}

	if (g_tagConnectSetting.bAdjustFontNames)	//У����������
	{
		m_lpAdjustFontNames->Selected(TRUE);
	}
	else
	{
		m_lpAdjustFontNames->Selected(FALSE);
	}

	if (g_tagConnectSetting.bAdjustSpacing)		//У�����
	{
		m_lpAdjustSpacing->Selected(TRUE);
	}
	else
	{
		m_lpAdjustSpacing->Selected(FALSE);
	}

	if (g_tagConnectSetting.bExtractImages)	 //����ͼƬ
	{
		m_lpExtractImages->Selected(TRUE);
	}
	else
	{
		m_lpExtractImages->Selected(FALSE);
	}

	if (g_tagConnectSetting.bExtractText)  //��������
	{
		m_lpExtractText->Selected(TRUE);
	}
	else
	{
		m_lpExtractText->Selected(FALSE);
	}

	if (g_tagConnectSetting.bShrinkFonts)		//��С����
	{
		m_lpShrinkFonts->Selected(TRUE);
	}
	else
	{
		m_lpShrinkFonts->Selected(FALSE);
	}

	if (g_tagConnectSetting.bDetectHeader)		//���ͷ��
	{
		m_lpDetectHeader->Selected(TRUE);
	}
	else
	{
		m_lpDetectHeader->Selected(FALSE);
	}

	if (g_tagConnectSetting.bSaveHiddenText)		//���������ı�
	{
		m_lpRemoveHiddenText->Selected(TRUE);
	}
	else
	{
		m_lpRemoveHiddenText->Selected(TRUE);
	}


	switch (g_tagConnectSetting.nDocumentType)	//�ĵ����ͣ� ���� ���� ����Ӧ
	{
	case 1:
		m_lpMutileColumn->Selected(TRUE);
		break;
	case 2:
		m_lpPleading->Selected(TRUE);
		break;
	default:
		m_lpSingleColumn->Selected(TRUE);
		break;
	}


	switch (g_tagConnectSetting.nPutoutExt)		//�����ʽ: RTF�ļ� DOC�ļ� DOCX�ļ�
	{
	case 1:
		m_lpFormat_DOC_Native->Selected(TRUE);
		break;
	case 2:
		m_lpFormat_DOCX->Selected(TRUE);
		break;
	default :
		m_lpFormat_RTF->Selected(TRUE);
		break;
	}


	switch (g_tagConnectSetting.nBkmkOpt)	//�����ǩ����ת����ǩ ÿҳ��ͷ������ǩ ��PDFһ��
	{
	case 1:
		m_lpBookMark_PAGESTART->Selected(TRUE);
		break;
	case 2:
		m_lpBookMark_PDF->Selected(TRUE);
		break;
	default :
		m_lpBookMark_None->Selected(TRUE);
		break;
	}

	switch (g_tagConnectSetting.nOcrOpt)		//OCR�ĵ�������ͼƬ������ ���������� ������ͼƬ
	{
	case 1:
		m_lpOCR_TEXT->Selected(TRUE);
		break;
	case 2:
		m_lpOCR_IMAGE->Selected(TRUE);
		break;
	default :
		m_lpOCR_TEXTIMAGE->Selected(TRUE);
		break;
	}

	CString strTimeOut;
	strTimeOut.Format(_T("%d"), g_tagConnectSetting.nTimeout);
	m_lpConvertTimeout->SetText(strTimeOut.GetBuffer(0));			//�ȴ�ת���ʱ�� ����

	if (g_tagConnectSetting.bOpenPath)	//ת���ɹ����Զ���Word���ڵ�Ŀ¼
	{
		m_lpAutoOpenFolderFinish->Selected(TRUE);
	}
	else
	{
		m_lpAutoOpenFolderFinish->Selected(FALSE);
	}
}

void CSetWnd::SaveSettingData()
{
	if (m_lpCNV_USE_SAT->IsSelected())		//��������
	{
		g_tagConnectSetting.nConversionMethod = 1;
	}

	if (m_lpCNV_TEXTBOX->IsSelected())
	{
		g_tagConnectSetting.nConversionMethod = 2;
	}

	if (m_lpCNV_TEXTFLOW->IsSelected())
	{
		g_tagConnectSetting.nConversionMethod = 3;
	}

	if (m_lpCNV_Default->IsSelected())
	{
		g_tagConnectSetting.nConversionMethod = 0;
	}

	g_tagConnectSetting.bAdjustFontNames = m_lpAdjustFontNames->IsSelected();	 //У����������

	g_tagConnectSetting.bAdjustSpacing = m_lpAdjustSpacing->IsSelected();	//У�����

	g_tagConnectSetting.bExtractImages = m_lpExtractImages->IsSelected(); //����ͼƬ

	g_tagConnectSetting.bExtractText = m_lpExtractText->IsSelected(); //��������

	g_tagConnectSetting.bShrinkFonts = m_lpShrinkFonts->IsSelected();	//��С����

	g_tagConnectSetting.bDetectHeader = m_lpDetectHeader->IsSelected();	//���ͷ��

	g_tagConnectSetting.bSaveHiddenText = m_lpRemoveHiddenText->IsSelected();	//���������ı�

	if (m_lpMutileColumn->IsSelected())	//�ĵ����ͣ� ���� ���� ����Ӧ
	{
		g_tagConnectSetting.nDocumentType = 1;
	}

	if (m_lpPleading->IsSelected())
	{
		g_tagConnectSetting.nDocumentType = 2;
	}

	if (m_lpSingleColumn->IsSelected())
	{
		g_tagConnectSetting.nDocumentType = 0;
	}


	if (m_lpFormat_DOC_Native->IsSelected())	//�����ʽ: RTF�ļ� DOC�ļ� DOCX�ļ�
	{
		g_tagConnectSetting.nPutoutExt = 1;
	}

	if (m_lpFormat_DOCX->IsSelected())
	{
		g_tagConnectSetting.nPutoutExt = 2;
	}

	if (m_lpFormat_RTF->IsSelected())
	{
		g_tagConnectSetting.nPutoutExt = 0;
	}

	if (m_lpBookMark_PAGESTART->IsSelected())	//�����ǩ����ת����ǩ ÿҳ��ͷ������ǩ ��PDFһ��
	{
		g_tagConnectSetting.nBkmkOpt = 1;
	}

	if (m_lpBookMark_PDF->IsSelected())
	{
		g_tagConnectSetting.nBkmkOpt = 2;
	}

	if (m_lpBookMark_None->IsSelected())
	{
		g_tagConnectSetting.nBkmkOpt = 0;
	}

	if (m_lpOCR_TEXT->IsSelected())		//OCR�ĵ�������ͼƬ������ ���������� ������ͼƬ
	{
		g_tagConnectSetting.nOcrOpt = 1;
	}

	if (m_lpOCR_IMAGE->IsSelected())
	{
		g_tagConnectSetting.nOcrOpt = 2;
	}

	if (m_lpOCR_TEXTIMAGE->IsSelected())
	{
		g_tagConnectSetting.nOcrOpt = 0;
	}

	CString strTimeOut = m_lpConvertTimeout->GetText();
	g_tagConnectSetting.nTimeout = _ttoi(strTimeOut.GetBuffer(0));	//�ȴ�ת���ʱ�� ����

	g_tagConnectSetting.bOpenPath = m_lpAutoOpenFolderFinish->IsSelected();	//ת���ɹ����Զ���Word���ڵ�Ŀ¼

}

void CSetWnd::Init()
{
	Create(g_hMainWnd, _T(""), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
	CenterWindow();

	m_lpTabOpt1 = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("tabLayout")));
	m_lpTabOpt2 = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("tabOut")));
	m_lpTabMain = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switchSetting")));

	m_lpCNV_Default = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("CNV_Default")));
	m_lpCNV_USE_SAT = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("CNV_USE_SAT")));
	m_lpCNV_TEXTBOX = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("CNV_TEXTBOX")));
	m_lpCNV_TEXTFLOW = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("CNV_TEXTFLOW")));

	m_lpAdjustFontNames = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("AdjustFontNames")));
	m_lpAdjustSpacing = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("AdjustSpacing")));

	m_lpExtractImages = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("ExtractImages")));
	m_lpExtractText = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("ExtractText")));
	m_lpShrinkFonts = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("ShrinkFonts")));
	m_lpDetectHeader = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("DetectHeader")));
	m_lpRemoveHiddenText = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("RemoveHiddenText")));
	m_lpSingleColumn = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("SingleColumn")));
	m_lpMutileColumn = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("MutileColumn")));
	m_lpPleading = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("Pleading")));
	m_lpFormat_RTF = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("Format_RTF")));
	m_lpFormat_DOC_Native = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("Format_DOC_Native")));
	m_lpFormat_DOCX = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("Format_DOCX")));
	m_lpBookMark_None = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("BookMark_None")));
	m_lpBookMark_PAGESTART = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("BookMark_PAGESTART")));
	m_lpBookMark_PDF = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("BookMark_PDF")));
	m_lpOCR_TEXTIMAGE = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("OCR_TEXTIMAGE")));
	m_lpOCR_TEXT = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("OCR_TEXT")));
	m_lpOCR_IMAGE = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("OCR_IMAGE")));
	m_lpConvertTimeout = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("ConvertTimeout")));
	m_lpAutoOpenFolderFinish = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("AutoOpenFolderFinish")));

	SetConnectSetting();
}

void CSetWnd::OnFinalMessage( HWND /*hWnd*/ )
{
	delete this;
}

// LRESULT CSetWnd::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
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

void CSetWnd::Notify(TNotifyUI& msg)
{
	if(msg.sType == _T("click")) 
	{
		CString strName = msg.pSender->GetName();
		if (_T("btnClose") == strName)		//�رնԻ���
		{
			Close(IDCANCEL);
		}
		else if (_T("btnOK") == strName)	//����ת������
		{
			//��������
			SaveSettingData();

			Close(IDOK);
		}
		else if (_T("btnRestore") == strName)	//�ָ�Ĭ������
		{
			InitConnectSetting();
			this->SetConnectSetting();
			Close(IDOK);
		}
	}
	else if(msg.sType == _T("selectchanged"))
	{
		if (m_lpTabOpt1->IsSelected())
		{
			m_lpTabMain->SelectItem(0);
		}

		if (m_lpTabOpt2->IsSelected())
		{
			m_lpTabMain->SelectItem(1);
		}
	}

	__super::Notify(msg);
};
