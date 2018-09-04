#pragma once
#include <stdint.h>

#include "d3doffscreenrender.h"

enum 
{
	_Render_Msg_Data = 0,
	_Render_Msg_ChangeMode,
};


typedef void (*PFUN_ShareMemReleaseCB)(void *pData);
class CShareMemCtrl
{
public:

	CShareMemCtrl();
	~CShareMemCtrl();

	int32_t CreateShareMem(HANDLE hMutex, void *pData, uint32_t u32Length,
		int32_t s32RefCount, PFUN_ShareMemReleaseCB pFunCB);

	int32_t ReleaseShareMem();

	int32_t GetShareMem(void *&pData, uint32_t &u32Length, bool boLockData = false);
private:
	void *m_pData;
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
	int32_t SendRenderMessage(uint32_t u32MsgType, void *pData, uint32_t u32Length);
	int32_t SendShareData(void *pData, uint32_t u32Length);
	int32_t ReleaseShareData(void *pHandle);

	bool m_boIsMediaThreadExit;
	
	HANDLE m_hRenderThread;
	HANDLE m_hSemForRendThread;
	uint32_t m_u32RenderThreadID;
	uint32_t m_u32RenderFPS;


	HANDLE m_hShareDataMutex;

	HWND m_hRenderWnd;
	D3DOffscreenRender m_csRender;

};

