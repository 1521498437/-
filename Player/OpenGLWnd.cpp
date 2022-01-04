#include "OpenGLWnd.h"
#include "PixelFmtConvert.h"
extern "C" {
#include <libavutil/frame.h>
#include <libswscale/swscale.h>
}
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")

#define VERTEXIN 0
#define TEXTUREIN 1
#define QUOTED(param) #param
//����shader
static const char* vsrc = QUOTED(
	attribute vec4 vertexIn;
	attribute vec2 textureIn;
	varying vec2 textureOut;
	void main(void) {
		gl_Position = vertexIn;
		textureOut = textureIn;
	}
);
//ƬԪshader
static const char* fsrc = QUOTED(
	varying vec2 textureOut; 
	uniform sampler2D tex_y; 
	uniform sampler2D tex_u; 
	uniform sampler2D tex_v; 
	void main(void) { 
		vec3 yuv; 
		vec3 rgb; 
		yuv.x = texture2D(tex_y, textureOut).r; 
		yuv.y = texture2D(tex_u, textureOut).r - 0.5; 
		yuv.z = texture2D(tex_v, textureOut).r - 0.5; 
		rgb = mat3(
			1,            1,            1,
			0,           -0.39465,  2.03211,
			1.13983, -0.58060,  0
		) * yuv; 
		gl_FragColor = vec4(rgb, 1); 
	}
);

OpenGLWnd::OpenGLWnd(QWidget* parent)
	: QOpenGLWidget(parent)
{
}

OpenGLWnd::~OpenGLWnd()
{
	makeCurrent();
	_vbo.destroy();
	_textureY->destroy();
	_textureU->destroy();
	_textureV->destroy();
	doneCurrent();
	DELETE_PTR(_converter);
}

/*///void OpenGLWnd::init(int width, int height)
{
	DELETE_PTR(_converter);
	_converter = new PixelFmtConvert(width, height, AV_PIX_FMT_YUV420P); // AV_PIX_FMT_NV16
}*///ת��_converter

/*///void OpenGLWnd::refresh(AVFrame* frame)
{
	if (_converter->cvtToBuf(frame, SWS_FAST_BILINEAR))
	{
		_mutex.lock();
		_yuvPtr = _converter->getOutBuf();
		_videoW = frame->width;
		_videoH = frame->height;
		_mutex.unlock();
		QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection); //Blocking
	}
}*///ת��_converter

void OpenGLWnd::initializeGL()
{
	initializeOpenGLFunctions();
	glEnable(GL_DEPTH_TEST);

	static const GLfloat vertices[]{
		//��������
		-1.0f, -1.0f,
		-1.0f, +1.0f,
		+1.0f, +1.0f,
		+1.0f, -1.0f,
		//��������
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
	};

	_vbo.create();
	_vbo.bind();
	_vbo.allocate(vertices, sizeof(vertices));

	QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	vshader->compileSourceCode(vsrc);
	QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	fshader->compileSourceCode(fsrc);

	_program = new QOpenGLShaderProgram(this);
	_program->addShader(vshader);
	_program->addShader(fshader);
	_program->bindAttributeLocation("vertexIn", VERTEXIN);
	_program->bindAttributeLocation("textureIn", TEXTUREIN);
	_program->link();
	_program->bind();
	_program->enableAttributeArray(VERTEXIN);
	_program->enableAttributeArray(TEXTUREIN);
	_program->setAttributeBuffer(VERTEXIN, GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
	_program->setAttributeBuffer(TEXTUREIN, GL_FLOAT, 8 * sizeof(GLfloat), 2, 2 * sizeof(GLfloat));

	_textureUniformY = _program->uniformLocation("tex_y");
	_textureUniformU = _program->uniformLocation("tex_u");
	_textureUniformV = _program->uniformLocation("tex_v");
	_textureY = new QOpenGLTexture(QOpenGLTexture::Target2D);
	_textureU = new QOpenGLTexture(QOpenGLTexture::Target2D);
	_textureV = new QOpenGLTexture(QOpenGLTexture::Target2D);
	_textureY->create();
	_textureU->create();
	_textureV->create();
	_idY = _textureY->textureId();
	_idU = _textureU->textureId();
	_idV = _textureV->textureId();
	glClearColor(0.0, 0.0, 0.0, 0.0);
}

void OpenGLWnd::paintGL()
{
	//    QMatrix4x4 m;
//    m.perspective(60.0f, 4.0f/3.0f, 0.1f, 100.0f );//͸�Ӿ��������ı仯��ͼ�θ��ű仯����Ļƽ�����ľ����ӵ㣨����ͷ��,��Ҫ��ͼ��������Ļ����һ�����롣
//    m.ortho(-2,+2,-2,+2,-10,10);//���ü�ƽ����һ������,�������½ǵ���ά�ռ������ǣ�left,bottom,-near��,���Ͻǵ��ǣ�right,top,-near�����Դ˴�Ϊ������ʾz�����Ϊ10��
					  //Զ�ü�ƽ��Ҳ��һ������,���½ǵ�ռ������ǣ�left,bottom,-far��,���Ͻǵ��ǣ�right,top,-far�����Դ˴�Ϊ������ʾz����СΪ-10��
					  //��ʱ�������Ļ�������Ļˮƽ���м䣬ֻ��ǰ�����ҵľ��������ơ�
	glActiveTexture(GL_TEXTURE0);  //��������ԪGL_TEXTURE0,ϵͳ�����
	glBindTexture(GL_TEXTURE_2D, _idY); //��y�����������id�����������Ԫ
	//ʹ���ڴ��е����ݴ���������y������������
	_mutex.lock();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, _videoW, _videoH, 0, GL_RED, GL_UNSIGNED_BYTE, _yuvPtr);
	//https://blog.csdn.net/xipiaoyouzi/article/details/53584798 �����������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE1); //��������ԪGL_TEXTURE1
	glBindTexture(GL_TEXTURE_2D, _idU);
	//ʹ���ڴ��е����ݴ���������u������������
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, _videoW >> 1, _videoH >> 1, 0, GL_RED,
		GL_UNSIGNED_BYTE, _yuvPtr + _videoW * _videoH);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE2); //��������ԪGL_TEXTURE2
	glBindTexture(GL_TEXTURE_2D, _idV);
	//ʹ���ڴ��е����ݴ���������v������������
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, _videoW >> 1, _videoH >> 1, 0, GL_RED,
		GL_UNSIGNED_BYTE, _yuvPtr + _videoW * _videoH * 5 / 4);
	_mutex.unlock();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//ָ��y����Ҫʹ����ֵ
	glUniform1i(_textureUniformY, 0);
	//ָ��u����Ҫʹ����ֵ
	glUniform1i(_textureUniformU, 1);
	//ָ��v����Ҫʹ����ֵ
	glUniform1i(_textureUniformV, 2);
	//ʹ�ö������鷽ʽ����ͼ��
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void OpenGLWnd::resizeGL(int w, int h)
{
	qDebug() << "resizeGL" << w << " " << h;
}
