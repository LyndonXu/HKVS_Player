#include "stdafx.h"
#include <stdint.h>

#include <set>

#include <timeapi.h>

#include "utils.h"
#include "GetSysInfo.h"

#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Winmm.lib")


CGetSysInfo::CGetSysInfo(void)
{
	 GetSystemTimes(&m_stPreIdleTime, &m_stPreKernelTime, &m_stPreUserTime);
	 m_u32NetworkStatistics = 1;
}

CGetSysInfo::~CGetSysInfo(void)
{
}

int64_t CompareFileTime(FILETIME time1, FILETIME time2)
{
	int64_t a = (int64_t)time1.dwHighDateTime << 32 | time1.dwLowDateTime;
	int64_t b = (int64_t)time2.dwHighDateTime << 32 | time2.dwLowDateTime;

	return   (b - a);
}
INT32 CGetSysInfo::GetCpuIdle(UINT32 &u32Idle)
{
	FILETIME idleTime;
	FILETIME kernelTime;
	FILETIME userTime;

	GetSystemTimes(&idleTime, &kernelTime, &userTime);

	int64_t idle = CompareFileTime(m_stPreIdleTime, idleTime);
	int64_t kernel = CompareFileTime(m_stPreKernelTime, kernelTime);
	int64_t user = CompareFileTime(m_stPreUserTime, userTime);

	m_stPreIdleTime = idleTime;
	m_stPreKernelTime = kernelTime;
	m_stPreUserTime = userTime;

	if ((kernel + user) != 0)
	{
		u32Idle = (UINT32)((idle) * 100 / (kernel + user));
	}
	else
	{
		return -1;
	}

	return 0;
}

INT32 CGetSysInfo::GetMemInfo(UINT64 &u64Total, UINT32 &u32Used)
{
	//   TODO:     Add   extra   initialization   here 
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);

	//   get   the   memory   status 
	GlobalMemoryStatusEx(&status);

	u64Total = status.ullTotalPhys;
	u32Used = status.dwMemoryLoad;

	return 0;
}
INT32 CGetSysInfo::GetNetworkInfo(UINT32 &u32InterfaceCount)
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) {
		PRINT("Error allocating memory needed to call GetAdaptersinfo\n");
		return -1;
	}
	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
		if (pAdapterInfo == NULL) 
		{
			PRINT("Error allocating memory needed to call GetAdaptersinfo\n");
			return -1;
		}
	}

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) != NO_ERROR)
	{
		PRINT("GetAdaptersInfo error %d\n", GetLastError());
		return -1;
	}

	{
		m_u32NetworkStatistics++;
		u32InterfaceCount = 0;
		pAdapter = pAdapterInfo;

		while (pAdapter != NULL)
		{
			if ((pAdapter->Type == MIB_IF_TYPE_ETHERNET) ||
				(pAdapter->Type == MIB_IF_TYPE_PPP) || 
				(pAdapter->Type == IF_TYPE_IEEE80211))
			{
				MIB_IFROW  stIfRow = {0};
				DWORD dwSize = sizeof(PMIB_IFROW);
				stIfRow.dwIndex = pAdapter->Index;
				if (GetIfEntry(&stIfRow) == NO_ERROR)
				{
					if (stIfRow.dwOperStatus >= IF_OPER_STATUS_CONNECTED)
					{
						u32InterfaceCount++;

						CMapNetworkInfoIter iter = m_csMapNetwork.find(pAdapter->Description);
						if (iter == m_csMapNetwork.end())
						{
							StNetworkInfo stInfo = { 0 };
							stInfo.u32PreGetTime = timeGetTime();
							stInfo.u32Statistics = m_u32NetworkStatistics;
							stInfo.u32InSpeed =
								stInfo.u32OutSpeed = 0;

							stInfo.u64PreInByte = stIfRow.dwInOctets * 8;
							stInfo.u64PreOutByte = stIfRow.dwOutOctets * 8;

							m_csMapNetwork.insert(make_pair(pAdapter->Description, stInfo));

						}
						else
						{
							UINT32 u32Time = timeGetTime();
							UINT32 u32TimeDiff = u32Time - iter->second.u32PreGetTime;
							iter->second.u32PreGetTime = u32Time;
							iter->second.u32Statistics = m_u32NetworkStatistics;

							if (u32TimeDiff != 0)
							{
								iter->second.u32InSpeed = (UINT32)((stIfRow.dwInOctets * 8 - iter->second.u64PreInByte) * 1000 / u32TimeDiff);
								iter->second.u32OutSpeed = (UINT32)((stIfRow.dwOutOctets * 8 - iter->second.u64PreOutByte) * 1000 / u32TimeDiff);
							}

							iter->second.u64PreInByte = stIfRow.dwInOctets * 8;
							iter->second.u64PreOutByte = stIfRow.dwOutOctets * 8;
						}
					}
				}
			}

			pAdapter = pAdapter->Next;
		}

		free(pAdapterInfo);

		{
			CMapNetworkInfoIter iter = m_csMapNetwork.begin();
			while (iter != m_csMapNetwork.end())
			{
				CMapNetworkInfoIter iterNext = iter;
				iterNext++;
				if (iter->second.u32Statistics != m_u32NetworkStatistics)
				{
					m_csMapNetwork.erase(iter);
				}
				iter = iterNext;
			}
		}
	}
	return 0;
}

INT32 CGetSysInfo::GetNetworkInfo(UINT32 u32Index, UINT32 &u32InSpeed, UINT32 &u32OutSpeed)
{
	if (m_csMapNetwork.size() == 0)
	{
		return -1;
	}
	if (u32Index >= m_csMapNetwork.size())
	{
		return -1;
	}

	CMapNetworkInfoIter iter = m_csMapNetwork.begin();
	for (UINT32 i = 0; i < u32Index; i++)
	{
		iter++;
	}

	u32InSpeed = iter->second.u32InSpeed;
	u32OutSpeed = iter->second.u32OutSpeed;

	return 0;
}

