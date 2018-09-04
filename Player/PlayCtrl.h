#pragma once
#include <stdint.h>

#include "d3doffscreenrender.h"

enum 
{
	_Render_Msg_Data = 0,
	_Render_Msg_ChangeMode,
};

enum
{
	_Save_Msg_Data = 0,
};

typedef struct _tagStFrameHeader
{
	uint32_t u32Header; /* 00 00 01 F1 */
	uint32_t u32EncodeType;
	union
	{
		uint16_t u16Width;
		uint16_t u16AudioChannel;
	};
	union 
	{
		uint16_t u16Height;
		uint16_t u16AudioSample;
	};
	uint32_t u32TimeStampHigh;
	uint32_t u32TimeStampLow;
	uint32_t u32ExternInfoNum;
	uint32_t u32Reserved;
	uint32_t u32CheckSum;
}StFrameHeader;

typedef struct _tagStParamV
{
	void *pData;
	uint32_t u32Length;
}StParamV;


typedef void (*PFUN_ShareMemReleaseCB)(void *pData);
class CShareMemCtrl
{
public:

	CShareMemCtrl();
	~CShareMemCtrl();

	int32_t CreateShareMem(HANDLE hMutex, void *pData, uint32_t u32Length,
		int32_t s32RefCount, PFUN_ShareMemReleaseCB pFunCB);
	int32_t CreateShareMem(HANDLE hMutex, StParamV *pParam, uint32_t u32Count,
		int32_t s32RefCount, PFUN_ShareMemReleaseCB pFunCB);
	int32_t GetShareMem(void *&pData, uint32_t &u32Length, bool boLockData = false);

private:
	int32_t ReleaseShareMem();

private:
	char *m_pData;
	uint32_t m_u32DataLenth;
	HANDLE m_hMutex;
	bool m_boIsLocked;
	int32_t m_s32RefCount;
	PFUN_ShareMemReleaseCB m_pFunCB;
};

class CPlayCtrl
{
public:
	CPlayCtrl();
	~CPlayCtrl();


	int32_t StopRender();
	int32_t BeginRender(HWND hWnd, uint32_t u32FPS = 30);
	uint32_t RenderThread();

	int32_t StopSave();
	int32_t BeginSave();
	uint32_t SaveThread();

	int32_t SendRenderMessage(uint32_t u32MsgType, void *pData, uint32_t u32Length);
	int32_t SendSaveMessage(uint32_t u32MsgType, void *pData, uint32_t u32Length);
	int32_t SendShareData(void *pData, uint32_t u32Length);
	int32_t ReleaseShareData(void *pHandle);


private:
	bool m_boIsMediaThreadExit;
	HANDLE m_hRenderThread;
	HANDLE m_hSemForRenderThread;
	uint32_t m_u32RenderThreadID;
	uint32_t m_u32RenderFPS;

	bool m_boIsSaveThreadExit;
	HANDLE m_hSaveThread;
	HANDLE m_hSemForSaveThread;
	uint32_t m_u32SaveThreadID;

	HANDLE m_hShareDataMutex;

	HWND m_hRenderWnd;
	D3DOffscreenRender m_csRender;

};

int32_t GetFrameHeaderCheckSum(StFrameHeader *pHeader, bool boIsCheck = false);

