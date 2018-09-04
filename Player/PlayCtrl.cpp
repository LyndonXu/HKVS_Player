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
			u32Length = pParam[i].u32Length;
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

, m_hShareDataMutex(NULL)

, m_hRenderWnd(NULL)

{
	m_hShareDataMutex = CreateMutex(NULL, FALSE, NULL);
}


CPlayCtrl::~CPlayCtrl()
{
	StopRender();
	StopSave();

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
				m_csRender.renderRGB((unsigned char *)pData, 800, 600);
				ReleaseShareData(pCtrl);
				pCtrl = NULL;
			}
			else
			{
				m_csRender.renderRGB(NULL, 800, 600);
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

	while (!m_boIsSaveThreadExit)
	{
		if (pCtrl != NULL)
		{
			FILE *pFile = fopen("123.rgb", "ab+");
			if (pFile != NULL)
			{
				void *pData = NULL;
				uint32_t u32Length = 0;
				if (pCtrl->GetShareMem(pData, u32Length) == 0)
				{
					fwrite(pData, 1, u32Length, pFile);
				}
				fclose(pFile);
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
					ReleaseShareData(pCtrl);
				}
				pCtrl = (CShareMemCtrl *)stMsg.wParam;
			}
		}
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

int32_t CPlayCtrl::SendShareData(void *pData, uint32_t u32Length)
{
	if (pData == NULL)
	{
		return -1;
	}

	CShareMemCtrl *pCtrl = new CShareMemCtrl;
	CShareMemCtrl *pCtrlSave = new CShareMemCtrl;	//NULL;//
	if (pCtrl == NULL)
	{
		if (pCtrlSave != NULL)
		{
			delete pCtrlSave;
		}
		return -1;
	}

	if (pCtrlSave == NULL)
	{
		if (pCtrl != NULL)
		{
			delete pCtrl;
		}
		return -1;
	}

	if (pCtrl->CreateShareMem(m_hShareDataMutex, pData, u32Length, 2, NULL) != 0)
	{
		delete pCtrl;
		delete pCtrlSave;
		return -1;
	}

	*pCtrlSave = *pCtrl;

	if (SendRenderMessage(_Render_Msg_Data, pCtrl, sizeof(CShareMemCtrl *)) != 0)
	{
		ReleaseShareData(pCtrl);
		return -1;
	}

	if (SendSaveMessage(_Save_Msg_Data, pCtrlSave, sizeof(CShareMemCtrl *)) != 0)
	{
		ReleaseShareData(pCtrlSave);
		return -1;
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
