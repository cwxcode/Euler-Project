#include "StdAfx.h"
#include ".\loop.h"

CLoop::CLoop(void)
{
	halfedge_h = NULL;
	host_f = NULL;
	next = prev = NULL;
}

CLoop::~CLoop(void)
{
}

BOOL CLoop::AddHalfEdge(CHalfEdge* phe1, CHalfEdge* phe2)
{
	if(!phe1 && !phe2)
	{
		return FALSE;
	}

	CHalfEdge	*ptemp = halfedge_h;

	if(!phe2)
	{
		if(!ptemp)
			halfedge_h = phe1;
		else
		{
			while(ptemp->next)
				ptemp = ptemp->next;
			ptemp->next = phe1;
			phe1->prev = ptemp;
		}
		phe1->host_lp = this;
	}
	else
	{
		phe1->next = phe1->adj = phe2;
		phe2->prev = phe2->adj = phe1;
		if(!ptemp)
		{
			halfedge_h = phe1;
			phe1->prev = phe2;
			phe2->next = phe1;
		}
		else
		{
			while(ptemp->next->GetVertex() != phe1->GetVertex())
				ptemp = ptemp->next;
			phe2->next = ptemp->next;
			ptemp->next->prev = phe2;
			ptemp->next = phe1;
			phe1->prev = ptemp;
		}
		phe1->host_lp = phe2->host_lp = this;
	}
	
	return TRUE;
}

BOOL CLoop::IsVertexIn(CVertex* pvert)
{
	if(!pvert)
		return FALSE;

	CHalfEdge	*he = halfedge_h;

	while(he)
	{
		if(pvert == he->GetVertex())
			return TRUE;
		he = he->next;
		if(he == halfedge_h)
			return FALSE;
	}

	return FALSE;
}
CHalfEdge* CLoop::FindHostHalfEdge(CVertex* pvert)
{
	if(!pvert)
		return NULL;

	CHalfEdge *rethe = NULL, *he = halfedge_h;

	while(he)
	{
		if(he->GetVertex() == pvert)
			rethe = he;
		he = he->next;
		if(he == halfedge_h)
			return rethe;
	}

	return rethe;
}
