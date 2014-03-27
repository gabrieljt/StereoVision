#ifndef SV_CAMERACALIBRATION_HPP
#define SV_CAMERACALIBRATION_HPP


#include <pylon/ImageEventHandler.h>

#include <string>


namespace Pylon
{
    class CInstantCamera;
    class CGrabResultPtr;
}

class CameraCalibration : public Pylon::CImageEventHandler
{
	public:
						CameraCalibration(std::string cameraName);

		virtual void	OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResultPtr);


	private:
		std::string		mCameraName;
		unsigned int 	mPhotosTaken;		
};

#endif // SV_CAMERACALIBRATION_HPP
