/**
* @file DataBuf.h
* @brief 媒体数据缓冲区
* @authors yandaoyang
* @date 2020/12/08
* @note 
*/

#pragma once
#include <memory>
//#include "ObjectPool.h"
#include "SyncBufferQueue.h"

struct AVPacket;
struct AVFrame;

struct AVData
{
public:
	char* data = 0;
	int size = 0;
	long long pts = 0;
	//创建空间，并复制data内容
	 AVData(char* data, int size, long long p = 0){
		this->data = (char*)malloc(size);
		memcpy(this->data, data, size);
		this->size = size;
		this->pts = p;
	}
	~AVData() {
		if (data) free(data);
		data = nullptr;
		size = 0;
	}
};

using PacketPtr = std::shared_ptr<AVPacket>;
using FramePtr = std::shared_ptr<AVFrame>;
using DataPtr = std::shared_ptr<AVData>;
struct DataBuf
{
public:
	static inline SyncBufferQueue<16, PacketPtr> VPacketQueue;
	static inline SyncBufferQueue<16, PacketPtr> APacketQueue;
	static inline SyncBufferQueue<3, FramePtr>   VFrameQueue;
	static inline SyncBufferQueue<3, FramePtr>   AFrameQueue;
	static inline SyncBufferQueue<10, DataPtr> VStreamingQueue;
	static inline SyncBufferQueue<10, DataPtr> AStreamingQueue;

public:
	static void Stop()
	{
		VPacketQueue.stop();
		APacketQueue.stop();
		VFrameQueue.stop();
		AFrameQueue.stop();
		VStreamingQueue.stop();
		AStreamingQueue.stop();
	}
	static void Clear()
	{
		VPacketQueue.clear();
		APacketQueue.clear();
		VFrameQueue.clear();
		AFrameQueue.clear();
		VStreamingQueue.clear();
		AStreamingQueue.clear();
	}
};
