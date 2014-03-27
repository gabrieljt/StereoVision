#include <SV/Utility.hpp>


// TODO: cross-platform configuration
const int           SV::MAX_NUMBER_OF_CAMERAS = 2;
// Windows: default.pfs ; Linux: default_linux.pfs
const char*         SV::CONFIGURATION_FILE = "Config/default_linux.pfs";    
// TODO: dynamic definition; Last checked 18-03-2014
// Time spent (ticks) in each iteration of the main loop (Camera Capture)
int                 SV::MAIN_LOOP_ITERATION_TIME = 6899925;                 
// Windows: 9018 ; Linux: 8192
const int           SV::INTER_PACKET_DELAY = 8192;                                      
// Windows: 6233 ; Linux: 4096
const int           SV::FRAME_TRANSMISSION_DELAY = 4096 + SV::MAIN_LOOP_ITERATION_TIME; 
const std::string   SV::lineBreak = "================================";
