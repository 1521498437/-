#include "SampleFmtConvert.h"
extern "C" 
{
	#include <libavutil/frame.h>
	#include <libswresample/swresample.h>
}
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swresample.lib")
#include <QDebug>


SampleFmtConvert::SampleFmtConvert(int channels, int sample_rate, int src_format, int dst_format)
{
	std::lock_guard<std::mutex> lck(_mutex);
	swr = swr_alloc();
	swr = swr_alloc_set_opts(swr,
		av_get_default_channel_layout(channels), //����ĸ�ʽ ��Ҫ��2!!!
		(AVSampleFormat)dst_format,              //���������ʽ AV_SAMPLE_FMT_S16
		sample_rate,                             //���������
		av_get_default_channel_layout(channels),
		(AVSampleFormat)src_format,
		sample_rate,
		0, nullptr);
	swr_init(swr);
	_dst_format = dst_format;
	if(!_out_buffer) _out_buffer.reset(new uint8_t[1024 * 1024]); //ȱ��
}

/*SampleFmtConvert::SampleFmtConvert(int dst_format)
{
	_dst_format = dst_format;
	_out_buffer.reset(new uint8_t[1024 * 1024]);
}*/

SampleFmtConvert::~SampleFmtConvert()
{
	if(swr) swr_free(&swr);//swr_close
}

/*void SampleFmtConvert::setFmt(int channels, int sample_rate, int src_format)
{
	std::lock_guard<std::mutex> lck(_mutex);
	if (swr) swr_free(&swr);
	swr = swr_alloc();
	swr = swr_alloc_set_opts(swr,
		av_get_default_channel_layout(channels), //����ĸ�ʽ ��Ҫ��2!!!
		(AVSampleFormat)_dst_format,             //���������ʽ AV_SAMPLE_FMT_S16
		sample_rate,                             //���������
		av_get_default_channel_layout(channels),
		(AVSampleFormat)src_format,
		sample_rate,
		0, nullptr);
	swr_init(swr);
}
static uint8_t* OutBuf = new uint8_t[1024 * 1024];
uint8_t* SampleFmtConvert::getOutBuf() //�����ַ�ʽ
{
	return OutBuf;
}*/

int SampleFmtConvert::cvtToBuf(AVFrame* frame, int /*algorithm*/)
{
	if (!frame) return false;
	std::lock_guard<std::mutex> lck(_mutex);
	if (!swr) return false; //���ǲ���
	uint8_t* data[2] = { /*OutBuf*/_out_buffer.get(), nullptr };
	//qDebug() << "frame->nb_samples" << frame->nb_samples; //������.wmv��14336  ����1024
	int ret = swr_convert( //@return number of samples output per channel, negative value on error  
						   //����������ˣ�������λnewҲ���У�2020.12.24 02:54  CCTV1Ȼ��CCTV6�ͱ����� 
						   //�����˶�λnew���������½ӿ�setFmt���Ǳ��ˣ�2020.12.24 03:23
		swr, data, 
		frame->nb_samples, /*1024*/
		(const uint8_t**)frame->data, 
		frame->nb_samples
	);
	//qDebug() << "swr_convert ret" << ret;
	int out_size = ret * frame->channels * av_get_bytes_per_sample((AVSampleFormat)_dst_format);
	//qDebug() << av_get_bytes_per_sample((AVSampleFormat)_dst_format); // 2
	return ret <= 0 ? ret : out_size;
}
