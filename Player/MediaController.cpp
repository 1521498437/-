#include "MediaController.h"
#include "Demuxer.h"

#include "PubDef.h"
#ifdef HARDWARE_DECODE
#include "HWDecoder.h"
#else
#include "Decoder.h"
#endif

#include "DataBuf.h"
#include "VideoThread.h"
#include "AudioThread.h"
#include "RtmpStreaming.h"
extern "C" 
{
	#include <libavformat/avformat.h>
	#include <libavdevice/avdevice.h> //nb
}
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avdevice.lib")
#include <cstring>
#include <mutex>
#include <QDebug>
#include <QCameraInfo>


MediaController* MediaController::GetInstance()
{
	static MediaController instance;
	return &instance;
}

MediaController::MediaController()
	:QObject(nullptr),
	_demuxer(new Demuxer),
	_vdecoder(new Decoder(Decoder::Type::VIDEO_DECODE)),
	_adecoder(new Decoder(Decoder::Type::AUDIO_DECODE)),
	_vthread(new VideoThread),
	_athread(new AudioThread),
	_streaming(new RtmpStreaming)
{
	static std::once_flag avof[2];
	std::call_once(avof[0], avformat_network_init);
	std::call_once(avof[1], avdevice_register_all);
	//ע���������
	_demuxer->registerErrorHandler(AVERROR_EOF, [this] {
		_isPlaying = false;
		emit sigPlayEnd();
	});
	_streaming->setConnErrCB(std::bind(&MediaController::sigRtmpError, this));//��RtmpStreaming�ĳ�QThread
	/*std::bind([this]() {
			printf("��Ƶ���Ž����ˣ�����\n");
	}));*/
}

MediaController::~MediaController()
{
	stop();/*??*/
	static std::once_flag avof;
	std::call_once(avof, avformat_network_deinit);
}

void MediaController::stop()
{
	DataBuf::Stop();
	_streaming->finish();
	_athread->stop();
	_vthread->stop();
	_adecoder->finish();
	_vdecoder->finish();
	_demuxer->finish();
}

bool MediaController::open(const OpenPara& para, IVideoCall* vcall, IAudioCall* acall)
{
	if (para.url.isEmpty() || para.url.isNull()) return false;
	char _url[1024] = { 0 };
	sprintf(_url, "%s", para.url.toLocal8Bit().data());
	last_url = para.url;
	_type = para.type;
	if (!_demuxer->open(_url, para.type == DEVICE ? true : false, para.timeout))
		return false;
	auto [vpara, vr] = _demuxer->copyVPara();
	auto [apara, ar] = _demuxer->copyAPara();
	DataBuf::Clear();
	_athread->setContinue(); //11����������Ƶ������Ȼ�������������Ƶ�ͱ����˵����⣬��������Ľ���ˣ����ն�λ�����λ�á�

	bool ret = false;
	if (_vdecoder->open(vpara, vr/*, Decoder::Type::VIDEO_DECODE*/)) //11ֱ���������
		ret = true;
	if (_adecoder->open(apara, ar/*, Decoder::Type::AUDIO_DECODE*/)) //00Ҫ����ֻ����Ƶ�������, ���Ľ�
		ret = true;
	_vthread->setCall(vcall, vpara);
	_athread->setCall(acall, apara);
	avcodec_parameters_free(&vpara);
	avcodec_parameters_free(&apara);
	if (ret) {
		_isPlaying = true;
		emit sigReadyPlay();
	}
	return ret;
}

void MediaController::close()
{
}

void MediaController::suspend() //��ͣ
{
	_vthread->pause(true);
	_athread->pause(true);
}

void MediaController::resume()
{
	_vthread->pause(false);
	_athread->pause(false);
}

bool MediaController::seek(double pos)
{
	_adecoder->current = _demuxer->duration * pos; //�����������˸bug
	DataBuf::Clear();
	_vdecoder->seek(pos * _demuxer->duration);
	if (!_demuxer->seek(pos))
	{
		_vdecoder->seek(0); //����һ��bug����������seek��������seek��ǰ�棬��������˸
		return false;
	}
	//_vthread->seek(pos * _demuxer->duration);  //��ͣ��ʱ���seek���⡣
	return true;
}

void MediaController::setPlaySpeed(double speed)
{
}

int64_t MediaController::getTotalMsec() const
{
	return _demuxer->duration;
}

int64_t MediaController::getPlayedMsec() const
{
	return _adecoder->current;
}

QDStringMap MediaController::getLiveList() const
{
	return QDStringMap();
}


QStringList MediaController::getDeviceList() const
{
	QStringList _list;
	Q_FOREACH(const QCameraInfo& info, QCameraInfo::availableCameras())
		_list << info.description();
	return _list;
}

void MediaController::startStreaming(const QString& url)
{
	if (_streaming->init(url.toStdString().c_str()))
		_streaming->run();
}
