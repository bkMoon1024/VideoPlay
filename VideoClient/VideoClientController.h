#pragma once

#include "EVlc.h"
#include "VideoClientDlg.h"

enum EVlcCommand {
	EVLC_PLAY = 1,
	EVLC_PAUSE,
	EVLC_STOP,
	EVLC_GET_VOLUME,
	EVLC_GET_POSITION
};

class VideoClientController
{
public:
	VideoClientController();
	~VideoClientController();

	int init(CWnd*& pWnd);
	int invoke();

	int setMedia(const string& strUrl);
	int setHwnd(const HWND hwnd);
	int play();
	int pause();
	int stop();

	float getPosition();
	int setPosition(const float& pos);

	int getVolume();
	int setVolume(const int& volume);

	vlcSize getMediaInfo();

	float videoCtrl(const EVlcCommand& cmd);

	string unicode2Utf8(wstring strIn);

protected:
	EVlc			m_vlc;
	CVideoClientDlg	m_dlg;
};

