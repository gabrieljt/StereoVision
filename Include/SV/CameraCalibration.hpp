#ifndef SV_CAMERACALIBRATION_HPP
#define SV_CAMERACALIBRATION_HPP


#include <pylon/ImageEventHandler.h>

#include <opencv2/core/core.hpp>

#include <string>
#include <fstream>


namespace Pylon
{
    class CInstantCamera;
    class CGrabResultPtr;
}

class CameraCalibration : public Pylon::CImageEventHandler
{
	public:
						CameraCalibration(std::string cameraName, unsigned int* grabCountPtr, std::ofstream* imageListFilePtr);

		virtual void	OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResultPtr);


	private:
		std::string		mCameraName;
		unsigned int* 	mGrabCountPtr;		
		std::ofstream*	mImageListFilePtr;
		cv::Size       	mPatternSize;
};

#endif // SV_CAMERACALIBRATION_HPP
