#ifndef SV_CAMERACONFIGURATION_HPP
#define SV_CAMERACONFIGURATION_HPP


#include <pylon/ConfigurationEventHandler.h>

#include <string>


// Forward Declaration
namespace Pylon
{
    class CInstantCamera;
}

class CameraConfiguration : public Pylon::CConfigurationEventHandler
{
    public:
                            CameraConfiguration(const char* configurationFile, const int interPacketDelay, int frameTransmissionDelay, std::string cameraName);
        void                OnOpened(Pylon::CInstantCamera& camera);
        void                OnGrabStarted(Pylon::CInstantCamera& camera);

    private:
        const char*         mConfigurationFile;
        const int           mInterPacketDelay;
        const int           mFrameTransmissionDelay;
        const std::string   mCameraName;        
};

#endif // SV_CAMERACONFIGURATION_HPP
