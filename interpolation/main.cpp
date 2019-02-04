#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>

using namespace cv;

auto targetWidth = 480;
auto targetHeight = 480;

/*
    The algorithm is implemented according to this post https://www.giassa.net/?page_id=207
*/
Mat nearestNeightbourInterpolation(Mat input)
{
    Mat output = Mat::zeros(targetHeight , targetWidth , CV_8U);

    auto xScale = static_cast<float>(targetWidth) / input.cols;
    auto yScale = static_cast<float>(targetHeight) / input.rows;

    for(auto x = 0; x < targetWidth; ++x)
    {
        for(auto y = 0; y < targetHeight; ++y)
        {
            auto fromX = static_cast<int>( std::round( (x + 1) / xScale));
            auto fromY = static_cast<int>(std::round( (y + 1) / yScale));

            output.at<uchar>(y , x) = input.at<uchar>(std::max(0 , fromY - 1) , std::max(0 , fromX - 1));
        }
    }

    return output;
}

/*
    The algorithm is implemented according to this post https://stackoverflow.com/a/32128947
*/
Mat bilinearInterpolation(Mat input)
{
    Mat output = Mat::zeros(targetHeight , targetWidth , CV_8U);

    auto xScale = static_cast<float>(targetWidth) / input.cols;
    auto yScale = static_cast<float>(targetHeight) / input.rows;

    for(auto x = 0; x < targetWidth; ++x)
    {
        for(auto y = 0; y < targetHeight; ++y)
        {
            auto origX = (x / xScale) + 1;
            auto origY = (y / yScale) + 1;
            auto x1 = std::floor(origX);
            auto x2 = std::ceil(origX);
            auto y1 = std::floor(origY);
            auto y2 = std::ceil(origY);

            auto x1Idx = std::max(0 , static_cast<int>(x1) - 2);
            auto x2Idx = std::max(0 , static_cast<int>(x2) - 2);
            auto y1Idx = std::max(0 , static_cast<int>(y1) - 2);
            auto y2Idx = std::max(0 , static_cast<int>(y2) - 2);


            auto r1  = 0;
            auto r2 = 0;

            if(x1 == x2)
            {
                r1 = input.at<uchar>(y1Idx , x1Idx);
                r2 = input.at<uchar>(y2Idx , x1Idx);
            }
            else
            {
                r1 = std::round(input.at<uchar>(y1Idx , x1Idx) + ((origX - x1) / 1.0) * (input.at<uchar>(y1Idx , x2Idx) - input.at<uchar>(y1Idx , x1Idx)));
                r2 = std::round(input.at<uchar>(y2Idx , x1Idx) + ((origX - x1) / 1.0) * (input.at<uchar>(y2Idx , x2Idx) - input.at<uchar>(y2Idx , x1Idx)));
            }
            
            auto P = 0;
            if(r1 == r2)
                P = r1;
            else
                P = std::max(.0f , std::min(255.0f , std::round((r2 + (origY - y2) / (y2 - y1) * (r1 - r2)))));

            output.at<uchar>(y , x) = P;
        }
    }

    return output;
}

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

    Mat nearestNeighbourOutput = nearestNeightbourInterpolation(input);
    Mat bilinearInterpolationOutput = bilinearInterpolation(input);

    namedWindow("Input Image", WINDOW_AUTOSIZE );
    namedWindow("nearest neighbour interpolation" , WINDOW_AUTOSIZE);
    namedWindow("bilinear interpolation" , WINDOW_AUTOSIZE);
    imshow("Input Image", input);
    imshow("nearest neighbour interpolation" , nearestNeighbourOutput);
    imshow("bilinear interpolation" , bilinearInterpolationOutput);
    waitKey(0);
    return 0;
}