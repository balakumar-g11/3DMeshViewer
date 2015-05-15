#ifndef C_VIEWER_APP_
#define C_VIEWER_APP_

#include "../../CMFileParser.h"
#include "../../CDataTypes.h"

#include <glut.h>
#include <gl/GL.h>

using namespace std;
using namespace MeshViewer::Utilities::Parser;

namespace MeshViewer
{
	namespace App
	{
		namespace Viewer
		{
			class CViewerApp
			{
			public:
				bool loadFile(string strFileName);

				bool showPointCloud();

				bool showWireFrame();

				bool showPolygonModel();

				static CViewerApp* getInstance();

				static void deleteInstance();

				void changeShadingType(int iShading);

				void drawGround();

				void drawBoundingBox();

				void drawAxes();

				void EnableLighting();
				
				void DisableLighting();

				void ShowBoundingBox(int show);

				void LightOnOff(int light);

				void ShowEdges(int show);

				CVector3D m_bTranslation;

				GLfloat m_bRotation[16];

			private:
				CViewerApp();

				CViewerApp(const CViewerApp&);

				void operator=(const CViewerApp&);

				~CViewerApp();

				bool updateFaceNormals();

				bool updateVertexNormals();

				bool findBoundingBox();
				
				void drawEdges();

				void initializeRotation();

				double calculateTotalArea(unsigned iVertID);

				static CViewerApp* m_pInstance;

				CMFileParser* m_pFileParser;

				map<unsigned, HalfEdge_Vert*> m_mapVertInfo;

				vector<HalfEdge_Face*> m_vecFaceInfo;

				vector<HalfEdge_Edge*> m_vecEdgeInfo;

				bool m_bIsSmooth;

				bool m_bIsLoaded;

				double m_dScaleFactor;

				bool m_bShowBoundingBox;

				bool m_bShowEdges;

				bool m_bLightControl;

				CVector3D m_bMaxVert;

				CVector3D m_bMinVert;
			};
		}
	}
}

#endif
