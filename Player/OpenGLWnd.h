/**
* @file OpenGLWnd.h
* @brief IVideoCallʵ����
* @authors ydy
* @date 2020/12/05
* @note ����Qt��OpenGL�ӿ���Ⱦ��ʾ
* ��ǰ�ڴ��Ѱ�ȫ���������������Ǽ�������
*/

#pragma once
#include <QMutex>
#include <QOpenGLBuffer>
#include <QOpenGLWidget>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include "IVideoCall.h"

//https://blog.csdn.net/wanghualin033/article/details/79683836
//class AbstractFmtConvert;
class BaseFmtConvert;
class OpenGLWnd : public QOpenGLWidget, protected QOpenGLFunctions, public IVideoCall
{
	Q_OBJECT

public:
	explicit OpenGLWnd(QWidget* p = Q_NULLPTR);
	~OpenGLWnd();

protected:
	///void init(int width, int height) Q_DECL_OVERRIDE; //ת��_converter
	///void refresh(AVFrame*) Q_DECL_OVERRIDE; //ת��_converter

protected:
	void initializeGL() Q_DECL_OVERRIDE;     //��ʼ��GL
	void paintGL() Q_DECL_OVERRIDE;          //ˢ����ʾ
	void resizeGL(int, int) Q_DECL_OVERRIDE; //���ڳߴ�仯

private:
	GLuint                _idY;
	GLuint                _idU;
	GLuint                _idV;
	QOpenGLBuffer         _vbo;
	QMutex                _mutex;
	uint                  _videoW;
	uint                  _videoH;
	GLuint                _textureUniformY;
	GLuint                _textureUniformU;
	GLuint                _textureUniformV;
	uchar                *_yuvPtr   { nullptr };
	QOpenGLShaderProgram *_program  { nullptr };
	QOpenGLTexture       *_textureY { nullptr };
	QOpenGLTexture       *_textureU { nullptr };
	QOpenGLTexture       *_textureV { nullptr };
	BaseFmtConvert       *_converter{ nullptr };
};

