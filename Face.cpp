#include "StdAfx.h"
#include ".\face.h"

CFace::CFace(void)
{
	static int id = 0;
	face_id = id ++;
	feq[0] = feq[1] = feq[2] = 0;
	host_s = NULL;
	loop_h = NULL;
	next = prev = NULL;
}

CFace::~CFace(void)
{
}

BOOL CFace::AddLoop(CLoop* ploop)
{
	if(!ploop)
	{
		return FALSE;
	}

	CLoop*	ptemp = loop_h;

	if(!ptemp)
		loop_h = ploop;
	else
	{
		while(ptemp->next)
		{
			ptemp = ptemp->next;
		}
		ptemp->next = ploop;
		ploop->prev = ptemp;
	}
	ploop->host_f = this;

	return TRUE;
}