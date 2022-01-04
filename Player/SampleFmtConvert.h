/**
* @file SampleFmtConvert.h
* @brief ��Ƶ(��)������ʽת����
* @authors yandaoyang
* @date 2020/12/07
* @note ��ǰ�ڴ��Ѱ�ȫ�������Ǽ�������
*/

#pragma once
#include "BaseFmtConvert.h"

struct SwrContext;
class SampleFmtConvert : public BaseFmtConvert
{
public:
	SampleFmtConvert(int channels, int sample_rate, int src_format, int dst_format);
	//SampleFmtConvert(int dst_format);
	~SampleFmtConvert();
	//void setFmt(int channels, int sample_rate, int src_format);

public:
	//uint8_t* getOutBuf() override;
	int cvtToBuf(AVFrame*, int algorithm) override;

private:
	SwrContext* swr{ nullptr };
};
