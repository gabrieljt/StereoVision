#include <SV/Application.hpp>

#include <stdexcept>
#include <iostream>


int main(int argc, char* argv[])
{
	int exitCode = 0;
    unsigned int n, w, h;
    float s, d;
    bool c;

    // Calibration Parameters default values
    c = true;  // are cameras calibrated?
    n = 20u;    // number of photos used in calibration
    w = 9u;     // chessboard corners width
    h = 6u;     // chessboard corners height
    s = 2.5f;   // chessboard square size (centimeters)
    d = 3.5f;   // delay after taking photo (seconds)
    // TODO: parse command line args    
    Application::CalibrationParameters calibrationParameters(c, n, w, h, s, d);

    try
    {
        Application app(calibrationParameters);
        app.run();
    }
    catch (std::exception& e)
    {
        std::cout << "EXCEPTION: " << e.what() << std::endl;
		exitCode = 1;
        std::cout << std::endl << "Press Enter key to exit.";
        std::cin.get();
    }
    std::cout << "Bye!" << std::endl;

	return exitCode;
}

/*
TODO: -h and parse parameters, this is just a reference and will be removed.
do
{
    std::cout << "Please enter the following parameters:";
    std::cout << "[N]umber of stereo photos (5 <= N <= 50): ";
    std::cin >> n;
    std::cout << "[W]idth of chessboard corners (W >= 2): ";
    std::cin >> w;
    std::cout << "[H]eight of chessboard corners (H >= 2 & H != W): ";
    std::cin >> h;
    std::cout << "[S]ize of chessboard square in centimeters (S >= 2.0): ";
    std::cin >> s;
    std::cout << "[D]elay between stereo photos capture in seconds (3.0 <= D <= 60.0): ";
    std::cin >> d;
} 
while ((n < 5u || n > 50u) || (w < 2u) || (h < 2u || h == w) || (s < 2.f) || (d < 3.f || d > 60.f));        
*/