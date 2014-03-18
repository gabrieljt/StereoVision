#include <SV/Application.hpp>
#include <SV/CameraConfiguration.hpp>
#include <SV/CameraCapture.hpp>
#include <SV/Utility.hpp>

#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <cassert>
#include <stdexcept>


Application::Application()
: mAutoInitTerm()
, mTransportLayerFactory(Pylon::CTlFactory::GetInstance())
, mDevices()
, mCameras(SV::MAX_NUMBER_OF_CAMERAS)
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
            new CameraConfiguration(SV::CONFIGURATION_FILE, SV::INTER_PACKET_DELAY, SV::FRAME_TRANSMISSION_DELAY * (int) (i + 1), cameraName), 
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

    std::cout << SV::lineBreak << std::endl << "Initializing Capture... Press ESC while focused on any window to exit." << std::endl;
    
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
