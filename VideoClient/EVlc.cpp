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
/// 设置播放路径
/// </summary>
/// <param name="strUrl">播放路径，必须为UTF8字符串</param>
/// <returns>0-操作成功，其他-操作失败</returns>
int EVlc::setMedia(const string& strUrl)
{
	if (!m_instance) {
		return -1;
	}
	m_media = libvlc_media_new_location(m_instance, strUrl.c_str());
	if (!m_media) {
		return -2;
	}
	m_player = libvlc_media_player_new_from_media(m_media);
	if (!m_player) {
		return -3;
	}
	return 0;
}

int EVlc::setHwnd(const HWND hwnd)
{
	if (!m_instance || !m_media || !m_player) {
		return -1;
	}
	
	libvlc_media_player_set_hwnd(m_player, hwnd);

	return 0;
}

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
		return 100.0 * libvlc_media_player_get_position(m_player);
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
