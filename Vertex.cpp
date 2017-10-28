#include "StdAfx.h"
#include ".\vertex.h"

CVertex::CVertex(void)
{
	static int id = 0;
	vertex_id = id ++;
	vcoord[0] = vcoord[1] = vcoord[2] = 0;
	next = prev = NULL;
}

CVertex::~CVertex(void)
{
}

void CVertex::SetCoord(GLdouble *point)
{
	vcoord[0] = point[0];
	vcoord[1] = point[1];
	vcoord[2] = point[2];
}