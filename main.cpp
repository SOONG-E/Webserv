#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdlib>
#include <iostream>

int displayError(const std::string& err_msg) {
  std::cerr << err_msg << '\n';
  return EXIT_FAILURE;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    return displayError("Invalid argument");
  }
  struct addrinfo hints, *addr_info;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  int gai_result = getaddrinfo(NULL, "80", &hints, &addr_info);
  if (gai_result != 0) return displayError(gai_strerror(gai_result));
  int socket_fd = socket(addr_info->ai_family, addr_info->ai_socktype,
                         addr_info->ai_protocol);
  if (socket_fd == -1) {
    return displayError(strerror(errno));
  }
  if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1)
    return displayError(strerror(errno));
  if (bind(socket_fd, addr_info->ai_addr, addr_info->ai_addrlen) == -1)
    return displayError(strerror(errno));
  if (listen(socket_fd, 128) == -1) return displayError(strerror(errno));

  return EXIT_SUCCESS;
}