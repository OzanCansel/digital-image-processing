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
 * @brief Does histogram equalization
 * Histogram equalization which are mentioned at Chapter 3.3 - Digital Image Processing (3rd Edition): Rafael C. Gonzalez
 *
 * @author Ozan Cansel
 * Contact: ozancansel@gmail.com
 * 
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define L 256

using namespace cv;

//Fills the array with zeros
void fillZero(double* arr, int size);

/* Chapter 3.3 is implemented. It performs 3.3-8. equation on the image.
* It is also useful to look at 
* https://www.tutorialspoint.com/dip/introduction_to_probability.htm and https://www.tutorialspoint.com/dip/histogram_equalization.htm
* pages for simpler explanation.
*/
Mat equalizeHistogram(Mat input);

int main(int argc, char** argv) {
        const String keys = 
	"{help h usage ?    || The program does histogram equalization on the image.}"
    "{input             | histogram-equalization.png | input image}"
    ;

    CommandLineParser cmdParser(argc , argv, keys);

    if (cmdParser.has("help"))
    {
        cmdParser.printMessage();
        return 0;
    }

	Mat input;

    input = imread(cmdParser.get<cv::String>("input").c_str());

    if ( !input.data )
    {
        printf("No input data \n");
        return -1;
    }

    cvtColor(input , input , COLOR_BGR2GRAY);

	auto equalizedImage = equalizeHistogram(input);

    imshow("input" , input);
	imshow("Histogram equalized output", equalizedImage);

    waitKey(0);

    return 0;
}

void fillZero(double* arr, int size)
{
	for (auto i = 0; i < size; ++i)
	{
		arr[i] = 0;
	}
}

Mat equalizeHistogram(Mat input)
{

	//Firstly calculate histogram
	double histogram[L];

	fillZero(histogram, L);

	for (auto y = 0; y < input.rows; ++y)
	{
		for (auto x = 0; x < input.cols; ++x)
		{
			histogram[input.at<uchar>(y, x)]++;
		}
	}

	//Secondly extract probability density function(PDF) from the histogram
	//pdf array  represents the pr(rj) part of Eq. 3.3-8
	double pdf[L];

	for (auto i = 0; i < L; ++i)
	{
		pdf[i] = histogram[i] / (input.rows * input.cols);
	}


	//Thirdly cumulative probability function(CDF) from PDF
	//                          k
	//Cdf function reprents the E pr(rj) part of Eq. 3.3-8
	//                          j=0
	double cdf[L];

	cdf[0] = pdf[0];

	for (auto i = 1; i < L; ++i)
	{
		cdf[i] = cdf[i - 1] + pdf[i];
	}

	Mat output(input.rows, input.cols, CV_8U);

	//As last, map old image intensity values
	for (auto y = 0; y < input.rows; ++y)
	{
		for (auto x = 0; x < input.cols; ++x)
		{
			auto oldIntensity = input.at<uchar>(y, x);

			//Full implementation of Equation 3.3-8
			output.at<uchar>(y, x) = std::round((L - 1) * cdf[oldIntensity]);
		}
	}

	return output;
}