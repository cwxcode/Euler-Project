#pragma once

class CHalfEdge;

class CVertex
{
public:
	CVertex(void);
	~CVertex(void);

public:
	int			vertex_id;
	GLdouble	vcoord[3];
	CVertex		*next;
	CVertex		*prev;

public:
	void SetCoord(GLdouble *point);
};
