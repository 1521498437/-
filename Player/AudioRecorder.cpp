#include "AudioRecorder.h"
#include "DataBuf.h"
#include <QDebug>
extern "C" {
#include <libavutil/time.h>
#include <libavutil/frame.h>
}
#pragma comment(lib, "avutil.lib")

AudioRecorder::AudioRecorder()
{
}

AudioRecorder::~AudioRecorder()
{
	deinit();
	finish();
}

void AudioRecorder::run()
{
	this->finish();
	_work_thread = std::thread([this] {
		_shutdown = false;
		int readSize = _nbSamples * _fmt.channelCount() * (_fmt.sampleSize() / 8);
		char* buf = new char[readSize];
		while (!_shutdown)
		{
			//一次读取一帧音频
			if (_input->bytesReady() < readSize)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}
			int size = 0;
			while (size != readSize)
			{
				int len = _device->read(buf + size, readSize - size);
				if (len < 0) break;
				size += len;
			}
			if (size != readSize) continue;

			AVFrame* frame = av_frame_alloc();
			//frame->

			//DataBuf::AStreamingQueue.enqueue(std::make_shared<AVData>(buf, readSize, av_gettime()));
		}
		delete[] buf;
	});
}

void AudioRecorder::finish()
{
	if (!_shutdown)
	{
		_shutdown = true;
		if (_work_thread.joinable())
			_work_thread.join();
	}
}

bool AudioRecorder::init(int sampleRate, int channelCount, int sampleSize, int nbSamples)
{
	//if (sampleRate<=0 || ...) return false;
	deinit();
	std::lock_guard<std::mutex> lck(_mtx);
	_nbSamples = nbSamples;
	_fmt.setSampleRate(sampleRate);
	_fmt.setChannelCount(channelCount);
	_fmt.setSampleSize(sampleSize); //byte
	_fmt.setCodec("audio/pcm");
	_fmt.setByteOrder(QAudioFormat::LittleEndian);
	_fmt.setSampleType(QAudioFormat::UnSignedInt);
	QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
	if (!info.isFormatSupported(_fmt))
	{
		qDebug() << "audio format not support" << endl;
		_fmt = info.nearestFormat(_fmt);
	}
	if(!_input) _input = new QAudioInput(_fmt);
	//开始录制音频
	_device = _input->start();
	return _device ? true : false;
}

void AudioRecorder::deinit()
{
	std::lock_guard<std::mutex> lck(_mtx);
	if (!_input || !_device) return;
	_input->stop();
	_device->close();
	delete _input;
	_input = nullptr;
	_device = nullptr;
}
