#pragma once
#include <stdint.h>
#include <set>

#include "d3doffscreenrender.h"

using namespace std;

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

#define FRAME_HEADER		(0xF1000000)

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



enum
{
	_Share_Data_Pos_Render = 0,
	_Share_Data_Pos_Save,

	_Share_Data_Pos_Reserved,
};
#define SHARE_DATA_RENDER	(1 << _Share_Data_Pos_Render)
#define SHARE_DATA_SAVE		(1 << _Share_Data_Pos_Save)


typedef struct _tagStLocalPlayIndex
{
	StFrameHeader stHeader;
	uint32_t u32Index;
	uint64_t u64FilePos;
	uint32_t u32RawDataLength;
	uint64_t u64RawDataFilePos;
}StLocalPlayIndex;


typedef set<StLocalPlayIndex> CLocalPlayIndex;
typedef set<StLocalPlayIndex>::iterator CLocalPlayIndexIter;

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

	int32_t StopLocalPlay();
	int32_t BeginLocalPlay(const TCHAR *pFileName);
	uint32_t LocalPlayThread();

	int32_t SendRenderMessage(uint32_t u32MsgType, void *pData, uint32_t u32Length);
	int32_t SendSaveMessage(uint32_t u32MsgType, void *pData, uint32_t u32Length);
	int32_t SendShareData(void *pData, uint32_t u32Length, uint32_t u32ShareFlag = SHARE_DATA_RENDER);
	int32_t SendShareData(StParamV *pParam, uint32_t u32Count, uint32_t u32ShareFlag = SHARE_DATA_RENDER);
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

	bool m_boIsLocalPlayThreadExit;
	HANDLE m_hLocalPlayThread;
	HANDLE m_hSemForLocalPlayThread;
	uint32_t m_u32LocalPlayThreadID;

	HANDLE m_hShareDataMutex;

	HWND m_hRenderWnd;
	D3DOffscreenRender m_csRender;

	string m_csStrSaveFolder;
	uint32_t m_u32SaveContinueTime; /* ms */

	wstring m_csStrLocalPlayFile;
	CLocalPlayIndex m_csLocalPlayIndex;
	HANDLE m_hLocalFile;

};

int32_t GetFrameHeaderCheckSum(StFrameHeader *pHeader, bool boIsCheck = false);

