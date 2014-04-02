#ifndef SV_APPLICATION_HPP
#define SV_APPLICATION_HPP


#include <pylon/TlFactory.h>
#include <pylon/InstantCameraArray.h>

#include <vector>
#include <string>


// Forward declaration
namespace SV
{
    struct CalibrationPattern;
}

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
        void                        registerCameraCalibration(unsigned int* grabCountPtr, std::ofstream* imageListFilePtr);
        void                        registerCameraCapture();
        

    private:
        Pylon::PylonAutoInitTerm    mAutoInitTerm;
        Pylon::CTlFactory&          mTransportLayerFactory;
        Pylon::DeviceInfoList_t     mDevices;
        Pylon::CInstantCameraArray  mCameras;   
        std::vector<std::string>    mCameraNames; 
        bool                        mCalibrated;
};

#endif // SV_APPLICATION_HPP
