#ifndef C_MFILE_PARSER_
#define C_MFILE_PARSER_

#include "CFile.h"
#include <vector>
#include <map>
#include "CDataTypes.h"

using namespace std;
using namespace MeshViewer::Utilities::File;

namespace MeshViewer{
	namespace Utilities{
		namespace Parser{
			class CMFileParser : public CFile
			{
			public:
				CMFileParser(IN string strFileName);

				~CMFileParser();

				bool createHalfEdgeLists(OUT map<unsigned, HalfEdge_Vert*>&, OUT vector<HalfEdge_Face*>&,
						OUT vector<HalfEdge_Edge*>&);
			
			private:
				bool getVertexInfo(OUT map<unsigned, HalfEdge_Vert*>&, OUT map<unsigned, vector<unsigned>>&);

				bool formFacesAndEdges(IN map<unsigned, vector<unsigned>>&, INOUT map<unsigned, HalfEdge_Vert*>&, OUT vector<HalfEdge_Face*>&,
						OUT vector<HalfEdge_Edge*>&);
			};
		}
	}
}

#endif
