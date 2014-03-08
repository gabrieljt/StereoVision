#include <SV/Application.hpp>
#include <SV/CameraConfiguration.hpp>
#include <SV/CameraCapture.hpp>

#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <cassert>
#include <stdexcept>


namespace
{
    // TODO: cross-platform configuration
    const int MAX_NUMBER_OF_CAMERAS = 2;
    const char* CONFIGURATION_FILE = "Config/default_linux.pfs";       // Windows: default.pfs ; Linux: default_linux.pfs
    const int INTER_PACKET_DELAY = 8192;                               // Windows: 9018 ; Linux: 8192
    const int FRAME_TRANSMISSION_DELAY = 4096;                         // Windows: 6233 ; Linux: 4096
    const std::string lineBreak = "================================";    
}

Application::Application()
: mAutoInitTerm()
, mTransportLayerFactory(Pylon::CTlFactory::GetInstance())
, mDevices()
, mCameras(MAX_NUMBER_OF_CAMERAS)
{
    std::cout << "Initializing Stereo Vision..." << std::endl;  

    // Validations
    if (mTransportLayerFactory.EnumerateDevices(mDevices) == 0)
        throw std::runtime_error("Application::Application() - No Camera Devices found");

    for (size_t i = 0; i < mDevices.size(); ++i)
    {       
        // Attach device to Pylon's camera array
        mCameras[i].Attach(mTransportLayerFactory.CreateDevice(mDevices[i]));
        Pylon::CInstantCamera &camera = mCameras[i];
        // Define Camera Name for logging and OpenCV NamedWindow
        std::string cameraName("Camera " + std::to_string(i) + ": ");
        cameraName += camera.GetDeviceInfo().GetModelName();
        // Register Camera's Configuration and Capture Events
        camera.RegisterConfiguration(
            new CameraConfiguration(CONFIGURATION_FILE, INTER_PACKET_DELAY, FRAME_TRANSMISSION_DELAY * (int) (i + 1), cameraName), 
            Pylon::RegistrationMode_ReplaceAll, 
            Pylon::Cleanup_Delete
        );
        camera.RegisterImageEventHandler(
            new CameraCapture(cameraName),
            Pylon::RegistrationMode_Append,
            Pylon::Cleanup_Delete
        );
    }
    // Triggers Configuration Event
    mCameras.Open();
}

void Application::run()
{       
    if (mCameras.IsOpen())            
        capture();    
    else     
        throw std::runtime_error("Application::run() - Failed to Open Cameras");    
    mCameras.Close();
}

void Application::capture()
{
    Pylon::CGrabResultPtr grabResultPtr;

    std::cout << lineBreak << std::endl << "Initializing Capture... Press ESC while focused on any window to exit." << std::endl;
    
    mCameras.StartGrabbing();       
    while(mCameras.IsGrabbing())
    {
        // Triggers Capture Event
        mCameras.RetrieveResult(5000, grabResultPtr, Pylon::TimeoutHandling_ThrowException);
    
        // Keyboard input break with ESC key
        int key = cv::waitKey(30);
        if((key & 255) == 27)
            break;        
    }
    mCameras.StopGrabbing();
}
