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

	// libvlc库使用范例
	// 1、创建实例
	int argc = 1;
	char* argv[2];
	argv[0] = (char*)"--ignore-config";	// 忽略配置
	libvlc_instance_t* instance =  libvlc_new(argc, argv);
	string file = unicode2Utf8(L"file:///C:\\Users\\12399\\Desktop\\测试.mp4");

	// 2、打开文件 加载媒体
	//libvlc_media_t* media = libvlc_media_new_path(instance, file.c_str());	// path是以当前程序/当前工程为路径去寻找文件
	libvlc_media_t* media = libvlc_media_new_location(instance, file.c_str());	// location是通过绝对路径寻找文件 不可使用相对路径
	
	// 3、创建播放器
	libvlc_media_player_t* player =  libvlc_media_player_new_from_media(media);

	// 4、开始播放
	int ret = 0;
	do {
		ret = libvlc_media_player_play(player);
		if (ret != 0) {
			cout << "播放视频错误！" << endl;
			break;
		}

		// 4.1、获取视频音量
		int vol = -1;
		while (vol == -1) {
			Sleep(10);
			vol = libvlc_audio_get_volume(player);
			cout << "音量为:" << vol << endl;
		}

		// 4.2、获取视频时长 单位：ms
		libvlc_time_t time = libvlc_media_player_get_length(player);
		cout << "视频时长:" << time << "毫秒" << endl;
		time /= 1000;
		printf("%02d:%02d:%02d\n", (int)time/3600, (int)time/60, (int)time%60);

		// 4.3、获取视频宽高
		int width = libvlc_video_get_width(player);
		int height = libvlc_video_get_height(player);
		cout << "视频宽：" << width << " 视频高：" << height << endl;

		// 4.4、获取播放位置
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

	// 这三个的释放顺序与创建顺序相反！
	// 5、释放播放器
	libvlc_media_player_release(player);

	// 6、释放媒体
	libvlc_media_release(media);

	// 7、释放实例
	libvlc_release(instance);
	

	return 0;
}