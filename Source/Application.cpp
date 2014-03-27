#include <SV/Application.hpp>
#include <SV/CameraCalibration.hpp>
#include <SV/CameraCapture.hpp>
#include <SV/CameraConfiguration.hpp>
#include <SV/Utility.hpp>

#include <opencv2/highgui/highgui.hpp>

#include <memory>
#include <iostream>
#include <cassert>
#include <stdexcept>


Application::Application()
: mAutoInitTerm()
, mTransportLayerFactory(Pylon::CTlFactory::GetInstance())
, mDevices()
, mCameras(SV::MAX_NUMBER_OF_CAMERAS)
, mCameraNames()
, mCalibrated(false)
{
    std::cout << "Initializing Stereo Vision..." << std::endl;
    scheduleCalibration();
    attachDevices();    
    // Triggers Configuration Event (CameraConfiguration.cpp)
    mCameras.Open();
}

void Application::run()
{       
    if (mCameras.IsOpen())            
        mCalibrated ? capture() : calibrate();
    else     
        throw std::runtime_error("Application::run() - Failed to Open Cameras");    
    mCameras.Close();
}

void Application::calibrate()
{
    std::cout << SV::lineBreak << "Initializing Stereo Calibration Procedure..." << std::endl;
    Pylon::CGrabResultPtr grabResultPtr;
    unsigned int n, w, h;
    float s, d;
    do
    {
        std::cout << "Please enter the following parameters:";
        std::cout << "[N]umber of stereo photos (5 <= N <= 50): ";
        std::cin >> n;
        std::cout << "[W]idth of chessboard corners (W >= 2): ";
        std::cin >> w;
        std::cout << "[H]eight of chessboard corners (H >= 2 & H != W): ";
        std::cin >> h;
        std::cout << "[S]ize of chessboard square in milimiters (S >= 2.0): ";
        std::cin >> s;
        std::cout << "[D]elay between stereo photos capture in seconds (3.0 <= D <= 60.0): ";
        std::cin >> d;
    } 
    while ((n < 5u || n > 50u) || (w < 2u) || (h < 2u || h == w) || (s < 2.f) || (d < 3.f || d > 60.f));
    std::cout << "N = " << n << std::endl;
    std::cout << "W = " << w << std::endl;
    std::cout << "H = " << h << std::endl;
    std::cout << "S = " << s << std::endl;
    std::cout << "D = " << d << std::endl;

    d *= 1000.f;
    std::unique_ptr<unsigned int> grabCountPtr(new unsigned int);
    auto grabCount = grabCountPtr.get();
    *grabCount = 0u;
    registerCameraCalibration(grabCount);    
    std::cout << "Prepare to Capture Images for Calibration!" << std::endl;
    // Cameras Synchronization: Round-Robin Strategy
    mCameras.StartGrabbing(Pylon::GrabStrategy_UpcomingImage);       
    while (mCameras.IsGrabbing() && *grabCount < n)
    {            
        // Triggers Calibration Event
        mCameras.RetrieveResult(Pylon::INFINITE, grabResultPtr, Pylon::TimeoutHandling_ThrowException);
    }
    mCameras.StopGrabbing();
    std::cout << "Stereo Photos Captured: " << *grabCount << "/" << n << std::endl;    
    
    std::cout << std::endl << "Calibrating Cameras..." << std::endl;
    auto startTime = cv::getTickCount();
    // TODO: perform calibration on saved stereo photos (fork exec?)
    auto finishTime = (cv::getTickCount() - startTime) / cv::getTickFrequency();

    SV::saveCalibrationTimestampFile();    
    mCalibrated = true;
    std::cout << "Calibration completed in " << finishTime << " seconds at " << SV::loadCalibrationTimestampFile() << std::endl;

    char option;
    bool selected;
    do 
    {
        std::cout << "Would you like to start Capturing? [y]es | [n]o: ";
        std::cin >> option;
        option == 'y' || option == 'n' ? selected = true : selected = false;
    }
    while (!selected);

    if (option == 'y')    
        capture();    
}

void Application::capture()
{
    std::cout << SV::lineBreak << "Initializing Capture... Press ESC while focused on any window to exit." << std::endl;
    registerCameraCapture();    
    Pylon::CGrabResultPtr grabResultPtr;
    
    mCameras.StartGrabbing();       
    while(mCameras.IsGrabbing())
    {
        auto startTime = cv::getTickCount();
        // Triggers Capture Event
        mCameras.RetrieveResult(5000, grabResultPtr, Pylon::TimeoutHandling_ThrowException);
    
        // Keyboard input break with ESC key
        int key = cv::waitKey(30);
        if((key & 255) == 27)
        {
            /*
            The time spent in each iteration of the main loop must be added in the FTD of each camera.
            Doing this results in less FPS, but avoid delays when displaying the captured image.
            This is the main trade-off: the more processing, the less frame per seconds.
            Check Utility files for more information.
            */
            auto finishTime = (cv::getTickCount() - startTime) / cv::getTickFrequency();
            std::cout << "Spent " << finishTime << " seconds in the last iteration." << std::endl;
            break;        
        }
    }
    mCameras.StopGrabbing();
}

void Application::scheduleCalibration()
{
    std::string timestamp = SV::loadCalibrationTimestampFile();

    if (timestamp == SV::NOT_CALIBRATED || timestamp == "")
        std::cout << "Cameras have never been calibrated. Scheduling calibration..." << std::endl;
    else
    {
        char option;
        bool selected;
        do 
        {
            std::cout << "Last calibration performed at " << timestamp << " . Would you like to calibrate again? [y]es | [n]o: ";
            std::cin >> option;
            option == 'y' || option == 'n' ? selected = true : selected = false;
        }
        while (!selected);
    
        if (option == 'y')
            std::cout << "Scheduling calibration..." << std::endl;
        else
        {
            std::cout << "Using calibration performed at " << timestamp << std::endl;
            mCalibrated = true;
        }
    }
}

void Application::attachDevices()
{
    if (mTransportLayerFactory.EnumerateDevices(mDevices) == 0)
        throw std::runtime_error("Application::Application() - No Camera Devices found");    

    for (size_t i = 0; i < mDevices.size(); ++i)
    {
        std::string cameraName, cameraModel;
        // Attach device to Pylon's camera array
        mCameras[i].Attach(mTransportLayerFactory.CreateDevice(mDevices[i]));
        Pylon::CInstantCamera &camera = mCameras[i];
        // Define Camera Name for logging and OpenCV NamedWindow
        i % 2 == 0 ? cameraName = "Left Camera" : cameraName = "Right Camera";
        cameraModel += camera.GetDeviceInfo().GetModelName();        
        mCameraNames.push_back(cameraName);
        // Register Camera's Configuration
        if (cameraModel != SV::EMULATED_CAMERA)
        {
            camera.RegisterConfiguration
            (
                new CameraConfiguration(SV::CONFIGURATION_FILE, SV::INTER_PACKET_DELAY, SV::FRAME_TRANSMISSION_DELAY * (int) (i + 1), cameraName), 
                Pylon::RegistrationMode_ReplaceAll, 
                Pylon::Cleanup_Delete
            );           
        }
    }    
}

void Application::registerCameraCalibration(unsigned int* grabCount)
{
    for (size_t i = 0; i < mDevices.size(); ++i)
    {
        mCameras[i].RegisterImageEventHandler
        (
            new CameraCalibration(mCameraNames[i], grabCount),
            Pylon::RegistrationMode_ReplaceAll,
            Pylon::Cleanup_Delete
        );
    }
}

void Application::registerCameraCapture()
{
    for (size_t i = 0; i < mDevices.size(); ++i)
    {
        mCameras[i].RegisterImageEventHandler
        (
            new CameraCapture(mCameraNames[i]),
            Pylon::RegistrationMode_ReplaceAll,
            Pylon::Cleanup_Delete
        );
    }
}
