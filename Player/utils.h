#pragma once

#include <stdint.h>
#include <string>
#include <stack>

#include <windows.h>

using namespace std;

#define PRINT(x, ...)	{char temp[512];sprintf_s(temp,512,"[%s, %d]: "##x, __FILE__, __LINE__, ##__VA_ARGS__);OutputDebugStringA(temp);}

class CritSec
{
public:
	CritSec() { InitializeCriticalSection(&m_CritSec); };
	~CritSec() { DeleteCriticalSection(&m_CritSec); };
	void Lock() { EnterCriticalSection(&m_CritSec); };
	void UnLock() { LeaveCriticalSection(&m_CritSec); };

private:
	CRITICAL_SECTION m_CritSec;
};

class AutoLock
{
public:
	AutoLock(CritSec * plock) {
		m_pLock = plock;
		m_pLock->Lock();
	};

	~AutoLock() {
		m_pLock->UnLock();
	};

private:
	CritSec * m_pLock;
};
typedef int32_t(*PFUN_GetFolderSizeCB)(const WCHAR *pName, WIN32_FIND_DATA *pInfo, void *pContext);

int64_t GetFolderSize(const WCHAR *pPath, PFUN_GetFolderSizeCB pFunCB = NULL, 
	void *pContext = NULL);

