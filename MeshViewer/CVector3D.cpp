#include "CVector3D.h"
#include <math.h>

using namespace std;
using namespace MeshViewer::Utilities::Math;

CVector3D::CVector3D(double fVertexX, double fVertexY, double fVertexZ)
{
	x = fVertexX;
	y = fVertexY;
	z = fVertexZ;
}

CVector3D::CVector3D()
{
	x = 0;
	y = 0;
	z = 0;
}

CVector3D::~CVector3D()
{

}

CVector3D::CVector3D(const CVector3D& copy)
{
	x = copy.x;
	y = copy.y;
	z = copy.z;
}

CVector3D CVector3D::operator+(CVector3D& cVertex)
{
	return CVector3D(x + cVertex.x, y + cVertex.y, z + cVertex.z);;
}

CVector3D CVector3D::operator-(CVector3D& cVertex)
{
	return CVector3D(x - cVertex.x, y - cVertex.y, z - cVertex.z);
}

void CVector3D::operator=(const CVector3D& cVertex)
{
	x = cVertex.x;
	y = cVertex.y;
	z = cVertex.z;
}

CVector3D CVector3D::operator*(double& fLambda)
{
	return CVector3D(fLambda * x, fLambda * y, fLambda * z);
}

CVector3D CVector3D::operator/(double& fDivBy)
{
	return CVector3D(x/fDivBy, y/fDivBy, z/fDivBy);
}

double CVector3D::dotProduct(CVector3D& cVertex1, CVector3D& cVertex2)
{
	return ((cVertex1.x*cVertex2.x) + (cVertex1.y*cVertex2.y) + (cVertex1.z*cVertex2.z));
}

CVector3D CVector3D::crossProduct(CVector3D& cVertex1, CVector3D& cVertex2)
{
	double fVertexX = ((cVertex1.y * cVertex2.z) - (cVertex1.z * cVertex2.y));
	double fVertexY = ((cVertex1.z * cVertex2.x) - (cVertex1.x * cVertex2.z));
	double fVertexZ = ((cVertex1.x * cVertex2.y) - (cVertex1.y * cVertex2.x));

	return CVector3D(fVertexX, fVertexY, fVertexZ);
}

double CVector3D::length()
{
	return sqrt((x * x) + (y * y) + (z * z));
}
