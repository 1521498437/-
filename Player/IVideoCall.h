/**
* @file IVideoCall.h
* @brief 视频显示接口
* @authors yandaoyang
* @date 2020/12/04
* @note 测试不同渲染方式的差异
*/

#pragma once
#define DELETE_PTR(ptr) if(ptr) { delete ptr; ptr=nullptr; }
#define DELETE_ARY(ptr) if(ptr) { delete[] ptr; ptr=nullptr; }

struct IVideoCall
{
	//virtual void init(int width, int height) = 0;
	virtual void init(int width, int height) {};
	virtual void refresh(uchar* data, int w, int h) = 0;
};

//2020.12.07测试（固定变量：本次测试中统一采用软解码、sws_scale转换像素格式）（未来可能改进：硬解码、libyuv库）
//测试视频：D:/Document/FFmpeg&SDL/FFMPEG/屌丝男士.mov
//CPU使用率：SDLWnd(39%~47%) > QPainterWnd(17%~22%) > QLabelWnd(8%~14%) > OpenGLWnd(7%~11%)
//内存占用率：OpenGLWnd(94.5MB) > SDLWnd(50.2MB) > QLabelWnd(38.6MB) > QPainterWnd(36.2;21.1MB)
//功能完善度：QPainterWnd > OpenGLWnd > QLabelWnd > SDLWnd
//编码难度/学习成本：OpenGLWnd > SDLWnd > QPainterWnd > QLabelWnd
