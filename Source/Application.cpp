#include <SV/Application.hpp>

#include <opencv2/opencv.hpp>

#include <iostream>


Application::Application(const std::string leftCameraWindow, const std::string rightCameraWindow)
: mLeftCameraWindow(leftCameraWindow)
, mRightCameraWindow(rightCameraWindow)
{
    std::cout << "Initializing Stereo Vision..." << std::endl;    
	cv::namedWindow(mLeftCameraWindow, CV_WINDOW_AUTOSIZE);
	cv::namedWindow(mRightCameraWindow, CV_WINDOW_AUTOSIZE);
	cv::moveWindow(mLeftCameraWindow, 0, 300);
	cv::moveWindow(mRightCameraWindow, 400, 300);
}

void Application::run()
{
    cv::waitKey(0);
}

void Application::update()
{
}

void Application::render()
{
}
