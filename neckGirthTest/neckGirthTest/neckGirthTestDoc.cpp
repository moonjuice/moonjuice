
// neckGirthTestDoc.cpp : CneckGirthTestDoc 類別的實作
//

#include "stdafx.h"
#include "neckGirthTest.h"

#include "neckGirthTestDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CneckGirthTestDoc

IMPLEMENT_DYNCREATE(CneckGirthTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CneckGirthTestDoc, CDocument)
END_MESSAGE_MAP()


// CneckGirthTestDoc 建構/解構

CneckGirthTestDoc::CneckGirthTestDoc()
{
	// TODO: 在此加入一次建構程式碼

}

CneckGirthTestDoc::~CneckGirthTestDoc()
{
}

BOOL CneckGirthTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此加入重新初始化程式碼
	// (SDI 文件會重用此文件)

	return TRUE;
}




// CneckGirthTestDoc 序列化

void CneckGirthTestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此加入儲存程式碼
	}
	else
	{
		// TODO: 在此加入載入程式碼
	}
}


// CneckGirthTestDoc 診斷

#ifdef _DEBUG
void CneckGirthTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CneckGirthTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CneckGirthTestDoc 命令
