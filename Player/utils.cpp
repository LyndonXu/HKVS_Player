#include "stdafx.h"
#include "utils.h"

int64_t GetFolderSize(const wchar_t *pPath, PFUN_GetFolderSizeCB pFunCB/* = NULL*/,
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
					//s.push(csTmp);
				}
			}
			else
			{
				if (CFile::GetStatus(csTmp.c_str(), stFileStatus))
				{
					s64Size += stFileStatus.m_size;
				}
			}
		} while (FindNextFile(hFile, &stNextInfo));
	}
	return s64Size;
}

int32_t Convert(const char *pStrSrc, wstring &csWscDest)
{
	if (pStrSrc == NULL)
	{
		return -1;
	}
	int s32SrcLen = strlen(pStrSrc);

	int s32UnicodeLen = MultiByteToWideChar(CP_ACP, 0, pStrSrc, -1, NULL, 0);

	wchar_t * pUnicode;
	pUnicode = new wchar_t[s32UnicodeLen + 1];

	memset(pUnicode, 0, (s32UnicodeLen + 1) * sizeof(wchar_t));

	MultiByteToWideChar(CP_ACP, 0, pStrSrc, -1, (LPWSTR)pUnicode,
		s32UnicodeLen);
	csWscDest = pUnicode;
	delete[] pUnicode;
	return 0;
}
int32_t Convert(const wchar_t *pwStrSrc, string &csStrDrst)
{
	if (pwStrSrc == NULL)
	{
		return -1;
	}
	int s32UnicodeSrcLen =  wcslen(pwStrSrc);

	int s32AnisLen = WideCharToMultiByte(CP_ACP, 0, pwStrSrc, -1, NULL, 0,
		NULL, NULL);

	char * pAnis;
	pAnis = new char[s32AnisLen + 1];

	memset(pAnis, 0, (s32AnisLen + 1) * sizeof(char));

	WideCharToMultiByte(CP_ACP, 0, pwStrSrc, -1, (LPSTR)pAnis,
		s32AnisLen, NULL, NULL);
	csStrDrst = pAnis;
	delete[] pAnis;
	return 0;
}

static bool FolderExists(CString s)
{
	DWORD attr;
	attr = GetFileAttributes(s);
	return (attr != (DWORD)(-1)) &&
		(attr & FILE_ATTRIBUTE_DIRECTORY);
}

bool SuperMkDir(CString P)
{
	int len = P.GetLength();
	if (len < 2)
		return false;

	if ('\\' == P[len - 1])
	{
		P = P.Left(len - 1);
		len = P.GetLength();
	}
	if (len <= 0)
		return false;

	if (len <= 3)
	{
		if (FolderExists(P))
			return true;
		else
			return false;
	}

	if (FolderExists(P))
		return true;



	CString Parent;
	Parent = P.Left(P.ReverseFind('\\'));

	bool Ret = false;
	if (Parent.GetLength() <= 0)
	{
		Ret = true;
	}
	else
	{
		Ret = SuperMkDir(Parent);
	}

	if (Ret)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = 0;
		Ret = (CreateDirectory(P, &sa) == TRUE);
		return Ret;
	}
	else
		return false;
}
