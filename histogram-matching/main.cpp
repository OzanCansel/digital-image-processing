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
 * @brief Does histogram matching
 * Histogram matching which is mentioned at Chapter 3.3.2 - Digital Image Processing (3rd Edition): Rafael C. Gonzalez
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

#define L 256

using namespace cv;

//Fills the array with zeros
void fillZero(double* arr, int size);

/* Chapter 3.3.2 is implemented.
* It is also useful to look at 
* https://stackoverflow.com/a/33047048
* pages for simpler explanation.
*/
Mat histogramMatching(Mat input, Mat templateImg);

double* calculateHistogram(Mat input);

//pdf array  represents the pr(rj) part of Eq. 3.3-8
double* calculatePdf(double* histogram, int totalPixelSize);

//                          k
//Cdf function reprents the E pr(rj) part of Eq. 3.3-8
//                          j=0
double* calculateCdf(double* pdf);

int main(int argc, char** argv) {
        const String keys = 
	"{help h usage ?    || The program does histogram matching.}"
    "{input             | histogram-matching-input.jpg | input image}"
	"{template          | histogram-matching-template.jpg | template image}"
    ;

    CommandLineParser cmdParser(argc , argv, keys);

    if (cmdParser.has("help"))
    {
        cmdParser.printMessage();
        return 0;
    }

	Mat input , templateImg;

    input = imread(cmdParser.get<cv::String>("input").c_str());
	templateImg = imread(cmdParser.get<cv::String>("template").c_str());

    if ( !input.data )
    {
        printf("No input data \n");
        return -1;
    }

	if (!templateImg.data)
	{
		std::cout << "No template data" << std::endl;
		return -1;
	}

    cvtColor(input , input , COLOR_BGR2GRAY);
	cvtColor(templateImg, templateImg, COLOR_BGR2GRAY);

	auto histogramMatchedImg = histogramMatching(input , templateImg);

    imshow("input" , input);
	imshow("template img", templateImg);
	imshow("Histogram matched output", histogramMatchedImg);

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

double* calculateHistogram(Mat input)
{
	double* inputHistogram = new double[L];
	fillZero(inputHistogram, L);

	for (auto y = 0; y < input.rows; ++y)
	{
		for (auto x = 0; x < input.cols; ++x)
		{
			inputHistogram[input.at<uchar>(y, x)]++;
		}
	}

	return inputHistogram;
}

double* calculatePdf(double* histogram , int totalPixelSize)
{
	double* pdf = new double[L];
	fillZero(pdf, L);

	for (auto i = 0; i < L; ++i)
	{
		pdf[i] = histogram[i] / totalPixelSize;
	}

	return pdf;
}

double* calculateCdf(double* pdf)
{
	double* cdf = new double[L];
	fillZero(cdf, L);
	cdf[0] = pdf[0];

	for (auto i = 1; i < L; ++i)
	{
		cdf[i] = cdf[i - 1] + pdf[i];
	}

	return cdf;
}

Mat histogramMatching(Mat input, Mat templateImg)
{
	Mat output(input.rows, input.cols, CV_8U);

	//Firstly calculate histogram
	double* inputHistogram = calculateHistogram(input);
	double* templateHistogram = calculateHistogram(templateImg);

	//Secondly calculate PDF
	double* inputPdf = calculatePdf(inputHistogram , input.rows * input.cols);
	double* templatePdf = calculatePdf(templateHistogram, templateImg.rows * templateImg.cols);

	//Thirdly calculate CDF
	double* inputCdf = calculateCdf(inputPdf);
	double* templateCdf = calculateCdf(templatePdf);


	double mapping[L];

	//Calculate histogram specification function
	for (auto i = 0; i < L; ++i)
	{
		auto hIntensity = (L - 1) * inputCdf[i];
		auto correspondingIdx = 0;
		auto minDifference = 256.0;
		for (auto j = 0; j < L; ++j)
		{
			auto currentIntensity = (L - 1) * templateCdf[j];
			auto difference = std::abs(currentIntensity - hIntensity);
			if (difference < minDifference)
			{
				correspondingIdx = j;
				minDifference = difference;
			}
		}

		mapping[i] = correspondingIdx;
	}

	//Apply histogram matching function to input
	for (auto y = 0; y < input.rows; ++y)
	{
		for (auto x = 0; x < input.cols; ++x)
		{
			output.at<uchar>(y, x) = mapping[input.at<uchar>(y , x)];
		}
	}

	double* outputHistogram = calculateHistogram(output);
	double* outputPdf = calculatePdf(outputHistogram, output.rows * output.cols);

	imshow("Input Histogram", dip::drawHistogram(inputPdf, L));
	imshow("Template Histogram", dip::drawHistogram(templatePdf, L));
	imshow("Output Histogram", dip::drawHistogram(outputPdf, L));


	return output;
}