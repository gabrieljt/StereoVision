#include <SV/Utility.hpp>

#include <iostream>
#include <fstream>
#include <ctime>


// TODO: cross-platform configuration
/* Camera Parameters */
const int           SV::MAX_NUMBER_OF_CAMERAS = 2;
// Windows: default.pfs ; Linux: default_linux.pfs
const char*         SV::CONFIGURATION_FILE = "Config/Camera/default_linux.pfs";    
// TODO: dynamic definition; Last checked 18-03-2014
// Time spent (ticks) in each iteration of the main loop (Camera Capture)
int                 SV::MAIN_LOOP_ITERATION_TIME = 6899925;                 
// Windows: 9018 ; Linux: 8192
const int           SV::INTER_PACKET_DELAY = 8192;                                      
// Windows: 6233 ; Linux: 4096
const int           SV::FRAME_TRANSMISSION_DELAY = 4096 + SV::MAIN_LOOP_ITERATION_TIME; 


/* Calibration Parameters */
const std::string	SV::CALIBRATION_TIMESTAMP_FILE = "Config/Calibration/timestamp.txt";
const std::string   SV::CALIBRATION_IMAGES_FILE = "Modules/StereoCalibration/list.txt";
const std::string	SV::CALIBRATION_IMAGES_PATH = "Modules/StereoCalibration/Images/";
const std::string   SV::CALIBRATION_IMAGES_RELATIVE_PATH = "../Images/";
const std::string	SV::CALIBRATION_IMAGE_LEFT = "left.ppm";
const std::string	SV::CALIBRATION_IMAGE_RIGHT = "right.ppm";
const std::string	SV::NOT_CALIBRATED = "NOT_CALIBRATED";


/* Other Parameters */
const std::string   SV::lineBreak = "================================\n";
const std::string	SV::EMULATED_CAMERA = "Emulation";
const std::string   SV::EMULATED_IMAGE = SV::CALIBRATION_IMAGES_PATH + "checkerboard.png";


/* Functions */
std::string SV::getTimestamp()
{
	time_t rawtime;
	struct tm * timeinfo;

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	return asctime(timeinfo);
}

std::string SV::loadCalibrationTimestampFile()
{
	std::string timestamp;
    std::ifstream timestampFile(SV::CALIBRATION_TIMESTAMP_FILE, std::ifstream::in);
    if (timestampFile.is_open())
    {
        std::getline(timestampFile, timestamp);
        timestampFile.close();
    }    
    
    return timestamp;
}

void SV::saveCalibrationTimestampFile()
{
	std::string timestamp = SV::getTimestamp();
    std::ofstream timestampFile(SV::CALIBRATION_TIMESTAMP_FILE, std::ofstream::out);
    if (timestampFile.is_open())
    {
        timestampFile << timestamp;
        timestampFile.close();
    }
}
