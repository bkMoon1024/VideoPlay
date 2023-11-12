#pragma once

#include "vlc.h"
#include <string>;

using namespace std;

typedef struct _vlcSize_t_ {
	int width;	// ��Ƶ���
	int height;	// ��Ƶ�߶�

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

	// ���ò��Ŵ���
	int setHwnd(const HWND hwnd);

	// ����
	int play();

	// ��ͣ����
	int pause();

	// ֹͣ����
	int stop();

	// ���Ž���
	float getPosition();
	int setPosition(const float& pos);

	// ����
	int getVolume();
	int setVolume(const int& volume);

	// ��ȡ��Ƶ���
	vlcSize getMediaInfo();

protected:
	libvlc_instance_t*			m_instance = nullptr;
	libvlc_media_t*				m_media = nullptr;
	libvlc_media_player_t*		m_player = nullptr;
};

