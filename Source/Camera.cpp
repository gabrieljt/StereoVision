#include <SV/Camera.hpp>

// TODO: generic class
#include <pylon/PylonIncludes.h>

Camera::Camera(const std::string name)
: mName(name)
, mFrame()
{    
}

void Camera::capture()
{

}

cv::Mat Camera::getFrame()
{
    return mFrame;
}
