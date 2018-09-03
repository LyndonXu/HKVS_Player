#pragma once

#include <map>
#include <string>

#include <IPHlpApi.h>

using namespace std;

typedef struct _tagStNetworkInfo
{
	UINT64 u64PreInByte;
	UINT64 u64PreOutByte;
	UINT32 u32PreGetTime;
	UINT32 u32InSpeed;
	UINT32 u32OutSpeed;
	UINT32 u32Statistics;
}StNetworkInfo;

typedef map<string, StNetworkInfo> CMapNetworkInfo;
typedef map<string, StNetworkInfo>::iterator CMapNetworkInfoIter;

class CGetSysInfo
{
public:
	CGetSysInfo(void);
	~CGetSysInfo(void);

public:

	INT32 GetCpuIdle(UINT32 &u32Idle);
	INT32 GetMemInfo(UINT64 &u64Total, UINT32 &u32Used);
	INT32 GetNetworkInfo(UINT32 &u32InterfaceCount);
	INT32 GetNetworkInfo(UINT32 u32Index, UINT32 &u32InSpeed, UINT32 &u32OutSpeed);


private:
	FILETIME m_stPreIdleTime;
	FILETIME m_stPreKernelTime;
	FILETIME m_stPreUserTime;

	CMapNetworkInfo m_csMapNetwork;
	UINT32 m_u32NetworkStatistics;
};
