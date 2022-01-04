#include "MatFilter.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#pragma comment(lib, "opencv_world3413.lib")
#pragma comment(lib, "opencv_world3413d.lib")
using namespace cv;

struct SmoothFilter : public MatFilter {
	SmoothFilter() { _self_name = "SmoothFilter"; }
	void filter(cv::Mat& src, cv::Mat& dst) override {
		cv::bilateralFilter(src, dst, _para, _para * 2, _para / 2);
	}
};

struct WhiteFilter : public MatFilter {
	WhiteFilter() { _self_name = "WhiteFilter"; }
	void filter(cv::Mat& src, cv::Mat& dst) override {
		dst = src.clone();
		for (int y = 0; y < dst.rows; y++)
		{
			for (int x = 0; x < dst.cols; x++)
			{
				for (int c = 0; c < 3; c++)
				{
					dst.at<Vec3b>(y, x)[c] = saturate_cast<uchar>((1 + _para / 40.) * (dst.at<Vec3b>(y, x)[c]) + 0);
				}
			}
		}
	}
};

struct ThinFilter : public MatFilter {
	ThinFilter() { _self_name = "ThinFilter"; }
	void filter(cv::Mat& src, cv::Mat& dst) override {
	}
};

struct BrightnessFilter : public MatFilter {
	BrightnessFilter() { _self_name = "BrightnessFilter"; }
	void filter(cv::Mat& src, cv::Mat& dst) override {
		dst = src.clone();
		for (int y = 0; y < dst.rows; y++)
		{
			for (int x = 0; x < dst.cols; x++)
			{
				for (int c = 0; c < 3; c++)
				{
					dst.at<Vec3b>(y, x)[c] = saturate_cast<uchar>((dst.at<Vec3b>(y, x)[c]) + _para * 3.5);
				}
			}
		}
	}
};

struct ContrastFilter : public MatFilter {
	ContrastFilter() { _self_name = "ContrastFilter"; }
	void filter(cv::Mat& src, cv::Mat& dst) override {
		dst = src.clone();
		for (int y = 0; y < dst.rows; y++)
		{
			for (int x = 0; x < dst.cols; x++)
			{
				for (int c = 0; c < 3; c++)
				{
					dst.at<Vec3b>(y, x)[c] = saturate_cast<uchar>((1 + _para / 20.) * (dst.at<Vec3b>(y, x)[c]) + 0);
				}
			}
		}
	}
};

struct SaturationFilter : public MatFilter {
	SaturationFilter() { _self_name = "SaturationFilter"; }
	void filter(cv::Mat& src, cv::Mat& dst) override {
		/*dst = src.clone();
		// define the iterator of the input image
		MatIterator_<Vec3f> inp_begin, inp_end;
		inp_begin = src.begin<Vec3f>();
		inp_end = src.end<Vec3f>();

		// define the iterator of the output image
		MatIterator_<Vec3f> out_begin, out_end;
		out_begin = dst.begin<Vec3f>();
		out_end = dst.end<Vec3f>();

		// increment (-100.0, 100.0)
		float Increment = /*-50.0 / 100.0;*/(_para - 10.) / 5.;

		/*float delta = 0;
		float minVal, maxVal;
		float t1, t2, t3;
		float L, S;
		float alpha;

		for (; inp_begin != inp_end; inp_begin++, out_begin++)
		{
			t1 = (*inp_begin)[0];
			t2 = (*inp_begin)[1];
			t3 = (*inp_begin)[2];

			minVal = std::min(std::min(t1, t2), t3);
			maxVal = std::max(std::max(t1, t2), t3);
			delta = (maxVal - minVal) / 255.0;
			L = 0.5 * (maxVal + minVal) / 255.0;
			S = std::max(0.5 * delta / L, 0.5 * delta / (1 - L));

			if (Increment > 0)
			{
				alpha = max(S, 1 - Increment);
				alpha = 1.0 / alpha - 1;
				(*out_begin)[0] = (*inp_begin)[0] + ((*inp_begin)[0] - L * 255.0) * alpha;
				(*out_begin)[1] = (*inp_begin)[1] + ((*inp_begin)[1] - L * 255.0) * alpha;
				(*out_begin)[2] = (*inp_begin)[2] + ((*inp_begin)[2] - L * 255.0) * alpha;
			}
			else
			{
				alpha = Increment;
				(*out_begin)[0] = L * 255.0 + ((*inp_begin)[0] - L * 255.0) * (1 + alpha);
				(*out_begin)[1] = L * 255.0 + ((*inp_begin)[1] - L * 255.0) * (1 + alpha);
				(*out_begin)[2] = L * 255.0 + ((*inp_begin)[2] - L * 255.0) * (1 + alpha);

			}
		}*/
	}
};

MatFilter::Ptr MatFilter::Create(FilterType type)
{
	switch (type)
	{
	case FilterType::kSMOOTH:
		return std::make_shared<SmoothFilter>();
	case FilterType::kWHITE:
		return std::make_shared<WhiteFilter>();
	case FilterType::kTHIN:
		return std::make_shared<ThinFilter>();
	case FilterType::kBRIGHTNESS:
		return std::make_shared<BrightnessFilter>();
	case FilterType::kCONTRAST:
		return std::make_shared<ContrastFilter>();
	case FilterType::KSATURATION:
		return std::make_shared<SaturationFilter>();
	default:
		return nullptr;
	}
}
