#include "StdAfx.h"
#include ".\halfedge.h"

CHalfEdge::CHalfEdge(void)
{
	edge = NULL;
	vtx = NULL;
	host_lp = NULL;
	next = prev = adj = NULL;
}

CHalfEdge::~CHalfEdge(void)
{
}

BOOL CHalfEdge::SetVertex(CVertex* pvert)
{
	if(!pvert)
	{
		return FALSE;
	}

	vtx = pvert;

	return TRUE;
}