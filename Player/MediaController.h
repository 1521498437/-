/**
* @file MediaController.h
* @brief ȫ��ý����Ʋ�
* @authors yandaoyang
* @date 2020/12/08
* @note ����ģʽ
*/

#pragma once
#include <QObject>
#include <QScopedPointer>
#define MEDIA_CTL MediaController::GetInstance()
using QDStringMap = QMap<QString, QString>;

class Demuxer;
class Decoder;
struct IVideoCall;
struct IAudioCall;
class VideoThread;
class AudioThread;
class RtmpStreaming;
class MediaController : public QObject
{
	Q_OBJECT

public:
	enum MediaType {
		NONE,
		FILE,
		LIVE,
		DEVICE,
		NODEV, //del able
	};
	struct OpenPara
	{
		QString url;
		//bool    isDevice;
		MediaType type;
		int     timeout;
		bool    reopen;
	};
	QString last_url; //????

public:
	static MediaController* GetInstance(); /* �����ӿ� */
	void stop();                           /* �ر������߳� */
	bool open(const OpenPara&, IVideoCall*, IAudioCall*);
	bool replay();                         /* ���²��� */
	void close();                          /* �ر�ý�� */
	void suspend();                        /* ��ͣ���� */
	void resume();                         /* �ָ����� */
	bool seek(double pos);                 /* ������ת */
	void setPlaySpeed(double speed);       /* ���ò������� */
	int64_t getTotalMsec() const;          /* ��ȡ��ʱ��(����) */
	int64_t getPlayedMsec() const;         /* ��ȡ�Ѳ���ʱ��(����) */
	QDStringMap getLiveList() const;       /* ��ȡֱ��Ƶ���б�(�����ļ�) */
	QStringList getDeviceList() const;     /* ��ȡ�����豸�б� *///��util����ظ���
	void startStreaming(const QString& url); /* ��ʼֱ������ */

	bool isPlaying() const { return _isPlaying; }; //�Ƿ��ڲ���״̬
	MediaType curPlayingType() const { return _type; } //��ȡ��ǰ�Ĳ�������

signals:
	void sigReadyPlay();
	void sigPlayEnd();
	void sigRtmpError();

private:
	bool    _isPlaying{ false };
	MediaType _type{ NONE };
	QScopedPointer<Demuxer>       _demuxer { nullptr }; /* �⸴���� */
	QScopedPointer<Decoder>       _vdecoder{ nullptr }; /* ��Ƶ������ */
	QScopedPointer<Decoder>       _adecoder{ nullptr }; /* ��Ƶ������ */
	QScopedPointer<VideoThread>   _vthread { nullptr }; /* ��Ƶˢ���߳� */
	QScopedPointer<AudioThread>   _athread { nullptr }; /* ��Ƶ�����߳� */
	QScopedPointer<RtmpStreaming> _streaming{ nullptr };/* rtmp�����߳� */

private:
	MediaController();
	~MediaController();
	MediaController(MediaController&&) = delete;
	MediaController(const MediaController&) = delete;
	MediaController& operator=(MediaController&&) = delete;
	MediaController& operator=(const MediaController&) = delete;
};
