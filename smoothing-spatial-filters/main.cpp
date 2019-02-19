/**
 *
 * Copyright (C) 2019
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @file main.cpp
 *
 * @brief The program apply smoothing spatial filters to the image. It applies Linear and Order-Statistic(Nonlinear) filters
 * Smoothing spatial filters which are mentioned at Chapter 3.5 - Digital Image Processing (3rd Edition): Rafael C. Gonzalez
 *
 * @author Ozan Cansel
 * Contact: ozancansel@gmail.com
 * 
 */

#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>
#include "utility.h"

using namespace cv;

/*
* Mask in the Figure 3.32a
*/
Mat applyBoxMask(Mat input);

/*
* Mask in the Figure 3.32b
*/
Mat applyWeightedAverageMask(Mat input);

/*
* Median Mask in 3.5.2
*/
Mat applyMedian(Mat f, int size);

/*
* Equation : 3.5-1
*/
int applyMask(Mat input, Mat mask);

//Utility
void printMat(Mat input);

//Iterates all points of f(x,y) and applies the w(x , y)
Mat iterateLinearMask(Mat f, Mat w);

int main(int argc, char** argv) {
        const String keys = 
	"{help h usage ?    || The program apply smoothing spatial filters to the image.}"
    "{input             | smoothing-spatial-filter.jpg | an image which the filters will be applied}"
	"{medianSize        | 9 | size of median mask}"
    ;

    CommandLineParser cmdParser(argc , argv, keys);

    if (cmdParser.has("help"))
    {
        cmdParser.printMessage();
        return 0;
    }

	Mat input , input2;

    input = imread(cmdParser.get<cv::String>("input").c_str());

    if ( !input.data )
    {
        printf("No input data \n");
        return -1;
    }

	auto medianSize = cmdParser.get<int>("medianSize");

    cvtColor(input , input , COLOR_BGR2GRAY);
	
	std::cout << "Box mask is being applied..." << std::endl;
	auto boxMaskApplied = applyBoxMask(input);
	std::cout << "-----------------------------------------" << std::endl;
	std::cout << "Weighted average mask is being applied..." << std::endl;
	auto weightedAverageMaskApplied = applyWeightedAverageMask(input);
	std::cout << "Median mask is being applied..." << std::endl;
	auto medianMaskApplied = applyMedian(input, medianSize);

    imshow("input" , input);
	imshow("Box Mask Applied", boxMaskApplied);
	imshow("Weighted Average Mask Applied", weightedAverageMaskApplied);
	imshow("Median Mask Applied", medianMaskApplied);

    waitKey(0);

    return 0;
}


Mat applyBoxMask(Mat input)
{
	Mat boxMask = (Mat_<uchar>(3, 3)
		<<	1, 1, 1,
			1, 1, 1,
			1, 1, 1);

	std::cout << "Box Mask => " << std::endl;
	printMat(boxMask);

	return iterateLinearMask(input, boxMask);
}

Mat applyWeightedAverageMask(Mat input)
{
	Mat weightedAverageMask = (Mat_<uchar>(3, 3)
		<< 1, 2, 1,
		2, 4, 2,
		1, 2, 1);

	std::cout << "Weighted Average Mask => " << std::endl;
	printMat(weightedAverageMask);

	return iterateLinearMask(input, weightedAverageMask);
}

int applyMask(Mat f, Mat w)
{
	auto result = 0;

	//w(x,y) * f(x,y) = w(s,t) * f(x + s, y + t)  
	for (auto y = 0; y < f.rows; ++y)
	{
		for (auto x = 0; x < f.cols; ++x)
		{
			result += w.at<uchar>(y, x) * f.at<uchar>(y, x);
		}
	}

	return result;
}

Mat applyMedian(Mat f , int size)
{
	auto m = size;
	auto n = size;
	auto a = (m - 1) / 2;
	auto b = (n - 1) / 2;
	auto padSize = size - 1;
	Mat fPadded = Mat::zeros(f.rows + 2 * padSize, f.cols + 2 * padSize, CV_8U);
	Mat g = Mat::zeros(f.rows + 2 * padSize, f.cols + 2 * padSize, CV_8U);

	//Pad is adding
	auto roi = fPadded(Rect(size - 1, size - 1, f.rows, f.cols));
	f.copyTo(roi);

	//Start with some margin, because we padded the Mat
	//Iterate all the points
	for (auto y = padSize - 1; y < fPadded.rows - padSize + 1; ++y)
	{
		for (auto x = padSize - 1; x < fPadded.cols - padSize + 1; ++x)
		{
			//Get the Mat which consist of neighbours of the point
			Mat neighbourhood = fPadded(Rect(x - a, y - b, size, size));

			std::vector<int> values;

			for (auto nX = 0; nX < neighbourhood.cols; ++nX)
			{
				for (auto nY = 0; nY < neighbourhood.rows; ++nY)
				{
					values.push_back(neighbourhood.at<uchar>(nY, nX));
				}
			}

			//Sort values
			std::sort(values.begin(), values.end(), std::less<int>());

			//Find meadian (value at the center of the vector)
			auto median = values.at(values.size() / 2 + 1);

			//Apply the filter
			g.at<uchar>(y, x) = static_cast<uchar>(median);
		}
	}

	//Return unpadded Mat
	return g(Rect(padSize, padSize, f.cols, f.rows));
}

Mat iterateLinearMask(Mat f, Mat w)
{
	auto m = w.cols;
	auto n = w.rows;
	auto a = (m - 1) / 2;
	auto b = (n - 1) / 2;
	auto padSize = w.rows - 1;
	Mat fPadded = Mat::zeros(f.rows + 2 * padSize, f.cols + 2 * padSize, CV_8U);
	Mat g = Mat::zeros(f.rows + 2 * padSize, f.cols + 2 * padSize, CV_8U);

	//Pad is adding
	auto roi = fPadded(Rect(w.cols - 1, w.rows - 1, f.rows, f.cols));
	f.copyTo(roi);

	auto sumOfMultipliers = 0;
	auto coefficient = .0;

	for (auto y = 0; y < w.rows; ++y)
	{
		for (auto x = 0; x < w.cols; ++x)
		{
			sumOfMultipliers += static_cast<int>(w.at<uchar>(y, x));
		}
	}

	coefficient = 1.0 / sumOfMultipliers;

	std::cout << "Coefficient is " << 1 << "/" << sumOfMultipliers << std::endl;

	//Start with some margin, because we padded the Mat
	//Iterate all the points
	for (auto y = padSize - 1; y < fPadded.rows - padSize + 1; ++y)
	{
		for (auto x = padSize - 1; x < fPadded.cols - padSize + 1; ++x)
		{
			//Get the Mat which consist of neighbours of the point
			Mat neighbourhood = fPadded(Rect(x - a, y - b, w.cols, w.cols));

			//Apply the filter
			g.at<uchar>(y, x) = coefficient * applyMask(neighbourhood, w);
		}
	}

	//Return unpadded Mat
	return g(Rect(padSize, padSize, f.cols, f.rows));
}

void printMat(Mat input)
{
	for (auto y = 0; y < input.rows; ++y)
	{
		for (auto x = 0; x < input.cols; ++x)
		{
			std::cout << (int)input.at<uchar>(y, x) << " ";
		}

		std::cout << std::endl;
	}
}