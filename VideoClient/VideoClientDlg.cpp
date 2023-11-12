
// VideoClientDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "VideoClient.h"
#include "VideoClientDlg.h"
#include "VideoClientController.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVideoClientDlg 对话框



CVideoClientDlg::CVideoClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VIDEOCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVideoClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PLAY, m_video);
	DDX_Control(pDX, IDC_SLIDER_POS, m_pos);
	DDX_Control(pDX, IDC_SLIDER_VOLUME, m_volume);
	DDX_Control(pDX, IDC_EDIT_URL, m_url);
	DDX_Control(pDX, IDC_BTN_PLAY, m_btnPlay);
}

BEGIN_MESSAGE_MAP(CVideoClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_PLAY, &CVideoClientDlg::OnBnClickedBtnPlay)
	ON_BN_CLICKED(IDC_BTN_STOP, &CVideoClientDlg::OnBnClickedBtnStop)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_POS, &CVideoClientDlg::OnTRBNThumbPosChangingSliderPos)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_VOLUME, &CVideoClientDlg::OnTRBNThumbPosChangingSliderVolume)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


// CVideoClientDlg 消息处理程序

BOOL CVideoClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetTimer(0, 500, NULL);
	m_pos.SetRange(0, 1);
	m_volume.SetRange(0, 100);
	m_volume.SetTicFreq(20);

	SetDlgItemText(IDC_STATIC_VOLUME, _T("100%"));
	SetDlgItemText(IDC_STATIC_TIME, _T("--:--:--/--:--:--"));
	m_controller->setHwnd(m_video.GetSafeHwnd());
	m_url.SetWindowTextW(_T("file:///D:\\workspace\\VS\\eDoCloud\\VideoPlay\\test.mp4"));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CVideoClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CVideoClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CVideoClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (0 == nIDEvent) {
		// 获取播放状态和进度信息
		float pos = m_controller->getPosition();
		int volume = m_controller->getVolume();
		// IDC_STATIC_VOLUME更新音量
		if (-1 != volume) {
			CString str;
			str.Format(_T("%d"), volume);
			SetDlgItemText(IDC_STATIC_VOLUME, str);
		}
		
		// IDC_STATIC_TIME 更新播放时间
		if (-1 != pos) {
			if (m_time <= 0.0f) {
				m_time = m_controller->getLength(); // 总时长
			}
			m_pos.SetRange(0, (int)m_time);
			CString str;
			str.Format(_T("%f/%f"), pos * m_time, m_time);
			SetDlgItemText(IDC_STATIC_TIME, str);
			TRACE(_T("pos%d\n"), pos);
			TRACE(_T("pos* time %d\n"), int(pos * m_time));
			m_pos.SetPos(int(pos * m_time));
		}


	}


	CDialogEx::OnTimer(nIDEvent);
}


void CVideoClientDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	// TODO: 在此处添加消息处理程序代码
	KillTimer(0);
}


void CVideoClientDlg::OnBnClickedBtnPlay()
{
	// TODO: 在此添加控件通知处理程序代码
	if (false == m_status) {
		CString url;
		m_url.GetWindowText(url);
		m_controller->setMedia(m_controller->unicode2Utf8((LPCTSTR)url));
		m_btnPlay.SetWindowTextW(_T("暂停"));
		m_status = true;
		m_controller->play();
	}
	else {
		m_btnPlay.SetWindowTextW(_T("播放"));
		m_status = false;
		m_controller->pause();
	}


}


void CVideoClientDlg::OnBnClickedBtnStop()
{
	// TODO: 在此添加控件通知处理程序代码
	m_btnPlay.SetWindowTextW(_T("播放"));
	m_status = false;
	m_controller->stop();
}

void CVideoClientDlg::OnTRBNThumbPosChangingSliderPos(NMHDR* pNMHDR, LRESULT* pResult)
{
	// 此功能要求 Windows Vista 或更高版本。
	// _WIN32_WINNT 符号必须 >= 0x0600。
	// 哪怕满足了该情况 滑动条依然没有触发该事件 而是触发了OnHScroll和OnVScroll
	NMTRBTHUMBPOSCHANGING* pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	TRACE("pos = %d, reason %d\n", pNMTPC->dwPos, pNMTPC->nReason);
	*pResult = 0;
}


void CVideoClientDlg::OnTRBNThumbPosChangingSliderVolume(NMHDR* pNMHDR, LRESULT* pResult)
{
	// 此功能要求 Windows Vista 或更高版本。
	// _WIN32_WINNT 符号必须 >= 0x0600。
	// 哪怕满足了该情况 滑动条依然没有触发该事件 而是触发了OnHScroll和OnVScroll
	NMTRBTHUMBPOSCHANGING* pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	TRACE("pos = %d, reason %d\n", pNMTPC->dwPos, pNMTPC->nReason);
	*pResult = 0;
}


void CVideoClientDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// pScrollBar用于判断当前是哪个滑动条
	// nPos表示当前滑动条的位置
	// nSBCode 表示当前对滑动条的操作
	TRACE("pos = %p, volume = %p, cur %p pos = %d, code = %d\n", &m_pos, &m_volume, pScrollBar, nPos, nSBCode);
	if (nSBCode == 5) {
		CString strTime;
		strTime.Format(_T("%d"), nPos);
		SetDlgItemText(IDC_STATIC_TIME, strTime);
		m_controller->setPosition(float(nPos) / m_time);
	}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CVideoClientDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// pScrollBar用于判断当前是哪个滑动条
	// nPos表示当前滑动条的位置
	// nSBCode 表示当前对滑动条的操作
	TRACE("pos = %p, volume = %p, cur %p pos = %d, code = %d\n", &m_pos, &m_volume, pScrollBar, nPos, nSBCode);
	if (nSBCode == 5) {
		CString strVolume;
		strVolume.Format(_T("%d%%"), 100 - nPos);
		SetDlgItemText(IDC_STATIC_VOLUME, strVolume);
		m_controller->setVolume(100 - nPos);
	}
	
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}