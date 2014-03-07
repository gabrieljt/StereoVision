#include <SV/Application.hpp>

#include <pylon/FeaturePersistence.h>
#include <GenApi/INodeMap.h>
#include <GenApi/Types.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <cstring>
#include <memory>
#include <stdexcept>


namespace
{
    // TODO: cross-platform configuration
	const char configurationFile[] = "Config/default_linux.pfs";   // Windows: default.pfs ; Linux: default_linux.pfs
	const int INTERPACKET_DELAY = 8192;                            // Windows: 9018 ; Linux: 8192
	const int FRAME_TRANSMISSION_DELAY = 4096;                     // Windows: 6233 ; Linux: 4096
}

Application::Application()
: mNumberOfCameras(2)
, mNamedWindows()
, mAutoInitTerm()
, mTransportLayerFactory(Pylon::CTlFactory::GetInstance())
, mDevices()
, mCameras(mNumberOfCameras)
{
	std::cout << "Initializing Stereo Vision..." << std::endl << std::endl;    

	if (mTransportLayerFactory.EnumerateDevices(mDevices) == 0)
		throw std::runtime_error("Application::Application() - No Camera Devices found");

	for (size_t i = 0; i < mDevices.size(); ++i)
	{	    
		// Attach device to Pylon's camera array; Get camera and it's nodemap pointers for configuration
		mCameras[i].Attach(mTransportLayerFactory.CreateDevice(mDevices[i]));
		Pylon::CInstantCamera &camera = mCameras[i];
		GenApi::INodeMap& nodeMap = camera.GetNodeMap();

		// Create OpenCV NamedWindow; Get camera information
		std::string cameraName("Camera ");
		cameraName += std::to_string(i) + ": ";
		cameraName += camera.GetDeviceInfo().GetModelName();
		mNamedWindows.push_back(cameraName);
		std::cout << "Found " + cameraName << std::endl;		
		cv::namedWindow(cameraName, CV_WINDOW_AUTOSIZE);
		cv::moveWindow(cameraName, (size_t) 100u * (i + (size_t) 1u), (size_t) 100u * (i + (size_t) 1u));
		
		// Load and display default settings
		camera.Open();		
		Pylon::CFeaturePersistence::Load(configurationFile, &nodeMap, true);
		std::cout << "Loaded default configuration for " + cameraName << std::endl;
		std::cout << std::endl;			
		std::cout << "Area Of Interest (AOI) Settings:" << std::endl;
		std::cout << "Width: " << GenApi::CIntegerPtr(nodeMap.GetNode("Width"))->GetValue() << std::endl;
		std::cout << "Height: " << GenApi::CIntegerPtr(nodeMap.GetNode("Height"))->GetValue() << std::endl;
		std::cout << "Offset X: " << GenApi::CIntegerPtr(nodeMap.GetNode("OffsetX"))->GetValue() << std::endl;
		std::cout << "Offset Y: " << GenApi::CIntegerPtr(nodeMap.GetNode("OffsetY"))->GetValue() << std::endl;
		std::cout << std::endl;
		std::cout << "Pixel Format: " << GenApi::CEnumerationPtr(nodeMap.GetNode("PixelFormat"))->ToString() << std::endl;
		std::cout << std::endl;
		std::cout << "Setting Network Parameters..." << std::endl;
		std::cout << "Packet Size: " << GenApi::CIntegerPtr(nodeMap.GetNode("GevSCPSPacketSize"))->GetValue() << std::endl;
		GenApi::CIntegerPtr interpacketDelay(nodeMap.GetNode("GevSCPD"));
		interpacketDelay->SetValue(INTERPACKET_DELAY);
		GenApi::CIntegerPtr frameTransmissionDelay(nodeMap.GetNode("GevSCFTD"));
		frameTransmissionDelay->SetValue((i + (size_t) 1u) * FRAME_TRANSMISSION_DELAY);
		std::cout << "Inter-Packet Delay: " << interpacketDelay->GetValue() << std::endl;
		std::cout << "Frame Transmission Delay: " << frameTransmissionDelay->GetValue() << std::endl;
		std::cout << std::endl;
	}	
}

void Application::run()
{	
	if (mCameras.IsOpen())
	{
		std::cout << "Initializing Capture..." << std::endl;
		capture();
	}
	mCameras.Close();
    cv::destroyAllWindows();
}

void Application::capture()
{
	Pylon::CGrabResultPtr grabResult;
	
	mCameras.StartGrabbing();	
	while(mCameras.IsGrabbing())
	{
		mCameras.RetrieveResult(5000, grabResult, Pylon::TimeoutHandling_ThrowException);
		if (grabResult->GrabSucceeded())
		{        
			auto cameraContextValue = grabResult->GetCameraContext();
			auto imageWidth = grabResult->GetWidth();
			auto imageHeight = grabResult->GetHeight();
			const auto *imageBuffer = (uint8_t *) grabResult->GetBuffer();
			/*
			std::cout << "Camera " <<  cameraContextValue << ": " << mCameras[cameraContextValue].GetDeviceInfo().GetModelName() << std::endl;
			std::cout << "SizeX: " << imageWidth << std::endl;
			std::cout << "SizeY: " << imageHeight << std::endl;
			std::cout << "Gray value of first pixel: " << (uint32_t) imageBuffer[0] << std::endl << std::endl;
			*/
			// OpenCV image
			auto image = cv::Mat(imageHeight, imageWidth, CV_8U);			
			// Copies from buffer into OpenCV image 
			std::memcpy(image.ptr(), imageBuffer, imageWidth * imageHeight); 			
			cv::imshow(mNamedWindows[cameraContextValue], image);
		    
			// Keyboard input break
			int key = cv::waitKey(30);
			if( key == 'q' || key == 'Q' || (key & 255) == 27 )
				break;
		}
	}
	mCameras.StopGrabbing();
}
