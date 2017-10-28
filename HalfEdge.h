#pragma once

#include "Edge.h"
#include "Vertex.h"

class CLoop;

class CHalfEdge
{
public:
	CHalfEdge(void);
	~CHalfEdge(void);

private:
	CVertex		*vtx;

public:
	CLoop		*host_lp;
	CEdge		*edge;
	CHalfEdge	*next;
	CHalfEdge	*prev;
	CHalfEdge	*adj;

	BOOL SetVertex(CVertex* pvert);
	CVertex* GetVertex(void) { return vtx;}
};
