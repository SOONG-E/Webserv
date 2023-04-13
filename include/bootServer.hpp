#ifndef BOOT_SERVER_HPP_
#define BOOT_SERVER_HPP_

#include <iostream>

#include "Config.hpp"
#include "ConfigParser.hpp"
#include "Error.hpp"
#include "ServerManager.hpp"
#include "utility.hpp"

void checkArgs(int argc);
void printLogo();
Config createConfig(int argc, char** argv);
ServerManager setServer(const Config& config);

#endif