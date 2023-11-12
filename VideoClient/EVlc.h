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

#ifdef WIN32
	// ���ò��Ŵ���
	int setHwnd(const HWND hwnd);
#endif

	// ����
	int play();

	// ��ͣ����
	int pause();

	// ֹͣ����
	int stop();

	// ���Ž��� ���ٷֱȷ���
	float getPosition();
	int setPosition(const float& pos);

	// ����
	int getVolume();
	int setVolume(const int& volume);
	
	// ��ȡ��Ƶʱ�� ��λ��s
	float getLength();

	// ��ȡ��Ƶ���
	vlcSize getMediaInfo();

	string unicode2Utf8(wstring strIn);

protected:
	libvlc_instance_t*			m_instance = nullptr;
	libvlc_media_t*				m_media = nullptr;
	libvlc_media_player_t*		m_player = nullptr;
	string						m_url;
#ifdef WIN32
	HWND						m_hwnd = nullptr;
#endif
};

