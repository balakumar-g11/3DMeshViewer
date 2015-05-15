#include "CFile.h"

using namespace std;
using namespace MeshViewer::Utilities::File;

CFile::CFile(IN string strFileName)
{
	if(strFileName.size() > 0)
	{
		m_strFileName = strFileName;
	}

	m_bFileOpen = false;
}

CFile::~CFile()
{

}

bool CFile::openFile()
{
	bool bResult = false;

	if (!m_bFileOpen)
	{
		m_fSourceFile.open(m_strFileName);
	}

	if (m_fSourceFile.is_open())
	{
		m_bFileOpen = true;
		bResult = true;
	}

	return bResult;
}

bool CFile::closeFile()
{
	bool bResult = false;

	if (m_bFileOpen)
	{
		m_fSourceFile.close();
	}

	bResult = true;
	return bResult;
}

string CFile::getFileName()
{
	return m_strFileName;
}

bool CFile::setFileName(IN string strFileName)
{
	bool bResult = false;

	if (strFileName.size() > 0)
	{
		m_strFileName = strFileName;
		bResult = true;
	}

	return bResult;
}

bool CFile::isFileOpen()
{
	return m_bFileOpen;
}
