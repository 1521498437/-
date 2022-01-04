#include "HWDecoder.h"

#ifdef HARDWARE_DECODE


#include "DataBuf.h"
extern "C"
{
#include <libavcodec/avcodec.h>
}
#pragma comment(lib, "avcodec.lib")
#include <thread>
#include <QDebug>


static AVPixelFormat hw_pix_fmt;

Decoder::Decoder(Type type)
	:_type(type)
{
	run();
}

Decoder::~Decoder()
{
	close();
	finish();
}

void Decoder::init(AVCodec* codec)
{
	//_hw_type = av_hwdevice_iterate_types(_hw_type); 迭代器
	_hw_type = av_hwdevice_find_type_by_name("cuda"); //"cuda"; // dxva2 d3d11va qsv(h264不支持)
	if (_hw_type != AV_HWDEVICE_TYPE_NONE)
	{
		for (int i = 0;; i++) {
			const AVCodecHWConfig* config = avcodec_get_hw_config(codec, i);
			if (!config) {
				fprintf(stderr, "Decoder %s does not support device type %s.\n",
					codec->name, av_hwdevice_get_type_name(_hw_type));
				return;
			}
			if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
				config->device_type == _hw_type) {
				hw_pix_fmt = config->pix_fmt;
				break;
			}
		}
	}
	_codec_ctx->get_format = [](AVCodecContext* ctx, const enum AVPixelFormat* pix_fmts)->enum AVPixelFormat {
		const enum AVPixelFormat* p;
		for (p = pix_fmts; *p != -1; p++)
			if (*p == hw_pix_fmt)
				return *p;
		fprintf(stderr, "Failed to get HW surface format.\n");
		return AV_PIX_FMT_NONE;
	};

	if (av_hwdevice_ctx_create(&_hw_device_ctx, _hw_type, NULL, NULL, 0) < 0) {
		fprintf(stderr, "Failed to create specified HW device.\n");
		return;
	}
	_codec_ctx->hw_device_ctx = av_buffer_ref(_hw_device_ctx);
}

void Decoder::run()
{
	this->finish();
	_work_thread = std::thread([this] {
		auto deleter = [](AVFrame* frame) {
			if (frame) av_frame_free(&frame);
		};
		_shutdown = false;
		while (!_shutdown)
		{
			PacketPtr pkt{ nullptr };
			//static int64_t master_timestamp{ 0 };
			if (_type == Type::AUDIO_DECODE)
			{
				DataBuf::APacketQueue.dequeue(pkt);
				if (!this->send(pkt.get()))
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(5));
					continue;
				}
				while (!_shutdown)
				{
					AVFrame* frame = this->recv();
					if (!frame) break;
					/** if (auto diff = frame->pts - master_timestamp; diff > 0)
					{
						deleter(frame);
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
						continue;
					} **/
					DataBuf::AFrameQueue.enqueue(FramePtr(frame, deleter));
				}
			}
			else if (_type == Type::VIDEO_DECODE)
			{
				DataBuf::VPacketQueue.dequeue(pkt);
				if (!this->send(pkt.get()))
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(5));
					continue;
				}
				while (!_shutdown)
				{
					AVFrame* frame = this->recv();
					if (!frame) break;
					/** master_timestamp = frame->pts; **/
					DataBuf::VFrameQueue.enqueue(FramePtr(frame, deleter));
					//std::this_thread::sleep_for(std::chrono::milliseconds(1000/30));
				}
			}
		}
		});
}

void Decoder::finish()
{
	if (!_shutdown)
	{
		_shutdown = true;
		if (_work_thread.joinable())
			_work_thread.join();
	}
}

bool Decoder::open(AVCodecParameters* para, AVRational* timebase)
{
	if (!para) return false;
	this->close();
	std::lock_guard<std::mutex> lck(_mutex);
	//找到解码器
	AVCodec* codec = avcodec_find_decoder(para->codec_id);
	if (!codec)
	{
		//avcodec_parameters_free(&para);
		return false;
	}
	//创建解码器上下文
	_codec_ctx = avcodec_alloc_context3(codec);
	//配置解码器上下文参数
	avcodec_parameters_to_context(_codec_ctx, para);
	//avcodec_parameters_free(&para);
	//配置解码线程数
	_codec_ctx->thread_count = (_type == Type::VIDEO_DECODE) ? 1 : std::thread::hardware_concurrency();
	//配置解码器时间积
	if (timebase) _codec_ctx->pkt_timebase = *timebase;

	//硬件解码器的初始化
	init(codec);

	//打开解码器
	int ret = avcodec_open2(_codec_ctx, nullptr, nullptr);
	if (ret != 0)
	{
		avcodec_free_context(&_codec_ctx);
		return false;
	}
	return true;
}

void Decoder::close()
{
	std::lock_guard<std::mutex> lck(_mutex);
	if (!_codec_ctx) return;
	avcodec_close(_codec_ctx);
	avcodec_free_context(&_codec_ctx);
}

bool Decoder::send(AVPacket* pkt)
{
	if (!pkt || pkt->size <= 0 || !pkt->data)
		return false;
	std::lock_guard<std::mutex> lck(_mutex);
	if (!_codec_ctx) return false;
	int ret = avcodec_send_packet(_codec_ctx, pkt);
	return ret == 0 ? true : false;
}

AVFrame* Decoder::recv()
{
	std::lock_guard<std::mutex> lck(_mutex);
	if (!_codec_ctx) return false;
	AVFrame* frame = av_frame_alloc();
	if (avcodec_receive_frame(_codec_ctx, frame) != 0)
	{
		av_frame_free(&frame);
		return nullptr;
	}

	/************ 硬件解码 ************/
	if (frame->format == hw_pix_fmt) //AV_PIX_FMT_CUDA
	{
		AVFrame* sw_frame = av_frame_alloc();
		if (av_hwframe_transfer_data(sw_frame, frame, 0) < 0)
		{
			av_frame_free(&frame);
			av_frame_free(&sw_frame);
			return nullptr;
		}
		else
		{
			av_frame_free(&frame);
			//av_frame_unref(frame); the same to av_frame_free
			if (_type == Type::VIDEO_DECODE) printf("hardware decode\n");
			printf("%d\n", hw_pix_fmt);
			return sw_frame;
		}
	}
	if(_type == Type::VIDEO_DECODE) printf("software decode\n");
	/************ 硬件解码 ************/

	return frame;
}


#endif