#include "StdAfx.h"
#include ".\solid.h"

CSolid::CSolid(void)
{
	static int id = 0;
	solid_id = id ++;
	face_h = NULL;
	edge_h = NULL;
	vert_h = NULL;
	next = prev = NULL;
}

CSolid::~CSolid(void)
{
}

BOOL CSolid::AddFace(CFace* pface)
{
	if(!pface)
	{
		return FALSE;
	}

	CFace *ptemp = face_h;

	if(!ptemp)
		face_h = pface;
	else
	{
		while(ptemp->next)
		{
			ptemp = ptemp->next;
		}
		ptemp->next = pface;
		pface->prev = ptemp;
	}
	pface->host_s = this;

	return TRUE;
}

BOOL CSolid::AddEdge(CEdge* pedge)
{
	if(!pedge)
	{
		return FALSE;
	}

	CEdge *ptemp = edge_h;

	if(!ptemp)
		edge_h = pedge;
	else
	{
		while(ptemp->next)
		{
			ptemp = ptemp->next;
		}
		ptemp->next = pedge;
		pedge->prev = ptemp;
	}

	return TRUE;
}

BOOL CSolid::AddVertex(CVertex* pvert)
{
	if(!pvert)
	{
		return FALSE;
	}

	CVertex *ptemp = vert_h;

	if(!ptemp)
		vert_h = pvert;
	else
	{
		while(ptemp->next)
		{
			ptemp = ptemp->next;
		}
		ptemp->next = pvert;
		pvert->prev = ptemp;
	}

	return TRUE;
}

CVertex* CSolid::GetVertexEnd(void)
{
	CVertex *v = vert_h;

	while(v->next)
	{
		v = v->next;
	}

	return v;
}

CVertex* CSolid::FindVertex(GLdouble *point)
{
	CVertex *v = vert_h;

	while(v)
	{
		if(v->vcoord[0] == point[0] &&
			v->vcoord[1] == point[1] &&
			v->vcoord[2] == point[2])
			return v;
		v = v->next;
	}

	return NULL;
}

CFace* CSolid::GetFaceEnd(void)
{
	CFace *ptemp = face_h;

	while(ptemp->next)
		ptemp = ptemp->next;

	return ptemp;
}
