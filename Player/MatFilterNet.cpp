#include "MatFilterNet.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#pragma comment(lib, "opencv_world3413.lib")

MatFilterNet* MatFilterNet::Get(uint8_t index)
{
	static MatFilterNet instances[1];
	return &instances[index];
}

void MatFilterNet::modFilter(FilterType type, double para)
{
	if (_filters.find(type) == _filters.end())
		_filters.emplace(type, MatFilter::Create(type));
	_filters.at(type)->setPara(para);
}

void MatFilterNet::delFilter(FilterType type)
{
	_filters.erase(type);
}

void MatFilterNet::filter(cv::Mat& src)
{
	for (auto& [k, v] : _filters) {
		cv::Mat dst;
		v->filter(src, dst);
		src = dst;
	}
}
