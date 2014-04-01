#include <SV/CameraCapture.hpp>

#include <pylon/InstantCamera.h>
#include <pylon/GrabResultPtr.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>


CameraCapture::CameraCapture(std::string cameraName)
: mCameraName(cameraName)
{
    cv::namedWindow(mCameraName, CV_WINDOW_AUTOSIZE);
    // TODO: load calibration parameters
}

void CameraCapture::OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResultPtr)
{
    if (grabResultPtr->GrabSucceeded())
    {        
        // OpenCV image CV_8U: 8-bits, 1 channel
        auto image = cv::Mat(grabResultPtr->GetHeight(), grabResultPtr->GetWidth(), CV_8UC1, grabResultPtr->GetBuffer());
        // Apply BayerGB8 Filter
        cv::cvtColor(image, image, CV_BayerGB2RGB);
        // Display image
        cv::imshow(mCameraName, image);
    }
    else
    {
        std::cout << mCameraName << " ERROR: " << grabResultPtr->GetErrorCode() << " " << grabResultPtr->GetErrorDescription() << std::endl;
    }
}
