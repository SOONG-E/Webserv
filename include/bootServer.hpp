#ifndef BOOT_SERVER_HPP_
#define BOOT_SERVER_HPP_

#include <iostream>

#include "Config.hpp"
#include "ConfigParser.hpp"
#include "Error.hpp"
#include "ServerManager.hpp"
#include "utility.hpp"

ServerManager setServer(int argc, char** argv);
void runServer(ServerManager manager);

#endif