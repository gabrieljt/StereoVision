#ifndef SV_CAMERACALIBRATION_HPP
#define SV_CAMERACALIBRATION_HPP


#include <pylon/ImageEventHandler.h>

#include <opencv2/core/core.hpp>

#include <string>
#include <fstream>
#include <utility>


namespace Pylon
{
    class CInstantCamera;
    class CGrabResultPtr;
}

class CameraCalibration : public Pylon::CImageEventHandler
{
	public:
								CameraCalibration(std::string cameraName, bool* synchronizedPtr, unsigned int* grabCountPtr, std::ofstream* imageListFilePtr, std::pair<bool, bool>* wroteToFilePairPtr);

		virtual void			OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResultPtr);


	private:
		std::string				mCameraName;
		bool*					mSynchronizedPtr;
		unsigned int* 			mGrabCountPtr;		
		std::ofstream*			mImageListFilePtr;
		std::pair<bool, bool>*	mWroteToFilePairPtr;
		cv::Size       			mPatternSize;
		float		 			mThreshold;
};

#endif // SV_CAMERACALIBRATION_HPP
