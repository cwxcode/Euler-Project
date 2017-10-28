#pragma once

#include "Loop.h"

class CSolid;

class CFace
{
public:
	CFace(void);
	~CFace(void);

private:
	CLoop	*loop_h;

public:
	int			face_id;
	CSolid		*host_s;
	CFace		*next;
	CFace		*prev;
	GLdouble	feq[3];

	BOOL AddLoop(CLoop* ploop);
	CLoop* GetLoopHead(void) { return loop_h; }
};
