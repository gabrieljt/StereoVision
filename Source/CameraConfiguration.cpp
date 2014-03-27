#include <SV/CameraConfiguration.hpp>
#include <SV/Utility.hpp>

#include <pylon/InstantCamera.h>
#include <pylon/FeaturePersistence.h>
#include <GenApi/INodeMap.h>
#include <GenApi/Types.h>

#include <iostream>


CameraConfiguration::CameraConfiguration(const char* configurationFile, const int interPacketDelay, int frameTransmissionDelay, std::string cameraName)
: mConfigurationFile(configurationFile)     
, mInterPacketDelay(interPacketDelay)                                   
, mFrameTransmissionDelay(frameTransmissionDelay)    
, mCameraName(cameraName)                         
{
}

void CameraConfiguration::OnOpened(Pylon::CInstantCamera& camera)
{
    GenApi::INodeMap& nodeMap = camera.GetNodeMap();
    Pylon::CFeaturePersistence::Load(mConfigurationFile, &nodeMap, true);

    std::cout << SV::lineBreak << "Attached and Opened " << mCameraName << std::endl;
    std::cout << "Loaded default configurations for " << camera.GetDeviceInfo().GetModelName() << std::endl;

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
    interpacketDelay->SetValue(mInterPacketDelay);
    GenApi::CIntegerPtr frameTransmissionDelay(nodeMap.GetNode("GevSCFTD"));
    frameTransmissionDelay->SetValue(mFrameTransmissionDelay);
    std::cout << "Inter-Packet Delay: " << interpacketDelay->GetValue() << std::endl;
    std::cout << "Frame Transmission Delay: " << frameTransmissionDelay->GetValue() << std::endl;
}

void CameraConfiguration::OnGrabStarted(Pylon::CInstantCamera& camera)
{    
    std::cout << mCameraName << " is Capturing." << std::endl;
}
