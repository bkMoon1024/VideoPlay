#include "pch.h"
#include "VideoClientController.h"

VideoClientController::VideoClientController()
{
	m_dlg.m_controller = this;
}

VideoClientController::~VideoClientController()
{
}

int VideoClientController::init(CWnd*& pWnd)
{
	pWnd = &m_dlg;
	return 0;
}

int VideoClientController::invoke()
{
	INT_PTR nResponse = m_dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
	}
	return nResponse;
}

int VideoClientController::setMedia(const string& strUrl)
{
	return m_vlc.setMedia(strUrl);
}

int VideoClientController::setHwnd(const HWND hwnd)
{
	return m_vlc.setHwnd(hwnd);
}

int VideoClientController::play()
{
	return m_vlc.play();
}

int VideoClientController::pause()
{
	return m_vlc.pause();
}

int VideoClientController::stop()
{
	return m_vlc.stop();
}

float VideoClientController::getPosition()
{
	return m_vlc.getPosition();
}

int VideoClientController::setPosition(const float& pos)
{
	return m_vlc.setPosition(pos);
}

int VideoClientController::getVolume()
{
	return m_vlc.getVolume();
}

int VideoClientController::setVolume(const int& volume)
{
	return m_vlc.setVolume(volume);
}

float VideoClientController::getLength()
{
	return m_vlc.getLength();
}

vlcSize VideoClientController::getMediaInfo()
{
	return m_vlc.getMediaInfo();
}

float VideoClientController::videoCtrl(const EVlcCommand& cmd)
{
	switch (cmd) {
	case EVLC_PLAY:
		return m_vlc.play();
	case EVLC_PAUSE:
		return m_vlc.pause();
	case EVLC_STOP:
		return m_vlc.stop();
	case EVLC_GET_VOLUME:
		return m_vlc.getVolume();
	case EVLC_GET_POSITION:
		return m_vlc.getPosition();
	default:
		break;
	}
	return -1;
}

string VideoClientController::unicode2Utf8(wstring strIn)
{
	return m_vlc.unicode2Utf8(strIn);
}
