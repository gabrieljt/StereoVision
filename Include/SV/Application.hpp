#ifndef SV_APPLICATION_HPP
#define SV_APPLICATION_HPP

#include <string>


class Application
{
    public:
                Application(const std::string leftCameraWindow, const std::string rightCameraWindow);
        void    run();

    private:
        void    update();
        void    render();

    private:
    	const std::string mLeftCameraWindow;
    	const std::string mRightCameraWindow;
};

#endif // SV_APPLICATION_HPP
