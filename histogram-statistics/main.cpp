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
 * @brief Program does image enchancement by doing local histogram processing with usage of histogram statistics
 * Local histogram processin and histogram statistics which are mentioned at Chapter 3.3.3, 3.3.4 - Digital Image Processing (3rd Edition): Rafael C. Gonzalez
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

double* calculateHistogram(Mat input);

//pdf array  represents the pr(rj) part of Eq. 3.3-8
double* calculatePdf(double* histogram, int totalPixelSize);

//                          k
//Cdf function reprents the E pr(rj) part of Eq. 3.3-8
//                          j=0
double* calculateCdf(double* pdf);

//Equation : 3.3-18
double calculateMean(double* pdf);

//Equation : 3.3-19
double calculateVarianceSquare(double* p, double m);

//Equation : 3.3-20
double calculateSampleMean(Mat input);

//Equation : 3.3-21
double calculateSampleVarianceSquare(Mat input, double m);

//Equation : 3.3-24
Mat imageEnchancement(Mat input, int sxy, double E, double k0, double mG, double k1, double k2, double vG);

int main(int argc, char** argv) {
        const String keys = 
	"{help h usage ?    || The program does image enchanchement with using histogram statistics. Default parameters are set as the book suggested.}"
    "{input             | histogram-statistics.jpg | input image}"
	"{Sxy               | 3                        | size of the region, 3 means 3x3}"
	"{E                 | 4.0                      | enchancement multiplier}"
	"{k0                | 0.4                      | minimum acceptable mean, which is k0*mG}"
	"{k1                | 0.02                     | lower bound of acceptable variance, which is k1*vG}"
	"{k2                | 0.4                      | upper bound of acceptable variance, which is k2*vG}"
    ;

    CommandLineParser cmdParser(argc , argv, keys);

    if (cmdParser.has("help"))
    {
        cmdParser.printMessage();
        return 0;
    }

	Mat input , templateImg;
    input = imread(cmdParser.get<cv::String>("input").c_str());

	auto sxy = cmdParser.get<int>("Sxy");

	if (sxy % 2 == 0)
	{
		std::cout << "Sxy cannot be even." << std::endl;
		return 1;
	}

	auto E = cmdParser.get<double>("E");
	auto k0 = cmdParser.get<double>("k0");
	auto k1 = cmdParser.get<double>("k1");
	auto k2 = cmdParser.get<double>("k2");

    if ( !input.data )
    {
        printf("No input data \n");
        return -1;
    }

    cvtColor(input , input , COLOR_BGR2GRAY);

	auto histogram = calculateHistogram(input);
	auto pdf = calculatePdf(histogram, input.rows * input.cols);

	auto mG = calculateMean(pdf);
	auto varianceG = std::sqrt(calculateVarianceSquare(pdf, mG));

	std::cout << "Global Mean : " << mG << " Global Variance : " << varianceG << std::endl;
	auto sampleMeanG = calculateSampleMean(input);
	auto sampleVarianceG = std::sqrt(calculateSampleVarianceSquare(input, sampleMeanG));
	std::cout << "Global Sample Mean : " << sampleMeanG << " Global Sample Variance : " << sampleVarianceG << std::endl;

	auto enchancedImage = imageEnchancement(input, sxy, E, k0, mG, k1, k2, varianceG);

    imshow("input" , input);
	imshow("enchanced image", enchancedImage);

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

double calculateMean(double* p)
{
	auto m = .0;

	for (auto i = 0; i < L; ++i)
	{
		auto ri = i;
		m += ri * p[ri];
	}

	return m;
}

double calculateVarianceSquare(double* p, double m)
{
	auto varianceSquare = .0;

	for (auto i = 0; i <= L - 1; ++i)
	{
		auto ri = i;

		varianceSquare += std::pow(ri - m, 2) * p[ri];
	}

	return varianceSquare;
}

double calculateSampleMean(Mat input)
{
	auto m = .0;
	auto M = input.cols;
	auto N = input.rows;

	for (auto x = 0; x <= M - 1; ++x)
	{
		for (auto y = 0; y <= N - 1; ++y)
		{
			m += input.at<uchar>(y, x);
		}
	}

	m *= 1.0 / (M * N);

	return m;
}

double calculateSampleVarianceSquare(Mat input, double m)
{
	auto varianceSquare = .0;
	auto M = input.cols;
	auto N = input.rows;

	for (auto x = 0; x <= M - 1; ++x)
	{
		for (auto y = 0; y <= N - 1; ++y)
		{
			varianceSquare += std::pow(static_cast<double>(input.at<uchar>(y, x)) - m , 2);
		}
	}

	varianceSquare *= 1.0 / (M * N);

	return varianceSquare;
}

Mat imageEnchancement(Mat input, int sxySize, double E, double k0, double mG, double k1, double k2, double vG)
{

	Mat output = Mat::zeros(input.rows, input.cols , CV_8U);

	auto f = [&](int x, int y) {   return input.at<uchar>(y, x); };

	auto offset = (sxySize - 1) / 2;
	for (auto y = offset; y < input.rows - offset; ++y)
	{
		for (auto x = offset; x < input.cols - offset; ++x)
		{
			Mat sxy = input(Rect(x - offset, y - offset, sxySize, sxySize));

			auto mSxy = calculateSampleMean(sxy);
			auto vSxy = calculateSampleVarianceSquare(sxy, mSxy);
			auto k0mg = k0 * mG;
			auto k1Vg = k1 * vG;
			auto k2Vg = k2 * vG;

			if (mSxy < k0mg && (k1Vg <= vSxy && vSxy <= k2Vg))
			{
				output.at<uchar>(y, x) = dip::stayInBoundaries(E * f(x, y) , dip::Upper(255) , dip::Lower(0));
			}
			else //otherwise
			{
				output.at<uchar>(y, x) = f(x, y);
			}
		}
	}

	return output;
}
