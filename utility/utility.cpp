#include "utility.h"

namespace dip
{
	cv::Mat drawHistogram(double* pdf, int range)
	{
		cv::Mat histogram = cv::Mat(range, range, CV_8UC3 , cv::Scalar(255 , 255 , 255));

		auto max = .0;

		for (auto i = 0; i < range; ++i)
		{
			if (pdf[i] > max)
				max = pdf[i];
		}

		for (auto i = 0; i < range; ++i)
		{
			double height = std::round((pdf[i] / max) * range);

			cv::line(histogram, cv::Point(i, range - 1), cv::Point(i, range - 1 - height), cv::Scalar(0 , 255 , 0));
		}

		return histogram;
	}
}
