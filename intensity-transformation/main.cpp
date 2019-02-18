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
 * @brief The program does intensity transformations such as log transformation, power-law(gamma) transformation
 * Intensity transformations which are mentioned at Chapter 3.2 - Digital Image Processing (3rd Edition): Rafael C. Gonzalez
 *
 * @author Ozan Cansel
 * Contact: ozancansel@gmail.com
 * 
 */

#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>

using namespace cv;

//Does power(also known as gamma) transformation, returns the output,  chapter 3.2.3
Mat powerTransformation(Mat input, double gamma);
//Does log transformation, returns the output, chapter 3.2.2
Mat logTransformation(Mat input , double gamma);
//Contrast strecthing algorithm 3.2.4
//It is benefitted from http://what-when-how.com/embedded-image-processing-on-the-tms320c6000-dsp/contrast-stretching-image-processing/
Mat contrastStretching(Mat input);
//Intensity Slicing, Figure 3.11(a) implementation
Mat intensitySlicing(Mat input , int from , int to);

int main(int argc, char** argv) {
        const String keys = 
	"{help h usage ?    || The program does intensity transformations such as log and power transformations.}"
    "{input             | intensity-transformation-dark.jpg | power and log transforming image}"
	"{gamma             | 1.15								| gamma value for power transformation}"
	"{input2            | contrast-stretching.jpg           | power and log transforming image}"
	"{slicingFrom       | 50								| A value of interval [A , B]}"
	"{slicingTo         | 81								| B value of interval [A , B]}"
    ;

    CommandLineParser cmdParser(argc , argv, keys);

    if (cmdParser.has("help"))
    {
        cmdParser.printMessage();
        return 0;
    }

	Mat input , input2;

    input = imread(cmdParser.get<cv::String>("input").c_str());
	input2 = imread(cmdParser.get<cv::String>("input2").c_str());
    auto gamma = cmdParser.get<double>("gamma");
	auto slicingFrom = cmdParser.get<int>("slicingFrom");
	auto slicingTo = cmdParser.get<int>("slicingTo");

    if ( !input.data )
    {
        printf("No input data \n");
        return -1;
    }

    cvtColor(input , input , COLOR_BGR2GRAY);
	cvtColor(input2, input2, COLOR_BGR2GRAY);
    auto powerTransformedOutput = powerTransformation(input , gamma);
    auto logTransformedOutput = logTransformation(input , gamma);
	auto contrastStrecthedOutput = contrastStretching(input2);
	auto intensitySlicedOutput = intensitySlicing(input2, slicingFrom, slicingTo);

    imshow("input1" , input);
    imshow((std::string("Power Transformation - Gamma ") + std::to_string(gamma)).c_str() , powerTransformedOutput);
    imshow("Log Transformation" , logTransformedOutput);
	imshow("input2", input2);
	imshow("Contrast Streching", contrastStrecthedOutput);
	imshow("Intensity Slicing", intensitySlicedOutput);

    waitKey(0);

    return 0;
}


Mat powerTransformation(Mat input, double gamma)
{
    Mat result = Mat::zeros(input.rows , input.cols, CV_8U);

    Mat normalized;
    Mat transformed = Mat::zeros(input.rows , input.cols, CV_64F);

    input.convertTo(normalized , CV_64F);
    
    const float c = 1.0;

    for(auto y = 0; y < normalized.rows; ++y)
    {
        for(auto x = 0; x < normalized.cols; ++x)
        {
            transformed.at<double>(y , x) = c *  (pow(normalized.at<double>(y , x) , gamma));
        }
    }

    transformed.convertTo(result , CV_8U);

    return result;
}

Mat logTransformation(Mat input, double gamma)
{
    Mat result = Mat::zeros(input.rows, input.cols, CV_8U);

    Mat normalized;
    Mat transformed = Mat::zeros(input.rows, input.cols, CV_64F);

    
    input.convertTo(normalized , CV_64F);

    normalized /= 255;
    
    const float c = 1.0;

    for(auto y = 0; y < normalized.rows; ++y)
    {
        for(auto x = 0; x < normalized.cols; ++x)
        {
            auto val = normalized.at<double>(y , x);
            auto logTransformationPixelResult = std::log(val + 1);
            transformed.at<double>(y , x) = c *  (std::log(normalized.at<double>(y , x) + 1.0));
        }
    }

    transformed *= 255;
    transformed.convertTo(result , CV_8U);

    return result;
}

Mat contrastStretching(Mat input)
{
	Mat result = Mat::zeros(input.rows, input.cols, CV_8U);

	auto rMin = 255;
	auto rMax = 0;
	
	//Find min and max values in the image
	for (auto y = 0; y < input.rows; ++y)
	{
		for (auto x = 0; x < input.cols; ++x)
		{
			auto current = input.at<uchar>(y, x);

			if (current < rMin)
				rMin = current;

			if (current > rMax)
				rMax = current;
		}
	}

	auto R = static_cast<double>(rMax - rMin);

	//Strecth contrast between rMin and rMax to 0 and 255
	for (auto y = 0; y < input.rows; ++y)
	{
		for (auto x = 0; x < input.cols; ++x)
		{
			auto p = input.at<uchar>(y, x);
			result.at<uchar>(y , x) = p = std::round(((p - rMin) / R) * 255.0);
		}
	}

	return result;
}

Mat intensitySlicing(Mat input , int from, int to)
{
	Mat result = Mat::zeros(input.rows, input.cols, CV_8U);

	auto rMin = 255;
	auto rMax = 0;

	//Find min and max values in the image
	for (auto y = 0; y < input.rows; ++y)
	{
		for (auto x = 0; x < input.cols; ++x)
		{
			auto current = input.at<uchar>(y, x);

			if (current < rMin)
				rMin = current;

			if (current > rMax)
				rMax = current;
		}
	}

	auto R = static_cast<double>(rMax - rMin);

	//Strecth contrast between rMin and rMax to 0 and 255
	for (auto y = 0; y < input.rows; ++y)
	{
		for (auto x = 0; x < input.cols; ++x)
		{
			auto p = input.at<uchar>(y, x);
			if (p >= from && p <= to)
				result.at<uchar>(y, x) = p = std::round(((p - rMin) / R) * 255.0);
			else
				result.at<uchar>(y, x) = rMin;
		}
	}

	return result;
}