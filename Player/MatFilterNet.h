#pragma once
#include <map>
#include "MatFilter.h"

class MatFilterNet
{
public:
	static MatFilterNet* Get(uint8_t index = 0);

	void modFilter(FilterType type, double para);

	void delFilter(FilterType type);

	void filter(cv::Mat& src);

private:
	std::map<FilterType, MatFilter::Ptr> _filters;
};

