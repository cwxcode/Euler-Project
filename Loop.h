#pragma once

#include "HalfEdge.h"

class CFace;

class CLoop
{
public:
	CLoop(void);
	~CLoop(void);

private:
	CHalfEdge	*halfedge_h;

public:
	CFace		*host_f;
	CLoop		*next;
	CLoop		*prev;

	BOOL AddHalfEdge(CHalfEdge* phe1, CHalfEdge* phe2);
	CHalfEdge* GetHalfEdgeHead(void) { return halfedge_h; }
	BOOL IsVertexIn(CVertex* pvert);
	CHalfEdge* FindHostHalfEdge(CVertex* pvert);
};
