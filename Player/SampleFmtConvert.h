/**
* @file SampleFmtConvert.h
* @brief 音频(重)采样格式转换类
* @authors yandaoyang
* @date 2020/12/07
* @note 当前内存已安全，待考虑加锁问题
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
