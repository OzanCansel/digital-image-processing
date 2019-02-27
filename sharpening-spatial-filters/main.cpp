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
 * @brief Program does sharpening spatial filters which are Laplacian and the gradient filter(also know as Sobel)
 * Sharpening spatial filters which are mentioned at Chapter 3.6 - Digital Image Processing (3rd Edition): Rafael C. Gonzalez
 *
 * @author Ozan Cansel
 * Contact: ozancansel@gmail.com
 * 
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "utility.h"

using namespace cv;

/*
* 3.37a
*/
Mat iterateMask(Mat f, Mat w);
/*
* 3.6-16, 3.6-17 => 3.6-18 Equation
*/
Mat iterateMaskForGradient(Mat f);
double applyFilter(Mat f, Mat w);

int main(int argc, char** argv) 
{
        const String keys = 
	"{help h usage ?    || Program does sharpening spatial filters by using Laplacian Derivation and applies the filter as stated in the figure 3.37b.}"
	"{input             | sharpening-spatial-filters.jpg | input image}"
    ;

    CommandLineParser cmdParser(argc , argv, keys);

    if (cmdParser.has("help"))
    {
        cmdParser.printMessage();
        return 0;
    }

	Mat input = imread(cmdParser.get<cv::String>("input").c_str());

	if (!input.data)
	{
		printf("No input data \n");
		return -1;
	}

	cvtColor(input, input, COLOR_BGR2GRAY);
	imshow("input", input);
	

	Mat laplacianInput;
	input.copyTo(laplacianInput);
	laplacianInput.convertTo(laplacianInput, CV_64FC1);

	//Apply laplacian filter
	//Figure: 3.37b
	Mat laplacianOrthogonalMask = (Mat_<int>(3, 3) <<
		-1, -1, -1,
		-1,  8, -1,
		-1, -1, -1);

	auto laplacianOrthogonalApplied = iterateMask(laplacianInput, laplacianOrthogonalMask);

	auto orthogonalMin = .0;
	auto orthogonalMax = .0;

	minMaxLoc(laplacianOrthogonalApplied, &orthogonalMin, &orthogonalMax);
	
	laplacianOrthogonalApplied = (laplacianOrthogonalApplied - orthogonalMin);
	minMaxLoc(laplacianOrthogonalApplied, &orthogonalMin, &orthogonalMax);
	laplacianOrthogonalApplied = laplacianOrthogonalApplied * (255.0 / orthogonalMax);

	Mat sharpenedOrthogonal = laplacianInput + laplacianOrthogonalApplied;

	minMaxLoc(sharpenedOrthogonal, &orthogonalMin, &orthogonalMax);

	sharpenedOrthogonal = (sharpenedOrthogonal - orthogonalMin);
	minMaxLoc(sharpenedOrthogonal, &orthogonalMin, &orthogonalMax);
	sharpenedOrthogonal = sharpenedOrthogonal * (255.0 / orthogonalMax);

	sharpenedOrthogonal.convertTo(sharpenedOrthogonal, CV_8U);
	laplacianOrthogonalApplied.convertTo(laplacianOrthogonalApplied, CV_8U);

	//Apply gradient filter
	auto gradientApplied = iterateMaskForGradient(laplacianInput);

	gradientApplied.convertTo(gradientApplied, CV_8U);
	imshow("Laplacian Orthogonal Mask Applied Scaled", laplacianOrthogonalApplied);
	imshow("Sharpened By Orthogonal", sharpenedOrthogonal);
	imshow("Gradient applied", gradientApplied);

    waitKey(0);

    return 0;
}

Mat iterateMask(Mat f, Mat w)
{

	auto m = w.cols;
	auto n = w.rows;
	auto a = (m - 1) / 2;
	auto b = (n - 1) / 2;
	auto padSize = w.rows - 1;
	Mat fPadded = Mat::zeros(f.rows + 2 * padSize, f.cols + 2 * padSize, CV_64F);
	Mat g = Mat::zeros(f.rows + 2 * padSize, f.cols + 2 * padSize, CV_64F);

	//Pad is adding
	auto roi = fPadded(Rect(w.cols - 1, w.rows - 1, f.cols, f.rows));
	f.copyTo(roi);

	//Start with some margin, because we padded the Mat
	//Iterate all the points
	for (auto y = padSize - 1; y < fPadded.rows - padSize + 1; ++y)
	{
		for (auto x = padSize - 1; x < fPadded.cols - padSize + 1; ++x)
		{
			//Get the Mat which consist of neighbours of the point
			Mat neighbourhood = fPadded(Rect(x - a, y - b, w.cols, w.cols));

			//Apply the filter
			g.at<double>(y, x) = applyFilter(neighbourhood , w);
		}
	}

	//Return unpadded Mat
	return g(Rect(padSize, padSize, f.cols, f.rows));
}

Mat iterateMaskForGradient(Mat f)
{
	//Equation: 3.6-16
	Mat gx = (Mat_<int>(3, 3) <<
		-1, -2, -1,
		0, 0, 0,
		1, 2, 1);

	//Equation: 3.6-17
	Mat gy = (Mat_<int>(3, 3) <<
		-1, 0, 1,
		-2, 0, 2,
		-1, 0, 1);


	auto m = gy.cols;
	auto n = gy.rows;
	auto a = (m - 1) / 2;
	auto b = (n - 1) / 2;
	auto padSize = gy.rows - 1;
	Mat fPadded = Mat::zeros(f.rows + 2 * padSize, f.cols + 2 * padSize, CV_64F);
	Mat M = Mat::zeros(f.rows + 2 * padSize, f.cols + 2 * padSize, CV_64F);

	//Pad is adding
	auto roi = fPadded(Rect(gx.cols - 1, gx.rows - 1, f.cols, f.rows));
	f.copyTo(roi);

	//Start with some margin, because we padded the Mat
	//Iterate all the points
	for (auto y = padSize - 1; y < fPadded.rows - padSize + 1; ++y)
	{
		for (auto x = padSize - 1; x < fPadded.cols - padSize + 1; ++x)
		{
			//Get the Mat which consist of neighbours of the point
			Mat neighbourhood = fPadded(Rect(x - a, y - b, gx.cols, gx.cols));

			//Find gx, gy

			//Calculate  Eq 3.6-16
			auto gxValue = applyFilter(neighbourhood, gx);
			//Calculate Eq 3.6-17
			auto gyValue = applyFilter(neighbourhood, gy);

			//Put the outputs into the Eq 3.6-18
			M.at<double>(y, x) = dip::stayInBoundaries(std::abs(gxValue) + std::abs(gyValue) , dip::Upper(255) , dip::Lower(0));
		}
	}

	//Return unpadded Mat
	return M(Rect(padSize, padSize, f.cols, f.rows));
}

double applyFilter(Mat f, Mat w)
{
	auto result = .0;

	cv::rotate(w, w, RotateFlags::ROTATE_180);

	//w(x,y) * f(x,y) = w(s,t) * f(x + s, y + t)  
	for (auto y = 0; y < f.rows; ++y)
	{
		for (auto x = 0; x < f.cols; ++x)
		{
			auto wi = w.at<int>(y, x);
			auto fi = f.at<double>(y, x);
			result += wi * fi;
		}
	}

	return result;
}