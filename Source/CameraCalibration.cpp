#include <SV/CameraCalibration.hpp>
#include <SV/Utility.hpp>

#include <pylon/InstantCamera.h>
#include <pylon/GrabResultPtr.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <cstdio>
#include <iostream>
#include <fstream>


CameraCalibration::CameraCalibration(std::string cameraName)
: mCameraName(cameraName)
, mPhotosTaken(0u)
{
	cv::namedWindow(mCameraName, CV_WINDOW_AUTOSIZE);
}

void CameraCalibration::OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResultPtr)
{
	if (grabResultPtr->GrabSucceeded())
    {        
        // OpenCV image CV_8U: 8-bits, 1 channel
        auto image = cv::Mat(grabResultPtr->GetHeight(), grabResultPtr->GetWidth(), CV_8UC1, grabResultPtr->GetBuffer());
        auto cameraContextValue = grabResultPtr->GetCameraContext();             
        std::string imagePath(SV::CALIBRATION_IMAGES_PATH);
        if (mPhotosTaken < 10u)
            imagePath += "0";
        imagePath += std::to_string(mPhotosTaken);        
        
        // TODO: save image only if found chessboard corners
        ++mPhotosTaken;        
        // Left Image
        if (cameraContextValue == 0)
           	imagePath += SV::CALIBRATION_IMAGE_LEFT;
        // Right Image
        else if (cameraContextValue == 1)
          	imagePath += SV::CALIBRATION_IMAGE_RIGHT;

     	cv::imwrite(imagePath, image);
     	printf("Photo %s saved.\n", imagePath.c_str());
    }
    else
    {
        std::cout << mCameraName << " ERROR: " << grabResultPtr->GetErrorCode() << " " << grabResultPtr->GetErrorDescription() << std::endl;
    }
}