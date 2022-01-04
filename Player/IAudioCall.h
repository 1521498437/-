/**
* @file IAudioCall.h
* @brief ÒôÆµ²¥·Å½Ó¿Ú
* @authors yandaoyang
* @date 2020/12/07
* @note
*/

#pragma once

struct AVFrame;
class BaseFmtConvert;
struct IAudioCall
{
	virtual ~IAudioCall() = default;
	virtual void init(int sample_rate, int channels, int format) = 0;
	virtual void write(AVFrame*) = 0;
	virtual int64_t noPlayMs() = 0;
};
