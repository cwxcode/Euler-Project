#pragma once

class CHalfEdge;

class CEdge
{
public:
	CEdge(void);
	~CEdge(void);

private:

public:
	CHalfEdge	*he1;
	CHalfEdge	*he2;
	CEdge		*next;
	CEdge		*prev;
};
