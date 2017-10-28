// EulerView.h : interface of the CEulerView class
//
#pragma once
#include "atltypes.h"

#define MAX_POINT	128
#define MAX_LOOP	64

#ifndef CALLBACK
#define CALLBACK
#endif

class CEulerView : public CView
{
protected: // create from serialization only
	CEulerView();
	DECLARE_DYNCREATE(CEulerView)

// Attributes
public:
	HGLRC	m_hRC; //Rendering Context
	CDC*	m_pDC; //Device Context

private:
	GLfloat m_xAngle;
	GLfloat m_yAngle;
	GLfloat m_xPos;
	GLfloat m_yPos;
	GLfloat m_zPos;

	CPoint	m_MouseDownPoint;

	GLUtesselator* tessobj;

	bool		m_bDraw;
	bool		m_bRButtonDown;
	bool		m_bWire;
	bool		m_bCoord;

	GLdouble	m_dPoint[MAX_POINT][3];
	GLdouble	m_dVector[3];
	GLdouble	m_dLength;

	GLint		m_iLoopPointNum[MAX_LOOP];
	GLint		m_iPointNum;
	GLint		m_iLoopNum;
	GLint       m_iPos;

// Operations
public:
	CEulerDoc* GetDocument() const;
	BOOL InitializeOpenGL(void);
	BOOL SetupPixelFormat(void);
	void SetupLighting(void);
	void RenderCoordinate(int iLen);
	void RenderScene(void);
	void Log(void);

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CEulerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg void OnViewFront();
	afx_msg void OnViewBack();
	afx_msg void OnViewTop();
	afx_msg void OnViewButtom();	
	afx_msg void OnViewCoordinate();
	afx_msg void OnUpdateViewCoordinate(CCmdUI *pCmdUI);
	afx_msg void OnViewWire();
	afx_msg void OnUpdateViewWire(CCmdUI *pCmdUI);
	afx_msg void OnViewSurface();
	afx_msg void OnUpdateViewSurface(CCmdUI *pCmdUI);

	afx_msg void OnCreateClear();
	afx_msg void OnCreateDrawpolygon();
	afx_msg void OnCreateSweep();
};

void CALLBACK vertexCallback(GLvoid* vertex);
void CALLBACK beginCallback(GLenum type);
void CALLBACK endCallback();
void CALLBACK errorCallback(GLenum errorCode);
void CALLBACK combineCallback(GLdouble coords[3], GLdouble *vertex_data[4], GLfloat weight[4], GLdouble **dataOut);
#ifndef _DEBUG  // debug version in EulerView.cpp
inline CEulerDoc* CEulerView::GetDocument() const
   { return reinterpret_cast<CEulerDoc*>(m_pDocument); }
#endif

