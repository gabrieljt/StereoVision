#include <SV/CameraCapture.hpp>
#include <SV/Utility.hpp>

#include <pylon/InstantCamera.h>
#include <pylon/GrabResultPtr.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <iostream>


CameraCapture::CameraCapture(std::string cameraName, SV::StereoPhoto* stereoPhotoPtr)
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
, mPatternSize()
, mStereoPhotoPtr(stereoPhotoPtr)
{
    cv::namedWindow(mCameraName, CV_WINDOW_AUTOSIZE);
    
    // Load Calibration Matrices from the XML files
    for (int i = 0; i < mCalibrationMatrices.size(); ++i)
    {
        cv::FileStorage fs(mCalibrationMatricesFiles[i], cv::FileStorage::READ);
        fs[mCalibrationMatricesNames[i]] >> mCalibrationMatrices[i];
        fs.release();
    }

    auto calibrationPattern = SV::loadCalibrationPatternFile();
    mPatternSize.width = calibrationPattern.w;
    mPatternSize.height = calibrationPattern.h;
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
        {
            cv::remap(image, undistortedImage, mCalibrationMatrices[MX1], mCalibrationMatrices[MY1], 0);
            mStereoPhotoPtr->matPair.first = undistortedImage;
        }
        else
        {
            cv::remap(image, undistortedImage, mCalibrationMatrices[MX2], mCalibrationMatrices[MY2], 0);
            mStereoPhotoPtr->matPair.second = undistortedImage;

            auto undistortedImageLeft = mStereoPhotoPtr->matPair.first;
            auto undistortedImageRight = mStereoPhotoPtr->matPair.second;
            auto leftCamera = mStereoPhotoPtr->cameras[0];
            auto rightCamera = mStereoPhotoPtr->cameras[1];

            std::vector<cv::Point2f> cornersLeft;
            auto resultLeft = cv::findChessboardCorners(undistortedImageLeft, mPatternSize, cornersLeft,
                cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);
            drawChessboardCorners(undistortedImageLeft, mPatternSize, cv::Mat(cornersLeft), resultLeft);

            std::vector<cv::Point2f> cornersRight;
            auto resultRight = cv::findChessboardCorners(undistortedImageRight, mPatternSize, cornersRight,
                cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);
            drawChessboardCorners(undistortedImageRight, mPatternSize, cv::Mat(cornersRight), resultRight);

            if (resultLeft && resultRight)
                std::cout << "Found " << cornersLeft.size() << " corners." << std::endl;           
            for (int i = 0; i < cornersLeft.size(); ++i)
            {
                auto pointLeftImage = cornersLeft[i];
                auto pointRightImage = cornersRight[i];
                auto QMat = mCalibrationMatrices[Q];
                auto d = pointRightImage.x - pointLeftImage.x;
                auto X = pointLeftImage.x * QMat.at<double>(0, 0) + QMat.at<double>(0, 3);
                auto Y = pointLeftImage.y * QMat.at<double>(1, 1) + QMat.at<double>(1, 3);
                auto Z = QMat.at<double>(2, 3);
                auto W = d * QMat.at<double>(3, 2) + QMat.at<double>(3, 3);

                X = X / W;
                Y = Y / W;
                Z = Z / W;
                printf("Corner %u >> X: %f; Y:%f; Z:%f;\n", i, X, Y, Z);
                std::string imageText("(" + std::to_string(X).substr(0,7) +
                                    ", " + std::to_string(Y).substr(0,7) +
                                    ", " + std::to_string(Z).substr(0,7) + ")");

                cv::RNG rng(0xFFFFFFFF);
                if (i == 0 || i == cornersLeft.size() - 1)
                {
                    cv::putText(undistortedImageLeft, imageText,
                        cv::Point2f(pointLeftImage.x, pointLeftImage.y),
                        rng.uniform(0,8), rng.uniform(10,10)*0.05+0.1, SV::openCVRandomColor(rng), rng.uniform(1, 1), 8);

                    cv::putText(undistortedImageRight, imageText,
                        cv::Point2f(pointRightImage.x, pointRightImage.y),
                        rng.uniform(0,8), rng.uniform(10,10)*0.05+0.1, SV::openCVRandomColor(rng), rng.uniform(1, 1), 8);
                }
            }
            cv::imshow(leftCamera, undistortedImageLeft);
            cv::imshow(rightCamera, undistortedImageRight);
        }
    }
    else
    {
        std::cout << mCameraName << "CameraCapture::OnImageGrabbed() ERROR: " << grabResultPtr->GetErrorCode() << " " << grabResultPtr->GetErrorDescription() << std::endl;
    }
}
