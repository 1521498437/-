#pragma once
#include <map>
#include <string>
#include <memory>
namespace cv { class Mat; }

enum class FilterType
{
	kSMOOTH,
	kWHITE,
	kTHIN,
	kBRIGHTNESS,
	kCONTRAST,
	KSATURATION
};

class MatFilter
{
public:
	using Ptr = std::shared_ptr<MatFilter>;
	static MatFilter::Ptr Create(FilterType);
	virtual ~MatFilter() = default;

public:
	void setPara(double para) { _para = para; }
	virtual void filter(cv::Mat& src, cv::Mat& dst) = 0;

protected:
	std::string _self_name;
	double _para{ 0. };
};

