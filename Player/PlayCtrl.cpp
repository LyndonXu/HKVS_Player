#include "stdafx.h"
#include <Windows.h>
#include <timeapi.h>
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

, m_csStrSaveFolder("")
, m_u32SaveContinueTime(1 * 60 * 1000)		/* 1 minute(s) */

, m_csStrLocalPlayFile(L"")

, m_hLocalFile(NULL)

{
	m_hShareDataMutex = CreateMutex(NULL, FALSE, NULL);
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
}

enum 
{
	_Render_Play,
	_Render_Pause,
	_Render_Stop,
};
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

	uint32_t u32RenderMode = _Render_Play;

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
			PRINT("render %d's frame %lld\n", u32Tmp, u64CurTime);
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
uint32_t CPlayCtrl::SaveThread()
{
	MSG stMsg = { 0 };
	/* creat a message queue */
	PeekMessage(&stMsg, NULL, 0, 0, PM_NOREMOVE);


	CShareMemCtrl *pCtrl = NULL;
	HANDLE hFile = NULL;
	uint64_t u64BeginTime = 0;
	uint64_t u64FrameCount = 0;

	while (!m_boIsSaveThreadExit)
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
				if (hFile == NULL)
				{
					TCHAR tcName[256];
					swprintf_s(tcName, 256, L"%lld.dat", u64CurTime);
					hFile = CreateFile(tcName, GENERIC_WRITE | GENERIC_READ,
						FILE_SHARE_READ,
						NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					u64BeginTime = u64CurTime;
				}

				if (hFile != NULL)
				{
					if (u64CurTime >=  u64BeginTime)
					{
						WriteFile(hFile, pData, u32Length, NULL, NULL);
						u64FrameCount++;

						if ((u64CurTime - u64BeginTime) > (uint64_t)m_u32SaveContinueTime)
						{
							CloseHandle(hFile);
							hFile = NULL;
						}
					}
				}

			}

			ReleaseShareData(pCtrl);
			pCtrl = NULL;
		}
		WaitForSingleObject(m_hSemForSaveThread, 10);

		while (PeekMessage(&stMsg, NULL, 0, 0, PM_REMOVE))
		{
			if (stMsg.message == _Save_Msg_Data)
			{
				if (pCtrl != NULL)
				{
					PRINT("loss frame!!\n")
					ReleaseShareData(pCtrl);
				}
				pCtrl = (CShareMemCtrl *)stMsg.wParam;
			}
		}
	}
	if (hFile != NULL)
	{
		CloseHandle(hFile);
		hFile = NULL;
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

	m_hLocalFile = CreateFile(pFileName, GENERIC_READ,
		FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hLocalFile == NULL)
	{
		return -1;
	}

	uint64_t u64ReadPos = 0;
	uint32_t u32Index = 0;
	while (true)
	{
		StLocalPlayIndex stIndex = {0};
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

	}

	if (m_csLocalPlayIndex.size() == 0)
	{
		return -1;
	}



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
uint32_t CPlayCtrl::LocalPlayThread()
{
	MSG stMsg = { 0 };
	/* creat a message queue */
	PeekMessage(&stMsg, NULL, 0, 0, PM_NOREMOVE);


	CShareMemCtrl *pCtrl = NULL;
	uint64_t u64BeginTime = 0;

	while (!m_boIsLocalPlayThreadExit)
	{
		WaitForSingleObject(m_hSemForLocalPlayThread, 10);

		while (PeekMessage(&stMsg, NULL, 0, 0, PM_REMOVE))
		{
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
