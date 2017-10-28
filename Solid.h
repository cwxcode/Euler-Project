#pragma once

#include "Face.h"
#include "Edge.h"
#include "Vertex.h"

class CSolid
{
public:
	CSolid(void);
	~CSolid(void);

private:
	CFace	*face_h;
	CEdge	*edge_h;
	CVertex	*vert_h;

public:
	int		solid_id;
	CSolid	*next;
	CSolid	*prev;

	BOOL AddFace(CFace* pface);
	BOOL AddEdge(CEdge* pedge);
	BOOL AddVertex(CVertex* pvert);
	CFace* GetFaceHead(void) { return face_h; }
	CEdge* GetEdgeHead(void) { return edge_h; }
	CVertex* GetVertexHead(void) { return vert_h; }
	CVertex* GetVertexEnd(void);
	CVertex* FindVertex(GLdouble *point);
	CFace* GetFaceEnd(void);
};
