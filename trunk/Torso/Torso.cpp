// Torso.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Torso.h"
#include "MainFrm.h"
#include <afxctl.h>

#include "TorsoDoc.h"
#include "TorsoView.h"
#include ".\torso.h"
#include "afxwin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTorsoApp

BEGIN_MESSAGE_MAP(CTorsoApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_HELP_REGISTER, OnHelpRegister)
	ON_COMMAND(ID_HELP_UNREGISTER, OnHelpUnregister)
END_MESSAGE_MAP()


// CTorsoApp construction

CTorsoApp::CTorsoApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CTorsoApp object

CTorsoApp theApp;

// CTorsoApp initialization

BOOL CTorsoApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	CoInitialize( NULL );

	// Parse command line for standard OLE commands

	CCommandLineInfo CmdInfo;
	ParseCommandLine(CmdInfo);

	// Check to see if launched as OLE server
	if (CmdInfo.m_bRunEmbedded || CmdInfo.m_bRunAutomated)
	{
		// Application was run with /Embedding or /Automation.

		// Register all OLE server (factories) as running.  This enables the
		//  OLE libraries to create objects from other applications.
		COleObjectFactory::RegisterAll();
		return TRUE;
	}
	//// Load up various strings
	//CString strClassID, strProgID;
	//strClassID.LoadString(IDS_CLASSID);
	//strProgID.LoadString(IDS_PROGID);

	//// Did the user request registration on the command-line?
	//if (CmdInfo.m_bRegister) {
	//	RegisterComponent(strProgID, FALSE);
	//	return FALSE;
	//}

	//// Did the user request unregistration on the command-line?
	//if (CmdInfo.m_bUnregister) {
	//	UnregisterComponent(strClassID, strProgID, FALSE);
	//	return FALSE;
	//}

	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CTorsoDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CTorsoView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_ctrlMyList;
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

// App command to run the dialog
void CTorsoApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CTorsoApp message handlers
VOID CTorsoApp::RegisterComponent(const CString& strProgID,
                                        BOOL  boolShowConfirmation)
{
	// Place the server into the system registry
	COleObjectFactory::UpdateRegistryAll();

	// Are we operating silently?
	if (!boolShowConfirmation)
		return;

	// Let the user know that registration is complete
	CString strMsg = "The ";
	strMsg += strProgID;
	strMsg += " server has been registered.";
	AfxMessageBox(strMsg, MB_ICONINFORMATION);
}

VOID CTorsoApp::UnregisterComponent(const CString& strClassID,
                                          const CString& strProgID,
                                          BOOL  boolShowConfirmation)
{
	// Transform the string-format CLASSID into a real CLSID
	CLSID CLSID_Component;
	BSTR bstrClassID = strClassID.AllocSysString();
	CLSIDFromString(bstrClassID, &CLSID_Component);
	SysFreeString(bstrClassID);

	// Unregister the class
	AfxOleUnregisterClass(CLSID_Component, strProgID);

	// Are we operating silently?
	if (!boolShowConfirmation)
		return;

	// Let the user know that unregistration is complete
	CString strMsg = "The ";
	strMsg += strProgID;
	strMsg += " server has been unregistered.";
	AfxMessageBox(strMsg, MB_ICONINFORMATION);
}

void CTorsoApp::OnHelpRegister()
{
	// TODO: Add your command handler code here
	CString strClassID, strProgID;
	strClassID.LoadString(IDS_CLSID_TORSO);
	strProgID.LoadString(IDS_PROGID_TORSO);

	RegisterComponent(strProgID, TRUE);
}

void CTorsoApp::OnHelpUnregister()
{
	// TODO: Add your command handler code here
	CString strClassID, strProgID;
	strClassID.LoadString(IDS_CLSID_TORSO);
	strProgID.LoadString(IDS_PROGID_TORSO);

	UnregisterComponent(strClassID, strProgID, FALSE);
}

