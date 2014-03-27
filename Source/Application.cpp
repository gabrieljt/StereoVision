#include <SV/Application.hpp>
#include <SV/CameraCalibration.hpp>
#include <SV/CameraCapture.hpp>
#include <SV/CameraConfiguration.hpp>
#include <SV/Utility.hpp>

#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <fstream>
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
    registerImageEventHandlers();
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
    std::cout << SV::lineBreak << "Initializing Calibration..." << std::endl;
    Pylon::CGrabResultPtr grabResultPtr;
    unsigned int n, w, h, grabCount;
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
    n *= 2u;
    grabCount = 0u;
    std::cout << "Prepare to Capture Images for Calibration!" << std::endl;
    mCameras.StartGrabbing();       
    while (mCameras.IsGrabbing() && grabCount < n)
    {
        if (grabCount % 2u == 0u)
            cv::waitKey(d);
            
        // Triggers Calibration Event
        mCameras.RetrieveResult(5000, grabResultPtr, Pylon::TimeoutHandling_ThrowException);
        ++grabCount;
    }
    mCameras.StopGrabbing();

    std::cout << std::endl << "Calibrating Cameras..." << std::endl;
    auto startTime = cv::getTickCount();
    // TODO: perform calibration on saved stereo photos
    std::string timestamp = SV::getTimestamp();
    std::ofstream timestampFile(SV::CALIBRATION_TIMESTAMP_FILE, std::ofstream::out);
    if (timestampFile.is_open())
    {
        timestampFile << timestamp;
        timestampFile.close();
    }
    auto finishTime = (cv::getTickCount() - startTime) / cv::getTickFrequency();
    mCalibrated = true;
    std::cout << "Calibration completed in " << finishTime << " at " << timestamp << std::endl;

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
    {
        registerImageEventHandlers();
        capture();
    }
}

void Application::capture()
{
    std::cout << SV::lineBreak << "Initializing Capture... Press ESC while focused on any window to exit." << std::endl;
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
    std::string timestamp;
    std::ifstream timestampFile(SV::CALIBRATION_TIMESTAMP_FILE, std::ifstream::in);
    if (timestampFile.is_open())
    {
        std::getline(timestampFile, timestamp);
        timestampFile.close();
    }    

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
        cameraName = "Camera " + std::to_string(i) + ": ";
        cameraModel += camera.GetDeviceInfo().GetModelName();
        cameraName += cameraModel;
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

void Application::registerImageEventHandlers()
{
    for (size_t i = 0; i < mDevices.size(); ++i)
    {
        mCalibrated ?
            mCameras[i].RegisterImageEventHandler
            (
                new CameraCapture(mCameraNames[i]),
                Pylon::RegistrationMode_ReplaceAll,
                Pylon::Cleanup_Delete
            )
        :
            mCameras[i].RegisterImageEventHandler
            (
                new CameraCalibration(mCameraNames[i]),
                Pylon::RegistrationMode_ReplaceAll,
                Pylon::Cleanup_Delete
            )
        ; 
    }
}
