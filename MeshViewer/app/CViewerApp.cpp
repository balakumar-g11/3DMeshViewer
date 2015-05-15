#include "../include/app/CViewerApp.h"
#include "../CVector3D.h"
#include <iostream>
#include <algorithm>

using namespace MeshViewer::App::Viewer;
using namespace MeshViewer::Utilities::Math;

CViewerApp* CViewerApp::m_pInstance = NULL;

CViewerApp::CViewerApp()
{
	m_bIsLoaded = false;
	m_bIsSmooth = true;
	m_bShowBoundingBox = false;
	m_bLightControl = true;
	m_bTranslation = CVector3D(0, 0, 0);
}

void CViewerApp::initializeRotation()
{
	m_bRotation[0] = 1.0;
	m_bRotation[1] = 0.0;
	m_bRotation[2] = 0.0;
	m_bRotation[3] = 0.0;
	m_bRotation[4] = 0.0;
	m_bRotation[5] = 1.0;
	m_bRotation[6] = 0.0;
	m_bRotation[7] = 0.0;
	m_bRotation[8] = 0.0;
	m_bRotation[9] = 0.0;
	m_bRotation[10] = 1.0;
	m_bRotation[11] = 0.0;
	m_bRotation[12] = 0.0;
	m_bRotation[13] = 0.0;
	m_bRotation[14] = 0.0;
	m_bRotation[15] = 1.0;
}

CViewerApp::~CViewerApp()
{
	if (m_pFileParser != NULL)
		delete m_pFileParser;
}

CViewerApp* CViewerApp::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CViewerApp();
	}

	return m_pInstance;
}

void CViewerApp::deleteInstance()
{
	if (m_pInstance != NULL)
		delete m_pInstance;
}

bool CViewerApp::loadFile(string strFileName)
{
	bool bResult = false;

	if (m_bIsLoaded && strFileName.compare(this->m_pFileParser->getFileName()) == 0)
	{
		bResult = true;
		return bResult;
	}

	if (strFileName.size() == 0)
		return bResult;

	if (m_pFileParser != NULL)
		delete m_pFileParser;

	m_pFileParser = new CMFileParser(strFileName);


	if (m_pFileParser != NULL)
	{
		m_mapVertInfo.clear();
		m_vecFaceInfo.clear();
		m_vecEdgeInfo.clear();

		bResult = m_pFileParser->createHalfEdgeLists(m_mapVertInfo, m_vecFaceInfo, m_vecEdgeInfo);

		updateFaceNormals();
		updateVertexNormals();
		findBoundingBox();
		initializeRotation();

		if (bResult)
			m_bIsLoaded = true;
	}
		
	bResult = true;
	return bResult;
}

double CViewerApp::calculateTotalArea(unsigned iVertID)
{
	HalfEdge_Edge* pInitEdge = m_mapVertInfo[iVertID]->m_pEdge;
	HalfEdge_Edge* pCurrEdge = pInitEdge;

	double dTotalArea = 0.0;

	bool isBoundary = true;
	while (pCurrEdge->m_pPairEdge != NULL)
	{
		if (pCurrEdge->m_pPairEdge->m_pNextEdge == pInitEdge)
		{
			isBoundary = false;
			break;
		}
		dTotalArea += pCurrEdge->m_pIncidentFace->m_fArea;

		pCurrEdge = pCurrEdge->m_pPairEdge->m_pNextEdge;
	}
	if (isBoundary)
	{
		pCurrEdge = pInitEdge;

		while (pCurrEdge->m_pPrevEdge->m_pPairEdge != NULL)
		{
			if (pCurrEdge->m_pPrevEdge->m_pPairEdge == pInitEdge)
			{
				break;
			}

			pCurrEdge = pCurrEdge->m_pPrevEdge->m_pPairEdge;
			dTotalArea += pCurrEdge->m_pIncidentFace->m_fArea;
		}
	}

	return dTotalArea;

}

bool CViewerApp::updateVertexNormals()
{
	bool bResult = false;

	map<unsigned, HalfEdge_Vert*>::iterator vertIter = m_mapVertInfo.begin();
	for (; vertIter != m_mapVertInfo.end(); vertIter++)
	{
		CVector3D vertNormal;
		double dTotalArea = calculateTotalArea(vertIter->first);

		HalfEdge_Edge* pInitEdge = (*vertIter).second->m_pEdge;
		HalfEdge_Edge* pCurrEdge = pInitEdge;

		bool isBoundary = true;
		while ( pCurrEdge->m_pPairEdge != NULL)
		{
			if (pCurrEdge->m_pPairEdge->m_pNextEdge == pInitEdge)
			{
				isBoundary = false;
				break;
			}

			double dWeight = (pCurrEdge->m_pIncidentFace->m_fArea / dTotalArea);

			vertNormal = (vertNormal + (pCurrEdge->m_pIncidentFace->m_faceNormal * dWeight));
			dTotalArea += pCurrEdge->m_pIncidentFace->m_fArea;

			pCurrEdge = pCurrEdge->m_pPairEdge->m_pNextEdge;
		}
		if (isBoundary)
		{
			pCurrEdge = pInitEdge;

			while (pCurrEdge->m_pPrevEdge->m_pPairEdge != NULL)
			{
				if (pCurrEdge->m_pPrevEdge->m_pPairEdge == pInitEdge)
				{
					break;
				}

				pCurrEdge = pCurrEdge->m_pPrevEdge->m_pPairEdge;
				dTotalArea += pCurrEdge->m_pIncidentFace->m_fArea;

				double dWeight = (pCurrEdge->m_pIncidentFace->m_fArea / dTotalArea);

				vertNormal = (vertNormal + (pCurrEdge->m_pIncidentFace->m_faceNormal * dWeight));
			}
		}

		double length = vertNormal.length();

		(*vertIter).second->m_vertNormal = vertNormal / length;

	}

	return bResult;
}

bool CViewerApp::updateFaceNormals()
{
	bool bResult = false;

	vector<HalfEdge_Face*>::iterator faceIter = m_vecFaceInfo.begin();
	for (; faceIter != m_vecFaceInfo.end(); faceIter++)
	{
		CVector3D firstEdge = ((*faceIter)->m_pEdge->m_pEndVert->m_vertInfo - (*faceIter)->m_pEdge->m_pPrevEdge->m_pEndVert->m_vertInfo);
		CVector3D secondEdge = ((*faceIter)->m_pEdge->m_pNextEdge->m_pEndVert->m_vertInfo - (*faceIter)->m_pEdge->m_pEndVert->m_vertInfo);

		CVector3D crossProduct = CVector3D::crossProduct(firstEdge, secondEdge);
		double length = crossProduct.length();

		(*faceIter)->m_faceNormal = crossProduct;
		(*faceIter)->m_fArea = 0.5f * length;
	}
	
	return bResult;
}

bool CViewerApp::findBoundingBox()
{
	bool bResult = false;

	vector<HalfEdge_Vert*> vecVert;
	vector<double> vecMaxVal;

	map<unsigned, HalfEdge_Vert*>::iterator vertIter = m_mapVertInfo.begin();
	for (; vertIter != m_mapVertInfo.end(); vertIter++)
	{
		vecVert.push_back(vertIter->second);
	}

	sort(vecVert.begin(), vecVert.end(), HalfEdge_Vert::greaterByX);

	m_bMaxVert.x = vecVert.front()->m_vertInfo.x;
	m_bMinVert.x = vecVert.back()->m_vertInfo.x;

	vecMaxVal.push_back(m_bMaxVert.x);

	sort(vecVert.begin(), vecVert.end(), HalfEdge_Vert::greaterByY);

	m_bMaxVert.y = vecVert.front()->m_vertInfo.y;
	m_bMinVert.y = vecVert.back()->m_vertInfo.y;

	vecMaxVal.push_back(m_bMaxVert.y);

	sort(vecVert.begin(), vecVert.end(), HalfEdge_Vert::greaterByZ);

	m_bMaxVert.z = vecVert.front()->m_vertInfo.z;
	m_bMinVert.z = vecVert.back()->m_vertInfo.z;

	vecMaxVal.push_back(m_bMaxVert.z);

	sort(vecMaxVal.begin(), vecMaxVal.end());

	m_dScaleFactor = vecMaxVal.back();

	bResult = true;
	return bResult;
}

bool CViewerApp::showPointCloud()
{
	bool bResult = false;

	if (m_mapVertInfo.size() == 0)
		return bResult;

	glTranslated(m_bTranslation.x, m_bTranslation.y, m_bTranslation.z);
	glMultMatrixf(m_bRotation);
	glScaled(1 / m_dScaleFactor, 1 / m_dScaleFactor, 1 / m_dScaleFactor);

	if(m_bLightControl)
		EnableLighting();

	glPointSize(2.0f);
	glShadeModel(GL_SMOOTH);
	glBegin(GL_POINTS);
	map<unsigned, HalfEdge_Vert*>::iterator vertIter = m_mapVertInfo.begin();
	for ( ; vertIter != m_mapVertInfo.end(); vertIter++)
	{
		glNormal3d(vertIter->second->m_vertNormal.x, vertIter->second->m_vertNormal.y, vertIter->second->m_vertNormal.z);
		glVertex3f(vertIter->second->m_vertInfo.x, vertIter->second->m_vertInfo.y, vertIter->second->m_vertInfo.z);
	}
	glEnd();
	glFlush();
	
	if (m_bLightControl)
		DisableLighting();

	if (m_bShowBoundingBox)
		drawBoundingBox();
	
	bResult = true;
	return bResult;
}

void CViewerApp::drawEdges()
{
	vector<HalfEdge_Face*>::iterator faceIter = m_vecFaceInfo.begin();
	for (; faceIter != m_vecFaceInfo.end(); faceIter++)
	{
		HalfEdge_Edge* pInitEdge = (*faceIter)->m_pEdge;
		HalfEdge_Edge* pCurrEdge = pInitEdge;

		if (!m_bIsSmooth)
			glNormal3d((*faceIter)->m_faceNormal.x, (*faceIter)->m_faceNormal.y, (*faceIter)->m_faceNormal.z);

		glBegin(GL_LINE_LOOP);
		do
		{
			if (m_bIsSmooth)
				glNormal3d(pCurrEdge->m_pEndVert->m_vertNormal.x, pCurrEdge->m_pEndVert->m_vertNormal.y, pCurrEdge->m_pEndVert->m_vertNormal.z);

			glVertex3f(pCurrEdge->m_pEndVert->m_vertInfo.x, pCurrEdge->m_pEndVert->m_vertInfo.y, pCurrEdge->m_pEndVert->m_vertInfo.z);
			pCurrEdge = pCurrEdge->m_pNextEdge;
		} while (pCurrEdge != pInitEdge);
		glEnd();
	}
	glFlush();
}

bool CViewerApp::showWireFrame()
{
	bool bResult = false;

	if (m_vecFaceInfo.size() == 0)
		return bResult;

	glTranslated(m_bTranslation.x, m_bTranslation.y, m_bTranslation.z);
	glMultMatrixf(m_bRotation);
	glScaled(1 / m_dScaleFactor, 1 / m_dScaleFactor, 1 / m_dScaleFactor);

	if (m_bLightControl)
		EnableLighting();
	
	if (m_bIsSmooth)
		glShadeModel(GL_SMOOTH);
	else
		glShadeModel(GL_FLAT);

	drawEdges();

	if (m_bLightControl)
		DisableLighting();

	if (m_bShowBoundingBox)
		drawBoundingBox();

	bResult = true;
	return bResult;
}

bool CViewerApp::showPolygonModel()
{
	bool bResult = false;

	if (m_vecFaceInfo.size() == 0)
		return bResult;

	glTranslated(m_bTranslation.x, m_bTranslation.y, m_bTranslation.z);
	glMultMatrixf(m_bRotation);
	glScaled(1 / m_dScaleFactor, 1 / m_dScaleFactor, 1 / m_dScaleFactor);

	if (m_bLightControl)
		EnableLighting();

	if (m_bIsSmooth)
		glShadeModel(GL_SMOOTH);
	else
		glShadeModel(GL_FLAT);
	glBegin(GL_TRIANGLES);
	vector<HalfEdge_Face*>::iterator faceIter = m_vecFaceInfo.begin();
	for (; faceIter != m_vecFaceInfo.end(); faceIter++)
	{
		HalfEdge_Edge* pInitEdge = (*faceIter)->m_pEdge;
		HalfEdge_Edge* pCurrEdge = pInitEdge;

		if (!m_bIsSmooth)
			glNormal3d((*faceIter)->m_faceNormal.x, (*faceIter)->m_faceNormal.y, (*faceIter)->m_faceNormal.z);

		do
		{
			if (m_bIsSmooth)
				glNormal3d(pCurrEdge->m_pEndVert->m_vertNormal.x, pCurrEdge->m_pEndVert->m_vertNormal.y, pCurrEdge->m_pEndVert->m_vertNormal.z);
			
			glVertex3d(pCurrEdge->m_pEndVert->m_vertInfo.x, pCurrEdge->m_pEndVert->m_vertInfo.y, pCurrEdge->m_pEndVert->m_vertInfo.z);
			
			pCurrEdge = pCurrEdge->m_pNextEdge;
		}while(pCurrEdge != pInitEdge);
	}
	glEnd();
	glFlush();

	if (m_bLightControl)
		DisableLighting();

	if (m_bShowEdges)
	{
		glColor3f(0, 0, 0);
		glLineWidth(1.5f);
		drawEdges();
	}
		
	if (m_bShowBoundingBox)
		drawBoundingBox();

	bResult = true;
	return bResult;
}

void CViewerApp::drawGround()
{
	GLfloat fSizes[2];     // Line width range metrics
	GLfloat fCurrSize;
	GLfloat fStep;

	glGetFloatv(GL_LINE_WIDTH_GRANULARITY, &fStep);
	glGetFloatv(GL_LINE_WIDTH_RANGE, fSizes);
	fCurrSize = fSizes[1];

	for (int i = -25; i <= 25; i++)
	{
		glBegin(GL_LINES);
		glEnable(GL_LINE_SMOOTH);
		if (i == 0)
		{
			fCurrSize += fStep;
			glLineWidth(fCurrSize);
			fCurrSize -= fStep;
		}
		else
			glLineWidth(fCurrSize);
		glColor3d(0.0f, 0.0f, 0.0f);
		
		glVertex3d((double)i, 0.0, -25);
		glVertex3d((double)i, 0.0, 25);
		
		glEnd();
		glFlush();
	}

	for (int i = -25; i <= 25; i++)
	{
		glBegin(GL_LINES);
		glEnable(GL_LINE_SMOOTH);
		if (i == 0)
		{
			fCurrSize += fStep;
			glLineWidth(fCurrSize);
			fCurrSize -= fStep;
		}
		else
			glLineWidth(fCurrSize);
		glColor3d(0.0f, 0.0f, 0.0f);

		glVertex3d(-25, 0.0, double(i));
		glVertex3d(25, 0.0, (double)i);
		
		glEnd();
		glFlush();
	}
}

void CViewerApp::drawBoundingBox()
{
	glColor3f(0, 0, 0);
	glLineWidth(2);
	glBegin(GL_LINES);
	glVertex3f(m_bMinVert.x, m_bMinVert.y, m_bMinVert.z);
	glVertex3f(m_bMaxVert.x, m_bMinVert.y, m_bMinVert.z);

	glVertex3f(m_bMaxVert.x, m_bMinVert.y, m_bMinVert.z);
	glVertex3f(m_bMaxVert.x, m_bMaxVert.y, m_bMinVert.z);

	glVertex3f(m_bMaxVert.x, m_bMaxVert.y, m_bMinVert.z);
	glVertex3f(m_bMinVert.x, m_bMaxVert.y, m_bMinVert.z);

	glVertex3f(m_bMinVert.x, m_bMaxVert.y, m_bMinVert.z);
	glVertex3f(m_bMinVert.x, m_bMinVert.y, m_bMinVert.z);

	glVertex3f(m_bMinVert.x, m_bMinVert.y, m_bMinVert.z);
	glVertex3f(m_bMinVert.x, m_bMinVert.y, m_bMaxVert.z);

	glVertex3f(m_bMinVert.x, m_bMinVert.y, m_bMaxVert.z);
	glVertex3f(m_bMaxVert.x, m_bMinVert.y, m_bMaxVert.z);

	glVertex3f(m_bMaxVert.x, m_bMinVert.y, m_bMaxVert.z);
	glVertex3f(m_bMaxVert.x, m_bMinVert.y, m_bMinVert.z);

	glVertex3f(m_bMaxVert.x, m_bMinVert.y, m_bMaxVert.z);
	glVertex3f(m_bMaxVert.x, m_bMaxVert.y, m_bMaxVert.z);

	glVertex3f(m_bMaxVert.x, m_bMaxVert.y, m_bMaxVert.z);
	glVertex3f(m_bMinVert.x, m_bMaxVert.y, m_bMaxVert.z);

	glVertex3f(m_bMinVert.x, m_bMaxVert.y, m_bMaxVert.z);
	glVertex3f(m_bMinVert.x, m_bMaxVert.y, m_bMinVert.z);

	glVertex3f(m_bMaxVert.x, m_bMaxVert.y, m_bMaxVert.z);
	glVertex3f(m_bMaxVert.x, m_bMaxVert.y, m_bMinVert.z);

	glVertex3f(m_bMinVert.x, m_bMaxVert.y, m_bMaxVert.z);
	glVertex3f(m_bMinVert.x, m_bMinVert.y, m_bMaxVert.z);

	glEnd();
	glFlush();
}

void drawCylinder()
{
	GLUquadric* quadobj = gluNewQuadric();
	gluCylinder(quadobj, 0.01, 0.01, 3, 3, 3);
	glPushMatrix();
	glTranslated(0, 0, 3);
	glutSolidCone(0.05, 0.1, 3, 3);
	glPopMatrix();
}

void CViewerApp::drawAxes()
{
	glColor3d(1, 0, 0);
	drawCylinder();
	glPushMatrix();
	glRotated(90, 0, 1, 0);
	glColor3d(0, 1, 0);
	drawCylinder();
	glPopMatrix();
	glPushMatrix();
	glRotated(-90, 1, 0, 0);
	glColor3d(0, 0, 1);
	drawCylinder();
	glPopMatrix();
}

void CViewerApp::changeShadingType(int iShading)
{
	if (iShading == 1)
	{
		m_bIsSmooth = true;
	}
	else
	{
		m_bIsSmooth = false;
	}
}

void CViewerApp::EnableLighting()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
}

void CViewerApp::DisableLighting()
{
	glDisable(GL_NORMALIZE);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
}

void CViewerApp::ShowBoundingBox(int show)
{
	if (show == 1)
		m_bShowBoundingBox = true;
	else
		m_bShowBoundingBox = false;
}

void CViewerApp::LightOnOff(int light)
{
	if (light == 1)
		m_bLightControl = true;
	else
		m_bLightControl = false;
}

void CViewerApp::ShowEdges(int show)
{
	if (show == 1)
		m_bShowEdges = true;
	else
		m_bShowEdges = false;
}
