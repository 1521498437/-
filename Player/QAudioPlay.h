/**
* @file QAudioPlay.h
* @brief IAudioCallʵ����
* @authors yandaoyang
* @date 2020/12/07
* @note ����Qt�Դ�����Ƶ���Žӿ�
* ��ǰ�ڴ��Ѱ�ȫ���������������Ǽ�������
*/

#pragma once
#include <memory>
#include <QAudioOutput>
#include "IAudioCall.h"
#define AUDIO_PLAY QAudioPlay::GetInstance()

class QAudioPlay : public IAudioCall
{
public:
	static QAudioPlay* GetInstance();
	~QAudioPlay();
	void changeVolume(qreal rate) {
		_output->setVolume(rate);
	}

protected:
	void clear();
	void init(int sample_rate, int channels, int format) override;
	void write(AVFrame*) override;
	int64_t noPlayMs() override;

private:
	QAudioPlay() = default;

private:
	static std::unique_ptr<QAudioPlay> _instance;
	int                                _bytes_per_second{ 0 };
	QIODevice*                         _io       { nullptr };
	std::unique_ptr<QAudioOutput>      _output   { nullptr };
	std::unique_ptr<BaseFmtConvert>    _converter{ nullptr };
};
