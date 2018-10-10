#include "stdafx.h"

#include <algorithm>

#include <Windows.h>
#include <MMSystem.h>
#include "PlayCtrl.h"


typedef struct _tagStShareMemHeader
{
	int32_t s32RefCount;
}StShareMemHeader;

CShareMemCtrl::CShareMemCtrl()
: m_pData(NULL)
, m_u32DataLenth(0)
, m_hMutex(NULL)
, m_boIsLocked(false)
, m_s32RefCount(1)
, m_pFunCB(NULL)
{

}
CShareMemCtrl::~CShareMemCtrl()
{
	ReleaseShareMem();
	if (m_boIsLocked && m_hMutex != NULL)
	{
		ReleaseMutex(m_hMutex);
	}
}

int32_t CShareMemCtrl::CreateShareMem(HANDLE hMutex, void *pData, uint32_t u32Length,
	int32_t s32RefCount, PFUN_ShareMemReleaseCB pFunCB)
{
	if (hMutex == NULL || pData == NULL)
	{
		return -1;
	}
	m_hMutex = hMutex;
	if ((m_pData = (char *)malloc(sizeof(StShareMemHeader) + u32Length)) != NULL)
	{
		memcpy(m_pData + sizeof(StShareMemHeader), pData, u32Length);
		m_u32DataLenth = u32Length;
		m_pFunCB = pFunCB;
		((StShareMemHeader *)m_pData)->s32RefCount = s32RefCount;
		return 0;
	}
	return -1;
}

int32_t CShareMemCtrl::CreateShareMem(HANDLE hMutex, StParamV *pParam, uint32_t u32Count,
	int32_t s32RefCount, PFUN_ShareMemReleaseCB pFunCB)
{
	if (hMutex == NULL || pParam == NULL)
	{
		return -1;
	}
	m_hMutex = hMutex;
	uint32_t u32Length = 0;
	for (uint32_t i = 0; i < u32Count; i++)
	{
		if (pParam[i].pData != NULL)
		{
			u32Length += pParam[i].u32Length;
		}
	}
	if ((m_pData = (char *)malloc(sizeof(StShareMemHeader) + u32Length)) != NULL)
	{
		uint32_t u32CopyLength = 0;
		for (uint32_t i = 0; i < u32Count; i++)
		{
			if (pParam[i].pData != NULL)
			{
				memcpy(m_pData + sizeof(StShareMemHeader) + u32CopyLength, 
					pParam[i].pData, pParam[i].u32Length);
				u32CopyLength += pParam[i].u32Length;
			}
		}
		m_u32DataLenth = u32Length;
		m_pFunCB = pFunCB;
		((StShareMemHeader *)m_pData)->s32RefCount = s32RefCount;
		return 0;
	}
	return -1;

}


int32_t CShareMemCtrl::ReleaseShareMem()
{

	if (m_hMutex != NULL && !m_boIsLocked)
	{
		WaitForSingleObject(m_hMutex, 1000);
		m_boIsLocked = true;
	}

	if (m_boIsLocked && m_hMutex != NULL)
	{

		((StShareMemHeader *)m_pData)->s32RefCount--;
		m_s32RefCount = ((StShareMemHeader *)m_pData)->s32RefCount;
		if (m_s32RefCount <= 0)
		{
			if (m_pFunCB != NULL)
			{
				m_pFunCB(m_pData + sizeof(StShareMemHeader));
			}
			free(m_pData);
			m_pData = NULL;
		}
		m_boIsLocked = false;
		ReleaseMutex(m_hMutex);
	}

	return m_s32RefCount;
}

int32_t CShareMemCtrl::GetShareMem(void *&pData, uint32_t &u32Length, bool boLockData/* = false*/)
{
	if (WaitForSingleObject(m_hMutex, 1000) == WAIT_OBJECT_0)
	{
		pData = m_pData + sizeof(StShareMemHeader);
		u32Length = m_u32DataLenth;
		if (boLockData)
		{
			m_boIsLocked = true;
		}
		else
		{
			ReleaseMutex(m_hMutex);
		}
	}
	else
	{
		return -1;
	}

	return 0;
}


CPlayCtrl::CPlayCtrl()
: m_boIsMediaThreadExit(true)
, m_hRenderThread(NULL)
, m_hSemForRenderThread(NULL)
, m_u32RenderThreadID(~0)
, m_u32RenderFPS(30)

, m_boIsSaveThreadExit(true)
, m_hSaveThread(NULL)
, m_hSemForSaveThread(NULL)
, m_u32SaveThreadID(~0)

, m_boIsLocalPlayThreadExit(true)
, m_hLocalPlayThread(NULL)
, m_hSemForLocalPlayThread(NULL)
, m_u32LocalPlayThreadID(~0)

, m_hShareDataMutex(NULL)

, m_hRenderWnd(NULL)

, m_csStrSaveFolder(L"")
, m_u32SaveContinueTime(1 * 60 * 1000)		/* 1 minute(s) */

, m_csStrLocalPlayFile(L"")

, m_hLocalFile(NULL)

, m_hFileListMutex(NULL)
, m_u64RecordFileTotalSize(0)

, m_hMsgWnd(NULL)
, m_u32MsgNumber(0)


{
	m_hShareDataMutex = CreateMutex(NULL, FALSE, NULL);
	m_hFileListMutex = CreateMutex(NULL, FALSE, NULL);
}


CPlayCtrl::~CPlayCtrl()
{
	StopRender();
	StopSave();
	StopLocalPlay();

	if (m_hShareDataMutex != NULL)
	{
		CloseHandle(m_hShareDataMutex);
		m_hShareDataMutex = NULL;
	}

	if (m_hFileListMutex != NULL)
	{
		CloseHandle(m_hShareDataMutex);
		m_hFileListMutex = NULL;
	}
}

int32_t CPlayCtrl::SetFolder(const wchar_t *pStrFolder)
{
	if (pStrFolder != NULL)
	{
		if (m_csStrSaveFolder != pStrFolder)
		{
			m_csStrSaveFolder = pStrFolder;
			GetRecordFolderSizeAndList();
		}
		return 0;
	}
	return -1;
}

int32_t CPlayCtrl::SetSaveContinusTime(uint32_t u32Time) /* min */
{
	if (u32Time <= 10 && u32Time != 0)
	{
		m_u32SaveContinueTime = u32Time * 60 * 1000;
	}
	return 0;
}


int32_t CPlayCtrl::RegisterWNDMsg(HWND hWnd, uint32_t u32MsgNum)
{
	if (hWnd == NULL)
	{
		return -1;
	}

	m_hMsgWnd = hWnd;
	m_u32MsgNumber = u32MsgNum;
	
	return 0;
}

#if 0
unsigned char u8Buf[800 * 600 * 3];
#endif
uint32_t CPlayCtrl::RenderThread()
{
	MSG stMsg = { 0 };
	/* creat a message queue */
	PeekMessage(&stMsg, NULL, 0, 0, PM_NOREMOVE);

	uint64_t u64FrameCnt = 0;
	uint64_t u64NextRenderTime = 0;
	uint64_t u64BaseRenderTime = 0;

	uint32_t u32RenderMode = _PlayMode_Play;

	uint32_t u32Tmp = 0;

	CShareMemCtrl *pCtrl = NULL;

	u64NextRenderTime = u64BaseRenderTime = timeGetTime();

	m_csRender.init(0, m_hRenderWnd, 800, 600);

	while (!m_boIsMediaThreadExit)
	{
		uint32_t u32SleepTime = 0;
		uint64_t u64CurTime = timeGetTime();
		if (u64CurTime >= u64NextRenderTime)
		{
#if 0
			/* render a frame */
			{
				static uint8_t u8Tmp = 0;
				unsigned char *pTmp = u8Buf;
				for (int32_t i = 0; i < 800 * 600; i++, pTmp += 3)
				{
					pTmp[0] = 0;
					pTmp[1] = 0;
					pTmp[2] = u8Tmp;
				}
				u8Tmp++;
				//memset(u8Buf, u8Tmp++, 800 * 600 * 3);
			}
			//PRINT("begin render %d\n", timeGetTime());
			m_csRender.renderRGB(u8Buf, 800, 600);
			//PRINT("end render %d\n", timeGetTime());
#endif
			if (pCtrl != NULL)
			{
				void *pData = NULL;
				uint32_t u32Length = 0;
				pCtrl->GetShareMem(pData, u32Length);
				unsigned char *pTmp = (unsigned char *)pData;
				StFrameHeader *pHeader = (StFrameHeader *)pTmp;
				m_csRender.renderRGB(pTmp + sizeof(StFrameHeader) + sizeof(uint32_t), pHeader->u16Width, pHeader->u16Height);
				ReleaseShareData(pCtrl);
				pCtrl = NULL;
			}
			else
			{
				m_csRender.renderRGB(NULL, 0, 0);
			}
			//PRINT("render %d's frame %lld\n", u32Tmp, u64CurTime);
			u64FrameCnt++;
			u64NextRenderTime = u64BaseRenderTime + u64FrameCnt * 1000 / m_u32RenderFPS;
		}
		else
		{

		}

		u64CurTime = timeGetTime();
		if (u64CurTime < u64NextRenderTime)
		{
			u32SleepTime = (uint32_t)(u64NextRenderTime - u64CurTime);
		}

		if (u32SleepTime != 0)
		{
			WaitForSingleObject(m_hSemForRenderThread, u32SleepTime);
		}
		/* peek all message */
		while (PeekMessage(&stMsg, NULL, 0, 0, PM_REMOVE))
		{
			/* ...... */
			if (stMsg.message == _Render_Msg_Data)
			{
				if (pCtrl != NULL)
				{
					ReleaseShareData(pCtrl);
					pCtrl = NULL;
				}
				pCtrl = (CShareMemCtrl *)(stMsg.wParam);
			}
		}
	}

	while (PeekMessage(&stMsg, NULL, 0, 0, PM_REMOVE))
	{
		/* ...... */
		if (stMsg.message == _Render_Msg_Data)
		{
			ReleaseShareData((void *)(stMsg.wParam));
		}
	}

	PostMessage(m_hMsgWnd, m_u32MsgNumber, _WND_Msg_ShowRectInvalidate, NULL);
	return 0;
}

unsigned __stdcall SaveThread(void *pArg)
{
	return ((CPlayCtrl *)(pArg))->SaveThread();
}

int32_t CPlayCtrl::StopSave()
{
	m_boIsSaveThreadExit = true;

	if (m_hSaveThread != NULL)
	{
		WaitForSingleObject(m_hSaveThread, 2000);
		CloseHandle(m_hSaveThread);
		m_hSaveThread = NULL;
	}

	if (m_hSemForSaveThread != NULL)
	{
		CloseHandle(m_hSemForSaveThread);
		m_hSemForSaveThread = NULL;
	}

	m_u32SaveThreadID = ~0;

	return 0;
}
int32_t CPlayCtrl::BeginSave()
{
	if (m_hSaveThread != NULL)
	{
		return 0;
	}

	m_boIsSaveThreadExit = false;

	m_hSemForSaveThread = CreateSemaphore(NULL, 0, 20, NULL);
	if (m_hSemForSaveThread == NULL)
	{
		return -1;
	}


	m_hSaveThread = (HANDLE)_beginthreadex(NULL, 0, ::SaveThread,
		this, 0, &m_u32SaveThreadID);

	if (m_hSaveThread == NULL)
	{
		StopSave();
		return -1;
	}
	return 0;
}

uint32_t CPlayCtrl::AddFileToList(wstring &csStrFileNameBackup, uint64_t u64FileSize)
{
	if (WaitForSingleObject(m_hFileListMutex, 10) == WAIT_OBJECT_0)
	{
		StRecoderFileInfo stInfo =
		{
			csStrFileNameBackup,
			u64FileSize,
		};

		m_csRecordFileList.insert(stInfo);

		m_u64RecordFileTotalSize += u64FileSize;

		if (m_hMsgWnd != NULL)
		{
			PostMessage(m_hMsgWnd, m_u32MsgNumber, _WND_Msg_FoldSize,
				(LPARAM)((m_u64RecordFileTotalSize + 1024 * 512) / (1024 * 1024)));
		}

		ReleaseMutex(m_hFileListMutex);
	}

	return 0;
}

uint32_t CPlayCtrl::SaveThread()
{
	MSG stMsg = { 0 };
	/* creat a message queue */
	PeekMessage(&stMsg, NULL, 0, 0, PM_NOREMOVE);


	CShareMemCtrl *pCtrl = NULL;
	//HANDLE hFile = NULL;
	FILE *pFile = NULL;
	uint64_t u64BeginTime = 0;
	uint64_t u64FrameCount = 0;

	wstring csStrFileNameBackup;
	uint64_t u64FileSize = 0;

	while (!m_boIsSaveThreadExit)
	{
		try
		{

			if (pCtrl != NULL)
			{
				void *pData = NULL;
				uint32_t u32Length = 0;
				uint64_t u64CurTime = 0;
				if (pCtrl->GetShareMem(pData, u32Length) == 0)
				{
					StFrameHeader *pHeader = (StFrameHeader *)pData;
					u64CurTime = pHeader->u32TimeStampHigh;
					u64CurTime = (u64CurTime << 32) + pHeader->u32TimeStampLow;
					//if (hFile == NULL)
					if (pFile == NULL)
					{
						wchar_t wcName[256];
						SYSTEMTIME stSysTime = { 0 };
						GetLocalTime(&stSysTime);

						swprintf_s(wcName, 256, L"%04d-%02d-%02d %02d-%02d-%02d_%03d.dat",
							stSysTime.wYear,
							stSysTime.wMonth,
							stSysTime.wDay,
							stSysTime.wHour,
							stSysTime.wMinute,
							stSysTime.wSecond,
							stSysTime.wMilliseconds);

						csStrFileNameBackup = wcName;
						u64FileSize = 0;

						wstring wcStrAbsName = m_csStrSaveFolder;
						wcStrAbsName.append(L"\\");
						wcStrAbsName.append(wcName);

						//hFile = CreateFile(wcStrAbsName.c_str(),
						//	GENERIC_WRITE,
						//	FILE_SHARE_WRITE,
						//	NULL, CREATE_ALWAYS, 0, NULL);
						errno_t err = _wfopen_s(&pFile, wcStrAbsName.c_str(), L"wb+");
						if (err == 0)
						{
						}
						else
						{
							PRINT("open file error\n");
						}
						
						u64BeginTime = u64CurTime;
					}

					//if (hFile != NULL)
					if (pFile != NULL)
					{
						if (u64CurTime >= u64BeginTime)
						{
							//PRINT("pre write: %d\n", u32Length);
							
							//WriteFile(hFile, pData, u32Length, NULL, NULL);
							fwrite(pData, 1, u32Length, pFile);
							
							//PRINT("end write: %d\n", u32Length)
							u64FileSize += u32Length;
							u64FrameCount++;

							if ((u64CurTime - u64BeginTime) > (uint64_t)m_u32SaveContinueTime)
							{
								//PRINT("diff %lld, continue %d\n", u64CurTime - u64BeginTime, m_u32SaveContinueTime);
								
								//CloseHandle(hFile);
								//hFile = NULL;
								
								fclose(pFile);
								pFile = NULL;

								AddFileToList(csStrFileNameBackup, u64FileSize);

								csStrFileNameBackup = L"";
								u64FileSize = 0;
							}
						}
					}

				}

				ReleaseShareData(pCtrl);
				pCtrl = NULL;
			}
		}
		catch (const std::exception&)
		{
			PRINT("exception\n");
		}
		WaitForSingleObject(m_hSemForSaveThread, 10);

		while (PeekMessage(&stMsg, NULL, 0, 0, PM_REMOVE))
		{
			if (stMsg.message == _Save_Msg_Data)
			{
				if (pCtrl != NULL)
				{
					try
					{
						PRINT("loss frame!!\n")
						ReleaseShareData(pCtrl);
					}
					catch (const std::exception&)
					{
						PRINT("exception\n")
					}
				}
				pCtrl = (CShareMemCtrl *)stMsg.wParam;
			}
		}
	}
	//if (hFile != NULL)
	//{
	//	CloseHandle(hFile);
	//	hFile = NULL;

	if (pFile != NULL)
	{
		fclose(pFile);
		pFile = NULL;

		AddFileToList(csStrFileNameBackup, u64FileSize);

		csStrFileNameBackup = L"";
		u64FileSize = 0;
	}
	while (PeekMessage(&stMsg, NULL, 0, 0, PM_REMOVE))
	{
		if (stMsg.message == _Save_Msg_Data)
		{
			ReleaseShareData((CShareMemCtrl *)stMsg.wParam);
		}
	}

	return 0;
}


unsigned __stdcall LocalPlayThread(void *pArg)
{
	return ((CPlayCtrl *)(pArg))->LocalPlayThread();
}

int32_t CPlayCtrl::StopLocalPlay()
{
	m_boIsLocalPlayThreadExit = true;

	if (m_hLocalPlayThread != NULL)
	{
		WaitForSingleObject(m_hLocalPlayThread, 2000);
		CloseHandle(m_hLocalPlayThread);
		m_hLocalPlayThread = NULL;
	}

	if (m_hSemForLocalPlayThread != NULL)
	{
		CloseHandle(m_hSemForLocalPlayThread);
		m_hSemForLocalPlayThread = NULL;
	}

	m_u32LocalPlayThreadID = ~0;

	m_csStrLocalPlayFile = L"";
	m_csLocalPlayIndex.clear();
	if (m_hLocalFile != NULL)
	{
		CloseHandle(m_hLocalFile);
	}
	m_hLocalFile = NULL;

	return 0;
}

namespace std{
	const bool operator <(const StLocalPlayIndex &Left, const StLocalPlayIndex &Right)
	{
		return Left.u32Index < Right.u32Index;
	}
}

int32_t CPlayCtrl::BeginLocalPlay(const TCHAR *pFileName)
{
	if (m_hLocalPlayThread != NULL)
	{
		if (m_csStrLocalPlayFile == pFileName)
		{
			return 0;
		}
		StopLocalPlay();
	}
	m_csStrLocalPlayFile = L"";
	m_hLocalFile = CreateFile(pFileName, GENERIC_READ,
		FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hLocalFile == NULL)
	{
		return -1;
	}

	m_csStrLocalPlayFile = pFileName;
	m_boIsLocalPlayThreadExit = false;

	m_hSemForLocalPlayThread = CreateSemaphore(NULL, 0, 20, NULL);
	if (m_hSemForLocalPlayThread == NULL)
	{
		return -1;
	}


	m_hLocalPlayThread = (HANDLE)_beginthreadex(NULL, 0, ::LocalPlayThread,
		this, 0, &m_u32LocalPlayThreadID);

	if (m_hLocalPlayThread == NULL)
	{
		StopLocalPlay();
		return -1;
	}
	return 0;
}

int32_t  CPlayCtrl::SendFileDataToRender(CLocalPlayIndexIter iter)
{
	uint32_t u32Length = iter->u32RawDataLength +
		sizeof(StFrameHeader) + sizeof(uint32_t);
	void *pData = malloc(u32Length);
	if (pData != NULL)
	{
		LARGE_INTEGER stLarge;
		DWORD dwReadLen = 0;

		stLarge.QuadPart = iter->u64FilePos;
		SetFilePointer(m_hLocalFile, stLarge.LowPart, &(stLarge.HighPart), FILE_BEGIN);

		ReadFile(m_hLocalFile, pData, u32Length, &dwReadLen, NULL);
		if (u32Length != dwReadLen)
		{
			free(pData);
			return -1;
		}
		SendShareData(pData, u32Length);
		free(pData);
	}
	return 0;
}

uint32_t CPlayCtrl::LocalPlayThread()
{
	MSG stMsg = { 0 };
	/* creat a message queue */
	PeekMessage(&stMsg, NULL, 0, 0, PM_NOREMOVE);


	uint64_t u64ReadPos = 0;
	uint32_t u32Index = 0;
	while (!m_boIsLocalPlayThreadExit)
	{
		StLocalPlayIndex stIndex = { 0 };
		uint32_t u32DataLength = 0;

		stIndex.u64FilePos = u64ReadPos;

		LARGE_INTEGER stLarge;
		DWORD dwReadLen = 0;

		stLarge.QuadPart = u64ReadPos;
		SetFilePointer(m_hLocalFile, stLarge.LowPart, &(stLarge.HighPart), FILE_BEGIN);

		if (!ReadFile(m_hLocalFile, &(stIndex.stHeader), sizeof(StFrameHeader), &dwReadLen, NULL))
		{
			break;
		}
		if (dwReadLen != sizeof(StFrameHeader))
		{
			break;
		}

		if (GetFrameHeaderCheckSum(&(stIndex.stHeader), true) != 0)
		{
			break;
		}

		u64ReadPos += dwReadLen;

		if (stIndex.stHeader.u32Header != FRAME_HEADER)
		{
			break;
		}


		if (!ReadFile(m_hLocalFile, &(stIndex.u32RawDataLength), sizeof(uint32_t), &dwReadLen, NULL))
		{
			break;
		}
		if (dwReadLen != sizeof(uint32_t))
		{
			break;
		}


		u64ReadPos += dwReadLen;

		stIndex.u64RawDataFilePos = u64ReadPos;

		u64ReadPos += stIndex.u32RawDataLength;
		stIndex.u32Index = u32Index++;

		m_csLocalPlayIndex.insert(stIndex);

		if ((u32Index & 0xFF) == 0xFF)
		{
			if (m_hMsgWnd != NULL)
			{
				PostMessage(m_hMsgWnd, m_u32MsgNumber, _WND_Msg_LocalPlayFileIndexSize, u32Index);
			}
		}

	}

	if (m_hMsgWnd != NULL)
	{
		PostMessage(m_hMsgWnd, m_u32MsgNumber, _WND_Msg_LocalPlayFileIndexSize, m_csLocalPlayIndex.size());
	}
	if (m_csLocalPlayIndex.size() == 0)
	{
		/* Send message to the windows */

		if (m_hLocalFile != NULL)
		{
			CloseHandle(m_hLocalFile);
			m_hLocalFile = NULL;
		}
		if (m_hMsgWnd != NULL)
		{
			PostMessage(m_hMsgWnd, m_u32MsgNumber, _WND_Msg_LocalPlayOut, 0);
		}
		return -1;
	}

	uint64_t u64ContinusTime = 0;
	CLocalPlayIndexIter iter = m_csLocalPlayIndex.begin();
	{
		uint64_t u64BeginTime = iter->stHeader.u32TimeStampHigh;
		u64BeginTime = (u64BeginTime << 32) + iter->stHeader.u32TimeStampLow;

		CLocalPlayIndexRIter rIter = m_csLocalPlayIndex.rbegin();
		uint64_t u64EndTime = rIter->stHeader.u32TimeStampHigh;
		u64EndTime = (u64EndTime << 32) + rIter->stHeader.u32TimeStampLow;

		u64ContinusTime = u64EndTime - u64BeginTime;
	}


	double d64RealFPS = (double)(m_csLocalPlayIndex.size());
	d64RealFPS  = d64RealFPS * 1000 / u64ContinusTime;

	double d64PlayFPSRate = 1.0;//30.0 / d64RealFPS;

	CShareMemCtrl *pCtrl = NULL;

	uint64_t u64NextReadTime = 0;
	uint64_t u64BaseReadTime = 0; 
	uint64_t u64FrameCnt = 0;
	uint32_t u32BaseFrameIndex = 0;

	u64NextReadTime = u64BaseReadTime = timeGetTime();

	uint32_t u32PlayMode = _PlayMode_Play;

	if (m_hMsgWnd != NULL)
	{
		PostMessage(m_hMsgWnd, m_u32MsgNumber, _WND_Msg_PlayMode, u32PlayMode);

		wchar_t *pStr = (wchar_t *)calloc(1, (m_csStrLocalPlayFile.length() + 4) * sizeof(wchar_t));
		wcscpy(pStr, m_csStrLocalPlayFile.c_str());
		PostMessage(m_hMsgWnd, m_u32MsgNumber, _WND_Msg_ShowName, (LPARAM)pStr);
	}

	while (!m_boIsLocalPlayThreadExit)
	{
		uint64_t u64CurTime = timeGetTime();
		if (u64CurTime >= u64NextReadTime)
		{
			if (u32PlayMode == _PlayMode_Play)
			{
				if (iter != m_csLocalPlayIndex.end())
				{
					double d64TimeDiffFromBase = (double)(u64NextReadTime - u64BaseReadTime);
					uint32_t u32NextFrame = u32BaseFrameIndex + (uint32_t)(d64TimeDiffFromBase * d64RealFPS * d64PlayFPSRate / 1000.0 + 0.5);
					//PRINT("u32NextFrame %d\n", u32NextFrame);
					if (u32NextFrame > iter->u32Index || u64NextReadTime == u64BaseReadTime)
					{
						for (uint32_t i = iter->u32Index; i < u32NextFrame; i++)
						{
							iter++;
							if (iter == m_csLocalPlayIndex.end())
							{
								iter--;
								break;
							}
						}
						if (iter != m_csLocalPlayIndex.end())
						{
							SendFileDataToRender(iter);
							if (m_hMsgWnd != NULL)
							{
								PostMessage(m_hMsgWnd, m_u32MsgNumber, _WND_Msg_LocalPlayFileCurIndex, iter->u32Index);
							}
						}

					}
				}

				u64FrameCnt++;
				u64NextReadTime = u64BaseReadTime + u64FrameCnt * 1000 / m_u32RenderFPS;
			}
			else if (u32PlayMode == _PlayMode_Pause)
			{

			}
		}

		WaitForSingleObject(m_hSemForLocalPlayThread, 10);

		while (PeekMessage(&stMsg, NULL, 0, 0, PM_REMOVE))
		{
			switch (stMsg.message)
			{
				case _LocalPlay_Msg_Play:
				{
					if (u32PlayMode != _PlayMode_Play)
					{
						u32PlayMode = _PlayMode_Play;

						if (iter == m_csLocalPlayIndex.end())
						{
							CLocalPlayIndexRIter rIter = m_csLocalPlayIndex.rbegin();
							u32BaseFrameIndex = rIter->u32Index;
						}
						else
						{
							u32BaseFrameIndex = iter->u32Index;
						}
						u64NextReadTime = u64BaseReadTime = timeGetTime();
						u64FrameCnt = 0;

						if (m_hMsgWnd != NULL)
						{
							PostMessage(m_hMsgWnd, m_u32MsgNumber, _WND_Msg_PlayMode, u32PlayMode);
						}

					}


					break;
				}
				case _LocalPlay_Msg_Pause:
				{
					if (u32PlayMode != _PlayMode_Pause)
					{
						u32PlayMode = _PlayMode_Pause;
						if (m_hMsgWnd != NULL)
						{
							PostMessage(m_hMsgWnd, m_u32MsgNumber, _WND_Msg_PlayMode, u32PlayMode);
						}

					}
					break;
				}
				case _LocalPlay_Msg_NextFrame:
				{
					if (iter != m_csLocalPlayIndex.end())
					{
						iter++;
						if (iter != m_csLocalPlayIndex.end())
						{
							//PRINT("Next Frame: %d\n", iter->u32Index);
							SendFileDataToRender(iter);
							if (m_hMsgWnd != NULL)
							{
								PostMessage(m_hMsgWnd, m_u32MsgNumber, _WND_Msg_LocalPlayFileCurIndex, iter->u32Index);
							}
						}
					}
					if (u32PlayMode != _PlayMode_Pause)
					{
						u32PlayMode = _PlayMode_Pause;
						if (m_hMsgWnd != NULL)
						{
							PostMessage(m_hMsgWnd, m_u32MsgNumber, _WND_Msg_PlayMode, u32PlayMode);
						}
					}
					break;
				}
				case _LocalPlay_Msg_PrevFrame:
				{
					if (iter != m_csLocalPlayIndex.begin())
					{
						iter--;
						//PRINT("Prev Frame: %d\n", iter->u32Index);
						SendFileDataToRender(iter);
						if (m_hMsgWnd != NULL)
						{
							PostMessage(m_hMsgWnd, m_u32MsgNumber, _WND_Msg_LocalPlayFileCurIndex, iter->u32Index);
						}
					}
					if (u32PlayMode != _PlayMode_Pause)
					{
						u32PlayMode = _PlayMode_Pause;
						if (m_hMsgWnd != NULL)
						{
							PostMessage(m_hMsgWnd, m_u32MsgNumber, _WND_Msg_PlayMode, u32PlayMode);
						}
					}
					break;
				}
				case _LocalPlay_Msg_ChangeRate:
				{
					int32_t s32Rate = (int32_t)stMsg.wParam;
					if (s32Rate != 0)
					{
						if (s32Rate > 0)
						{
							d64PlayFPSRate = (double)s32Rate;
						}
						else
						{
							s32Rate = 0 - s32Rate;
							d64PlayFPSRate = (double)s32Rate;
							d64PlayFPSRate = 1.0 / d64PlayFPSRate;
						}
						if (u32PlayMode == _PlayMode_Play)
						{
							u64NextReadTime = u64BaseReadTime = timeGetTime();
							u64FrameCnt = 0;

							if (iter == m_csLocalPlayIndex.end())
							{
								CLocalPlayIndexRIter rIter = m_csLocalPlayIndex.rbegin();
								u32BaseFrameIndex = rIter->u32Index;
							}
							else
							{
								u32BaseFrameIndex = iter->u32Index;
							}

						}
					}
					break;
				}
				case _LocalPlay_Msg_Jump:
				{
					uint32_t u32JumpIndex = (uint32_t)stMsg.wParam;
					CLocalPlayIndexIter iterJump = m_csLocalPlayIndex.end();
					if ((u32JumpIndex) <= m_csLocalPlayIndex.size())
					{
						if (u32JumpIndex != 0)
						{
							u32JumpIndex--;
						}
						
						StLocalPlayIndex stIndex = { 0 };
						stIndex.u32Index = u32JumpIndex;
						iterJump = m_csLocalPlayIndex.find(stIndex);
					}
					if (iterJump == m_csLocalPlayIndex.end())
					{
						break;
					}
				
					iter = iterJump;

					if (u32PlayMode == _PlayMode_Play)
					{
						u64NextReadTime = u64BaseReadTime = timeGetTime();
						u64FrameCnt = 0;
						u32BaseFrameIndex = iter->u32Index;
					}

					SendFileDataToRender(iter);
					if (m_hMsgWnd != NULL)
					{
						PostMessage(m_hMsgWnd, m_u32MsgNumber, _WND_Msg_LocalPlayFileCurIndex, iter->u32Index);
					}

					break;
				}
				default:
					break;
			}
		}
	}

	if (m_hLocalFile != NULL)
	{
		CloseHandle(m_hLocalFile);
		m_hLocalFile = NULL;
	}

	while (PeekMessage(&stMsg, NULL, 0, 0, PM_REMOVE))
	{
	}

	if (m_hMsgWnd != NULL)
	{
		PostMessage(m_hMsgWnd, m_u32MsgNumber, _WND_Msg_LocalPlayOut, 0);
	}
	return 0;
}

int32_t CPlayCtrl::StopRender()
{
	m_boIsMediaThreadExit = true;

	if (m_hRenderThread != NULL)
	{
		WaitForSingleObject(m_hRenderThread, 2000);
		CloseHandle(m_hRenderThread);
		m_hRenderThread = NULL;
	}

	if (m_hSemForRenderThread != NULL)
	{
		CloseHandle(m_hSemForRenderThread);
		m_hSemForRenderThread = NULL;
	}

	m_u32RenderThreadID = ~0;

	m_hRenderWnd = NULL;
	m_u32RenderFPS = 30;

	return 0;
}


unsigned __stdcall RenderThread(void *pArg)
{
	return ((CPlayCtrl *)(pArg))->RenderThread();
}


int32_t CPlayCtrl::BeginRender(HWND hWnd, uint32_t u32FPS/* = 30*/)
{
	if (m_hRenderThread != NULL)
	{
		m_hRenderWnd = hWnd;
		return 0;
	}

	m_boIsMediaThreadExit = false;

	m_hRenderWnd = hWnd;
	m_u32RenderFPS = u32FPS;

	m_hSemForRenderThread = CreateSemaphore(NULL, 0, 20, NULL);
	if (m_hSemForRenderThread == NULL)
	{
		return -1;
	}


	m_hRenderThread = (HANDLE)_beginthreadex(NULL, 0, ::RenderThread,
		this, 0, &m_u32RenderThreadID);

	if (m_hRenderThread == NULL)
	{
		StopRender();
		return -1;
	}
	return 0;
}


int32_t CPlayCtrl::SendRenderMessage(uint32_t u32MsgType, void *pData, uint32_t u32Length)
{
	if (m_u32RenderThreadID == ~0)
	{
		return -1;
	}
	if (u32MsgType == _Render_Msg_Data || u32MsgType == _Render_Msg_ChangeMode)
	{
		if (!PostThreadMessage(m_u32RenderThreadID, u32MsgType, (WPARAM)pData, (LPARAM)u32Length))
		{
			return -1;
		}
		ReleaseSemaphore(m_hSemForRenderThread, 1, NULL);
	}

	return 0;
}

int32_t CPlayCtrl::SendSaveMessage(uint32_t u32MsgType, void *pData, uint32_t u32Length)
{
	if (m_u32SaveThreadID == ~0)
	{
		return -1;
	}
	if (u32MsgType == _Save_Msg_Data)
	{
		if (!PostThreadMessage(m_u32SaveThreadID, u32MsgType, (WPARAM)pData, (LPARAM)u32Length))
		{
			return -1;
		}
		ReleaseSemaphore(m_hSemForSaveThread, 1, NULL);
	}

	return 0;
}

int32_t CPlayCtrl::SendLocalPlayMessage(uint32_t u32MsgType, void *pData, uint32_t u32Length)
{
	if (m_u32LocalPlayThreadID == ~0)
	{
		return -1;
	}
	if (u32MsgType < _LocalPlay_Msg_Reserved)
	{
		if (!PostThreadMessage(m_u32LocalPlayThreadID, u32MsgType, (WPARAM)pData, (LPARAM)u32Length))
		{
			PRINT("PostThreadMessage: %d\n", GetLastError());
			return -1;
		}
		ReleaseSemaphore(m_hSemForLocalPlayThread, 1, NULL);
	}

	return 0;
}


int32_t CPlayCtrl::SendShareData(StParamV *pParam, uint32_t u32Count,
	uint32_t u32ShareFlag/* = SHARE_DATA_RENDER*/)
{
	if (pParam == NULL)
	{
		return -1;
	}
	CShareMemCtrl *pCtrl[_Share_Data_Pos_Reserved] = { NULL };

	int32_t s32ShareCount = 0;
	int32_t s32FirstPos = -1;
	bool boIsMemError = false;
	for (int32_t i = 0; i < _Share_Data_Pos_Reserved; i++)
	{
		if ((u32ShareFlag & (1 << i)) != 0)
		{
			pCtrl[i] = new CShareMemCtrl;
			if (pCtrl[i] == NULL)
			{
				boIsMemError = true;
				break;
			}
			s32ShareCount++;
			if (s32FirstPos < 0)
			{
				s32FirstPos = i;
			}
		}
	}
	if (boIsMemError)
	{
		for (int32_t i = 0; i < _Share_Data_Pos_Reserved; i++)
		{
			if (pCtrl[i] != NULL)
			{
				delete pCtrl[i];
			}
		}
		return -1;
	}


	if (pCtrl[s32FirstPos]->CreateShareMem(m_hShareDataMutex, pParam, u32Count, s32ShareCount, NULL) != 0)
	{
		for (int32_t i = 0; i < _Share_Data_Pos_Reserved; i++)
		{
			if (pCtrl[i] != NULL)
			{
				delete pCtrl[i];
			}
		}
		return -1;
	}

	for (int32_t i = (s32FirstPos + 1); i < _Share_Data_Pos_Reserved; i++)
	{
		if ((u32ShareFlag & (1 << i)) != 0)
		{
			*pCtrl[i] = *pCtrl[s32FirstPos];
		}
	}

	if ((u32ShareFlag & SHARE_DATA_RENDER) != 0)
	{
		if (SendRenderMessage(_Render_Msg_Data, pCtrl[_Share_Data_Pos_Render], sizeof(CShareMemCtrl *)) != 0)
		{
			ReleaseShareData(pCtrl[_Share_Data_Pos_Render]);
		}
	}

	if ((u32ShareFlag & SHARE_DATA_SAVE) != 0)
	{
		if (SendSaveMessage(_Save_Msg_Data, pCtrl[_Share_Data_Pos_Save], sizeof(CShareMemCtrl *)) != 0)
		{
			ReleaseShareData(pCtrl[_Share_Data_Pos_Save]);
			return -1;
		}
	}
	return 0;

}

int32_t CPlayCtrl::SendShareData(void *pData, uint32_t u32Length, uint32_t u32ShareFlag/* = SHARE_DATA_RENDER*/)
{
	if (pData == NULL)
	{
		return -1;
	}

	CShareMemCtrl *pCtrl[_Share_Data_Pos_Reserved] = { NULL };
	
	int32_t s32ShareCount = 0;
	int32_t s32FirstPos = -1;
	bool boIsMemError = false;
	for (int32_t i = 0; i < _Share_Data_Pos_Reserved; i++)
	{
		if ((u32ShareFlag & (1 << i)) != 0)
		{
			pCtrl[i] = new CShareMemCtrl;
			if (pCtrl[i] == NULL)
			{
				boIsMemError = true;
				break;
			}
			s32ShareCount++;
			if (s32FirstPos < 0)
			{
				s32FirstPos = i;
			}
		}
	}
	if (boIsMemError)
	{
		for (int32_t i = 0; i < _Share_Data_Pos_Reserved; i++)
		{
			if (pCtrl[i] != NULL)
			{
				delete pCtrl[i];
			}
		}
		return -1;
	}


	if (pCtrl[s32FirstPos]->CreateShareMem(m_hShareDataMutex, pData, u32Length, s32ShareCount, NULL) != 0)
	{
		for (int32_t i = 0; i < _Share_Data_Pos_Reserved; i++)
		{
			if (pCtrl[i] != NULL)
			{
				delete pCtrl[i];
			}
		}
		return -1;
	}

	for (int32_t i = (s32FirstPos + 1); i < _Share_Data_Pos_Reserved; i++)
	{
		if ((u32ShareFlag & (1 << i)) != 0)
		{
			*pCtrl[i] = *pCtrl[s32FirstPos];
		}
	}

	if ((u32ShareFlag & SHARE_DATA_RENDER) != 0)
	{
		if (SendRenderMessage(_Render_Msg_Data, pCtrl[_Share_Data_Pos_Render], sizeof(CShareMemCtrl *)) != 0)
		{
			ReleaseShareData(pCtrl[_Share_Data_Pos_Render]);
		}
	}

	if ((u32ShareFlag & SHARE_DATA_SAVE) != 0)
	{
		if (SendSaveMessage(_Save_Msg_Data, pCtrl[_Share_Data_Pos_Save], sizeof(CShareMemCtrl *)) != 0)
		{
			ReleaseShareData(pCtrl[_Share_Data_Pos_Save]);
			return -1;
		}
	}
	return 0;
}

int32_t CPlayCtrl::ReleaseShareData(void *pHandle)
{
	CShareMemCtrl *pCtrl = (CShareMemCtrl *)pHandle;
	if (pCtrl == NULL)
	{
		return -1;
	}
	//pCtrl->ReleaseShareMem();
	delete pCtrl;
	return 0;
}

int32_t GetFrameHeaderCheckSum(StFrameHeader *pHeader, bool boIsCheck)
{
	if (pHeader == NULL)
	{
		return -1;
	}

	uint32_t u32Check = 0;
	uint32_t *pCheck = (uint32_t *)pHeader;
	for (uint32_t i = 0; i < offsetof(StFrameHeader, u32CheckSum) / sizeof(uint32_t); i++)
	{
		u32Check += pCheck[i];
	}

	if (boIsCheck)
	{
		if ((u32Check + pHeader->u32CheckSum) == 0)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		pHeader->u32CheckSum = 0 - u32Check;
	}
	return 0;
}

int32_t GetFolderSizeCB(const wchar_t *pName, WIN32_FIND_DATA *pInfo, void *pContext)
{
	if (pContext == NULL)
	{
		return -1;
	}
	CPlayCtrl *pThis = (CPlayCtrl *)pContext;

	return pThis->GetFolderSizeCBInner(pName, pInfo);
}

namespace std
{
	bool operator <(const StRecoderFileInfo &Left, const StRecoderFileInfo &Right)
	{
		return Left.cwStrName < Right.cwStrName;
	}
}

int32_t CPlayCtrl::GetFolderSizeCBInner(const wchar_t *pName, WIN32_FIND_DATA *pInfo)
{
	if (wcsstr(pInfo->cFileName, L".dat") != NULL)
	{
		StRecoderFileInfo stInfo;
		stInfo.cwStrName = pInfo->cFileName;
		stInfo.u64FileSize = pInfo->nFileSizeHigh;

		stInfo.u64FileSize = (stInfo.u64FileSize << 32) + pInfo->nFileSizeLow;

		m_csRecordFileList.insert(stInfo);
		m_u64RecordFileTotalSize += stInfo.u64FileSize;
	}

	return 0;
}
int64_t CPlayCtrl::GetRecordFolderSizeAndList()
{
	if (m_hFileListMutex == NULL)
	{
		return -1;
	}
	if (WaitForSingleObject(m_hFileListMutex, 1000) != WAIT_OBJECT_0)
	{
		return -1;
	}

	m_csRecordFileList.clear();
	m_u64RecordFileTotalSize = 0;

	SuperMkDir(m_csStrSaveFolder.c_str());

	GetFolderSize(m_csStrSaveFolder.c_str(), GetFolderSizeCB, this);


	ReleaseMutex(m_hFileListMutex);

	if (m_hMsgWnd != NULL)
	{
		PostMessage(m_hMsgWnd, m_u32MsgNumber, _WND_Msg_FoldSize,
			(LPARAM)((m_u64RecordFileTotalSize + 1024 * 512) / (1024 * 1024)));
	}

	return 0;
}

int32_t CPlayCtrl::ReduceFolderSize(uint64_t u64ExpectSize)
{
	if (m_hFileListMutex == NULL)
	{
		return -1;
	}
	if (WaitForSingleObject(m_hFileListMutex, 1000) != WAIT_OBJECT_0)
	{
		return -1;
	}

	u64ExpectSize *= (1024 * 1024);

	CRecoderFileListIter iter = m_csRecordFileList.begin();
	while (iter != m_csRecordFileList.end() &&
		m_u64RecordFileTotalSize > u64ExpectSize)
	{
		wstring cwStrABSName = m_csStrSaveFolder + L"\\" + iter->cwStrName;
		if (DeleteFile(cwStrABSName.c_str()))
		{
			m_u64RecordFileTotalSize -= iter->u64FileSize;
			CRecoderFileListIter iterBack = iter;
			iter++;

			m_csRecordFileList.erase(iterBack);
		}
		else
		{
			iter++;
		}
	}


	ReleaseMutex(m_hFileListMutex);

	if (m_hMsgWnd != NULL)
	{
		PostMessage(m_hMsgWnd, m_u32MsgNumber, _WND_Msg_FoldSize,
			(LPARAM)((m_u64RecordFileTotalSize + 1024 * 512) / (1024 * 1024)));
	}

	return 0;
}


namespace std
{
	bool operator < (const StRecoderFileInfo &Left, wchar_t *pRight)
	{
		return Left.cwStrName < pRight;
	}
	bool operator < (wchar_t *pLeft, const StRecoderFileInfo &Right )
	{
		wstring csStr = pLeft;
		return csStr < Right.cwStrName;
	}
}

int32_t CPlayCtrl::SearchSaveFolder(uint64_t u64TimeBegin, uint64_t u64TimeEnd,
	PFUN_SearchSaveFolderCB pFunCB, void *pContext)
{
	if (m_hFileListMutex == NULL)
	{
		return -1;
	}

	if (u64TimeEnd < u64TimeBegin)
	{
		return -1;
	}

	wchar_t wcStrBegin[128];
	wchar_t wcStrEnd[128];

	tm stTime = { 0 };
	_localtime64_s(&stTime, (__time64_t *)(&u64TimeBegin));
	swprintf_s(wcStrBegin, 128, L"%04d-%02d-%02d %02d-%02d-%02d",
		stTime.tm_year + 1900,
		stTime.tm_mon + 1,
		stTime.tm_mday,
		stTime.tm_hour,
		stTime.tm_min,
		stTime.tm_sec);

	_localtime64_s(&stTime, (__time64_t *)(&u64TimeEnd));
	swprintf_s(wcStrEnd, 128, L"%04d-%02d-%02d %02d-%02d-%02d",
		stTime.tm_year + 1900,
		stTime.tm_mon + 1,
		stTime.tm_mday,
		stTime.tm_hour,
		stTime.tm_min,
		stTime.tm_sec);


	if (WaitForSingleObject(m_hFileListMutex, 1000) != WAIT_OBJECT_0)
	{
		return -1;
	}

	wchar_t *pStr = wcStrBegin;	
	CRecoderFileListIter iterBegin;
	CRecoderFileListIter iterEnd;

	pair<CRecoderFileListIter, CRecoderFileListIter> csPair;
	csPair = equal_range(m_csRecordFileList.begin(), m_csRecordFileList.end(), pStr);

	if (csPair.first == csPair.second)	/* begin or end */
	{
		if (csPair.first == m_csRecordFileList.end())
		{
			ReleaseMutex(m_hFileListMutex);
			return -1;
		}
	}
	iterBegin = csPair.second;

	pStr = wcStrEnd;
	csPair = equal_range(m_csRecordFileList.begin(), m_csRecordFileList.end(), pStr);
	if (csPair.first == csPair.second)	/* begin or end */
	{
		if (csPair.first == m_csRecordFileList.begin())
		{
			if (iterBegin != m_csRecordFileList.begin())
			{
				ReleaseMutex(m_hFileListMutex);
				return -1;
			}
		}
	}
	iterEnd = csPair.first;

	if (pFunCB != NULL)
	{
		for (CRecoderFileListIter iter = iterBegin; iter != iterEnd; iter++)
		{
			pFunCB((&(*iter)), pContext);
		}
	}

	ReleaseMutex(m_hFileListMutex);

	return 0;
}
