#ifndef SV_UTILITY_HPP
#define SV_UTILITY_HPP


#include <string>


namespace SV
{
	/* Camera Parameters */
    extern const int           	MAX_NUMBER_OF_CAMERAS;
    extern const char*         	CONFIGURATION_FILE;
    extern int                 	MAIN_LOOP_ITERATION_TIME;
    extern const int           	INTER_PACKET_DELAY;
    extern const int           	FRAME_TRANSMISSION_DELAY;

    
    /* Calibration Parameters */
    extern const std::string	CALIBRATION_TIMESTAMP_FILE;
    extern const std::string    CALIBRATION_IMAGES_PATH;
    extern const std::string    CALIBRATION_IMAGE_LEFT;
    extern const std::string    CALIBRATION_IMAGE_RIGHT;
    extern const std::string	NOT_CALIBRATED;


    /* Other Parameters */
    extern const std::string	lineBreak;
    extern const std::string    EMULATED_CAMERA;


    /* Functions */
    std::string                 getTimestamp();
}

#endif // SV_UTILITY_HPP
