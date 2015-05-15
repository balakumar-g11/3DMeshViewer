#ifndef C_FILE_
#define C_FILE_

#include <string>
#include <fstream>
#include "CDataTypes.h"

using namespace std;

namespace MeshViewer{
	namespace Utilities{
		namespace File{
			class CFile
			{
			public:
				CFile(IN string strFileName);

				virtual ~CFile();

				bool openFile();

				bool isFileOpen();

				bool closeFile();

				bool setFileName(IN string strFileName);

				string getFileName();

			protected:
				fstream m_fSourceFile;

			private:
				string m_strFileName;

				bool m_bFileOpen;
			};
		}
	}
}

#endif
