#include <SV/Application.hpp>

#include <stdexcept>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

void usage()
{
    std::cerr << "-u    >> prints usage" << std::endl;
    std::cerr << "-c    >> [C]alibrate cameras with default values" << std::endl;
    std::cerr << "-n N  >> [N]umber of stereo photos (5 <= N <= 50)" << std::endl;
    std::cerr << "-w W  >> [W]idth of chessboard corners (W >= 2)" << std::endl;
}

int main(int argc, char** argv)
{
	int exitCode = 0, option;
    unsigned int n = 20, w = 9, h = 6;
    float s = 2.5, d = 3.5;
    bool c, defaultValues = false;

    argc > 1 ? c = false : c = true;

    opterr = 0;
    while ((option = getopt(argc, argv, "ucnwhsd:")) != -1)
    {
        switch (option)
        {
            case 'u':
                usage();
                return 0;
            case 'c':
                defaultValues = true;
                break;
            case 'n':
                n = (unsigned int) optarg;
                break;
            case 'w':
                w = (unsigned int) optarg;
                break;
            case 'h':
                h = (unsigned int) optarg;
                break;
            case 's':
                s = atof(optarg);
                break;
            case 'd':
                d = atof(optarg);
                break;
            case '?':
                usage();
                return 1;
            default:
                abort();
        }

        if (defaultValues)
        {
            n = 20u;    // number of photos used in calibration
            w = 9u;     // chessboard corners width
            h = 6u;     // chessboard corners height
            s = 2.5f;   // chessboard square size (centimeters)
            d = 3.5f;   // delay after taking photo (seconds)
        }            
        else if ((n < 5u || n > 50u) || (w < 2u) || (h < 2u || h == w) || (s < 2.f) || (d < 3.f || d > 60.f)) 
        {
            usage();
            return 1;
        }
    }

    try
    {
        Application::CalibrationParameters calibrationParameters(c, n, w, h, s, d);
        Application app(calibrationParameters);
        app.run();
    }
    catch (std::exception& e)
    {
        std::cout << "EXCEPTION: " << e.what() << std::endl;
		exitCode = 1;
    }

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
