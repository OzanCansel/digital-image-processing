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
 * @brief Program does spatial correlation and convolution
 * Spatial correlation and convolution which are mentioned at Chapter 3.4.2 - Digital Image Processing (3rd Edition): Rafael C. Gonzalez
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

//Equation : 3.3-24
Mat correlate(Mat f , Mat w);
Mat convolute(Mat f, Mat w);

//Equation : 3.4-1
uchar applyFilter(Mat f, Mat w);
Mat rotate(Mat input, double angle);

//Utility
void printMat(Mat input);

int main(int argc, char** argv) 
{
        const String keys = 
	"{help h usage ?    || The program does spatial correlation and convolution, the values are hardcoded into the program as in the figure 3.30.}"
    ;

    CommandLineParser cmdParser(argc , argv, keys);

    if (cmdParser.has("help"))
    {
        cmdParser.printMessage();
        return 0;
    }

	Mat f = (Mat_<uchar>(5, 5) <<	0, 0, 0, 0, 0,
									0, 0, 0, 0, 0,
									0, 0, 1, 0, 0,
									0, 0, 0, 0, 0,
									0, 0, 0, 0, 0);


	Mat w = (Mat_<uchar>(3, 3) <<	1 , 2 , 3 ,
									4 , 5 , 6 ,
									7 , 8 , 9);

	auto correlated = correlate(f, w);
	auto convoluted = convolute(f, w);

	std::cout << "f(x,y) => " << std::endl;
	printMat(f);
	std::cout << std::endl << "w(x,y) => " << std::endl;
	printMat(w);

	std::cout << std::endl << "Correlated" << std::endl;
	printMat(correlated);
	std::cout << std::endl << "Convoluted" << std::endl;
	printMat(convoluted);

    waitKey(0);

    return 0;
}

Mat correlate(Mat f, Mat w)
{
	Mat output;
	auto m = w.cols;
	auto n = w.rows;
	auto a = (m - 1) / 2;
	auto b = (n - 1) / 2;
	auto padSize = w.rows - 1;
	Mat padded = Mat::zeros(f.rows + 2 * padSize, f.cols + 2 * padSize, CV_8U);
	Mat result = Mat::zeros(f.rows + 2 * padSize, f.cols + 2 * padSize, CV_8U);

	//Pad is adding
	auto roi = padded(Rect(w.cols - 1, w.rows - 1, f.rows, f.cols));
	f.copyTo(roi);

	//Start with some margin, because we padded the Mat
	//Iterate all the points
	for (auto y = padSize - 1; y < padded.rows - padSize + 1; ++y)
	{
		for (auto x = padSize - 1; x < padded.cols - padSize + 1; ++x)
		{
			//Get the Mat which consist of neighbours of the point
			Mat neighbourhood = padded(Rect(x - a, y - b, w.cols, w.cols));

			//Apply the filter
			result.at<uchar>(y, x) = applyFilter(neighbourhood, w);
		}
	}

	//Return unpadded Mat
	return result(Rect(padSize, padSize, f.cols, f.rows));
}

Mat convolute(Mat f, Mat w)
{
	Mat convolutedW;
	cv::rotate(w, convolutedW , RotateFlags::ROTATE_180);

	return correlate(f , convolutedW);
}

uchar applyFilter(Mat f, Mat w)
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

	return dip::stayInBoundaries(result , dip::Upper(255) , dip::Lower(0));
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