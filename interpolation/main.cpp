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
#include "utility.h"

using namespace cv;

/*
*    It was benefitted from https://www.giassa.net/?page_id=207 while the algorithm is implementing.
*/
Mat nearestNeightbourInterpolation(Mat input , int targetWidth , int targetHeight);

Mat bilinearInterpolation(Mat input , int targetWidth , int targetHeight);

/*
*	It was benefitted from https://www.paulinternet.nl/?page=bicubic while the algorithm is implementing.
*/
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
	imshow(std::string("bicubic interpolation") + sizeInformationText, bicubicInterpolationOutput);
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

			using namespace dip;

			Point coordinate;
			coordinate.x = static_cast<int>(std::round((x + 1) / xScale)) - 1;
			coordinate.y = static_cast<int>(std::round((y + 1) / yScale)) - 1;


			//Check boundaries
			coordinate.x = stayInBoundaries(coordinate.x, Upper(input.cols - 1) , Lower(0));
			coordinate.y = stayInBoundaries(coordinate.y, Upper(input.rows - 1), Lower(0));

			output.at<uchar>(y, x) = input.at<uchar>(coordinate);
		}
	}

	return output;
}

/*
*	We are fitting a line between two points in linear interpolation
*	So the equation for the line is 
*	f(x) = ax + b
*	Let's try to solve this equation and write it in dependant to points.
*	Our x values will be changed between 0 to 1.0.
*	So let's find f(1) and f(0)
*	f(1) =  a + b
*	f(0) =  b
*	from this two equations, we can state our f(x) by dependent to f(1) and f(0) when 0 <= x <= 1.0
*   b = f(0)
*	a = f(1) - f(0)
*	f(x) = (f(1) - f(0))x + f(0)
*
*/
double linearInterpolation(double p[2], double x)
{
	return (p[1] - p[0]) * x + p[0];
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

			using namespace dip;

			auto origX = (x / xScale);
			auto origY = (y / yScale);

			//Matrix of 4 nearest neighbours pixel values
			cv::Point coordinateMatrixOfNeighbours[2][2];

			cv::Point p00;
			p00.x = std::floor(origX);
			p00.y = std::floor(origY);

			//Assign coordinates
			for (auto yi = 0; yi < 2; ++yi)
			{
				for (auto xi = 0; xi < 2; ++xi)
				{
					Point coordinate;
					coordinate.x = p00.x + xi;
					coordinate.y = p00.y + yi;
					coordinateMatrixOfNeighbours[yi][xi] = coordinate;
				}
			}

			//Check boundaries
			for (auto yi = 0; yi < 2; ++yi)
			{
				for (auto xi = 0; xi < 2; ++xi)
				{
					Point p = coordinateMatrixOfNeighbours[yi][xi];
					p.x = stayInBoundaries(p.x , Upper(input.cols - 1) , Lower(0));
					p.y = stayInBoundaries(p.y , Upper(input.rows - 1) , Lower(0));
					coordinateMatrixOfNeighbours[yi][xi] = p;
				}
			}

			double intensityMatrixOfNeighbours[2][2];

			for (auto yi = 0; yi < 2; ++yi)
			{
				for (auto xi = 0; xi < 2; ++xi)
				{
					intensityMatrixOfNeighbours[yi][xi] = input.at<uchar>(coordinateMatrixOfNeighbours[yi][xi]);
				}
			}

			auto xVal = origX - std::floor(origX);
			auto yVal = origY - std::floor(origY);

			double horizontallyInterpolatedIntensityMatrix[2];

			//Do horizontal interpolation
			for (auto i = 0; i < 2; ++i)
			{
				horizontallyInterpolatedIntensityMatrix[i] = linearInterpolation(intensityMatrixOfNeighbours[i], xVal);
			}

			//Do vertical interpolation
			auto bothHorizontalAndVerticalInterpolatedIntensityValue = std::round(linearInterpolation(horizontallyInterpolatedIntensityMatrix, yVal));
			bothHorizontalAndVerticalInterpolatedIntensityValue = stayInBoundaries(bothHorizontalAndVerticalInterpolatedIntensityValue, Upper(255), Lower(0));

			output.at<uchar>(y, x) = bothHorizontalAndVerticalInterpolatedIntensityValue;
		}
	}

	return output;
}

double cubicInterpolation(double p[4] , double x)
{
	return p[1] + 0.5 * x*(p[2] - p[0] + x * (2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + x * (3.0*(p[1] - p[2]) + p[3] - p[0])));
}

Mat bicubicInterpolation(Mat input, int targetWidth, int targetHeight)
{
	Mat output = Mat::zeros(targetHeight , targetWidth , CV_8U);
	auto widthRatio = static_cast<double>(targetWidth) / input.cols;
	auto heightRatio = static_cast<double>(targetHeight) / input.rows;

	using namespace dip;

	auto upperWidthBoundary = input.cols - 1;
	auto upperHeightBoundary = input.rows - 1;

	for (auto y = 0; y < output.rows; ++y)
	{
		for (auto x = 0; x < output.cols; ++x)
		{

			//Conceptually, we are looking for center point of the above points
			auto centerX = x / widthRatio;
			auto centerY = y / heightRatio;

			//Matrix of 16 nearest neighbours pixel values
			cv::Point coordinateMatrixOfNeighbours[4][4];

			//Calculate coordinate of p00
			cv::Point p00;
			p00.x = std::floor(centerX) - 1;
			p00.y = std::floor(centerY) - 1;

			//Calculate all the pixels p00 to p33
			for (auto yi = 0; yi < 4; ++yi)
			{
				for (auto xi = 0; xi < 4; ++xi)
				{
					Point p;
					p.x = p00.x + xi;
					p.y = p00.y + yi;
					coordinateMatrixOfNeighbours[yi][xi] = p;
				}
			}

			//Make sure that all the pixel coordinates in boundaries of the input image
			for (auto yi = 0; yi < 4; ++yi)
			{
				for (auto xi = 0; xi < 4; ++xi)
				{
					Point p = coordinateMatrixOfNeighbours[yi][xi];

					p.x = stayInBoundaries(p.x, Upper(input.cols - 1), Lower(0));
					p.y = stayInBoundaries(p.y, Upper(input.rows - 1), Lower(0));

					coordinateMatrixOfNeighbours[yi][xi] = p;
				}
			}

			double intensityMatrixOfNeighbours[4][4];

			//Filling intensity matrix according to coordinate matrix
			for (auto yi = 0; yi < 4; ++yi)
			{
				for (auto xi = 0; xi < 4; ++xi)
				{
					auto coordinate = coordinateMatrixOfNeighbours[yi][xi];
					intensityMatrixOfNeighbours[yi][xi] = input.at<uchar>(coordinate);
				}
			}

			double horizontalInterpolationResultMatrix[4];

			auto yVal = centerY - std::floor(centerY);
			auto xVal = centerX - std::floor(centerX);

			//Do interpolation in horizontal
			for (auto i = 0; i < 4; ++i)
			{
				auto cubicInterpolatedIntensity = cubicInterpolation(intensityMatrixOfNeighbours[i] , xVal);

				//Check boundaries 0 to 255
				cubicInterpolatedIntensity = stayInBoundaries(cubicInterpolatedIntensity, Upper(255), Lower(0));
				horizontalInterpolationResultMatrix[i] = cubicInterpolatedIntensity;
			}

			//As far, we obtained four interpolated points
			//So now lets interpolate four points to one point
			auto bothHorizontalAndVerticalInterpolatedIntensity = std::round(cubicInterpolation(horizontalInterpolationResultMatrix, yVal));
			bothHorizontalAndVerticalInterpolatedIntensity = stayInBoundaries(bothHorizontalAndVerticalInterpolatedIntensity, Upper(255), Lower(0));

			output.at<uchar>(y, x) = bothHorizontalAndVerticalInterpolatedIntensity;
		}
	}

	return output;
}