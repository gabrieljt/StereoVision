#ifndef SV_CAMERACAPTURE_HPP
#define SV_CAMERACAPTURE_HPP


#include <pylon/ImageEventHandler.h>

#include <string>


// Forward Declaration
namespace Pylon
{
    class CInstantCamera;
    class CGrabResultPtr;
}

class CameraCapture : public Pylon::CImageEventHandler
{
    public:
    					CameraCapture(std::string cameraName);

        virtual void    OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResultPtr);


    private:
        std::string		mCameraName;
};

#endif // SV_CAMERACAPTURE_HPP
