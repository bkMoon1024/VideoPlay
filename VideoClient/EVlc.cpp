#include "pch.h"
#include "EVlc.h"
#include <conio.h>

EVlc::EVlc()
{
	m_instance = libvlc_new(0, NULL);
}

EVlc::~EVlc()
{
	if (m_player) {
		libvlc_media_player_release(m_player);
		m_player = nullptr;
	}
	if (m_media) {
		libvlc_media_release(m_media);
		m_media = nullptr;
	}
	if (m_instance) {
		libvlc_release(m_instance);
		m_instance = nullptr;
	}
}

/// <summary>
/// ���ò���·��
/// </summary>
/// <param name="strUrl">����·��������ΪUTF8�ַ���</param>
/// <returns>0-�����ɹ�������-����ʧ��</returns>
int EVlc::setMedia(const string& strUrl)
{
	if (!m_instance || !m_hwnd) {
		return -1;
	}
	if (strUrl == m_url){
		return 0;
	}
	m_url = strUrl;

	if (m_media) {
		libvlc_media_release(m_media);
		m_media = nullptr;
	}
	m_media = libvlc_media_new_location(m_instance, strUrl.c_str());
	if (!m_media) {
		return -2;
	}
	if (m_player) {
		libvlc_media_player_release(m_player);
		m_player = nullptr;
	}
	m_player = libvlc_media_player_new_from_media(m_media);
	if (!m_player) {
		return -3;
	}

	CRect rect;
	GetWindowRect(m_hwnd, rect);
	string strRatio;
	strRatio.resize(32);
	sprintf((char*)strRatio.c_str(), "%d:%d", rect.Width(), rect.Height());
	libvlc_video_set_aspect_ratio(m_player, strRatio.c_str());
	libvlc_media_player_set_hwnd(m_player, m_hwnd);
	return 0;
}

#ifdef WIN32
int EVlc::setHwnd(const HWND hwnd)
{
	m_hwnd = hwnd;
	return 0;
}
#endif

int EVlc::play()
{
	if (!m_instance || !m_media || !m_player) {
		return -1;
	}
	return libvlc_media_player_play(m_player);
}

int EVlc::pause()
{
	if (!m_instance || !m_media || !m_player) {
		return -1;
	}

    libvlc_media_player_pause(m_player);
	return 0;
}

int EVlc::stop()
{
	if (!m_instance || !m_media || !m_player) {
		return -1;
	}

	libvlc_media_player_stop(m_player);
	return 0;
}

float EVlc::getPosition()
{
	if (!m_instance || !m_media || !m_player) {
		return -1;
	}

	if (!_kbhit()) {
		return libvlc_media_player_get_position(m_player);
	}
	return 0;
}

int EVlc::setPosition(const float& pos)
{
	if (!m_instance || !m_media || !m_player) {
		return -1;
	}
	libvlc_media_player_set_position(m_player, pos);
	return 0;
}

int EVlc::getVolume()
{
	if (!m_instance || !m_media || !m_player) {
		return -1;
	}
	return libvlc_audio_get_volume(m_player);
}

int EVlc::setVolume(const int& volume)
{
	if (!m_instance || !m_media || !m_player) {
		return -1;
	}
	return libvlc_audio_set_volume(m_player, volume);
}

float EVlc::getLength()
{
	if (!m_instance || !m_media || !m_player) {
		return 0;
	}
	libvlc_time_t time = libvlc_media_player_get_length(m_player);
	float ret = time / 1000.0f;
	return ret;
}

vlcSize EVlc::getMediaInfo()
{
	vlcSize size;
	if (!m_instance || !m_media || !m_player) {
		return size;
	}

	size.width = libvlc_video_get_width(m_player);
	size.height = libvlc_video_get_height(m_player);

	return size;
}

string EVlc::unicode2Utf8(wstring strIn)
{
	string str;

	str.resize(strIn.size() * 3);
	int length = ::WideCharToMultiByte(CP_UTF8, 0, strIn.c_str(), strIn.size(), NULL, 0, NULL, NULL);
	str.resize(length + 1);
	length = ::WideCharToMultiByte(CP_UTF8, 0, strIn.c_str(), strIn.size(), (LPSTR)str.c_str(), length, NULL, NULL);

	return str;
}
