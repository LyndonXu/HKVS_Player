#pragma once
#include <stdint.h>
#include <set>

#include "d3doffscreenrender.h"

using namespace std;


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
	_PlayMode_Play,
	_PlayMode_Pause,
	_PlayMode_NextFrame,
	_PlayMode_PrevFrame,
	_PlayMode_Stop,
};

enum
{
	_Render_Msg_Data = WM_USER + 200,
	_Render_Msg_ChangeMode,
};

enum
{
	_Save_Msg_Data = WM_USER + 200,
};

enum
{
	_LocalPlay_Msg_Play = WM_USER + 200,
	_LocalPlay_Msg_Pause,
	_LocalPlay_Msg_NextFrame,
	_LocalPlay_Msg_PrevFrame,
	_LocalPlay_Msg_ChangeRate,	/* WPARAM: rate[-30, 0) (0, 30] */
	_LocalPlay_Msg_Jump,		/* WPARAM: index */

	_LocalPlay_Msg_Reserved,
};


enum
{
	_WND_Msg_FoldSize = 0,						/* LPRARM: size / (1024 * 1024) (Mb) */
	_WND_Msg_PlayMode,							/* LPRARM: _PlayMode_* */
	_WND_Msg_LocalPlayFileIndexSize,			/* LPRARM: Index Size */
	_WND_Msg_LocalPlayFileCurIndex,				/* LPRARM: Current play Index */
	_WND_Msg_ShowRectInvalidate,				/* NULL */
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

typedef struct _tagStRecoderFileInfo
{
	wstring cwStrName;
	uint64_t u64FileSize;
}StRecoderFileInfo;


typedef set<StLocalPlayIndex> CLocalPlayIndex;
typedef set<StLocalPlayIndex>::iterator CLocalPlayIndexIter;
typedef set<StLocalPlayIndex>::reverse_iterator CLocalPlayIndexRIter;


typedef set<StRecoderFileInfo> CRecoderFileList;
typedef set<StRecoderFileInfo>::iterator CRecoderFileListIter;


class CPlayCtrl
{
public:
	CPlayCtrl();
	~CPlayCtrl();


	int32_t SetFolder(const wchar_t *pStrFolder);
	int32_t SetSaveContinusTime(uint32_t u32Time);/* min */
	int32_t RegisterWNDMsg(HWND hWnd, uint32_t u32MsgNum);

	int32_t StopRender();
	int32_t BeginRender(HWND hWnd, uint32_t u32FPS = 30);
	uint32_t RenderThread();

	int32_t StopSave();
	int32_t BeginSave();
	uint32_t SaveThread();

	int32_t StopLocalPlay();
	int32_t BeginLocalPlay(const TCHAR *pFileName);
	uint32_t LocalPlayThread();

	int32_t SendRenderMessage(uint32_t u32MsgType, void *pData = NULL, uint32_t u32Length = 0);
	int32_t SendSaveMessage(uint32_t u32MsgType, void *pData = NULL, uint32_t u32Length = 0);
	int32_t SendLocalPlayMessage(uint32_t u32MsgType, void *pData = NULL, uint32_t u32Length = 0);
	int32_t SendShareData(void *pData, uint32_t u32Length, uint32_t u32ShareFlag = SHARE_DATA_RENDER);
	int32_t SendShareData(StParamV *pParam, uint32_t u32Count, uint32_t u32ShareFlag = SHARE_DATA_RENDER);
	int32_t ReleaseShareData(void *pHandle);


	int64_t GetRecordFolderSizeAndList();
	int32_t ReduceFolderSize(uint64_t u64ExpectSize);/* MB */

	friend int32_t GetFolderSizeCB(const wchar_t *pName, WIN32_FIND_DATA *pInfo, void *pContext);

private:
	int32_t GetFolderSizeCBInner(const wchar_t *pName, WIN32_FIND_DATA *pInfo);
	int32_t SendFileDataToRender(CLocalPlayIndexIter iter);
	uint32_t AddFileToList(wstring &csStrFileNameBackup, uint64_t u64FileSize);

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

	wstring m_csStrSaveFolder;
	uint32_t m_u32SaveContinueTime; /* ms */

	wstring m_csStrLocalPlayFile;
	CLocalPlayIndex m_csLocalPlayIndex;
	HANDLE m_hLocalFile;

	HANDLE m_hFileListMutex;
	CRecoderFileList m_csRecordFileList;
	uint64_t m_u64RecordFileTotalSize;

	HWND m_hMsgWnd;
	uint32_t m_u32MsgNumber;

};

int32_t GetFrameHeaderCheckSum(StFrameHeader *pHeader, bool boIsCheck = false);

