#pragma once

#include "afxtempl.h"

class CPointList;
class CLoopList
{
public:
	CLoopList(void);
	~CLoopList(void);

public:
	CPointList*	m_pPointHead;
	CLoopList*	next;

public:
	void Add(CPointList* pPoint);
};

class CPointList
{
public:
	CPointList(void);
	~CPointList(void);

public:
	CPointList*	next;

public:
	GLdouble	point[3];
};