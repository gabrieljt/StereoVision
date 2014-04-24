#include <SV/CameraCalibration.hpp>
#include <SV/Utility.hpp>

#include <pylon/InstantCamera.h>
#include <pylon/GrabResultPtr.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <iostream>
#include <fstream>


CameraCalibration::CameraCalibration(std::string cameraName, bool* synchronizedPtr, unsigned int* grabCountPtr, std::ofstream* imageListFilePtr, std::pair<bool, bool>* wroteToFilePairPtr)
: mCameraName(cameraName)
, mSynchronizedPtr(synchronizedPtr)
, mGrabCountPtr(grabCountPtr)
, mImageListFilePtr(imageListFilePtr)
, mWroteToFilePairPtr(wroteToFilePairPtr)
, mPatternSize()
, mThreshold(0.f)
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
        std::string imagePath;
        SV::EMULATION_MODE ? imagePath = SV::EMULATED_IMAGES_PATH : imagePath = SV::CALIBRATION_IMAGES_PATH;
        
        if (*mGrabCountPtr < 10u)
            imagePath += "0";
        imagePath += std::to_string(*mGrabCountPtr);        
        
        auto cameraContextValue = grabResultPtr->GetCameraContext();        
        // Left Image
        if (cameraContextValue == 0)
        {
           	imagePath += SV::CALIBRATION_IMAGE_LEFT;
            if (!mWroteToFilePairPtr->first)
            {
                *mImageListFilePtr << imagePath << std::endl;            
                mWroteToFilePairPtr->first = true;            
            }
        }
        // Right Image
        else if (cameraContextValue == 1)
        {
            imagePath += SV::CALIBRATION_IMAGE_RIGHT;
            if (!mWroteToFilePairPtr->second)
            {
                *mImageListFilePtr << imagePath << std::endl;            
                mWroteToFilePairPtr->second = true;
            }
        }        
        
        // OpenCV image CV_8U: 8-bits, 1 channel        
        auto imageCamera = cv::Mat(grabResultPtr->GetHeight(), grabResultPtr->GetWidth(), CV_8UC1, grabResultPtr->GetBuffer());
        auto imageGray = cv::Mat(grabResultPtr->GetHeight(), grabResultPtr->GetWidth(), CV_8UC1);
        auto image = cv::Mat(grabResultPtr->GetHeight(), grabResultPtr->GetWidth(), CV_8UC1);
        if (SV::EMULATION_MODE)
            image = cv::imread(imagePath);
        else
        {
            cv::cvtColor(imageCamera, imageCamera, CV_BayerGB2RGB);                    
            cv::cvtColor(imageCamera, imageGray, CV_BGR2GRAY);        
            cv::threshold(imageGray, image, mThreshold, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);
        }        
        
        std::vector<cv::Point2f> corners;
        auto foundChessboardCorners = cv::findChessboardCorners(image, mPatternSize, corners,
                cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);
        // Save image only if found chessboard corners            
        if (foundChessboardCorners)
        {
            cv::imwrite(imagePath, image);                    
            std::cout << "Photo [" << imagePath << "] saved." << std::endl;
            if (cameraContextValue == 0)
                *mSynchronizedPtr = true;
            if (cameraContextValue == 1 && *mSynchronizedPtr)
            {
                *mGrabCountPtr += 1u;
                *mSynchronizedPtr = false;                                
                mWroteToFilePairPtr->first = false;
                mWroteToFilePairPtr->second = false;
            }
            drawChessboardCorners(image, mPatternSize, cv::Mat(corners), foundChessboardCorners);            
        }
        else
        {            
            *mSynchronizedPtr = false;
        }

        cv::imshow(mCameraName, image);
    }
    else
    {
        std::cout << mCameraName << "CameraCalibration::OnImageGrabbed() ERROR: " << grabResultPtr->GetErrorCode() << " " << grabResultPtr->GetErrorDescription() << std::endl;
    }
}