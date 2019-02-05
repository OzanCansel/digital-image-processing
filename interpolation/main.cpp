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
 * @brief The program does resizing by using nearest neighbour, bilinear interpolation algorithms.
 * These resizing techniques are mentioned in Chapter 2.4.4 - Digital Image Processing (3rd Edition): Rafael C. Gonzalez
 *
 * @author Ozan Cansel
 * Contact: ozancansel@gmail.com
 *
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>

using namespace cv;

/*
    It was benefitted from https://www.giassa.net/?page_id=207 while the algorithm is implementing.
*/
Mat nearestNeightbourInterpolation(Mat input , int targetWidth , int targetHeight);

/*
    It was benefitted from https://stackoverflow.com/a/32128947 while the algorithm is implementing.
*/
Mat bilinearInterpolation(Mat input , int targetWidth , int targetHeight);

Mat bicubicInterpolation(Mat input, int targetWidth, int targetHeight);

int main(int argc , char** argv){

    const String keys = 
    "{help h usage ?    |                   | print this message }"
    "{width             | 640               | width of the target image}"
    "{height            | 480               | height of the target image}"
    "{path              | interpolation.jpg | path to the used image}"
    ;

    CommandLineParser cmdParser(argc , argv, keys);
    Mat input;   

    if (cmdParser.has("help"))
    {
        cmdParser.printMessage();
        return 0;
    }

    if(cmdParser.has("path"))
    {
        input = imread(cmdParser.get<cv::String>("path").c_str());
    }
    else
    {
        input = imread("interpolation.jpg");
    }

	auto targetWidth = 0;
	auto targetHeight = 0;


    if(cmdParser.has("width"))
        targetWidth = cmdParser.get<int>("width");
    if(cmdParser.has("height"))
        targetHeight = cmdParser.get<int>("height");

    if ( !input.data )
    {
        printf("No input data \n");
        return -1;
    }

    //Change color format to grayscale
    cvtColor(input , input, COLOR_BGR2GRAY);

    Mat nearestNeighbourOutput = nearestNeightbourInterpolation(input , targetWidth , targetHeight);
    Mat bilinearInterpolationOutput = bilinearInterpolation(input , targetWidth , targetHeight);
	Mat bicubicInterpolationOutput = bicubicInterpolation(input, targetWidth, targetHeight);

	auto inputInformationText = std::string("Size(w:") + std::to_string(input.cols) + std::string(" h:") + std::to_string(input.rows) + std::string(")");
	auto sizeInformationText = std::string("Actual(w:") + std::to_string(input.cols) + std::string(" h:") + std::to_string(input.rows) + 
		std::string(") Target(w:") + std::to_string(targetWidth) + std::string(" h:") + std::to_string(targetHeight) + std::string(")");

    imshow(std::string("Input Image") = inputInformationText, input);
    imshow(std::string("nearest neighbour interpolation") + sizeInformationText , nearestNeighbourOutput);
    imshow(std::string("bilinear interpolation") + sizeInformationText , bilinearInterpolationOutput);
    waitKey(0);
    return 0;
}

Mat nearestNeightbourInterpolation(Mat input , int targetWidth , int targetHeight)
{
	Mat output = Mat::zeros(targetHeight, targetWidth, CV_8U);

	auto xScale = static_cast<float>(targetWidth) / input.cols;
	auto yScale = static_cast<float>(targetHeight) / input.rows;

	for (auto x = 0; x < targetWidth; ++x)
	{
		for (auto y = 0; y < targetHeight; ++y)
		{
			auto fromX = static_cast<int>(std::round((x + 1) / xScale));
			auto fromY = static_cast<int>(std::round((y + 1) / yScale));

			output.at<uchar>(y, x) = input.at<uchar>(std::max(0, fromY - 1), std::max(0, fromX - 1));
		}
	}

	return output;
}

Mat bilinearInterpolation(Mat input, int targetWidth, int targetHeight)
{
	Mat output = Mat::zeros(targetHeight, targetWidth, CV_8U);

	auto xScale = static_cast<float>(targetWidth) / input.cols;
	auto yScale = static_cast<float>(targetHeight) / input.rows;

	for (auto x = 0; x < targetWidth; ++x)
	{
		for (auto y = 0; y < targetHeight; ++y)
		{
			auto origX = (x / xScale) + 1;
			auto origY = (y / yScale) + 1;
			auto x1 = std::floor(origX);
			auto x2 = std::ceil(origX);
			auto y1 = std::floor(origY);
			auto y2 = std::ceil(origY);

			auto x1Idx = std::max(0, static_cast<int>(x1) - 2);
			auto x2Idx = std::max(0, static_cast<int>(x2) - 2);
			auto y1Idx = std::max(0, static_cast<int>(y1) - 2);
			auto y2Idx = std::max(0, static_cast<int>(y2) - 2);


			auto r1 = 0;
			auto r2 = 0;

			if (x1 == x2)
			{
				r1 = input.at<uchar>(y1Idx, x1Idx);
				r2 = input.at<uchar>(y2Idx, x1Idx);
			}
			else
			{
				r1 = std::round(input.at<uchar>(y1Idx, x1Idx) + ((origX - x1) / 1.0) * (input.at<uchar>(y1Idx, x2Idx) - input.at<uchar>(y1Idx, x1Idx)));
				r2 = std::round(input.at<uchar>(y2Idx, x1Idx) + ((origX - x1) / 1.0) * (input.at<uchar>(y2Idx, x2Idx) - input.at<uchar>(y2Idx, x1Idx)));
			}

			auto P = 0;
			if (r1 == r2)
				P = r1;
			else
				P = std::max(.0f, std::min(255.0f, std::round((r2 + (origY - y2) / (y2 - y1) * (r1 - r2)))));

			output.at<uchar>(y, x) = P;
		}
	}

	return output;
}

Mat bicubicInterpolation(Mat input, int targetWidth, int targetHeight)
{
	Mat output;

	return output;
}