
// neckGirthTestDoc.h : CneckGirthTestDoc ���O������
//


#pragma once


class CneckGirthTestDoc : public CDocument
{
protected: // �ȱq�ǦC�ƫإ�
	CneckGirthTestDoc();
	DECLARE_DYNCREATE(CneckGirthTestDoc)

// �ݩ�
public:

// �@�~
public:

// �мg
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// �{���X��@
public:
	virtual ~CneckGirthTestDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ͪ��T�������禡
protected:
	DECLARE_MESSAGE_MAP()
};


