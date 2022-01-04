/**
* @file BaseFmtConvert.h
* @brief ����֡��ʽת������
* @authors yandaoyang
* @date 2020/12/05
* @note 
*/

#pragma once
#include <mutex>
#include <memory>

struct AVFrame;
class BaseFmtConvert
{
public:
	BaseFmtConvert() = default;
	virtual ~BaseFmtConvert() = default;

public:
	virtual int cvtToBuf(AVFrame*, int algorithm) = 0;
	/*virtual*/ uint8_t* getOutBuf() { return _out_buffer.get(); }

protected:
	std::mutex                 _mutex;
	int                        _dst_format{ -1 };
	std::unique_ptr<uint8_t[]> _out_buffer{ nullptr };
};
