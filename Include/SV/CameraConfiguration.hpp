#ifndef SV_CAMERACONFIGURATION_HPP
#define SV_CAMERACONFIGURATION_HPP


#include <pylon/ConfigurationEventHandler.h>


// Forward Declaration
namespace Pylon
{
    class CInstantCamera;
}

class CameraConfiguration : public Pylon::CConfigurationEventHandler
{
    public:
                        CameraConfiguration(const char* configurationFile, const int interPacketDelay, int frameTransmissionDelay);
        void            OnOpened(Pylon::CInstantCamera& camera);

    private:
        const char*     mConfigurationFile;
        const int       mInterPacketDelay;
        const int       mFrameTransmissionDelay;
};

#endif // SV_CAMERACONFIGURATION_HPP
