#include <SV/CameraCapture.hpp>

#include <pylon/InstantCamera.h>
#include <pylon/GrabResultPtr.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <iostream>


CameraCapture::CameraCapture(std::string cameraName)
: mCameraName(cameraName)
, mCalibrationMatrices()
, mCalibrationMatricesFiles({
                                SV::CALIBRATION_XML_FILES_PATH + "Q.xml", 
                                SV::CALIBRATION_XML_FILES_PATH + "mx1.xml", 
                                SV::CALIBRATION_XML_FILES_PATH + "my1.xml", 
                                SV::CALIBRATION_XML_FILES_PATH + "mx2.xml",
                                SV::CALIBRATION_XML_FILES_PATH + "my2.xml"
                            })
, mCalibrationMatricesNames({"Q", "mx1", "my1", "mx2", "my2"})
, mCalibrationPattern(SV::loadCalibrationPatternFile())
{
    cv::namedWindow(mCameraName, CV_WINDOW_AUTOSIZE);
    // Load Calibration Matrices from the XML files
    for (int i = 0; i < mCalibrationMatrices.size(); ++i)
    {
        cv::FileStorage fs(mCalibrationMatricesFiles[i], cv::FileStorage::READ);
        fs[mCalibrationMatricesNames[i]] >> mCalibrationMatrices[i];
        fs.release();
    }
}

void CameraCapture::OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResultPtr)
{
    if (grabResultPtr->GrabSucceeded())
    {        
        // OpenCV image CV_8U: 8-bits, 1 channel
        auto image = cv::Mat(grabResultPtr->GetHeight(), grabResultPtr->GetWidth(), CV_8UC1, grabResultPtr->GetBuffer());
        auto cameraContextValue = grabResultPtr->GetCameraContext();

        if (SV::EMULATION_MODE)        
            cameraContextValue == 0 ? image = cv::imread(SV::EMULATED_IMAGES_PATH + "04left.ppm") : image = cv::imread(SV::EMULATED_IMAGES_PATH + "04right.ppm");

        cv::Mat undistortedImage;
        if (cameraContextValue == 0)
            cv::remap(image, undistortedImage, mCalibrationMatrices[MX1], mCalibrationMatrices[MY1], 0);
        else
            cv::remap(image, undistortedImage, mCalibrationMatrices[MX2], mCalibrationMatrices[MY2], 0);

        cv::Size patternSize(mCalibrationPattern.w, mCalibrationPattern.h);        
        std::vector<cv::Point2f> corners;
        auto foundChessboard = cv::findChessboardCorners(undistortedImage, patternSize, corners,
            cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);

        if (foundChessboard)
        {
            drawChessboardCorners(undistortedImage, patternSize, cv::Mat(corners), foundChessboard);    
            std::cout << "Corners Found: " << corners.size() << std::endl;           
        }
        // Display image
        cv::imshow(mCameraName, undistortedImage);
    }
    else
    {
        std::cout << mCameraName << "CameraCapture::OnImageGrabbed() ERROR: " << grabResultPtr->GetErrorCode() << " " << grabResultPtr->GetErrorDescription() << std::endl;
    }
}
