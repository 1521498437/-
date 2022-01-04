/**
* @file PixelFmtConvert.h
* @brief ���ظ�ʽת����
* @authors yandaoyang
* @date 2020/12/05
* @note ��ǰ�ڴ��Ѱ�ȫ�������Ǽ�������
*/

#pragma once
#include "BaseFmtConvert.h"

struct SwsContext;
class PixelFmtConvert : public BaseFmtConvert
{
public:
	PixelFmtConvert(int width, int height, int dst_format);
	PixelFmtConvert(int dst_format);
	~PixelFmtConvert();
	int width() const { return _width; }
	int height() const { return _height; }
	//return: 1--true 0--false
	int cvtToBuf(AVFrame*, int algorithm) override;

protected:
	void initOutBuf(int width, int height);

private:
	using deleter = void(*)(AVFrame*);
	std::unique_ptr<AVFrame, deleter> _picture;
	SwsContext* _sws{ nullptr };
	int _width = 0;
	int _height = 0;
};
