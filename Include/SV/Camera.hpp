#ifndef SV_CAMERA_HPP
#define SV_CAMERA_HPP

#include <opencv2/core/core.hpp>

#include <string>


class Camera
{
	public:
                            Camera(const std::string name);
        void                capture();
        cv::Mat             getFrame();

    private:
        const std::string   mName;
        cv::Mat             mFrame;
};

#endif // SV_CAMERA_HPP
