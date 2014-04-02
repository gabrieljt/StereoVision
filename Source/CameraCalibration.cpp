#include <SV/CameraCalibration.hpp>
#include <SV/Utility.hpp>

#include <pylon/InstantCamera.h>
#include <pylon/GrabResultPtr.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <iostream>
#include <fstream>


CameraCalibration::CameraCalibration(std::string cameraName, unsigned int* grabCountPtr, std::ofstream* imageListFilePtr)
: mCameraName(cameraName)
, mGrabCountPtr(grabCountPtr)
, mImageListFilePtr(imageListFilePtr)
, mPatternSize()
{
	cv::namedWindow(mCameraName, CV_WINDOW_AUTOSIZE);

	auto calibrationPattern = SV::loadCalibrationPatternFile();
    mPatternSize.width = calibrationPattern.w;
    mPatternSize.height = calibrationPattern.h;
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

        std::vector<cv::Point2f> corners;
        auto foundChessboardCorners = cv::findChessboardCorners(image, mPatternSize, corners,
                cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);
        // Save image only if found chessboard corners            
        if (foundChessboardCorners)
        {
            *mImageListFilePtr << imagePath << std::endl;
            cv::imwrite(imagePath, image);                    
            std::cout << "Photo [" << imagePath << "] saved." << std::endl;
            if (cameraContextValue == 1)
                *mGrabCountPtr += 1u;
            drawChessboardCorners(image, mPatternSize, cv::Mat(corners), foundChessboardCorners);            
        }
        cv::imshow(mCameraName, image);
    }
    else
    {
        std::cout << mCameraName << "CameraCalibration::OnImageGrabbed() ERROR: " << grabResultPtr->GetErrorCode() << " " << grabResultPtr->GetErrorDescription() << std::endl;
    }
}