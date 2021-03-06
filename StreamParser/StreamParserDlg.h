
// StreamParserDlg.h : header file
//

#pragma once

#include "TimeUtility.h"
#include "StreamParser.hpp"
#include <vector>
using namespace std;
// CStreamParserDlg dialog
class CStreamParserDlg : public CDialogEx
{
// Construction
public:
	CStreamParserDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_STREAMPARSER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ctlListFrame;

	vector<vector<int>*> m_vecFrame;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLvnGetdispinfoListFrame(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListFrame(NMHDR *pNMHDR, LRESULT *pResult);
	
	static UINT __stdcall ThreadReadFile(void *p)
	{
		CStreamParserDlg *pThis = (CStreamParserDlg*)p;
		return pThis->PeadFile();
	}

	bool m_bReadFile = false;
	HANDLE m_hThreadReadFile = nullptr;
	bool m_bReadFinish = false;

	UINT PeadFile()
	{
		int nBufferSize = 1 * 1024 * 1024;
		byte *pFileBuffer = new byte[nBufferSize];
		ZeroMemory(pFileBuffer, nBufferSize);
		CString strFile = _T("E:\\DVORecord\\TestDAV\\0-4051.dav");
		try
		{
			CFile file(strFile, CFile::modeRead);
			int nWidth = 0, nHeight = 0, nFramerate = 0;
			int nOffsetStart = 0;
			int nOffsetMatched = 0;
			
			while (m_bReadFile && !m_bReadFinish)
			{
				int nReadLength = file.Read(pFileBuffer, nBufferSize);
				if (nReadLength < nBufferSize)
				{
					file.Close();
					m_bReadFinish = true;
					break;
				}
				do
				{
					if (m_pStreamParser->InputStream(pFileBuffer,nReadLength) == IPC_Succeed)
						break;
					Sleep(50);
				} while (m_bReadFile);
				Sleep(50);
			}
		}
		catch (CFileException* e)
		{
			TCHAR szError[1024];
			e->GetErrorMessage(szError, 1024);
			AfxMessageBox(szError);
		}

		return 0;
	}

	static UINT __stdcall ThreadParserFile(void *p)
	{
		CStreamParserDlg *pThis = (CStreamParserDlg*)p;
		return pThis->ParserFile();
	}
	CStreamParser *m_pStreamParser = nullptr;
	bool m_bParserFile = false;
	HANDLE m_hThreadParserFile = nullptr;

	static void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt)
	{
		int ret;

		ret = avcodec_send_packet(dec_ctx, pkt);
		if (ret < 0) {
			TraceMsgA("Error sending a packet for decoding\n");
			return;
			//exit(1);
		}

		while (ret >= 0) {
			ret = avcodec_receive_frame(dec_ctx, frame);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
				return;
			else if (ret < 0) {
				TraceMsgA("Error during decoding\n");
				return;
				// exit(1);
			}
		}
	};
	UINT ParserFile()
	{
		const AVCodec *codec;
		AVCodecContext *AvCodecCtx = NULL;
		AVFrame *frame;
		int ret;
		AVPacket *pkt;
		AVPacket *pAvPkt = av_packet_alloc();
		__int64 nTotalFrames = 0;
		vector<int>* pVecFrame = new vector<int>;
		m_vecFrame.push_back(pVecFrame);
		double dfT1 = GetExactTime();
		m_pStreamParser = new CStreamParser;
		m_pStreamParser->InitStreamParser();
		pkt = av_packet_alloc();
		codec = avcodec_find_decoder(AV_CODEC_ID_H264);
		AvCodecCtx = avcodec_alloc_context3(codec);
		if (avcodec_open2(AvCodecCtx, codec, NULL) < 0) 
		{
			AfxMessageBox(L"Could not open codec\n");
		
		}
		frame = av_frame_alloc();
		if (!frame) {
			AfxMessageBox(L"Could not allocate video frame\n");
			exit(1);
		}

		while (m_bParserFile)
		{
			if (m_pStreamParser->ParserFrame(pAvPkt) && pAvPkt->size > 0)
			{// 得到裸数据流
				if (pAvPkt->size > 100 * 1024)
				{
					TraceMsgA("%s FrameSize = %d\tFrames = %d.\n", __FUNCTION__, pAvPkt->size, pVecFrame->size());
					pVecFrame = new vector<int>;
					m_vecFrame.push_back(pVecFrame);
				}
				decode(AvCodecCtx, frame, pAvPkt);
				pVecFrame->push_back(pAvPkt->size);
				nTotalFrames++;
			}
			else
			{

			}
			if (TimeSpanEx(dfT1) >= 0.250f)
			{
				dfT1 = GetExactTime();
				m_ctlListFrame.SetItemCount(m_vecFrame.size());
				m_ctlListFrame.Invalidate();
				SetDlgItemInt(IDC_STATIC_FRAMES, nTotalFrames);
			}
			Sleep(20);
		}
		av_packet_free(&pAvPkt);
		delete m_pStreamParser;
		return 0;
	}
	afx_msg void OnBnClickedButtonParser();
	afx_msg void OnBnClickedButtonStop();
};
