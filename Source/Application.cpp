#include <SV/Application.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <memory>
#include <stdexcept>


Application::Application()
: mNumberOfCameras(2)
, mNamedWindows()
, mAutoInitTerm()
, mTransportLayerFactory(Pylon::CTlFactory::GetInstance())
, mDevices()
, mCameras(mNumberOfCameras)
{
    std::cout << "Initializing Stereo Vision..." << std::endl;    

    if (mTransportLayerFactory.EnumerateDevices(mDevices) == 0)
        throw std::runtime_error("Application::Application() - No Camera Devices found");

    for (size_t i = 0; i < mDevices.size(); ++i)
    {
        // Attach Device to Pylon's Camera Array
        mCameras[i].Attach(mTransportLayerFactory.CreateDevice(mDevices[i]));

        // Create OpenCV NamedWindow
        std::string cameraName("Camera ");
        cameraName += std::to_string(i) + ": ";
        cameraName += mCameras[i].GetDeviceInfo().GetModelName();
        mNamedWindows.push_back(cameraName);
        cv::namedWindow(cameraName, CV_WINDOW_AUTOSIZE);
        cv::moveWindow(cameraName, 100u * (i + 1u), 100u * (i + 1u));        
    }	
}

void Application::run()
{
    std::cout << "Press any key to start video capture." << std::endl;
    cv::waitKey(0);    
    capture();
    render();
}

void Application::capture()
{
    mCameras.StartGrabbing();
    Pylon::CGrabResultPtr ptrGrabResult;
    int imagesToCapture = 10;

    for(int i = 0; i < imagesToCapture && mCameras.IsGrabbing(); ++i)
    {
        mCameras.RetrieveResult(5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);
        auto cameraContextValue = ptrGrabResult->GetCameraContext();
        auto imageWidth = ptrGrabResult->GetWidth();
        auto imageHeight = ptrGrabResult->GetHeight();
        const auto *imageBuffer = (uint8_t *) ptrGrabResult->GetBuffer();

        std::cout << "Camera " <<  cameraContextValue << ": " << mCameras[cameraContextValue].GetDeviceInfo().GetModelName() << std::endl;
        // Now, the image data can be processed.
        std::cout << "GrabSucceeded: " << ptrGrabResult->GrabSucceeded() << std::endl;
        std::cout << "SizeX: " << imageWidth << std::endl;
        std::cout << "SizeY: " << imageHeight << std::endl;
        std::cout << "Gray value of first pixel: " << (uint32_t) imageBuffer[0] << std::endl << std::endl;

        // Instead do this
        auto img = cv::Mat(imageHeight, imageWidth, CV_8UC1); // manages it's own memory

        // copies from Result.Buffer into img 
        //memcpy(img.ptr(),ptrGrabResult->GetBuffer(),ptrGrabResult->GetWidth()*ptrGrabResult->GetHeight()); 
       /* for (int irow = 0; irow < imageHeight; ++irow) {
            memcpy(img.ptr(irow),imageBuffer + (irow * imageWidth), imageWidth);
        }
        cv::imshow(mNamedWindows[cameraContextValue], img);
        cv::waitKey(0);*/
    }
}

void Application::render()
{
}
