#include <SV/CameraCalibration.hpp>
#include <SV/Utility.hpp>

#include <pylon/InstantCamera.h>
#include <pylon/GrabResultPtr.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <fstream>


CameraCalibration::CameraCalibration(std::string cameraName, unsigned int* grabCountPtr, std::ofstream* imageListFilePtr)
: mCameraName(cameraName)
, mGrabCountPtr(grabCountPtr)
, mImageListFilePtr(imageListFilePtr)
{
	//cv::namedWindow(mCameraName, CV_WINDOW_AUTOSIZE);
}

void CameraCalibration::OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResultPtr)
{
	if (grabResultPtr->GrabSucceeded())
    {        
        // OpenCV image CV_8U: 8-bits, 1 channel        
        auto image = cv::Mat(grabResultPtr->GetHeight(), grabResultPtr->GetWidth(), CV_8UC1, grabResultPtr->GetBuffer());
        auto cameraContextValue = grabResultPtr->GetCameraContext();                     
        std::string imagePath;

        SV::EMULATION_MODE ? imagePath = SV::EMULATED_IMAGES_PATH : imagePath = SV::CALIBRATION_IMAGES_PATH;
        
        if (*mGrabCountPtr < 10u)
            imagePath += "0";
        imagePath += std::to_string(*mGrabCountPtr);        
        
        // Left Image
        if (cameraContextValue == 0)
           	imagePath += SV::CALIBRATION_IMAGE_LEFT;
        // Right Image
        else if (cameraContextValue == 1)
          	imagePath += SV::CALIBRATION_IMAGE_RIGHT;
        
        if (SV::EMULATION_MODE)
            image = cv::imread(imagePath);

        // TODO: save image only if found chessboard corners
        bool foundChessboardCorners;
        SV::EMULATION_MODE ? 
            foundChessboardCorners = true 
        :
            foundChessboardCorners = true;

        if (foundChessboardCorners)
        {
            *mImageListFilePtr << imagePath << std::endl;
            cv::imwrite(imagePath, image);                    
            std::cout << "Photo [" << imagePath << "] saved." << std::endl;
            if (cameraContextValue == 1)
                *mGrabCountPtr += 1u;
        }
    }
    else
    {
        std::cout << mCameraName << "CameraCalibration::OnImageGrabbed() ERROR: " << grabResultPtr->GetErrorCode() << " " << grabResultPtr->GetErrorDescription() << std::endl;
    }
}