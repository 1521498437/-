/**
* @file MediaController.h
* @brief 全局媒体控制层
* @authors yandaoyang
* @date 2020/12/08
* @note 单例模式
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
	static MediaController* GetInstance(); /* 单例接口 */
	void stop();                           /* 关闭所有线程 */
	bool open(const OpenPara&, IVideoCall*, IAudioCall*);
	bool replay();                         /* 重新播放 */
	void close();                          /* 关闭媒体 */
	void suspend();                        /* 暂停播放 */
	void resume();                         /* 恢复播放 */
	bool seek(double pos);                 /* 进度跳转 */
	void setPlaySpeed(double speed);       /* 设置播放速率 */
	int64_t getTotalMsec() const;          /* 获取总时长(毫秒) */
	int64_t getPlayedMsec() const;         /* 获取已播放时长(毫秒) */
	QDStringMap getLiveList() const;       /* 获取直播频道列表(配置文件) */
	QStringList getDeviceList() const;     /* 获取本地设备列表 *///和util里的重复了
	void startStreaming(const QString& url); /* 开始直播推流 */

	bool isPlaying() const { return _isPlaying; }; //是否处于播放状态
	MediaType curPlayingType() const { return _type; } //获取当前的播放类型

signals:
	void sigReadyPlay();
	void sigPlayEnd();
	void sigRtmpError();

private:
	bool    _isPlaying{ false };
	MediaType _type{ NONE };
	QScopedPointer<Demuxer>       _demuxer { nullptr }; /* 解复用器 */
	QScopedPointer<Decoder>       _vdecoder{ nullptr }; /* 视频解码器 */
	QScopedPointer<Decoder>       _adecoder{ nullptr }; /* 音频解码器 */
	QScopedPointer<VideoThread>   _vthread { nullptr }; /* 视频刷新线程 */
	QScopedPointer<AudioThread>   _athread { nullptr }; /* 音频播放线程 */
	QScopedPointer<RtmpStreaming> _streaming{ nullptr };/* rtmp推流线程 */

private:
	MediaController();
	~MediaController();
	MediaController(MediaController&&) = delete;
	MediaController(const MediaController&) = delete;
	MediaController& operator=(MediaController&&) = delete;
	MediaController& operator=(const MediaController&) = delete;
};
