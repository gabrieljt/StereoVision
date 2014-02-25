#ifndef SV_APPLICATION_HPP
#define SV_APPLICATION_HPP

#include <pylon/PylonIncludes.h>

#include <string>


class Application
{
    public:
                                    Application();
        void                        run();

    private:
        void                        capture();
        void                        render();

    private:
        const size_t                mNumberOfCameras;        
        std::vector<std::string>    mNamedWindows;          // OpenCV NamedWindows
        Pylon::PylonAutoInitTerm    mAutoInitTerm;
        Pylon::CTlFactory&          mTransportLayerFactory;
        Pylon::DeviceInfoList_t     mDevices;
        Pylon::CInstantCameraArray  mCameras;              
};

#endif // SV_APPLICATION_HPP