#ifndef SV_APPLICATION_HPP
#define SV_APPLICATION_HPP


#include <pylon/TlFactory.h>
#include <pylon/InstantCameraArray.h>

#include <vector>
#include <string>


class Application
{
    public:
                                    Application();
        void                        run();


    private:
        void                        calibrate();
        void                        capture();
        void                        scheduleCalibration();
        void                        attachDevices();
        void                        registerImageEventHandlers();
        

    private:
        Pylon::PylonAutoInitTerm    mAutoInitTerm;
        Pylon::CTlFactory&          mTransportLayerFactory;
        Pylon::DeviceInfoList_t     mDevices;
        Pylon::CInstantCameraArray  mCameras;   
        std::vector<std::string>    mCameraNames; 
        bool                        mCalibrated;
};

#endif // SV_APPLICATION_HPP
