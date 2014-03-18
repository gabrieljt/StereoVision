#include <SV/Utility.hpp>


// TODO: cross-platform configuration
const int           SV::MAX_NUMBER_OF_CAMERAS = 2;
const char*         SV::CONFIGURATION_FILE = "Config/default_linux.pfs";    // Windows: default.pfs ; Linux: default_linux.pfs
const int           SV::INTER_PACKET_DELAY = 8192;                          // Windows: 9018 ; Linux: 8192
const int           SV::FRAME_TRANSMISSION_DELAY = 4096;                    // Windows: 6233 ; Linux: 4096
const std::string   SV::lineBreak = "================================";
