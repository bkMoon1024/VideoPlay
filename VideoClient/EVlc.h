#pragma once

#include "vlc.h"
#include <string>;

using namespace std;

typedef struct _vlcSize_t_ {
	int width;	// 视频宽度
	int height;	// 视频高度

	_vlcSize_t_(int width = 0, int height = 0) {
		this->width = width;
		this->height = height;
	}

	_vlcSize_t_(const _vlcSize_t_& size) {
		this->width = width;
		this->height = height;
	}

	_vlcSize_t_& operator=(const _vlcSize_t_& size) {
		if (this != &size) {
			this->width = width;
			this->height = height;
		}

		return *this;
	}
} vlcSize;

class EVlc
{
public:
	EVlc();
	~EVlc();

	int setMedia(const string& strUrl);

	// 设置播放窗口
	int setHwnd(const HWND hwnd);

	// 播放
	int play();

	// 暂停播放
	int pause();

	// 停止播放
	int stop();

	// 播放进度
	float getPosition();
	int setPosition(const float& pos);

	// 音量
	int getVolume();
	int setVolume(const int& volume);

	// 获取视频宽高
	vlcSize getMediaInfo();

protected:
	libvlc_instance_t*			m_instance = nullptr;
	libvlc_media_t*				m_media = nullptr;
	libvlc_media_player_t*		m_player = nullptr;
};

