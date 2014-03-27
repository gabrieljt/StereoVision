#include <SV/CameraCalibration.hpp>
#include <SV/Utility.hpp>

#include <pylon/InstantCamera.h>
#include <pylon/GrabResultPtr.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <fstream>


CameraCalibration::CameraCalibration(std::string cameraName, unsigned int* grabCountPtr, std::ofstream* imageListFilePtr, bool emulated)
: mCameraName(cameraName)
, mGrabCountPtr(grabCountPtr)
, mImageListFilePtr(imageListFilePtr)
, mEmulated(emulated)
{
	//cv::namedWindow(mCameraName, CV_WINDOW_AUTOSIZE);
}

void CameraCalibration::OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResultPtr)
{
	if (grabResultPtr->GrabSucceeded())
    {        
        // OpenCV image CV_8U: 8-bits, 1 channel        
        cv::Mat image;
        mEmulated ? image = cv::imread(SV::EMULATED_IMAGE) : image = cv::Mat(grabResultPtr->GetHeight(), grabResultPtr->GetWidth(), CV_8UC1, grabResultPtr->GetBuffer());
        auto cameraContextValue = grabResultPtr->GetCameraContext();             
        std::string imagePath(SV::CALIBRATION_IMAGES_PATH);
        std::string imageName;
        if (*mGrabCountPtr < 10u)
            imageName = "0";
        imageName += std::to_string(*mGrabCountPtr);        
        
        // Left Image
        if (cameraContextValue == 0)
           	imageName += SV::CALIBRATION_IMAGE_LEFT;
        // Right Image
        else if (cameraContextValue == 1)
          	imageName += SV::CALIBRATION_IMAGE_RIGHT;
        imagePath += imageName;

        // TODO: save image only if found chessboard corners
        bool foundChessboardCorners = true;
        if (foundChessboardCorners)
        {
            *mImageListFilePtr << SV::CALIBRATION_IMAGES_RELATIVE_PATH + imageName;
            *mImageListFilePtr << std::endl;
            cv::imwrite(imagePath, image);                    
            std::cout << "Photo [" << imagePath << "] saved." << std::endl;
            if (cameraContextValue == 1)
                *mGrabCountPtr += 1u;
        }
    }
    else
    {
        std::cout << mCameraName << " ERROR: " << grabResultPtr->GetErrorCode() << " " << grabResultPtr->GetErrorDescription() << std::endl;
    }
}