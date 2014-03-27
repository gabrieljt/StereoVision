#include <SV/Application.hpp>
#include <SV/CameraCalibration.hpp>
#include <SV/CameraCapture.hpp>
#include <SV/CameraConfiguration.hpp>
#include <SV/Utility.hpp>

#include <opencv2/highgui/highgui.hpp>

#include <cstdio>
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
    printf("Initializing Stereo Vision...\n");  
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
    printf("%sInitializing Calibration...\n", SV::lineBreak.c_str());
    Pylon::CGrabResultPtr grabResultPtr;
    unsigned int n, w, h, grabCount;
    float s, d;
    do
    {
        printf("Please enter the following parameters in a single line separated by blank spaces:\n\
            [N]umber of stereo photos: 5 <= N <= 50\n\
            [W]idth of chessboard corners: W >= 2\n\
            [H]eight of chessboard corners: H >= 2; H != W\n\
            [S]ize of chessboard square in milimiters: S >= 2.0\n\
            [D]elay between stereo photos capture in seconds: 3.0 <= D <= 60.0\n\
            Sample input: 20 9 6 2.3 3.0\n\
            Your Input: ");
        scanf("%u %u %u %f %f", &n, &w, &h, &s, &d);
    } 
    while ((n < 5u || n > 50u) || (w < 2u) || (h < 2u || h == w) || (s < 2.f) || (d < 3.f || d > 60.f));
    printf("N = %u\nW = %u\nH = %u\nS = %f\nD = %f\n", n, w, h, s, d);

    d *= 1000.f;
    n *= 2u;
    grabCount = 0u;
    mCameras.StartGrabbing();       
    while (mCameras.IsGrabbing() && grabCount < n)
    {
        if (grabCount % 2u == 0u)
        {
            // Keyboard input break with ESC key
            int key = cv::waitKey(d);
            if((key & 255) == 27)
                break;        
        }

        // Triggers Calibration Event
        mCameras.RetrieveResult(5000, grabResultPtr, Pylon::TimeoutHandling_ThrowException);
        ++grabCount;
    }
    mCameras.StopGrabbing();
    auto startTime = cv::getTickCount();
    // TODO: perform calibration on saved pair of photos
    std::string timestamp = SV::getTimestamp();
    std::ofstream timestampFile(SV::CALIBRATION_TIMESTAMP_FILE, std::ofstream::out);
    if (timestampFile.is_open())
    {
        timestampFile << timestamp;
        timestampFile.close();
    }
    auto finishTime = (cv::getTickCount() - startTime) / cv::getTickFrequency();
    printf("Calibration completed in %f seconds at %s.\n", finishTime, timestamp.c_str());    

    char option;
    do 
    {
        printf("Would you like to start Capturing? [Y]es / [N]o: ");
        std::cin >> option;
    }
    while (option != 'Y' || option != 'y' || option != 'N' || option != 'n');

    if (option == 'Y' || option == 'y')
    {
        // Register Capture Event for cameras
        for (size_t i = 0; i < mDevices.size(); ++i)
        {
            mCameras[i].RegisterImageEventHandler
            (
                new CameraCapture(mCameraNames[i]),
                Pylon::RegistrationMode_ReplaceAll,
                Pylon::Cleanup_Delete
            );
        }
        capture();
    }
}

void Application::capture()
{
    printf("%sInitializing Capture... Press ESC while focused on any window to exit.\n", SV::lineBreak.c_str());
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
            printf("Spent %f seconds in the last iteration.\n", finishTime);
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
        printf("Cameras have never been calibrated. Scheduling calibration...\n");
    else
    {
        char option;
        do 
        {
            printf("Last calibration performed at %s. Would you like to calibrate again? [Y]es / [N]o: ", timestamp.c_str());
            std::cin >> option;
        }
        while (option != 'Y' || option != 'y' || option != 'N' || option != 'n');
    
        if (option == 'Y' || option == 'y')
            printf("Scheduling calibration...\n");            
        else
        {
            printf("Using calibration performed at %s.\n", timestamp.c_str());
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
        // Register Camera's Configuration and Capture Events
        if (cameraModel != SV::EMULATED_CAMERA)            
        {
            camera.RegisterConfiguration
            (
                new CameraConfiguration(SV::CONFIGURATION_FILE, SV::INTER_PACKET_DELAY, SV::FRAME_TRANSMISSION_DELAY * (int) (i + 1), cameraName), 
                Pylon::RegistrationMode_ReplaceAll, 
                Pylon::Cleanup_Delete
            );        
            
            mCalibrated ?
                camera.RegisterImageEventHandler
                (
                    new CameraCapture(cameraName),
                    Pylon::RegistrationMode_Append,
                    Pylon::Cleanup_Delete
                )
            :
                camera.RegisterImageEventHandler
                (
                    new CameraCalibration(cameraName),
                    Pylon::RegistrationMode_Append,
                    Pylon::Cleanup_Delete
                )
            ;
        }
    }
}
