#include <SV/Application.hpp>
#include <SV/CameraCalibration.hpp>
#include <SV/CameraCapture.hpp>
#include <SV/CameraConfiguration.hpp>

#include <opencv2/highgui/highgui.hpp>

#include <memory>
#include <utility>
#include <cassert>
#include <iostream>
#include <stdexcept>

Application::Application(CalibrationParameters calibrationParameters)
: mAutoInitTerm()
, mTransportLayerFactory(Pylon::CTlFactory::GetInstance())
, mDevices()
, mCameras(SV::MAX_NUMBER_OF_CAMERAS)
, mCameraNames()
, mCalibrationParameters(calibrationParameters)
{
    scheduleCalibration();
    attachDevices();    
    // Triggers Configuration Event (CameraConfiguration.cpp)
    mCameras.Open();
}

void Application::run()
{       
    if (mCameras.IsOpen())            
        mCalibrationParameters.calibrated ? capture() : calibrate();
    else     
        throw std::runtime_error("Application::run() - Failed to Open Cameras");    
    mCameras.Close();
}

void Application::calibrate()
{
    Pylon::CGrabResultPtr grabResultPtr;
    SV::saveCalibrationPatternFile(mCalibrationParameters.width, mCalibrationParameters.height, mCalibrationParameters.size);    

    // Setup Variables and Pointers shared between Cameras    
    float d = mCalibrationParameters.delay * 1000.f;
    std::unique_ptr<bool> synchronizedPtr(new bool);
    auto synchronized = synchronizedPtr.get();
    *synchronized = false;
    std::unique_ptr<unsigned int> grabCountPtr(new unsigned int);
    auto grabCount = grabCountPtr.get();
    *grabCount = 0u;
    auto currentGrabCount = 0u;    
    std::unique_ptr<std::ofstream> imageListFilePtr(new std::ofstream(SV::CALIBRATION_IMAGES_FILE, std::ofstream::out));
    auto imageListFile = imageListFilePtr.get();
    std::unique_ptr<std::pair<bool, bool>> wroteToFilePairPtr(new std::pair<bool, bool>);
    auto wroteToFilePair = wroteToFilePairPtr.get();
    wroteToFilePair->first = false;
    wroteToFilePair->second = false;

    std::cout << "Prepare to Capture Images for Calibration!" << std::endl;
    registerCameraCalibration(synchronized, grabCount, imageListFile, wroteToFilePair);    
    
    // Cameras Synchronization: Round-Robin Strategy
    mCameras.StartGrabbing(Pylon::GrabStrategy_UpcomingImage);       
    while (mCameras.IsGrabbing() && *grabCount < mCalibrationParameters.numberPhotos)
    {            
        // Triggers Calibration Event
        mCameras.RetrieveResult(Pylon::INFINITE, grabResultPtr, Pylon::TimeoutHandling_ThrowException);

        if (*grabCount > currentGrabCount)
        {
            currentGrabCount = *grabCount;
            cv::waitKey(d);
        }
        else
            cv::waitKey(30);
    }
    mCameras.StopGrabbing();
    imageListFile->close();
    std::cout << "Stereo Photos Captured: " << *grabCount << "/" << mCalibrationParameters.numberPhotos << std::endl;        

    // Start Stereo Calibration Module
    auto startTime = cv::getTickCount();
    SV::forkExecStereoCalibrationModule(mCalibrationParameters.width, mCalibrationParameters.height, mCalibrationParameters.size);
    auto finishTime = (cv::getTickCount() - startTime) / cv::getTickFrequency();

    SV::saveCalibrationTimestampFile();    
    mCalibrationParameters.calibrated = true;
    std::cout << "Stereo Calibration completed in " << finishTime << " seconds at " << SV::loadCalibrationTimestampFile() << std::endl;

    capture();    
}

void Application::capture()
{
    std::cout << SV::lineBreak << "Initializing Capture. Press ESC while focused on any window to exit." << std::endl;
    std::unique_ptr<SV::StereoPhoto> stereoPhotoPtr(new SV::StereoPhoto);
    auto stereoPhoto = stereoPhotoPtr.get();
    
    registerCameraCapture(stereoPhoto);    
    Pylon::CGrabResultPtr grabResultPtr;    
    
    // Cameras Synchronization: Round-Robin Strategy
    mCameras.StartGrabbing(Pylon::GrabStrategy_UpcomingImage);       
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
    {
        std::cout << "Cameras have never been calibrated. Scheduling calibration..." << std::endl;
        mCalibrationParameters.calibrated = false;
    }
    else if (!mCalibrationParameters.calibrated)
    {
        std::cout << "Overriding last calibration performed at " << timestamp << std::endl;
    }
    else
    {
        std::cout << "Using calibration performed at " << timestamp << std::endl;
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
        else
            SV::EMULATION_MODE = true;
    }    
}

void Application::registerCameraCalibration(bool* synchronizedPtr, unsigned int* grabCountPtr, std::ofstream* imageListFilePtr, std::pair<bool, bool>* wroteToFilePairPtr)
{
    for (size_t i = 0; i < mDevices.size(); ++i)
    {
        mCameras[i].RegisterImageEventHandler
        (
            new CameraCalibration(mCameraNames[i], synchronizedPtr, grabCountPtr, imageListFilePtr, wroteToFilePairPtr),
            Pylon::RegistrationMode_ReplaceAll,
            Pylon::Cleanup_Delete
        );
    }
}

void Application::registerCameraCapture(SV::StereoPhoto* stereoPhotoPtr)
{
    for (size_t i = 0; i < mDevices.size(); ++i)
    {
        stereoPhotoPtr->cameras[i] = mCameraNames[i];
        mCameras[i].RegisterImageEventHandler
        (
            new CameraCapture(mCameraNames[i], stereoPhotoPtr),
            Pylon::RegistrationMode_ReplaceAll,
            Pylon::Cleanup_Delete
        );
    }
}
