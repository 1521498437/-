/**
* @file IVideoCall.h
* @brief ��Ƶ��ʾ�ӿ�
* @authors yandaoyang
* @date 2020/12/04
* @note ���Բ�ͬ��Ⱦ��ʽ�Ĳ���
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

//2020.12.07���ԣ��̶����������β�����ͳһ��������롢sws_scaleת�����ظ�ʽ����δ�����ܸĽ���Ӳ���롢libyuv�⣩
//������Ƶ��D:/Document/FFmpeg&SDL/FFMPEG/��˿��ʿ.mov
//CPUʹ���ʣ�SDLWnd(39%~47%) > QPainterWnd(17%~22%) > QLabelWnd(8%~14%) > OpenGLWnd(7%~11%)
//�ڴ�ռ���ʣ�OpenGLWnd(94.5MB) > SDLWnd(50.2MB) > QLabelWnd(38.6MB) > QPainterWnd(36.2;21.1MB)
//�������ƶȣ�QPainterWnd > OpenGLWnd > QLabelWnd > SDLWnd
//�����Ѷ�/ѧϰ�ɱ���OpenGLWnd > SDLWnd > QPainterWnd > QLabelWnd
