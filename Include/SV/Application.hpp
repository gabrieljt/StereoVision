#ifndef SV_APPLICATION_HPP
#define SV_APPLICATION_HPP

#include <SV/Utility.hpp>

#include <pylon/TlFactory.h>
#include <pylon/InstantCameraArray.h>

#include <vector>
#include <string>
#include <fstream>


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
        void                        registerCameraCalibration(bool* synchronizedPtr, unsigned int* grabCountPtr, std::ofstream* imageListFilePtr, std::pair<bool, bool>* wroteToFilePairPtr);
        void                        registerCameraCapture(SV::StereoPhoto* stereoPhotoPtr);
        

    private:
        Pylon::PylonAutoInitTerm    mAutoInitTerm;
        Pylon::CTlFactory&          mTransportLayerFactory;
        Pylon::DeviceInfoList_t     mDevices;
        Pylon::CInstantCameraArray  mCameras;   
        std::vector<std::string>    mCameraNames; 
        bool                        mCalibrated;
};

#endif // SV_APPLICATION_HPP
