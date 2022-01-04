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
	void run(); //�����߳�
	void finish(); //ֹͣ�߳�
	bool init(int, int, int, int = 1024); //��ʼ����Դ
	void deinit(); //�ͷ���Դ

private:
	int _nbSamples = 1024;
	QAudioFormat _fmt;
	QAudioInput* _input{ nullptr };
	QIODevice* _device{ nullptr };
	std::mutex _mtx;
	std::thread _work_thread;
	std::atomic_bool _shutdown{ false };
};

