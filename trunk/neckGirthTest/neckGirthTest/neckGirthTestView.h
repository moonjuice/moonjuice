
// neckGirthTestView.h : CneckGirthTestView ���O������
//


#pragma once


class CneckGirthTestView : public CView
{
protected: // �ȱq�ǦC�ƫإ�
	CneckGirthTestView();
	DECLARE_DYNCREATE(CneckGirthTestView)

// �ݩ�
public:
	CneckGirthTestDoc* GetDocument() const;

// �@�~
public:

// �мg
public:
	virtual void OnDraw(CDC* pDC);  // �мg�H�yø���˵�
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �{���X��@
public:
	virtual ~CneckGirthTestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ͪ��T�������禡
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // neckGirthTestView.cpp ������������
inline CneckGirthTestDoc* CneckGirthTestView::GetDocument() const
   { return reinterpret_cast<CneckGirthTestDoc*>(m_pDocument); }
#endif

