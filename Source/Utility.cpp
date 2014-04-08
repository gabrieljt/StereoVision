#include <SV/Utility.hpp>

#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


// TODO: cross-platform configuration
/* Camera Parameters */
const int           SV::MAX_NUMBER_OF_CAMERAS = 2;
// Windows: default.pfs ; Linux: default_linux.pfs
const char*         SV::CONFIGURATION_FILE = "Config/Camera/default_linux_lowres.pfs";    
// TODO: dynamic definition; Last checked 18-03-2014
// Time spent (ticks) in each iteration of the main loop (Camera Capture)
int                 SV::MAIN_LOOP_ITERATION_TIME = 0;//6899925;                 
// Windows: 9018 ; Linux: 8192
const int           SV::INTER_PACKET_DELAY = 8192;                                      
// Windows: 6233 ; Linux: 4096
const int           SV::FRAME_TRANSMISSION_DELAY = 4096 + SV::MAIN_LOOP_ITERATION_TIME; 


/* Calibration Parameters */
const std::string   SV::CALIBRATION_BIN = "StereoCalibration";
const std::string	SV::CALIBRATION_TIMESTAMP_FILE = "Config/Calibration/timestamp.txt";
const std::string   SV::CALIBRATION_PATTERN_FILE = "Config/Calibration/pattern.txt";
const std::string   SV::CALIBRATION_XML_FILES_PATH = "Config/Calibration/XMLFiles/";
const std::string   SV::CALIBRATION_IMAGES_FILE = "Config/Calibration/list.txt";
const std::string	SV::CALIBRATION_IMAGES_PATH = "Config/Calibration/Images/";
const std::string	SV::CALIBRATION_IMAGE_LEFT = "left.ppm";
const std::string	SV::CALIBRATION_IMAGE_RIGHT = "right.ppm";
const std::string	SV::NOT_CALIBRATED = "NOT_CALIBRATED";


/* Emulation Parameters */
bool                SV::EMULATION_MODE = false;
const std::string   SV::EMULATED_CAMERA = "Emulation";
const std::string   SV::EMULATED_IMAGES_FILE = "Config/Calibration/list_emulation.txt";
const std::string   SV::EMULATED_IMAGES_PATH = SV::CALIBRATION_IMAGES_PATH + "Emulation/";


/* Other Parameters */
const std::string   SV::lineBreak = "================================\n";


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
	std::string timestamp("1");
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

SV::CalibrationPattern SV::loadCalibrationPatternFile()
{
    unsigned int w, h;
    float s;
    CalibrationPattern calibrationPattern(0u, 0u, 0.f);

    std::ifstream patternFile(SV::CALIBRATION_PATTERN_FILE, std::ifstream::in);
    if (patternFile.is_open())
    {
        patternFile >> calibrationPattern.w;
        patternFile >> calibrationPattern.h;
        patternFile >> calibrationPattern.s;
        patternFile.close();
    }        

    return calibrationPattern;
}

void SV::saveCalibrationPatternFile(unsigned int w, unsigned int h, float s)
{
    CalibrationPattern calibrationPattern(w, h, s);
    std::ofstream patternFile(SV::CALIBRATION_PATTERN_FILE, std::ofstream::out);
    if (patternFile.is_open())
    {
        patternFile << calibrationPattern.w << std::endl;
        patternFile << calibrationPattern.h << std::endl;
        patternFile << calibrationPattern.s << std::endl;
        patternFile.close();
    }
}

int SV::forkExecStereoCalibrationModule(unsigned int w, unsigned int h, float s)
{
    pid_t result, stereoCalibrationPID;
    int status;
    std::string width = std::to_string(w);
    std::string height = std::to_string(h);
    std::string size = std::to_string(s);

    result = fork();

    if (result == -1) 
    {
        std::cout << "SV::forkExecStereoCalibrationModule() ERROR: fork() status " << result << std::endl;
        return result;
    }

    /* Son Process - StereoCalibration */
    if (result == 0)
    {         
        std::cout << "Executing Stereo Calibration Module..." << std::endl;
        result = execl(SV::CALIBRATION_BIN.c_str(), SV::CALIBRATION_BIN.c_str(), 
            SV::CALIBRATION_IMAGES_FILE.c_str(), width.c_str(), height.c_str(), size.c_str(), 
            SV::CALIBRATION_XML_FILES_PATH.c_str(), NULL);
        std::cout << "SV::forkExecStereoCalibrationModule() ERROR: execl() status " << result << std::endl;
        if (result == -1)
            exit(0);
    }
    /* Father Process - StereoVision */
    else
    {         
        stereoCalibrationPID = wait(&status);
    }

    return 0;
}

cv::Scalar SV::openCVRandomColor(cv::RNG& rng)
{
    int color = (unsigned) rng;
    return cv::Scalar(color&255, (color >> 8)&255, (color >> 16)&255);
}
