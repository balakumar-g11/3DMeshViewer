#ifndef I_FILE_
#define I_FILE_

#include "CDataTypes.h"
#include <string>

using namespace std;

namespace MeshViewer{
	namespace Utilities{
		namespace File{
			class IFile
			{
			public:
				virtual bool openFile() = 0;

				virtual bool setFileName(IN string strFileName) = 0;

				virtual bool getFileName(OUT string& strFileName) = 0;
			};
		}
	}
}

#endif
