#ifndef SV_APPLICATION_HPP
#define SV_APPLICATION_HPP


#include <pylon/TlFactory.h>
#include <pylon/InstantCameraArray.h>


class Application
{
    public:
                                    Application();
        void                        run();

    private:
        void                        capture();

    private:
        Pylon::PylonAutoInitTerm    mAutoInitTerm;
        Pylon::CTlFactory&          mTransportLayerFactory;
        Pylon::DeviceInfoList_t     mDevices;
        Pylon::CInstantCameraArray  mCameras;              
};

#endif // SV_APPLICATION_HPP
