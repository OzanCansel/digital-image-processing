#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>

using namespace cv;

//Does power(also known as gamma) transformation, returns the output
Mat powerTransformation(Mat input, double gamma);
//Does log transformation, returns the output
Mat logTransformation(Mat input , double gamma);

int main(int argc, char** argv) {
        const String keys = 
    "{help h usage ?    |                           | print this message }"
    "{input             | transformation_darker.jpg | input image}"
    "{gamma             | 1.15                      | gamma value for power transformation}"
    ;

    CommandLineParser cmdParser(argc , argv, keys);
    Mat input;   

    if (cmdParser.has("help"))
    {
        cmdParser.printMessage();
        return 0;
    }

    input = imread(cmdParser.get<cv::String>("input").c_str());
    auto gamma = cmdParser.get<double>("gamma");

    if ( !input.data )
    {
        printf("No input data \n");
        return -1;
    }

    cvtColor(input , input , COLOR_BGR2GRAY);
    auto powerTransformedOutput = powerTransformation(input , gamma);
    auto logTransformedOutput = logTransformation(input , gamma);

    imshow("Original" , input);
    imshow((std::string("Power Transformation - Gamma ") + std::to_string(gamma)).c_str() , powerTransformedOutput);
    imshow("Log Transformation" , logTransformedOutput);
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
            auto logTransformationPixelResult = std::logf(val + 1);
            transformed.at<double>(y , x) = c *  (std::logf(normalized.at<double>(y , x) + 1.0));
        }
    }

    transformed *= 255;
    transformed.convertTo(result , CV_8U);

    return result;
}