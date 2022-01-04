#include "Demuxer.h"
#include "DataBuf.h"
extern "C" 
{
	#include <libavformat/avformat.h>
}
#pragma comment(lib, "avformat.lib")
#include <QDebug>
#pragma execution_character_set("utf-8")

Demuxer::Demuxer()
{
	//av_dict_set(&_opts, "rtsp_transport", "tcp", 0);
	//av_dict_set(&_opts, "max_delay", "500", 0);
	//av_dict_set(&_opts, "list_devices", "true", 0);
	//av_dict_set(&_opts, "timeout", "3", 0); //这个是用来设定打开流的超时时间，有些协议有用，有些协议没用
	run();
}

Demuxer::~Demuxer()
{
	close();
	finish();
}

void Demuxer::run()
{
	this->finish();
	_work_thread = std::thread([this] {
		auto deleter = [](AVPacket* pkt) { if(pkt) av_packet_free(&pkt); };
		_shutdown = false;
		while (!_shutdown)
		{
			//关于播放暂停的实现，因为有缓冲的原因，所以不应在demux模块中进行
			/*std::unique_lock<std::mutex> lck(_resume_mutx);
			_resume_cond.wait(lck, [this] { return _shutdown || _resume; });*/
			AVPacket* pkt = this->read();
			if (!pkt)
			{
				//close avformatcontext()???
				std::this_thread::sleep_for(std::chrono::milliseconds(5));
				continue;
			}
			if (pkt->stream_index == _vindex)
			{
				DataBuf::VPacketQueue.enqueue(PacketPtr(pkt, deleter));
				//printf("v\n");
			}
			else if (pkt->stream_index == _aindex)
			{
				DataBuf::APacketQueue.enqueue(PacketPtr(pkt, deleter));
				//printf("     a\n");
			}
			//qDebug() << "V: " << DataBuf::VPacketQueue.size();
			//qDebug() << "            A: " << DataBuf::APacketQueue.size();
		}
	});
}

void Demuxer::finish()
{
	if (!_shutdown)
	{
		_shutdown = true;
		if (_work_thread.joinable())
			_work_thread.join();
	}
}

bool Demuxer::open(const char* url, bool isDevice, int timeout)
{
	if (!url) return false;
	this->close();
	std::lock_guard<std::mutex> lck(_mutex);
	qDebug() << "开始打开视频流" << url;
	//setOpenTimeout(timeout); //设置了这个之后打开《春晚.mov》出现stream1, offset0x28: partial file
	int ret = avformat_open_input(&_fmt_ctx, url, isDevice ? av_find_input_format("dshow") : nullptr, &_opts);
	if (!_fmt_ctx) return false; //打开香港卫视rtmp后出现空指针
	qDebug() << "结束打开视频流" << _fmt_ctx->start_time;
	if (ret != 0) goto failed;/*{
		//qDebug() << "open input" << url << "failed";
		char errinfo[AV_ERROR_MAX_STRING_SIZE] = { 0 };
		qDebug() << av_make_error_string(errinfo, AV_ERROR_MAX_STRING_SIZE, ret);
		return false;
	}*/
	ret = avformat_find_stream_info(_fmt_ctx, nullptr);
	if (ret != 0) goto failed;/*{
		//qDebug() << "find stream info" << url << "failed";
		char errinfo[AV_ERROR_MAX_STRING_SIZE] = { 0 };
		qDebug() << av_make_error_string(errinfo, AV_ERROR_MAX_STRING_SIZE, ret);
		return false;
	}*/
	duration = _fmt_ctx->duration / AV_TIME_BASE / 0.001;
	qDebug() << "视频总时长为" << duration << "毫秒";
	av_dump_format(_fmt_ctx, 0, url, 0);
	_vindex = av_find_best_stream(_fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
	_aindex = av_find_best_stream(_fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
	//av_seek_frame(_fmt_ctx, _aindex, -1, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME); //打开瞬间seek到0
	return true;

failed:
	char errinfo[AV_ERROR_MAX_STRING_SIZE] = { 0 };
	qDebug() << av_make_error_string(errinfo, AV_ERROR_MAX_STRING_SIZE, ret);
	return false;
}

bool Demuxer::seek(double pos)
{
	std::lock_guard<std::mutex> lck(_mutex);
	if (!_fmt_ctx) return false;
	avformat_flush(_fmt_ctx);
	if (_aindex >= 0)
	{
		int64_t seek_pos = _fmt_ctx->streams[_aindex]->duration * pos;
		int ret = av_seek_frame(_fmt_ctx, _aindex, seek_pos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
		return ret < 0 ? false : true;
	}
	if (_vindex >= 0)
	{
		int64_t seek_pos = _fmt_ctx->streams[_vindex]->duration * pos;
		if (0 > av_seek_frame(_fmt_ctx, _vindex, seek_pos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME))
			return false;
		return true;
	}
	return false;
}

void Demuxer::close()
{
	std::lock_guard<std::mutex> lck(_mutex);
	if (!_fmt_ctx) return;
	avformat_flush(_fmt_ctx); //有了
	avformat_close_input(&_fmt_ctx);//???有个清理来着
	duration = 0;
	/* pause(false); */
}

AVPacket* Demuxer::read()
{
	std::lock_guard<std::mutex> lck(_mutex);
	if (!_fmt_ctx) return nullptr;
	AVPacket* pkt = av_packet_alloc();
	int ret = av_read_frame(_fmt_ctx, pkt);////////////////error handle
	if (ret != 0)
	{
		av_packet_free(&pkt);
		handleReadError(ret);
		return nullptr;
	}
	//pts、dts转换为毫秒
	//pkt->pts = pkt->pts * av_q2d(_fmt_ctx->streams[pkt->stream_index]->time_base) * 1;
	//printf("P---->%ld\n", pkt->pts);
	//pkt->dts = pkt->dts * av_q2d(_fmt_ctx->streams[pkt->stream_index]->time_base) * 1;
	//current = pkt->pts * av_q2d(_fmt_ctx->streams[pkt->stream_index]->time_base) * 1000;
	//pts的转换操作现改为在Decoder模块中的recv方法中进行
	return pkt;
}

std::tuple<AVCodecParameters*, AVRational*> Demuxer::copyVPara()
{
	std::lock_guard<std::mutex> lck(_mutex);
	if (!_fmt_ctx || _vindex < 0) //无视频流
		return std::make_tuple(nullptr, nullptr);
	AVCodecParameters* para = avcodec_parameters_alloc();
	qDebug() << "video index:" << _vindex;
	avcodec_parameters_copy(para, _fmt_ctx->streams[_vindex]->codecpar);
	return std::make_tuple(para, &_fmt_ctx->streams[_vindex]->time_base);
}

std::tuple<AVCodecParameters*, AVRational*> Demuxer::copyAPara()
{
	std::lock_guard<std::mutex> lck(_mutex);
	if (!_fmt_ctx || _aindex < 0)  //无音频流
		return std::make_tuple(nullptr, nullptr);
	AVCodecParameters* para = avcodec_parameters_alloc();
	qDebug() << "audio index:" << _aindex;
	avcodec_parameters_copy(para, _fmt_ctx->streams[_aindex]->codecpar);
	return std::make_tuple(para, &_fmt_ctx->streams[_aindex]->time_base);
}

void Demuxer::registerErrorHandler(int errnum, const std::function<void(void)>& callback)
{
	if (_err_handler.find(errnum) != _err_handler.end()) {
		_err_handler[errnum] = callback;
	} else {
		_err_handler.insert(std::make_pair(errnum, callback));
	}
}

void Demuxer::pause(bool flag)
{
	if (!flag)
	{
		_resume = true;
		_resume_cond.notify_all();
	}
	else
	{
		_resume = false;
	}
}

void Demuxer::setOpenTimeout(int seconds)
{
	if (!_fmt_ctx)
	{
		_fmt_ctx = avformat_alloc_context();
	}
	static int timeout = 0;
	timeout = seconds;
	auto callback = [](void* p)->int {
		::time_t lasttime = *(::time_t*)p;
		if (lasttime > 0)
			if (::time(NULL) - lasttime > timeout)
				return 1;
		return 0;
	};
	_fmt_ctx->interrupt_callback.callback = callback;
	_fmt_ctx->interrupt_callback.opaque = &_lasttime;
	_lasttime = ::time(NULL);
}

void Demuxer::handleReadError(int errnum)
{
	char errinfo[AV_ERROR_MAX_STRING_SIZE] = { 0 };
	qDebug() << av_make_error_string(errinfo, AV_ERROR_MAX_STRING_SIZE, errnum);
	if (_err_handler.find(errnum) != _err_handler.end())
	{
		std::invoke(_err_handler.at(errnum));
	}
	avformat_close_input(&_fmt_ctx); ////////////////////一定要关掉么，不然会一直返回end of file
}
