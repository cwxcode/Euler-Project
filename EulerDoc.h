// EulerDoc.h : interface of the CEulerDoc class
//
#pragma once

#include "Solid.h"

#define MAX_POINT	128
#define MAX_LOOP	64

class CEulerDoc : public CDocument
{
protected: // create from serialization only
	CEulerDoc();
	DECLARE_DYNCREATE(CEulerDoc)

// Attributes
public:
	CSolid	*solid;

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CEulerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	CSolid* mvfs(GLdouble *point);
	CHalfEdge* mev(GLdouble *point1, GLdouble *point2, CLoop* lp);
	CLoop* mef(GLdouble *point1, GLdouble *point2, CLoop* lp);
	CLoop* kemr(GLdouble *point1, GLdouble *point2, CLoop* lp);
	void kfmrh(CLoop* outlp, CLoop* lp);
	void sweep(CFace* pFace, GLdouble dx, GLdouble dy, GLdouble dz);

	CSolid* CreateBlock(void);
	void Log(void);
	void LogOut(CLoop *pLoop);
	CSolid* HighSweep(GLdouble point[][3], GLint* pointnum, GLint loopnum, GLdouble* vector, GLdouble len);
	void CalcNormal(CSolid	*pSolid);
};


