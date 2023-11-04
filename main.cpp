#include <iostream>
#include <cstring>
#include <conio.h>
#include "vlc.h"
#include <Windows.h>

using namespace std;

string unicode2Utf8(wstring strIn) {
	string str;

	str.resize(strIn.size() * 3);
	int length = ::WideCharToMultiByte(CP_UTF8, 0, strIn.c_str(), strIn.size(), NULL, 0, NULL, NULL);
	str.resize(length + 1);
	length = ::WideCharToMultiByte(CP_UTF8, 0, strIn.c_str(), strIn.size(), (LPSTR)str.c_str(), length, NULL, NULL);

	return str;
}


int main() {

	// libvlc��ʹ�÷���
	// 1������ʵ��
	int argc = 1;
	char* argv[2];
	argv[0] = (char*)"--ignore-config";	// ��������
	libvlc_instance_t* instance =  libvlc_new(argc, argv);
	string file = unicode2Utf8(L"file:///C:\\Users\\12399\\Desktop\\����.mp4");

	// 2�����ļ� ����ý��
	//libvlc_media_t* media = libvlc_media_new_path(instance, file.c_str());	// path���Ե�ǰ����/��ǰ����Ϊ·��ȥѰ���ļ�
	libvlc_media_t* media = libvlc_media_new_location(instance, file.c_str());	// location��ͨ������·��Ѱ���ļ� ����ʹ�����·��
	
	// 3������������
	libvlc_media_player_t* player =  libvlc_media_player_new_from_media(media);

	// 4����ʼ����
	int ret = 0;
	do {
		ret = libvlc_media_player_play(player);
		if (ret != 0) {
			cout << "������Ƶ����" << endl;
			break;
		}

		// 4.1����ȡ��Ƶ����
		int vol = -1;
		while (vol == -1) {
			Sleep(10);
			vol = libvlc_audio_get_volume(player);
			cout << "����Ϊ:" << vol << endl;
		}

		// 4.2����ȡ��Ƶʱ�� ��λ��ms
		libvlc_time_t time = libvlc_media_player_get_length(player);
		cout << "��Ƶʱ��:" << time << "����" << endl;
		time /= 1000;
		printf("%02d:%02d:%02d\n", (int)time/3600, (int)time/60, (int)time%60);

		// 4.3����ȡ��Ƶ���
		int width = libvlc_video_get_width(player);
		int height = libvlc_video_get_height(player);
		cout << "��Ƶ��" << width << " ��Ƶ�ߣ�" << height << endl;

		// 4.4����ȡ����λ��
		while (!_kbhit()) {
			printf("%f\n", 100.0 * libvlc_media_player_get_position(player));
			Sleep(500);
		}

		getchar();
		libvlc_media_player_pause(player);
		getchar();
		libvlc_media_player_play(player);
		getchar();
		libvlc_media_player_stop(player);

	} while (0);

	// ���������ͷ�˳���봴��˳���෴��
	// 5���ͷŲ�����
	libvlc_media_player_release(player);

	// 6���ͷ�ý��
	libvlc_media_release(media);

	// 7���ͷ�ʵ��
	libvlc_release(instance);
	

	return 0;
}