// EulerDoc.cpp : implementation of the CEulerDoc class
//

#include "stdafx.h"
#include "Euler.h"
#include <math.h>

#include "EulerDoc.h"
#include ".\eulerdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEulerDoc

IMPLEMENT_DYNCREATE(CEulerDoc, CDocument)

BEGIN_MESSAGE_MAP(CEulerDoc, CDocument)
END_MESSAGE_MAP()


// CEulerDoc construction/destruction

CEulerDoc::CEulerDoc()
{
	// TODO: add one-time construction code here
	solid = NULL;
}

CEulerDoc::~CEulerDoc()
{
}

BOOL CEulerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CEulerDoc serialization

void CEulerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CEulerDoc diagnostics

#ifdef _DEBUG
void CEulerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEulerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CEulerDoc commands

//////////////////////////////////////////////////////////////////////////
// Make one solid, one face, one loop and one vertex
// \param point: the coordinate of the new vertex
// \return: the new solid
//////////////////////////////////////////////////////////////////////////
CSolid* CEulerDoc::mvfs(GLdouble *point)
{
	CSolid	*newSolid;
	CFace	*newFace;
	CLoop	*newLoop;
	CVertex	*newVert;
	newSolid = new CSolid;
	newFace = new CFace;
	newLoop = new CLoop;
	newVert = new CVertex;

	// Set the coordinate of the new vertex
	newVert->SetCoord(point);

	// Add the new face to the new solid
	newSolid->AddFace(newFace);

	// Add the new vertex to the new solid
	newSolid->AddVertex(newVert);

	// Add the new loop to the new face
	newFace->AddLoop(newLoop);
	
	return newSolid;
}

//////////////////////////////////////////////////////////////////////////
// Make one edge, two halfedges and one vertex
// \param point: the new vertex of the edge
// \param v1: the start vertex of the edge
// \param lp: the loop contain the edge
// \return: one of the halfedges
//////////////////////////////////////////////////////////////////////////
CHalfEdge* CEulerDoc::mev(GLdouble *point1, GLdouble *point2, CLoop* lp)
{
	CSolid		*pSolid = lp->host_f->host_s;
	CVertex		*v1, *newVert;
	CEdge		*newEdge;
	CHalfEdge	*he1, *he2;
	newVert = new CVertex;
	newEdge = new CEdge;
	he1 = new CHalfEdge;
	he2 = new CHalfEdge;

	// Find the start vertex of the edge
	v1 = pSolid->FindVertex(point1);
	if(!v1)
	{
		AfxMessageBox("Can't find the point1!");
		return NULL;
	}

	// Set the coordinate of the new vertex
	newVert->SetCoord(point2);

	// Set v1 as the start vertex of halfedge1
	he1->SetVertex(v1);

	// Set v2 as the start vertex of halfedge2
	he2->SetVertex(newVert);

	// Add two new halfedges to the loop
	lp->AddHalfEdge(he1, he2);

	// Set the new edge to new halfedges
	he1->edge = he2->edge = newEdge;
	newEdge->he1 = he1;
	newEdge->he2 = he2;

	// Add the new edge to the solid
	pSolid->AddEdge(newEdge);

	// Add the new vertex to the solid
	pSolid->AddVertex(newVert);

	return he1;
}

//////////////////////////////////////////////////////////////////////////
// Make one edge, one face and one loop
// \param point1: the latest vertex of the solid
// \param point2: the old vertex of the edge
// \param lp: the loop contain the edge
// \return: the new loop
//////////////////////////////////////////////////////////////////////////
CLoop* CEulerDoc::mef(GLdouble *point1, GLdouble *point2, CLoop* lp)
{
	CSolid		*pSolid = lp->host_f->host_s;
	CVertex		*v1, *v2;
	// Find the two given vertex
	v1 = pSolid->FindVertex(point1);
	if(!v1)
	{
		AfxMessageBox("Can't find the point1!");
		return NULL;
	}
	v2 = pSolid->FindVertex(point2);
	if(!v2)
	{
		AfxMessageBox("Can't find the point2!");
		return NULL;
	}
	// two vertex must in the same loop
	if(!lp->IsVertexIn(v1) && !lp->IsVertexIn(v2))
		return NULL;

	CFace	*newFace;
	CLoop	*newLoop;
	CEdge	*newEdge;
	CHalfEdge	*he1, *he2, *temphe1, *temphe2;
	newFace = new CFace;
	newLoop = new CLoop;
	newEdge = new CEdge;
	he1 = new CHalfEdge;
	he2 = new CHalfEdge;

	// Find two halfedges start with two vertexs
	temphe1 = lp->FindHostHalfEdge(v1);
	temphe2 = lp->FindHostHalfEdge(v2);

	// Change halfedges' host loop to new loop
	temphe1->prev->next = NULL;
	newLoop->AddHalfEdge(temphe1, NULL);
	while(temphe1 != temphe2)
		temphe1 = temphe1->next;
	temphe1->prev->next = he2;
	he2->prev = temphe1->prev;
	he2->next = newLoop->GetHalfEdgeHead();
	newLoop->GetHalfEdgeHead()->prev = he2;
	temphe1->prev = NULL;

	// Add halfedge start with vertex one to the old loop % close this loop
	lp->AddHalfEdge(he1, NULL);
	he1->next = temphe2;
	temphe2->prev = he1;

	// Set two halfedges' start vertex and adjacent
	he1->SetVertex(v1);
	he2->SetVertex(v2);
	he1->adj = he2;
	he2->adj = he1;

	// Set the new edge and add to the solid
	newEdge->he1 = he1;
	newEdge->he2 = he2;
	pSolid->AddEdge(newEdge);

	// Add new face to the solid
	pSolid->AddFace(newFace);

	// Add new loop to the new face
	newFace->AddLoop(newLoop);

	return newLoop;
}

//////////////////////////////////////////////////////////////////////////
// Kill one edge & make one loop	
// \param point1: one vertex of the edge need to be killed
// \param point2: another vertex of the edge need to be killed
// \param lp: the loop contain the edge
// \return: the new loop
//////////////////////////////////////////////////////////////////////////
CLoop* CEulerDoc::kemr(GLdouble *point1, GLdouble *point2, CLoop* lp)
{
	CSolid		*pSolid = lp->host_f->host_s;
	CVertex		*v1, *v2;
	// Find the two given vertex
	v1 = pSolid->FindVertex(point1);
	if(!v1)
	{
		AfxMessageBox("Can't find the point1!");
		return NULL;
	}
	v2 = pSolid->FindVertex(point2);
	if(!v2)
	{
		AfxMessageBox("Can't find the point2!");
		return NULL;
	}
	// two vertex must in the same loop
	if(!lp->IsVertexIn(v1) && !lp->IsVertexIn(v2))
		return NULL;

	CLoop		*newLoop;
	CHalfEdge	*he1, *he2;
	CEdge		*edge;
	newLoop = new CLoop;

	// Find two halfedges start with two vertexs
	he1 = lp->GetHalfEdgeHead();
	while(he1)
	{
		if(he1->GetVertex() == v1 && he1->next->GetVertex() == v2)
			break;
		he1 = he1->next;
	}
	he2 = he1->adj;

	// Make a newloop and delete two halfedges
	newLoop->AddHalfEdge(he1->next, NULL);
	he2->prev->next = he1->next;
	he1->next->prev = he2->prev;
	he1->prev->next = he2->next;
	he2->next->prev = he1->prev;
	he1->next = he1->prev = he2->next = he2->prev = NULL;
	delete he1;
	delete he2;

	// Find the edge and delete it
	edge = pSolid->GetEdgeHead();
	while(edge)
	{
		if(edge->he1 == he1)
			break;
		edge = edge->next;
	}
	edge->prev->next = edge->next;
	edge->next->prev = edge->prev;
	edge->next = edge->prev = NULL;

	// Add new loop to the face
	lp->host_f->AddLoop(newLoop);

	return newLoop;
}

//////////////////////////////////////////////////////////////////////////
// Kill one face & make one loop, so make a hole	
// \param outlp: the out loop of the face need to be added a loop
// \param lp: the loop of the face need to be killed
// \return: void
//////////////////////////////////////////////////////////////////////////
void CEulerDoc::kfmrh(CLoop* outlp, CLoop* lp)
{
	if(!outlp || !lp)
	{
		AfxMessageBox("The loop is NULL!");
		return;
	}

	CSolid		*pSolid = lp->host_f->host_s;

	// Add the loop to the face
	outlp->host_f->AddLoop(lp);

	// Get the face need to be killed
	CFace* pFace = pSolid->GetFaceEnd();

	// Delete it
	pFace->prev->next = NULL;

	delete pFace;
}

//////////////////////////////////////////////////////////////////////////
// Sweep a solid on the face	
// \param pFace: the face need to be sweeped
// \param dx, dy, dz: the direction and length
// \return: void
//////////////////////////////////////////////////////////////////////////
void CEulerDoc::sweep(CFace* pFace, GLdouble dx, GLdouble dy, GLdouble dz)
{
	CFace		*pEnd;
	CHalfEdge	*pHe, *pHead;
	CLoop		*pLoop, *newLoop, *pOutLoop;
	GLdouble	*pPoint;
	GLdouble	point1[3], point2[3], first[3], last[3];
	BOOL		bOut = TRUE; // Show that if is the outloop of the top face

	// Remember the last inside loop's face 
	pEnd = pFace->host_s->GetFaceEnd();
	// Start with the second face, because the first face is for buttom
	pFace = pFace->next;
	while(pFace)
	{
		// Get the first point of the loop
		newLoop = pLoop = pFace->GetLoopHead();
		pHe = pHead = pLoop->GetHalfEdgeHead();
		pPoint = pHe->GetVertex()->vcoord;
		point1[0] = pPoint[0];
		point1[1] = pPoint[1];
		point1[2] = pPoint[2];
		// first[] is used for close the top face
		// last[] is used for side face
		first[0] = last[0] = point2[0] = point1[0] + dx;
		first[1] = last[1] = point2[1] = point1[1] + dy;
		first[2] = last[2] = point2[2] = point1[2] + dz;
		// Make the new edge
		mev(point1, point2, pLoop);
		// Goto next halfedge
		pHe = pHe->next;
		while(pHe->GetVertex() != pHead->GetVertex())
		{
			// Get the point
			pPoint = pHe->GetVertex()->vcoord;
			point1[0] = pPoint[0];
			point1[1] = pPoint[1];
			point1[2] = pPoint[2];
			point2[0] = point1[0] + dx;
			point2[1] = point1[1] + dy;
			point2[2] = point1[2] + dz;
			// Make the new edge
			mev(point1, point2, newLoop);
			// Make a new side face
			newLoop = mef(point2, last, newLoop);
			// Remember the lastest point
			last[0] = point2[0];
			last[1] = point2[1];
			last[2] = point2[2];
			pHe = pHe->next;
		}
		// Close the top face
		newLoop = mef(first, last, newLoop);

		if(bOut)
		{
			pOutLoop = newLoop;
			bOut = FALSE;
		}
		else
			kfmrh(pOutLoop, newLoop);

		if(pFace == pEnd)
			break;

		pFace = pFace->next;
	}
}

//////////////////////////////////////////////////////////////////////////
void CEulerDoc::CalcNormal(CSolid	*pSolid)
{
	CFace		*pFace;
	CHalfEdge	*pHalfEdge, *pHeHead;
	CVertex		*pVert1, *pVert2, *pVert3;
	GLdouble	sub1[3], sub2[3], normal[3];
	GLdouble	sum_norm;

	pFace = pSolid->GetFaceHead();
	while(pFace)
	{
		pHeHead = pHalfEdge = pFace->GetLoopHead()->GetHalfEdgeHead();
		pVert1 = pHalfEdge->GetVertex();
		pVert2 = pHalfEdge->next->GetVertex();
		pVert3 = pHalfEdge->next->next->GetVertex();
		sub1[0] = pVert1->vcoord[0] - pVert2->vcoord[0];
		sub1[1] = pVert1->vcoord[1] - pVert2->vcoord[1];
		sub1[2] = pVert1->vcoord[2] - pVert2->vcoord[2];
		sub2[0] = pVert2->vcoord[0] - pVert3->vcoord[0];
		sub2[1] = pVert2->vcoord[1] - pVert3->vcoord[1];
		sub2[2] = pVert2->vcoord[2] - pVert3->vcoord[2];
		normal[0] = sub1[1] * sub2[2] - sub2[1] * sub1[2];
		normal[1] = sub1[2] * sub2[0] - sub2[2] * sub1[0];
		normal[2] = sub1[0] * sub2[1] - sub2[0] * sub1[1];
		sum_norm = normal[0] + normal[1] + normal[2];
		sum_norm = fabs(sum_norm);
		normal[0] = normal[0] / sum_norm;
		normal[1] = normal[1] / sum_norm;
		normal[2] = normal[2] / sum_norm;
		pFace->feq[0] = normal[0];
		pFace->feq[1] = normal[1];
		pFace->feq[2] = normal[2];
		pFace = pFace->next;
	}
}

//////////////////////////////////////////////////////////////////////////
CSolid* CEulerDoc::CreateBlock(void)
{
	CSolid	*pSolid, *pTemp;
	CLoop	*pLoop, *pHead;
	GLdouble point[16][3] = { {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}, {0.0, 1.0, 0.0},
							{0.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {1.0, 0.0, 1.0}, {0.0, 0.0, 1.0},
							{0.2, 0.2, 1.0}, {0.8, 0.2, 1.0}, {0.8, 0.8, 1.0}, {0.2, 0.8, 1.0},
							{0.2, 0.8, 0.0}, {0.8, 0.8, 0.0}, {0.8, 0.2, 0.0}, {0.2, 0.2, 0.0} };

	// No-Sweeping
	pSolid = mvfs(point[0]);
	pHead = pLoop = pSolid->GetFaceHead()->GetLoopHead();
	mev(point[0], point[1], pLoop);
	mev(point[1], point[2], pLoop);
	mev(point[2], point[3], pLoop);
	pLoop = mef(point[3], point[0], pLoop);
	mev(point[3], point[4], pLoop);
	mev(point[2], point[5], pLoop);
	pLoop = mef(point[5], point[4], pLoop);
	mev(point[1], point[6], pLoop);
	pLoop = mef(point[6], point[5], pLoop);
	mev(point[0], point[7], pLoop);
	pLoop = mef(point[7], point[6], pLoop);
	pLoop = mef(point[4], point[7], pLoop);
	
	mev(point[7], point[8], pLoop);
	mev(point[8], point[9], pLoop);
	mev(point[9], point[10], pLoop);
	mev(point[10], point[11], pLoop);
	pLoop = mef(point[11], point[8], pLoop);
	kemr(point[7], point[8], pLoop->host_f->prev->GetLoopHead());

	mev(point[11], point[12], pLoop);
	mev(point[10], point[13], pLoop);
	mev(point[9], point[14], pLoop);
	mev(point[8], point[15], pLoop);
	pLoop = mef(point[13], point[12], pLoop);
	pLoop = mef(point[14], point[13], pLoop);
	pLoop = mef(point[15], point[14], pLoop);
	pLoop = mef(point[12], point[15], pLoop);

	kfmrh(pSolid->GetFaceHead()->GetLoopHead(), pLoop);
	CalcNormal(pSolid);

	Log();

	// Insert the new solid
	if(!solid)
		solid = pSolid;
	else
	{
		pTemp = solid;
		while(pTemp->next)
			pTemp = pTemp->next;
		pTemp->next = pSolid;
	}

	return pSolid;
}

CSolid* CEulerDoc::HighSweep(GLdouble point[][3], GLint* pointnum, GLint loopnum, GLdouble* vector, GLdouble len)
{
	CSolid	*pSolid, *pTemp;
	CLoop	*pLoop, *pHead;
	int		i, j, pos;

	// Create the solid
	pSolid = mvfs(point[1]);
	pHead = pLoop = pSolid->GetFaceHead()->GetLoopHead();

	// Create the out loop
	for(i = 1;i < pointnum[1]-1;i ++)
	{
		mev(point[i], point[i+1], pLoop);
	}
	mef(point[i], point[1], pLoop);
	pos = i+1;

	// Create the inside loops
	for(i = 2;i < loopnum; i ++)
	{
		mev(point[1], point[pos+1], pHead);
		for(j = 1;j < pointnum[i]-1;j ++)
		{
			mev(point[pos+j], point[pos+j+1], pHead);
		}
		mef(point[pos+j], point[pos+1], pHead);
		kemr(point[1], point[pos+1], pHead);
		pos = pos+j+1;
	}

	sweep(pSolid->GetFaceHead(), vector[0] * len, vector[1] * len, vector[2] * len);
	CalcNormal(pSolid);

	// Insert the new solid
	if(!solid)
		solid = pSolid;
	else
	{
		pTemp = solid;
		while(pTemp->next)
			pTemp = pTemp->next;
		pTemp->next = pSolid;
	}

	Log();

	return pSolid;
}

//////////////////////////////////////////////////////////////////////////
void CEulerDoc::Log(void)
{
	ofstream outfile;
	outfile.open("Log.txt", ios::out);

	CSolid		*pSolid = solid;
	CFace		*pFace;
	CLoop		*pLoop;
	CHalfEdge	*pHe;
	CEdge		*pEdge;
	CVertex		*pVert;
	int			loopnum;
	double		*normal;

	while(pSolid)
	{
		// 输出Solid
		outfile << "Solid: " << pSolid->solid_id << endl;
		// 输出Face
		pFace = pSolid->GetFaceHead();
		while(pFace)
		{
			outfile << "	Face: " << pFace->face_id << endl;

			outfile << "	Normal: "<< pFace->feq[0] << " " << pFace->feq[1] << " " << pFace->feq[2] << endl;
			// 输出Loop
			loopnum = 0;
			pLoop = pFace->GetLoopHead();
			while(pLoop)
			{
				outfile << "		Loop: " << loopnum++ << endl;
				// 输出正向的HalfEdge的Vertex
				outfile << "			[Next]" << endl;
				pHe = pLoop->GetHalfEdgeHead();
				outfile << "			Vertex: " << pHe->GetVertex()->vertex_id << endl;
				pHe = pHe->next;
				while(pHe != pLoop->GetHalfEdgeHead())
				{
					outfile << "			Vertex: " << pHe->GetVertex()->vertex_id << endl;
					pHe = pHe->next;
				}
				// 输出逆向的HalfEdge的Vertex
				/*outfile << "			[Previous]" << endl;
				pHe = pLoop->GetHalfEdgeHead();
				outfile << "			Head Vertex: " << pHe->GetVertex()->vertex_id << endl;
				pHe = pHe->prev;
				while(pHe != pLoop->GetHalfEdgeHead())
				{
					outfile << "			Vertex: " << pHe->GetVertex()->vertex_id << endl;
					pHe = pHe->prev;
				}*/
				pLoop = pLoop->next;
			}
			pFace = pFace->next;
		}
		outfile << "----------------------------------------------------------------------" << endl;
		outfile << "Solid: " << pSolid->solid_id << endl;
		pEdge = pSolid->GetEdgeHead();
		while(pEdge)
		{
			outfile << "	Edge: " << pEdge->he1->GetVertex()->vertex_id << "-" << pEdge->he2->GetVertex()->vertex_id << endl;
			pEdge = pEdge->next;
		}
		outfile << "----------------------------------------------------------------------" << endl;
		outfile << "Solid: " << pSolid->solid_id << endl;
		pVert= pSolid->GetVertexHead();
		while(pVert)
		{
			outfile << "	Vertex: " << pVert->vertex_id;
			outfile << " [" << pVert->vcoord[0] << ", " << pVert->vcoord[1] << ", " << pVert->vcoord[2] << "]" << endl;
			pVert = pVert->next;
		}
		pSolid = pSolid->next;
		outfile << "----------------------------------------------------------------------" << endl;
		outfile << "----------------------------------------------------------------------" << endl;
	}

	outfile.close();
}

void CEulerDoc::LogOut(CLoop *pLoop)
{
	ofstream outfile;
	outfile.open("Log.txt", ios::out);

	CHalfEdge	*pHe;

	pHe = pLoop->GetHalfEdgeHead();
	outfile << endl << "Head Vertex: " << pHe->GetVertex()->vertex_id << endl;
	pHe = pHe->next;
	while(pHe != pLoop->GetHalfEdgeHead())
	{
		outfile << "Vertex: " << pHe->GetVertex()->vertex_id << endl;
		pHe = pHe->next;
	}

	outfile.close();
}