#include "CMFileParser.h"
#include <iostream>
#include <sstream>
#include <glut.h>
#include <gl/GL.h>

using namespace std;
using namespace MeshViewer::Utilities::Parser;

CMFileParser::CMFileParser(IN string strFileName) : CFile(strFileName)
{
	
}

CMFileParser::~CMFileParser()
{

}

bool CMFileParser::getVertexInfo(OUT map<unsigned, HalfEdge_Vert*>& mapVertInfo, OUT map<unsigned, vector<unsigned>>& mapFaceInfo)
{
	bool bResult = false;
	
	if(isFileOpen())
	{
		string strLine = "";
		while (getline(m_fSourceFile, strLine))
		{
			vector<string> tokens;
			string delimiter = " ";
			istringstream strStream(strLine);

			string strInfoType = "";

			strStream >> strInfoType;

			if (strInfoType.compare("Vertex") == 0)
			{
				int iVertID;
				strStream >> iVertID;

				HalfEdge_Vert* pVert = new HalfEdge_Vert();

				pVert->m_iVertID = iVertID;
				
				strStream >> pVert->m_vertInfo.x;
				strStream >> pVert->m_vertInfo.y;
				strStream >> pVert->m_vertInfo.z;

				mapVertInfo[iVertID] = pVert;
			}
			else if (strInfoType.compare("Face") == 0)
			{
				int iFaceID;
				strStream >> iFaceID;

				int iVertID;
				while (strStream >> iVertID)
				{
					mapFaceInfo[iFaceID].push_back(iVertID);
				}
				if (mapFaceInfo[iFaceID].size() < MIN_VERT_FOR_FACE)
					mapFaceInfo.erase(iFaceID);
			}

		}

		bResult = true;
	}

	return bResult;
}

bool CMFileParser::formFacesAndEdges(IN map<unsigned, vector<unsigned>>& mapFaceDetails, INOUT map<unsigned, HalfEdge_Vert*>& mapVertInfo,
	OUT vector<HalfEdge_Face*>& vecFaceInfo, OUT vector<HalfEdge_Edge*>& vecEdgeInfo)
{
	bool bResult = false;

	map<pair<unsigned, unsigned>, HalfEdge_Edge*> mapFormedEdges;

	map<unsigned, vector<unsigned>>::iterator faceIter = mapFaceDetails.begin();
	for (; faceIter != mapFaceDetails.end(); faceIter++)
	{
		HalfEdge_Face* pFace = new HalfEdge_Face();
		pFace->m_iFaceID = faceIter->first;
		vecFaceInfo.push_back(pFace);
		vector<unsigned>::iterator faceVertIter = (*faceIter).second.begin();
		int firstVertex = (*faceVertIter);
		HalfEdge_Edge* pPrevEdge = NULL;
		HalfEdge_Edge* pFirstEdge = NULL;
		for (; faceVertIter != (*faceIter).second.end(); faceVertIter++)
		{
			int iCurrVertex = (*faceVertIter);
			int iNextVertex = firstVertex;
			bool bIsLastEdge = false;
			faceVertIter++;
						
			if (faceVertIter != (*faceIter).second.end())
				iNextVertex = (*faceVertIter);
			else
				bIsLastEdge = true;

			HalfEdge_Edge* pEdge = new HalfEdge_Edge();
			pEdge->m_pIncidentFace = pFace;
			if (pPrevEdge == NULL)
			{
				pFirstEdge = pEdge;
				pFace->m_pEdge = pEdge;
			}
			else
			{
				pPrevEdge->m_pNextEdge = pEdge;
				pEdge->m_pPrevEdge = pPrevEdge;
			}

			if (mapVertInfo[iCurrVertex]->m_pEdge == NULL)
			{
				mapVertInfo[iCurrVertex]->m_pEdge = pEdge;
			}

			if (bIsLastEdge)
			{
				pEdge->m_pEndVert = mapVertInfo[firstVertex];
				pEdge->m_pNextEdge = pFirstEdge;
				pFirstEdge->m_pPrevEdge = pEdge;
			}
			else
			{
				pEdge->m_pEndVert = mapVertInfo[iNextVertex];
			}

			pPrevEdge = pEdge;

			mapFormedEdges[make_pair(iCurrVertex, iNextVertex)] = pEdge;

			if (mapFormedEdges.find(make_pair(iNextVertex, iCurrVertex)) != mapFormedEdges.end())
			{
				mapFormedEdges[make_pair(iNextVertex, iCurrVertex)]->m_pPairEdge = pEdge;
				pEdge->m_pPairEdge = mapFormedEdges[make_pair(iNextVertex, iCurrVertex)];
			}
			faceVertIter--;
		}
	}

	map<pair<unsigned, unsigned>, HalfEdge_Edge*>::iterator edgeIter = mapFormedEdges.begin();
	for (; edgeIter != mapFormedEdges.end(); edgeIter++)
	{
		vecEdgeInfo.push_back(edgeIter->second);
	}

	return bResult;
}

bool CMFileParser::createHalfEdgeLists(OUT map<unsigned, HalfEdge_Vert*>& mapVertInfo, OUT vector<HalfEdge_Face*>& vecFaceInfo,
		OUT vector<HalfEdge_Edge*>& vecEdgeInfo)
{
	bool bResult = false;

	openFile();

	map<unsigned, vector<unsigned>> mapFaceDetails;

	bResult = getVertexInfo(mapVertInfo, mapFaceDetails);

	if (!bResult)
		return bResult;

	bResult = formFacesAndEdges(mapFaceDetails, mapVertInfo, vecFaceInfo, vecEdgeInfo);

	cout << "Number of Faces : " << vecFaceInfo.size() << endl;
	cout << "Number of Edges : " << vecEdgeInfo.size() / 2 << endl;

	closeFile();

	bResult = true;
	return bResult;
}
