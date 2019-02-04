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
 * @brief The program does affine transformations such as scaling, rotation, translation, vertical and horizontal shearing
 * Affine transformations which are mentioned at Chapter 2.6.5 - Digital Image Processing (3rd Edition): Rafael C. Gonzalez
 *
 * @author Ozan Cansel
 * Contact: ozancansel@gmail.com
 *
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include "utility.h"

using namespace cv;
using namespace dip;

/**
 *  Commonly used spatial coordinate affine transformation is defined by Wolberg [1990], which has the general form
 *  T is the transformation operator such as Rotation, scaling. translation etc.
 *                                |t11  t12 0|
 *  [x y 1] = [v w 1] T = [v w 1] |t21  t22 0|
 *                                |t31  t32 1|
 **/

/**
 * Scale matrix
 *  -----------------
 * |cx  0   0|  =>  x = cx * v
 * |0   cy  0|      y = cy * w
 * |0   0   1|
 * 
 **/
Mat scale(Mat input, double vFactor , double hFactor);

/**
 * Rotation matrix>
 *  -----------------
 * |cosϴ    sin0    0|  =>  x = v*costϴ + (-w*sinϴ)
 * |-sinϴ   cosϴ    0|      y = v*sinϴ + w*cosϴ
 * |0       0       1|
 *  
 **/
Mat rotate(Mat input , double angle);

/**
 * Translation matrix
 *  -----------------
 * |1   0   0|  =>  x = v + tx
 * |0   1   0|      y = w + ty
 * |tx  ty  1|
 *  
 **/
Mat translate(Mat input, cv::Size size);

/**
 * Vertical shearing matrix
 *  -----------------
 * |1   0   0|  =>  x = v + w*sv
 * |sv  1   0|      y = w
 * |0   0   1|
 *  
 **/
Mat shearV(Mat input, double shearVal);

/**
 * Horizontal shearing matrix
 *  -----------------
 * |1   sh  0|  =>  x = v
 * |0   1   0|      y = v*sh + w
 * |0   0   1|
 *  
 **/
Mat shearH(Mat input, double shearVal);

int main(int argc, char** argv) {
            const String keys = 
    "{help h usage ?    |                           | The program does affine transformations such as scale, rotation, translation, vertical and horizontal shearing.}"
    "{input             | affine-transformation.jpg | input image}"
    "{scale             | 1.7,0.7                   | scale ratio}"
    "{rotation          | 45                        | rotation angle}"
    "{translation       | 60,60                     | shift P(x,y)}"
    "{shearV            | 0.3                       | vertical shear value}"
    "{shearH            | 0.4                       | horizontal shear value}"
    ;

    CommandLineParser cmdParser(argc , argv, keys);
    Mat input;   

    if (cmdParser.has("help"))
    {
        cmdParser.printMessage();
        return 0;
    }

    input = imread(cmdParser.get<cv::String>("input").c_str());

    if ( !input.data )
    {
        printf("No input data \n");
        return -1;
    }

	//Parsing user inputs
	std::vector<std::string> xyScale;
	dip::split(cmdParser.get<cv::String>("scale") , xyScale , ',');

	auto xFactor = std::stod(xyScale.at(0));
	auto yFactor = std::stod(xyScale.at(1));

	auto rotation = cmdParser.get<int>("rotation");

	std::vector<std::string> xyTranslation;
	dip::split(cmdParser.get<cv::String>("translation"), xyTranslation, ',');
	
	auto xTranslation = std::stoi(xyTranslation.at(0));
	auto yTranslation = std::stoi(xyTranslation.at(1));

	auto shearVerticalFactor = cmdParser.get<double>("shearV");
	auto shearHorizontalFactor = cmdParser.get<double>("shearH");

	//All processing wil be done in grayscale for simplicity
	cvtColor(input, input, COLOR_BGR2GRAY);

	//Convert degrees to radian
	//Notice : Rotating matrix flips the image on y axis defaultly, it is added for biasing.
	auto degreesInRadian = (180 + rotation) / 180.0 * CV_PI;

	auto scaledImg = scale(input, yFactor, xFactor);
	auto rotatedImg = rotate(input, degreesInRadian);
	auto translatedImg = translate(input, cv::Size(xTranslation, yTranslation));
	auto shearedVImg = shearV(input , shearVerticalFactor);
	auto shearedHImg = shearH(input, shearHorizontalFactor);

	imshow("original", input);
	imshow(std::string("Scaled vFactor :") + std::to_string(yFactor) + std::string(" hFactor ") + std::to_string(xFactor), scaledImg);
	imshow(std::string("Rotated by ") + std::to_string(rotation) + std::string(" degree"), rotatedImg);
	imshow(std::string("Translated by x : ") + std::to_string(xTranslation) + std::string(" y : ") + std::to_string(yTranslation) , translatedImg);
	imshow(std::string("Sheared vertically by ") + std::to_string(shearVerticalFactor), shearedVImg);
	imshow(std::string("Sheared horizontally by ") + std::to_string(shearHorizontalFactor), shearedHImg);
	waitKey(0);

    return 0;
}


Mat scale(Mat input, double vFactor , double hFactor)
{
    Mat horizontallyScaled = Mat::zeros(input.rows , std::round(input.cols * hFactor)  , CV_8U);
    Mat completelyScaled = Mat::zeros(std::round(input.rows * vFactor) , horizontallyScaled.cols , CV_8U);

    //Firstly strecth horizontally
    for(auto w = 0; w < horizontallyScaled.rows; ++w)
    {
        for(auto v = 0; v < horizontallyScaled.cols; ++v)
        {
			auto rounded = std::round(v / hFactor);
            horizontallyScaled.at<uchar>(w , v) = input.at<uchar>(w , rounded);
        }
    }

    //After stretch vertically stretched image vertically
    for(auto w = 0; w < completelyScaled.rows; ++w)
    {
        for(auto v = 0 ; v < completelyScaled.cols; ++v)
        {
            completelyScaled.at<uchar>(w , v) = horizontallyScaled.at<uchar>(std::round(w / vFactor) , v);
        }
    }

    return completelyScaled;
}

Mat rotate(Mat input , double angle)
{
    Mat rotated = Mat::zeros(input.rows * 2 , input.cols * 2 , CV_8U);

	auto centerX = input.cols / 2;
	auto centerY = input.rows / 2;
	for (auto w = 0; w < input.rows; ++w)
	{
		for (auto v = 0; v < input.cols; ++v)
		{
			auto x = std::round((centerX - v) * std::cos(angle) - (centerY - w) * std::sin(angle));
			auto y = std::round((centerX - v) * std::sin(angle) + (centerY - w) * std::cos(angle));

			//Center the point to the frame, these lines are not part of the rotating algorithm
			y += input.rows - 1;
			x += input.cols - 1;

			if (x < 0)
				x = 0;
			if (y < 0)
				y = 0;

			rotated.at<uchar>(y, x) = input.at<uchar>(w, v);
		}
	}

    return rotated;
}

Mat translate(Mat input, cv::Size translation)
{
    Mat output = Mat::zeros(input.rows,  input.cols , CV_8U);

	for (auto w = 0; w < input.rows; ++w)
	{
		for (auto v = 0; v < input.cols; ++v)
		{
			auto x = v + translation.width;
			auto y = w + translation.height;

			//This codeblock shifts overflowed pixels
			x %= input.cols;
			y %= input.rows;

			output.at<uchar>(y, x) = input.at<uchar>(w, v);
		}
	}
    
    return output;
}

Mat shearV(Mat input, double sv)
{
	//Calculating output frame size
    Mat output = Mat::zeros(input.rows , input.cols + (sv * input.cols), CV_8U);
    
	for (auto w = 0; w < input.rows; ++w)
	{
		for (auto v = 0; v < input.cols; ++v)
		{
			auto x = v + sv * w;
			auto y = w;

			output.at<uchar>(y, x) = input.at<uchar>(w, v);
		}
	}

    return output;
}

Mat shearH(Mat input, double sh)
{
	//Calculating output frame size
	Mat output = Mat::zeros(input.rows + std::round(sh * input.rows) + 3, input.cols, CV_8U);

	for (auto w = 0; w < input.rows; ++w)
	{
		for (auto v = 0; v < input.cols; ++v)
		{
			auto x = v;
			auto y = std::round(sh * v + w);

			output.at<uchar>(y, x) = input.at<uchar>(w, v);
		}
	}

	return output;
}