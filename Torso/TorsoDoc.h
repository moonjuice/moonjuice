// TorsoDoc.h : interface of the CTorsoDoc class
//

#pragma once

#include "TorsoStructure.h"
#include <Node.h>

#include "ISegment.h"
#include "ILeg.h"
#include "IArm_h.h"

class CTorsoDoc : public CDocument
{
protected: // create from serialization only
	CTorsoDoc();
	DECLARE_DYNCREATE(CTorsoDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CTorsoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CTorsoStructure m_Torso;

	SmartPtr<CMovNode> m_pBody;
	void CTorsoDoc::JointArm(void);
	void CTorsoDoc::JointLeg(void);
	
	ISegment *m_pISegment;
	ILeg *m_pILeg;
	ISPlam *m_pISPlam;
	ISFoot *m_pISFoot;
	IArm *m_pIArm;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	int m_nActive;
	afx_msg void OnStructureMinwaist();
	afx_msg void OnMaleCenterline();
	afx_msg void OnMaleGirths();
	afx_msg void OnMaleVerticallines();
	afx_msg void OnMaleGrid();
	afx_msg void OnStructureLoadupperneck();
	afx_msg void OnFileSegment();
	afx_msg void OnFileTorso();
	afx_msg void OnFileArm();
	afx_msg void OnFileLeg();
	afx_msg void OnFileSetdir();
};


