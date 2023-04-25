#ifndef PROCESS_HPP_
#define PROCESS_HPP_

#include <string>

struct Process {
  Process() : phase(0){};

  int phase;

  int pid;
  int input_fd;
  int output_fd;

  std::string message_to_send;
  std::string message_received;
};

#endif