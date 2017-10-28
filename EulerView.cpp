// EulerView.cpp : implementation of the CEulerView class
//

#include "stdafx.h"
#include "Euler.h"

#include "EulerDoc.h"
#include "EulerView.h"
#include ".\eulerview.h"

#include "SweepDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CEulerView

IMPLEMENT_DYNCREATE(CEulerView, CView)

BEGIN_MESSAGE_MAP(CEulerView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_VIEW_FRONT, OnViewFront)
	ON_COMMAND(ID_VIEW_BACK, OnViewBack)
	ON_COMMAND(ID_VIEW_TOP, OnViewTop)
	ON_COMMAND(ID_VIEW_BUTTOM, OnViewButtom)
	ON_COMMAND(ID_CREATE_DRAWPOLYGON, OnCreateDrawpolygon)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_CREATE_SWEEP, OnCreateSweep)
	ON_COMMAND(ID_VIEW_WIRE, OnViewWire)
	ON_COMMAND(ID_VIEW_SURFACE, OnViewSurface)
	ON_COMMAND(ID_CREATE_CLEAR, OnCreateClear)
	ON_COMMAND(ID_VIEW_COORDINATE, OnViewCoordinate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COORDINATE, OnUpdateViewCoordinate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WIRE, OnUpdateViewWire)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SURFACE, OnUpdateViewSurface)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CEulerView construction/destruction

CEulerView::CEulerView()
: m_xAngle(0)
, m_yAngle(0)
, m_xPos(0)
, m_yPos(0)
, m_zPos(-5)
, m_dLength(0)
, m_iPointNum(1)
, m_iLoopNum(1)
, m_iPos(0)
, m_MouseDownPoint(0)
, tessobj(NULL)
, m_bDraw(false)
, m_bRButtonDown(false)
, m_bWire(false)
, m_bCoord(true)
{
	// TODO: add construction code here
	int i, j;
	for(i = 0;i < MAX_POINT;i ++)
		for(j = 0;j < 3;j ++)
			m_dPoint[i][j] = 0;
	for(i = 0;i < MAX_LOOP;i ++)
		m_iLoopPointNum[i] = 0;
	m_dVector[0] = m_dVector[1] = m_dVector[2] = 0;
}

CEulerView::~CEulerView()
{
}

BOOL CEulerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}

// CEulerView drawing

void CEulerView::OnDraw(CDC* /*pDC*/)
{
	CEulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	// Clear out the color & depth buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	RenderScene();

	// Tell OpenGL to flush its pipeline
	glFinish();

	// Now Swap the buffers
	SwapBuffers( m_pDC->GetSafeHdc() );
}


// CEulerView printing

BOOL CEulerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CEulerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CEulerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CEulerView diagnostics

#ifdef _DEBUG
void CEulerView::AssertValid() const
{
	CView::AssertValid();
}

void CEulerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEulerDoc* CEulerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEulerDoc)));
	return (CEulerDoc*)m_pDocument;
}
#endif //_DEBUG

// CEulerView message handlers
BOOL CEulerView::InitializeOpenGL(void)
{    
	//Get a DC for the Client Area
	m_pDC = new CClientDC(this);
	//Failure to Get DC
	if(m_pDC == NULL)
	{
		MessageBox("Error Obtaining DC");
		return FALSE;
	}

	//Failure to set the pixel format
	if(!SetupPixelFormat())
	{
		return FALSE;
	}

	//Create Rendering Context
	m_hRC = ::wglCreateContext (m_pDC->GetSafeHdc ());
	//Failure to Create Rendering Context
	if(m_hRC == 0)
	{
		MessageBox("Error Creating RC");
		return FALSE;
	}

	//Make the RC Current
	if(::wglMakeCurrent (m_pDC->GetSafeHdc (), m_hRC)==FALSE)
	{
		MessageBox("Error making RC Current");
		return FALSE;
	}

	//Specify Black as the clear color
	glClearColor(0.0f,0.0f,0.0f,0.0f);

	//Specify the back of the buffer as clear depth
	glClearDepth(1.0f);

	//Enable Depth Testing
	glEnable(GL_DEPTH_TEST);

	// Enable Face Culling
	glEnable(GL_CULL_FACE);

	// Cull the back face
	glCullFace(GL_BACK);

	// Use smooth shading
	glShadeModel(GL_SMOOTH);

	// Enable & setup the light
	SetupLighting();

	// Create a tessrlator object
	tessobj = gluNewTess();
	// Register the callback function
	gluTessCallback(tessobj, GLU_TESS_VERTEX, (void (__stdcall *)(void))vertexCallback);
	gluTessCallback(tessobj, GLU_TESS_BEGIN, (void (__stdcall *)(void))beginCallback);
	gluTessCallback(tessobj, GLU_TESS_END, endCallback);
	gluTessCallback(tessobj, GLU_TESS_ERROR, (void (__stdcall *)(void))errorCallback);
	gluTessCallback(tessobj, GLU_TESS_COMBINE, (void (__stdcall *)(void))combineCallback);
	// Set the winding rule
	gluTessProperty(tessobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);

	return TRUE;
}

BOOL CEulerView::SetupPixelFormat(void)
{
	static PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
			1,                              // version number
			PFD_DRAW_TO_WINDOW |            // support window
			PFD_SUPPORT_OPENGL |		    // support OpenGL
			PFD_DOUBLEBUFFER,				// double buffered
			PFD_TYPE_RGBA,                  // RGBA type
			24,                             // 24-bit color depth
			0, 0, 0, 0, 0, 0,               // color bits ignored
			0,                              // no alpha buffer
			0,                              // shift bit ignored
			0,                              // no accumulation buffer
			0, 0, 0, 0,                     // accum bits ignored
			16,                             // 16-bit z-buffer
			0,                              // no stencil buffer
			0,                              // no auxiliary buffer
			PFD_MAIN_PLANE,                 // main layer
			0,                              // reserved
			0, 0, 0                         // layer masks ignored
	};

	int m_nPixelFormat = ::ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd);
	// Failure to Choose the Pixel Format
	if ( m_nPixelFormat == 0 )
	{
		return FALSE;
	}

	// Failure sets the pixel format 
	if ( ::SetPixelFormat(m_pDC->GetSafeHdc(), m_nPixelFormat, &pfd) == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}

void CEulerView::SetupLighting(void)
{
	//Lighting Parameters
	GLfloat	lmodel_ambient[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat ambient[] = {1.0, 1.0, 1.0};

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	//Specify a single directional light & material
	//GLfloat diffuse1[] = { 1.0, 1.0, 1.0 };
	//GLfloat specular1[] = { 1.0, 1.0, 1.0 };
	GLfloat position1[] = { 10.0, 10.0, 10.0, 0.0 };
	GLfloat spot_direction[] = { -1.0, -1.0, -1.0 };

	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse1);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specular1);
	//glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0);
	//glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
	glLightfv(GL_LIGHT0, GL_POSITION, position1);

	//Enable Lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

#define MAX_COORD	30

void CEulerView::RenderCoordinate(int iLen)
{
	int		i;
	GLfloat x_ambient[] = { 1.0, 0.0, 0.0}, 
			y_ambient[] = { 0.0, 1.0, 0.0}, 
			z_ambient[] = { 0.0, 0.0, 1.0},
			axis_ambient[] = { 0.5, 0.5, 0.5},
			other_ambient[] = { 0.1, 0.1, 0.1};

	glLineWidth(1.0);
	glBegin(GL_LINES);
	for(i = -MAX_COORD;i <= MAX_COORD;i = i + iLen)
	{
		if(i == 0)
		{
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, x_ambient);	// X--Red
			glVertex3i(0, 0, 0);	 
			glVertex3i(MAX_COORD, 0, 0); 
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, y_ambient);	// Y--Green
			glVertex3i(0, 0, 0);
			glVertex3i(0, MAX_COORD, 0);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, z_ambient);	// Z--Blue
			glVertex3i(0, 0, 0);
			glVertex3i(0, 0, MAX_COORD);

			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, axis_ambient);
			glVertex3i(0, 0, 0);	// -X
			glVertex3i(-MAX_COORD, 0, 0);
			glVertex3i(0, 0, 0);	// -Y
			glVertex3i(0, -MAX_COORD, 0);
			glVertex3i(0, 0, 0);	// -Z
			glVertex3i(0, 0, -MAX_COORD);
		}
		else
		{
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, other_ambient);
			glVertex3i(MAX_COORD, 0, i);
			glVertex3i(-MAX_COORD, 0, i);
			glVertex3i(i, 0, MAX_COORD);
			glVertex3i(i, 0, -MAX_COORD);

			glVertex3i(MAX_COORD, i, 0);
			glVertex3i(-MAX_COORD, i, 0);
			glVertex3i(i, MAX_COORD, 0);
			glVertex3i(i, -MAX_COORD, 0);
		}
	}
	glEnd();
}

void CEulerView::RenderScene(void)
{
	CEulerDoc	*pDoc = GetDocument();
	CSolid		*pSolid;
	CFace		*pFace;
	CLoop		*pLoop;
	CHalfEdge	*pHalfEdge, *pHeHead; 
	GLint		i, j, pos = 0;
	GLfloat		draw_ambient[] = { 1.0, 1.0, 1.0},
				mat_ambient[] = { 0.5, 0.5, 0.5 }, 
				mat_specular[] = { 1.0, 1.0, 1.0, 1.0 },
				shininess[] = { 50.0 };
	GLfloat		position1[] = { 10.0, 10.0, 10.0, 0.0 };

	glLoadIdentity(); 
	glTranslatef(m_xPos, m_yPos, m_zPos);
	glRotatef(m_xAngle, 1.0f,0.0f,0.0f);
	glRotatef(m_yAngle, 0.0f,1.0f,0.0f);

	if(m_bCoord)
		RenderCoordinate(1);

	glLightfv(GL_LIGHT0, GL_POSITION, position1);

	//////////////////////////////////////////////////////////////////////////
	// Draw
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, draw_ambient);
	for(i = 1; i <= m_iLoopNum ;i ++)
	{
		pos += m_iLoopPointNum[i-1];
		glBegin(GL_LINE_STRIP);
		for(j = 1;j <= m_iLoopPointNum[i]; j++)
			glVertex3d(m_dPoint[pos + j][0], m_dPoint[pos + j][1], m_dPoint[pos + j][2]);
		glEnd();
	}
	//////////////////////////////////////////////////////////////////////////
	pSolid = pDoc->solid;

	if(m_bWire)
	{
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, draw_ambient);
		glLineWidth(2.0);
		glBegin(GL_LINES);
		while(pSolid)
		{
			pFace = pSolid->GetFaceHead();
			while(pFace)
			{
				pLoop = pFace->GetLoopHead();
				while(pLoop)
				{
					pHalfEdge = pHeHead = pLoop->GetHalfEdgeHead();
					while(pHalfEdge)
					{
						glVertex3dv(pHalfEdge->GetVertex()->vcoord);
						glVertex3dv(pHalfEdge->next->GetVertex()->vcoord);
						pHalfEdge = pHalfEdge->next;
						if(pHalfEdge == pHeHead)
							break;
					}
					pLoop = pLoop->next;
				}
				pFace = pFace->next;			
			}
			pSolid = pSolid->next;
		}
		glEnd();
	}
	else
	{
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
		glEnable(GL_NORMALIZE);
		while(pSolid)
		{
			pFace = pSolid->GetFaceHead();
			while(pFace)
			{
				gluTessBeginPolygon(tessobj, NULL);
				glNormal3dv(pFace->feq);
				pLoop = pFace->GetLoopHead();
				while(pLoop)
				{
					gluTessBeginContour(tessobj);
					pHalfEdge = pHeHead = pLoop->GetHalfEdgeHead();
					while(pHalfEdge)
					{
						gluTessVertex(tessobj, pHalfEdge->GetVertex()->vcoord, pHalfEdge->GetVertex()->vcoord);
						pHalfEdge = pHalfEdge->next;
						if(pHalfEdge == pHeHead)
							break;
					}
					gluTessEndContour(tessobj);
					pLoop = pLoop->next;
				}
				gluTessEndPolygon(tessobj);
				pFace = pFace->next;			
			}
			pSolid = pSolid->next;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Message Function
//////////////////////////////////////////////////////////////////////////
int CEulerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	//Initialize OpenGL Here
	InitializeOpenGL();

	return 0;
}

void CEulerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	GLdouble aspect_ratio; // width/height ratio

	// Return if the either the width or the height of the Window is 0
	if ( 0 >= cx || 0 >= cy )
	{
		return;
	}

	// select the full client area
	glViewport(0, 0, cx, cy);

	// compute the aspect ratio
	// this will keep all dimension scales equal
	aspect_ratio = (GLdouble)cx/(GLdouble)cy;

	// select the projection matrix and clear it
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// select the viewing volume
	gluPerspective(45.0f, aspect_ratio, .01f, 200.0f);

	// switch back to the modelview matrix and clear it
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

BOOL CEulerView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	//comment out the original call
	//return CView::OnEraseBkgnd(pDC);
	//Tell Windows not to erase the background
	return TRUE;
}

void CEulerView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: Add your message handler code here

	//Make the RC non-current
	if(::wglMakeCurrent (0,0) == FALSE)
	{
		MessageBox("Could not make RC non-current");
	}

	//Delete the rendering context
	if(::wglDeleteContext (m_hRC)==FALSE)
	{
		MessageBox("Could not delete RC");
	}

	//Delete the DC
	if(m_pDC)
	{
		delete m_pDC;
	}
	//Set it to NULL
	m_pDC = NULL;
	gluDeleteTess(tessobj);
}

//////////////////////////////////////////////////////////////////////////
// Keyboard & Mouse
//////////////////////////////////////////////////////////////////////////
void CEulerView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	switch (nChar)
	{
		case VK_UP:          
			m_yPos = m_yPos - 0.5f;
			break;
		case VK_DOWN:   
			m_yPos = m_yPos + 0.5f;
			break;
		case VK_LEFT:       
			m_xPos = m_xPos + 0.5f;
			break;
		case VK_RIGHT:     
			m_xPos = m_xPos - 0.5f;
			break;
		case VK_ESCAPE:
			exit(0);
		default:                   
			MessageBox("Press the arrow keys only!");
			break;
	} 

	InvalidateRect(NULL,FALSE);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CEulerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	m_zPos = m_zPos + zDelta / 100;

	InvalidateRect(NULL,FALSE);

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CEulerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	GLint realy;
	GLdouble wx, wy, wz;
	// TODO: Add your message handler code here and/or call default
	m_bRButtonDown = FALSE;
	m_MouseDownPoint = point;

	SetCapture();
	if(m_bDraw)
	{
		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
		realy = viewport[3] - (GLint) point.y - 1;
		gluUnProject((GLdouble)point.x, (GLdouble)realy, 0.0,  
			mvmatrix, projmatrix, viewport, &wx, &wy, &wz);
		m_dPoint[m_iPos + m_iPointNum][0] = m_dPoint[m_iPos + m_iPointNum + 1][0] = wx * (-m_zPos) / 0.01;
		m_dPoint[m_iPos + m_iPointNum][1] = m_dPoint[m_iPos + m_iPointNum + 1][1] = wy * (-m_zPos) / 0.01;
		m_dPoint[m_iPos + m_iPointNum][2] = m_dPoint[m_iPos + m_iPointNum + 1][2] = wz + m_zPos;
		m_iPointNum ++;
		m_iLoopPointNum[m_iLoopNum] = m_iPointNum;
		//Redraw the view
		InvalidateRect(NULL,FALSE);
	}
	CView::OnLButtonDown(nFlags, point);
}

void CEulerView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(!m_bDraw)
	{
		m_MouseDownPoint = CPoint(0,0);
		ReleaseCapture();
	}
	CView::OnLButtonUp(nFlags, point);
}

void CEulerView::OnMouseMove(UINT nFlags, CPoint point)
{
	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	GLint realy;
	GLdouble wx, wy, wz;
	// TODO: Add your message handler code here and/or call default
	// Check if we have captured the mouse
	if (GetCapture()==this)
	{
		if(m_bDraw)
		{
			glGetIntegerv(GL_VIEWPORT, viewport);
			glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
			glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
			realy = viewport[3] - (GLint) point.y - 1;
			gluUnProject((GLdouble)point.x, (GLdouble)realy, 0.0,  
				mvmatrix, projmatrix, viewport, &wx, &wy, &wz);
			m_dPoint[m_iPos + m_iPointNum][0] = wx * (-m_zPos) / 0.01;
			m_dPoint[m_iPos + m_iPointNum][1] = wy * (-m_zPos) / 0.01;
			m_dPoint[m_iPos + m_iPointNum][2] = wz + m_zPos;	
			//Redraw the view
			InvalidateRect(NULL,FALSE);
		}
		else
		{
			//Increment the object rotation angles
			m_xAngle+=(point.y-m_MouseDownPoint.y)/3.6;
			m_yAngle+=(point.x-m_MouseDownPoint.x)/3.6;
			//Redraw the view
			InvalidateRect(NULL,FALSE);
			//Set the mouse point
			m_MouseDownPoint=point;
		}
	}

	CView::OnMouseMove(nFlags, point);
}

void CEulerView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_bRButtonDown = TRUE;
	//m_bDraw = FALSE;
	m_dPoint[m_iPos + m_iPointNum][0] = m_dPoint[m_iPos + 1][0];
	m_dPoint[m_iPos + m_iPointNum][1] = m_dPoint[m_iPos + 1][1];
	m_dPoint[m_iPos + m_iPointNum][2] = m_dPoint[m_iPos + 1][2];
	m_iLoopPointNum[m_iLoopNum] = m_iPointNum;
	m_iPos += m_iPointNum;
	m_iPointNum = 1;
	m_iLoopNum ++;

	Log();

	//Redraw the view
	InvalidateRect(NULL,FALSE);

	m_MouseDownPoint=CPoint(0,0);
	ReleaseCapture();

	CView::OnRButtonDown(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////
// Tessellation
//////////////////////////////////////////////////////////////////////////
void CALLBACK beginCallback(GLenum type)
{
	glBegin(type);
}

void CALLBACK endCallback(void)
{
	glEnd();
}

void CALLBACK errorCallback(GLenum errorCode)
{
	const GLubyte	*estring;

	estring = gluErrorString(errorCode);
	fprintf(stderr, "Tessellation Error: %s\n", estring);
	exit(0);
}

void CALLBACK vertexCallback(GLvoid	*vertex)
{
	glVertex3dv((GLdouble *)vertex);
}

void CALLBACK combineCallback(GLdouble coords[3], GLdouble *vertex_data[4], GLfloat weight[4], GLdouble **dataOut)
{
	GLdouble *vertex;
	int	i;

	vertex = (GLdouble *)malloc(6 * sizeof(GLdouble));
	vertex[0] = coords[0];
	vertex[1] = coords[1];
	vertex[2] = coords[2];
	for(i = 3;i < 6;i ++)
		vertex[i] = weight[0] * vertex_data[0][i] 
					+ weight[1] * vertex_data[1][i] 
					+ weight[2] * vertex_data[2][i] 
					+ weight[3] * vertex_data[3][i];
	*dataOut = vertex;
}

//////////////////////////////////////////////////////////////////////////
// Menu -> View
//////////////////////////////////////////////////////////////////////////
void CEulerView::OnViewCoordinate()
{
	// TODO: Add your command handler code here
	if(m_bCoord)
		m_bCoord = FALSE;
	else
		m_bCoord = TRUE;
	InvalidateRect(NULL,FALSE);
}

void CEulerView::OnUpdateViewCoordinate(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_bCoord);
}

//////////////////////////////////////////////////////////////////////////
void CEulerView::OnViewFront()
{
	// TODO: Add your command handler code here
	m_xPos = m_yPos = 0;
	m_zPos = -5;
	m_xAngle = m_yAngle = 0;
	InvalidateRect(NULL,FALSE);
}

void CEulerView::OnViewBack()
{
	// TODO: Add your command handler code here
	m_xPos = m_yPos = 0;
	m_zPos = -5;
	m_xAngle = 0;
	m_yAngle = 180;
	InvalidateRect(NULL,FALSE);
}

void CEulerView::OnViewTop()
{
	// TODO: Add your command handler code here
	m_xPos = m_yPos = 0;
	m_zPos = -5;
	m_xAngle = 90;
	m_yAngle = 0;
	InvalidateRect(NULL,FALSE);
}

void CEulerView::OnViewButtom()
{
	// TODO: Add your command handler code here
	m_xPos = m_yPos = 0;
	m_zPos = -5;
	m_xAngle = 270;
	m_yAngle = 0;
	InvalidateRect(NULL,FALSE);
}

//////////////////////////////////////////////////////////////////////////
void CEulerView::OnViewWire()
{
	// TODO: Add your command handler code here
	m_bWire = TRUE;
	InvalidateRect(NULL,FALSE);
}

void CEulerView::OnUpdateViewWire(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_bWire);
}

void CEulerView::OnViewSurface()
{
	// TODO: Add your command handler code here
	m_bWire = FALSE;
	InvalidateRect(NULL,FALSE);
}

void CEulerView::OnUpdateViewSurface(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(!m_bWire);
}

//////////////////////////////////////////////////////////////////////////
// Menu -> Create
//////////////////////////////////////////////////////////////////////////
void CEulerView::OnCreateClear()
{
	int i, j;
	CEulerDoc* pDoc = GetDocument();
	// TODO: Add your command handler code here
	pDoc->solid = NULL;

	for(i = 1;i <= m_iPos;i ++)
		for(j = 0;j < 3;j ++)
			m_dPoint[i][j] = 0;

	for(i = 1;i < m_iLoopNum;i ++)
		m_iLoopPointNum[i] = 0;
	m_iLoopNum = 1;
	m_iPos = 0;

	InvalidateRect(NULL,FALSE);
}

//////////////////////////////////////////////////////////////////////////
void CEulerView::OnCreateDrawpolygon()
{
	int i, j;
	// TODO: Add your command handler code here
	OnViewFront();

	for(i = 0;i <= m_iPos;i ++)
		for(j = 0;j < 3;j ++)
			m_dPoint[i][j] = 0;

	for(i = 0;i < m_iLoopNum;i ++)
		m_iLoopPointNum[i] = 0;

	m_iPointNum = m_iLoopNum = 1;
	m_iPos = 0;

	m_bDraw = TRUE;
}

void CEulerView::OnCreateSweep()
{
	CSweepDlg	dlg;
	// TODO: Add your command handler code here
	dlg.m_vx = m_dVector[0];
	dlg.m_vy = m_dVector[1];
	dlg.m_vz = m_dVector[2];
	dlg.m_dLen = m_dLength;

	if(dlg.DoModal() == IDOK)
	{
		m_dVector[0] = dlg.m_vx;
		m_dVector[1] = dlg.m_vy;
		m_dVector[2] = dlg.m_vz;
		m_dLength = dlg.m_dLen;
		m_bDraw = FALSE;
		CEulerDoc* pDoc = GetDocument();
		pDoc->HighSweep(m_dPoint, m_iLoopPointNum, m_iLoopNum, m_dVector, m_dLength);
		InvalidateRect(NULL,FALSE);
	}
}

//////////////////////////////////////////////////////////////////////////
// Log
//////////////////////////////////////////////////////////////////////////
void CEulerView::Log(void)
{
	ofstream outfile;
	outfile.open("Log1.txt", ios::out);

	int i;

	outfile << "Loop Number: " << m_iLoopNum << endl;
	outfile << "----------------------------------------------------------------------" << endl;
	for(i = 1;i <= m_iLoopNum;i ++)
	{
		outfile << "Loop: " << i << endl;
		outfile << "	Point number: " << m_iLoopPointNum[i] << endl;
	}
	outfile << "----------------------------------------------------------------------" << endl;
	outfile << "Current Pos: " << m_iPos << endl;

	outfile.close();
}
