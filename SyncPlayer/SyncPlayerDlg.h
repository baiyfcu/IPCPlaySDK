
// SyncPlayerDlg.h : 头文件
//

#pragma once
#include "IPCPlaySDK.h"
#include "FrameView.h"
#include "DialogVideo.h"
struct ThreadParam
{
	void *pDialog;
	CDialogVideo *pPlayer;
	CString strFile;
	IPC_PLAYHANDLE hPlayHandle;
	UINT	nID;
};

struct FrameTime
{
	bool bIFrame;
	long nFrameSize;
	TCHAR szTextIFrame[16];
	time_t tFrameTime;
	TCHAR szTextFrameTime[32];
	FrameTime(time_t timeInput,int nsize,bool bKeyFrame = false)
	{
		ZeroMemory(this, sizeof(FrameTime));
		bIFrame = bKeyFrame;
		nFrameSize = nsize;
		tFrameTime = timeInput;
	}
};
typedef shared_ptr<FrameTime> FrameTimePtr;
// CSyncPlayerDlg 对话框
class CSyncPlayerDlg : public CDialogEx
{
// 构造
public:
	CSyncPlayerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SYNCPLAYER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	CVideoFrame *m_pVideoFrame = nullptr;
	CWndSizeManger* m_pWndSizeManager = nullptr;

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBrowse(UINT nID);
	bool SaveConfig();
	bool LoadConfig();
	
	CString m_strFile[16];
	HANDLE  m_hReadFile[16];
	CDialogVideo *m_pPlayer[16];
	bool	m_bThreadRun = false;
	IPC_PLAYHANDLE	m_hSyncSource = nullptr;
	static  UINT __stdcall ThreadReadFile(void *p)
	{
		ThreadParam *TP = (ThreadParam*)p;
		shared_ptr<ThreadParam> TPPtr(TP);
		return ((CSyncPlayerDlg *)(TP->pDialog))->ReadFileRun(TP->nID);
	}
	UINT	ReadFileRun(UINT nIdex);
	UINT	ReadFileRun2(UINT nIdex);
	time_t  tFirstFrameTime = 0;

	IPC_PLAYHANDLE	m_hAsyncPlayHandle[4];
	time_t	m_tFrameOffset = 0;
	MMRESULT	m_nTimeEvent = 0;
// 	static void  MMTIMECALLBACK(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
// 	{
// 		CSyncPlayerDlg* pThis = (CSyncPlayerDlg *)dwUser;
// 		if (pThis->m_hAsyncPlayHandle && pThis->m_tFrameOffset)
// 		{
// 			int nStatus = ipcplay_AsyncSeekFrame(pThis->m_hAsyncPlayHandle, pThis->m_tFrameOffset);
// 			if (nStatus != IPC_Succeed)
// 				TraceMsgA("%s ipcplay_AsyncSeekFrame(%d).\n", __FUNCTION__, nStatus);
// 			pThis->m_tFrameOffset += 200;
// 		}
// 	}
	vector<CDialogVideo *> m_vecDlgVideo;
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonTest();
	UINT_PTR	m_nTestTimer = 0;
	vector<FrameTimePtr> vecFrameTime[16];
	CFrameView*	m_pFrameView = nullptr;
	
	int m_nTestTimes;
	afx_msg void OnBnClickedButtonStoptest();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int m_nPlayerCount = 0;
	afx_msg void OnBnClickedButtonVideofilemanager();
};
