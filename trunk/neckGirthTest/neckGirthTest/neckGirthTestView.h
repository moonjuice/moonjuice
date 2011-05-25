
// neckGirthTestView.h : CneckGirthTestView 類別的介面
//


#pragma once


class CneckGirthTestView : public CView
{
protected: // 僅從序列化建立
	CneckGirthTestView();
	DECLARE_DYNCREATE(CneckGirthTestView)

// 屬性
public:
	CneckGirthTestDoc* GetDocument() const;

// 作業
public:

// 覆寫
public:
	virtual void OnDraw(CDC* pDC);  // 覆寫以描繪此檢視
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 程式碼實作
public:
	virtual ~CneckGirthTestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 產生的訊息對應函式
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // neckGirthTestView.cpp 中的偵錯版本
inline CneckGirthTestDoc* CneckGirthTestView::GetDocument() const
   { return reinterpret_cast<CneckGirthTestDoc*>(m_pDocument); }
#endif

