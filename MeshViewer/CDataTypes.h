#ifndef C_DATA_TYPE_
#define C_DATA_TYPE_

#include <vector>
#include <utility>
#include <glut.h>
#include <gl/GL.h>
#include "CVector3D.h"

using namespace std;
using namespace MeshViewer::Utilities::Math;

#define IN
#define OUT
#define INOUT

#define THREE_DIMENSION 3
#define MIN_VERT_FOR_FACE 3

enum CameraType{
	CAMERA_3D = 0,
	CAMERA_XZ,
	CAMERA_YZ,
	CAMERA_XY
};


struct HalfEdge_Edge;

struct ColorType
{
	float red;
	float green;
	float blue;
	float alpha;
};

struct HalfEdge_Vert
{
	//Vertex ID
	unsigned m_iVertID;

	//X, Y, Z information of the vertex
	CVector3D m_vertInfo;

	//One of the edges starting from vertex
	HalfEdge_Edge* m_pEdge;

	//Vertex normal
	CVector3D m_vertNormal;
	
	HalfEdge_Vert()
	{
		m_pEdge = NULL;
	}

	static bool greaterByX(HalfEdge_Vert* vert1, HalfEdge_Vert* vert2)
	{
		return vert1->m_vertInfo.x > vert2->m_vertInfo.x;
	}

	static bool greaterByY(HalfEdge_Vert* vert1, HalfEdge_Vert* vert2)
	{
		return vert1->m_vertInfo.y > vert2->m_vertInfo.y;
	}

	static bool greaterByZ(HalfEdge_Vert* vert1, HalfEdge_Vert* vert2)
	{
		return vert1->m_vertInfo.z > vert2->m_vertInfo.z;
	}

};

struct HalfEdge_Face
{
	//Face ID
	unsigned m_iFaceID;

	//One of the edges bordering the face
	HalfEdge_Edge* m_pEdge;

	//Face normal
	CVector3D m_faceNormal;

	//Area of Face
	double m_fArea;

	HalfEdge_Face()
	{
		m_pEdge = NULL;
	}
};

struct HalfEdge_Edge
{
	//Twin edge of the respective edge
	HalfEdge_Edge* m_pPairEdge;

	//Next Edge around the face
	HalfEdge_Edge* m_pNextEdge;

	//Previous Edge around the face
	HalfEdge_Edge* m_pPrevEdge;

	//End Vertex information
	HalfEdge_Vert* m_pEndVert;

	//Incident Face of the edge
	HalfEdge_Face* m_pIncidentFace;

	HalfEdge_Edge()
	{
		m_pPairEdge = NULL;
		m_pNextEdge = NULL;
		m_pPrevEdge = NULL;
		m_pEndVert = NULL;
		m_pIncidentFace = NULL;
	}
};

#endif
