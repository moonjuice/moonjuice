
// neckGirthTest.cpp : �w�q���ε{�������O�欰�C
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "neckGirthTest.h"
#include "MainFrm.h"

#include "neckGirthTestDoc.h"
#include "neckGirthTestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CneckGirthTestApp

BEGIN_MESSAGE_MAP(CneckGirthTestApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CneckGirthTestApp::OnAppAbout)
	// �̾ڤ��R�O���з��ɮ�
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// �зǦC�L�]�w�R�O
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CneckGirthTestApp �غc

CneckGirthTestApp::CneckGirthTestApp()
{

	// TODO: �b���[�J�غc�{���X�A
	// �N�Ҧ����n����l�]�w�[�J InitInstance ��
}

// �Ȧ����@�� CneckGirthTestApp ����

CneckGirthTestApp theApp;


// CneckGirthTestApp ��l�]�w

BOOL CneckGirthTestApp::InitInstance()
{
	// ���p���ε{����T�M����w�ϥ� ComCtl32.dll 6 (�t) �H�᪩���A
	// �ӱҰʵ�ı�Ƽ˦��A�b Windows XP �W�A�h�ݭn InitCommonControls()�C
	// �_�h����������إ߳��N���ѡC
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// �]�w�n�]�t�Ҧ��z�Q�n�Ω����ε{������
	// �q�α�����O�C
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// ��l�� OLE �{���w
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// �зǪ�l�]�w
	// �p�G�z���ϥγo�ǥ\��åB�Q���
	// �̫᧹�����i�����ɤj�p�A�z�i�H
	// �q�U�C�{���X�������ݭn����l�Ʊ`���A
	// �ܧ��x�s�]�w�Ȫ��n�����X
	// TODO: �z���ӾA�׭ק惡�r��
	// (�Ҧp�A���q�W�٩β�´�W��)
	SetRegistryKey(_T("���� AppWizard �Ҳ��ͪ����ε{��"));
	LoadStdProfileSettings(4);  // ���J�з� INI �ɿﶵ (�]�t MRU)

	// �n�����ε{�������d���C���d���t�d�b���B
	// �ج[�����P�˵����i��s��
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CneckGirthTestDoc),
		RUNTIME_CLASS(CMainFrame),       // �D SDI �ج[����
		RUNTIME_CLASS(CneckGirthTestView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);



	// ��R�з� Shell �R�O�BDDE�B�ɮ׶}�����ɪ��R�O�C
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// �b�R�O�C�����w�������R�O�C�p�G�w�ϥ� 
	// /RegServer�B/Register�B/Unregserver �� /Unregister �Ұ����ε{���A�N�Ǧ^ FALSE�C
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// �Ȫ�l�Ƥ@�ӵ����A�ҥH��ܨç�s�ӵ���
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// �u���b SDI ���ε{��������m�r���ɡA�~�I�s DragAcceptFiles
	// �o�|�o�ͩ� ProcessShellCommand ����
	return TRUE;
}



// �� App About �ϥ� CAboutDlg ��ܤ��

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ܤ�����
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

// �{���X��@
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// �����ܤ�������ε{���R�O
void CneckGirthTestApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CneckGirthTestApp �T���B�z�`��



