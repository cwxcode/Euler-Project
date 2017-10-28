#pragma once

class CNode
{
public:
	CNode(void);
	~CNode(void);

public:
	virtual CNode *new(CNode *pParent);
	virtual void addlist(CNode *pIn, CNode *pParent);
};
