#include "QAudioPlay.h"
#include "SampleFmtConvert.h"
#include <QAudioFormat>
extern "C" 
{
	#include <libavutil/frame.h>
	#include <libswresample/swresample.h>
}
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swresample.lib")
#include <QDebug>

std::unique_ptr<QAudioPlay> QAudioPlay::_instance{new QAudioPlay};

QAudioPlay* QAudioPlay::GetInstance()
{
	//static QAudioPlay instance;
	return QAudioPlay::_instance.get();
}

QAudioPlay::~QAudioPlay()
{
	clear();
}

void QAudioPlay::clear()
{
	if (_io) {
		_io->close();
		_io = nullptr;
	}
	if (_output) {
		_output->stop();
		//DELETE_PTR(_output);
	}
	//DELETE_PTR(_converter);
}

void QAudioPlay::init(int sample_rate, int channels, int format)
{
	clear();
	//_converter����Ӧ���붨λnew���Ѽ��룩������ָ�루�Ѽ��룩֧��
	//if (!_converter)
	_converter.reset(new SampleFmtConvert(channels, sample_rate, format, AV_SAMPLE_FMT_S16));
	/*else {
		_converter.get()->~BaseFmtConvert();
		new(_converter.get())SampleFmtConvert(channels, sample_rate, format, AV_SAMPLE_FMT_S16);
	}*/
	/*if (!_converter)
		_converter.reset(new SampleFmtConvert(AV_SAMPLE_FMT_S16));
	((SampleFmtConvert*)_converter.get())->setFmt(channels, sample_rate, format);*/
	qDebug() << "channels" << channels;
	qDebug() << "sample_rate" << sample_rate;
	qDebug() << "format" << format;
	_bytes_per_second = sample_rate * channels * (16 / 8)/*sample_size*/;

	QAudioFormat fmt;
	fmt.setSampleRate(sample_rate);
	fmt.setSampleSize(16);
	fmt.setChannelCount(channels);
	fmt.setCodec("audio/pcm");
	fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setSampleType(QAudioFormat::SignedInt);
	_output.reset(new QAudioOutput(fmt)); //ȱ��
	_output->setBufferSize(65535 * 2); //Ĭ�ϴ�СΪ����࣬�����Ĭ�ϴ�С�Ļ������š�����.wmv��ֱ�ӿ���
									   //������Ϊ������̫СbytesFree()ʼ��С��cvtToBuf�ķ���ֵ����������
	_io = _output->start();
}

void QAudioPlay::write(AVFrame* frame)
{
	if (!frame || !_converter) return;
	int size = _converter->cvtToBuf(frame, -1);
	if (size <= 0) return;
	//if (_output->bufferSize() < size)
	//{
	//	_output->setBufferSize(1); //�в�ͨ
	//}
	for (;;)
	{
		//qDebug() << "bufferSize()" << _output->bufferSize();
		//qDebug() << "size" << size;
		if (_output->bytesFree() < size)
		{
			//qDebug() << "----------------";
			//qDebug() << "byte free" << _output->bytesFree();
			//qDebug() << "buff size" << size;
			//qDebug() << "----------------";
			continue;
		}
		qint64 ret = _io->write((const char*)_converter->getOutBuf(), size);
		//qDebug() << "----------------";
		//qDebug() << "byte free" << _output->bytesFree();
		//qDebug() << "write ret" << ret;
		//qDebug() << "----------------";
		//return ret == size ? true : false;
		break;
	}
	/*int period = _output->periodSize();
	int chunks = _output->bytesFree() / period;
	for (int i = 0; i < chunks; i++)
	{

	}*/
}

int64_t QAudioPlay::noPlayMs()
{
	if (!_output) return 0;
	int no_play_bytes = _output->bufferSize() - _output->bytesFree();
	return _bytes_per_second <= 0 ? 0 : no_play_bytes / _bytes_per_second * 1000;
}
