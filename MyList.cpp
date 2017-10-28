#include "StdAfx.h"
#include ".\mylist.h"

CLoopList::CLoopList(void)
{
	next = NULL;
	m_pPointHead = NULL;
}

CLoopList::~CLoopList(void)
{
}

void CLoopList::Add(CPointList* pPoint)
{
	CPointList* ptr = m_pPointHead;

	if(!ptr)
		m_pPointHead = pPoint;
	else
	{
		while(ptr->next)
			ptr = ptr->next;
		ptr->next = pPoint;
	}
}

CPointList::CPointList(void)
{
	next = NULL;
	point[0] = point[1] = point[2] = 0;
}

CPointList::~CPointList(void)
{
}