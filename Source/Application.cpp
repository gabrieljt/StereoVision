#include <SV/Application.hpp>
#include <SV/CameraConfiguration.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <cstring>
#include <cassert>
#include <stdexcept>


namespace
{
    // TODO: cross-platform configuration
    const int NUMBER_OF_CAMERAS = 2;
	const char* CONFIGURATION_FILE = "Config/default_linux.pfs";       // Windows: default.pfs ; Linux: default_linux.pfs
	const int INTER_PACKET_DELAY = 8192;                               // Windows: 9018 ; Linux: 8192
	const int FRAME_TRANSMISSION_DELAY = 4096;                         // Windows: 6233 ; Linux: 4096
    const std::string lineBreak = "================================";    
}

Application::Application()
: mNamedWindows()
, mAutoInitTerm()
, mTransportLayerFactory(Pylon::CTlFactory::GetInstance())
, mDevices()
, mCameras(NUMBER_OF_CAMERAS)
{
	std::cout << "Initializing Stereo Vision..." << std::endl;  

    // Validations
	if (mTransportLayerFactory.EnumerateDevices(mDevices) == 0)
		throw std::runtime_error("Application::Application() - No Camera Devices found");
    assert(mDevices.size() == NUMBER_OF_CAMERAS);

	for (size_t i = 0; i < mDevices.size(); ++i)
	{	
        // Attach device to Pylon's camera array
        mCameras[i].Attach(mTransportLayerFactory.CreateDevice(mDevices[i]));
        Pylon::CInstantCamera &camera = mCameras[i];
        //Register Configuration and Image Capture Events
        camera.RegisterConfiguration(
            new CameraConfiguration(CONFIGURATION_FILE, INTER_PACKET_DELAY, FRAME_TRANSMISSION_DELAY * (int) (i + 1)), 
            Pylon::RegistrationMode_ReplaceAll, 
            Pylon::Cleanup_Delete
        );
		// Create OpenCV NamedWindow
		std::string cameraName("Camera " + std::to_string(i) + ": ");
        cameraName += camera.GetDeviceInfo().GetModelName();
		mNamedWindows.push_back(cameraName);
		cv::namedWindow(cameraName, CV_WINDOW_AUTOSIZE);
		cv::moveWindow(cameraName, 100 * (i + 1), 100 * (i + 1));
	}
    // Triggers Camera Configuration	
    mCameras.Open();
}

void Application::run()
{	
	if (mCameras.IsOpen())
	{
        std::cout << lineBreak << std::endl;         
		std::cout << "Initializing Capture..." << std::endl;
		capture();
	}
    else 
    {
        throw std::runtime_error("Application::run() - Failed to Open Cameras");
    }
	mCameras.Close();
    cv::destroyAllWindows();
}

void Application::capture()
{
	Pylon::CGrabResultPtr grabResult;
	
	mCameras.StartGrabbing();	
    std::cout << "Capture started. Press ESC while focused on any window to exit." << std::endl;
	while(mCameras.IsGrabbing())
	{
		mCameras.RetrieveResult(5000, grabResult, Pylon::TimeoutHandling_ThrowException);
		if (grabResult->GrabSucceeded())
		{        
            // OpenCV image CV_8U: 8-bits, 1 channel
            auto image = cv::Mat(grabResult->GetHeight(), grabResult->GetWidth(), CV_8UC1, grabResult->GetBuffer());
            // Apply BayerGB8 Filter
            cv::cvtColor(image, image, CV_BayerGB2RGB);
            // Display image
		    cv::imshow(mNamedWindows[grabResult->GetCameraContext()], image);
			// Keyboard input break with ESC key
			int key = cv::waitKey(30);
			if((key & 255) == 27)
				break;
		}
	}
	mCameras.StopGrabbing();
}
