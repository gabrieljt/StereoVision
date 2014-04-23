#ifndef SV_CAMERACAPTURE_HPP
#define SV_CAMERACAPTURE_HPP


#include <SV/Utility.hpp>

#include <pylon/ImageEventHandler.h>

#include <opencv2/core/core.hpp>

#include <array>
#include <string>


// Forward Declaration
namespace Pylon
{
    class CInstantCamera;
    class CGrabResultPtr;
}

namespace
{
	const int Q 	= 0;
	const int MX1 	= 1;
	const int MY1 	= 2;
	const int MX2	= 3;
	const int MY2 	= 4;	
}

class CameraCapture : public Pylon::CImageEventHandler
{
    public:
    										CameraCapture(std::string cameraName, SV::StereoPhoto* stereoPhotoPtr);

        virtual void    					OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResultPtr);


    private:
        std::string							mCameraName;
        std::array<cv::Mat, 5>				mCalibrationMatrices;
        const std::array<std::string, 5>	mCalibrationMatricesFiles;
        const std::array<std::string, 5>	mCalibrationMatricesNames;
        cv::Size                            mPatternSize;
        SV::StereoPhoto*                    mStereoPhotoPtr;
        float                               mThreshold;
};

#endif // SV_CAMERACAPTURE_HPP
