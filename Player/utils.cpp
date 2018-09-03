#include "stdafx.h"
#include "utils.h"

int64_t GetFolderSize(const WCHAR *pPath, PFUN_GetFolderSizeCB pFunCB/* = NULL*/,
	void *pContext/* = NULL*/)
{
	stack<wstring> s;
	s.push(pPath);
	HANDLE hFile = NULL;
	WIN32_FIND_DATA stNextInfo = { 0 };
	CFileStatus stFileStatus = { 0 };
	int64_t s64Size = 0;
	while (!s.empty()) {
		wstring csWStrStatckPath = s.top();
		s.pop();
		wstring csWStrStatckPathComplete = csWStrStatckPath;
		csWStrStatckPathComplete.append(L"\\*.*");
		hFile = FindFirstFile(csWStrStatckPathComplete.c_str(), &stNextInfo);
		do
		{
			wstring csTmp = csWStrStatckPath;
			csTmp.append(L"\\").append(stNextInfo.cFileName);

			if (pFunCB != NULL)
			{
				pFunCB(csTmp.c_str(), &stNextInfo, pContext);
			}

			if (stNextInfo.dwFileAttributes  & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (lstrcmp(stNextInfo.cFileName, L".") && lstrcmp(stNextInfo.cFileName, L".."))
				{
					s.push(csTmp);
				}
			}
			else
			{
				CFile::GetStatus(csTmp.c_str(), stFileStatus);
				s64Size += stFileStatus.m_size;
			}
		} while (FindNextFile(hFile, &stNextInfo));
	}
	return s64Size;
}