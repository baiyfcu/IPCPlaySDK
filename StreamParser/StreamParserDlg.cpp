
// StreamParserDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StreamParser.h"
#include "StreamParserDlg.h"
#include "afxdialogex.h"
#include "Utility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CStreamParserDlg dialog



CStreamParserDlg::CStreamParserDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CStreamParserDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStreamParserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CStreamParserDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_FRAME, &CStreamParserDlg::OnLvnGetdispinfoListFrame)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FRAME, &CStreamParserDlg::OnLvnItemchangedListFrame)
	ON_BN_CLICKED(IDC_BUTTON_PARSER, &CStreamParserDlg::OnBnClickedButtonParser)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CStreamParserDlg::OnBnClickedButtonStop)
END_MESSAGE_MAP()


// CStreamParserDlg message handlers

BOOL CStreamParserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_ctlListFrame.SubclassDlgItem(IDC_LIST_FRAME,this);


	m_ctlListFrame.SetExtendedStyle(m_ctlListFrame.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER /*|LVS_EX_CHECKBOXES|LVS_EX_SUBITEMIMAGES*/);
	m_ctlListFrame.InsertColumn(0, _T("No"), LVCFMT_LEFT, 50);
	for (int i = 1; i <= 64; i++)
	{
		CString strItem;
		strItem.Format(_T("Fr_%02d"), i);
		m_ctlListFrame.InsertColumn(i, strItem, LVCFMT_LEFT, 60);
	}
	ShowWindow(SW_MAX);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CStreamParserDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CStreamParserDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CStreamParserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CStreamParserDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	CRect rt(2, 28, cx, cy);
	if (GetDlgItem(IDC_LIST_FRAME)->GetSafeHwnd())
		MoveDlgItem(IDC_LIST_FRAME, rt);
}


void CStreamParserDlg::OnLvnGetdispinfoListFrame(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	
	*pResult = 0;
	LV_ITEM *plvItem = &(pDispInfo)->item;
	int k = plvItem->iItem;
	if (k >= m_vecFrame.size())
	{
		return;
	}
	vector<int> *pInfo = m_vecFrame[k];
	if (!pInfo)
		return;
	
	if (plvItem->mask & LVIF_TEXT)
	{
		switch (plvItem->iSubItem)
		{
		case 0:
		
			_stprintf_s(plvItem->pszText, plvItem->cchTextMax, _T("%d"), k);
			break;
		
		default:
		{
			if ((plvItem->iSubItem -1)< pInfo->size())
				_itot_s((*pInfo)[plvItem->iSubItem -1], plvItem->pszText, plvItem->cchTextMax,10);
		}
		}
	}
	*pResult = 0;
}


void CStreamParserDlg::OnLvnItemchangedListFrame(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CStreamParserDlg::OnBnClickedButtonParser()
{
	m_bParserFile = true;
	m_bReadFile = true;

	m_hThreadReadFile = (HANDLE)_beginthreadex(nullptr, 0, ThreadReadFile, this, 0, 0);
	m_hThreadParserFile = (HANDLE)_beginthreadex(nullptr, 0, ThreadParserFile, this, 0, 0);

	

}


void CStreamParserDlg::OnBnClickedButtonStop()
{
	m_bParserFile = false;
	m_bReadFile = false;
}
