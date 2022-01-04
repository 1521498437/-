#pragma once
#include <mutex>
#include <atomic>
#include <thread>
#include <QAudioInput>

class AudioRecorder
{
public:
	AudioRecorder();
	~AudioRecorder();
	void run(); //启动线程
	void finish(); //停止线程
	bool init(int, int, int, int = 1024); //初始化资源
	void deinit(); //释放资源

private:
	int _nbSamples = 1024;
	QAudioFormat _fmt;
	QAudioInput* _input{ nullptr };
	QIODevice* _device{ nullptr };
	std::mutex _mtx;
	std::thread _work_thread;
	std::atomic_bool _shutdown{ false };
};

