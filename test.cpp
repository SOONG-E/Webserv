#include <netinet/in.h>
#include <sys/socket.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "./include/HttpRequest.hpp"
#include "./include/HttpRequestParser.hpp"

using namespace std;

int main(int argc, char *argv[]) {
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    cout << "Failed to create socket" << endl;
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(80);
  server_address.sin_addr.s_addr = INADDR_ANY;
  int bind_result = ::bind(server_socket, (struct sockaddr *)&server_address,
                           sizeof(server_address));
  if (bind_result == -1) {
    cout << "Failed to bind socket" << endl;
    exit(EXIT_FAILURE);
  }

  int listen_result = listen(server_socket, 5);
  if (listen_result == -1) {
    cout << "Failed to listen for incoming connections" << endl;
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in client_address;
  socklen_t client_address_size = sizeof(client_address);
  int client_socket = accept(server_socket, (struct sockaddr *)&client_address,
                             &client_address_size);
  if (client_socket == -1) {
    cout << "Failed to accept incoming connection" << endl;
    exit(EXIT_FAILURE);
  }

  char buffer[1024];
  int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
  if (bytes_received == -1) {
    cout << "Failed to receive data from client" << endl;
    exit(EXIT_FAILURE);
  }

  cout << buffer << endl;

  // // Send response to client
  // const char* response = "Hello from server";
  // int bytes_sent = send(client_socket, response, strlen(response), 0);
  // if (bytes_sent == -1) {
  //     cout << "Failed to send response to client" << endl;
  //     exit(EXIT_FAILURE);
  // }

  // // Close the sockets
  // close(client_socket);
  // close(server_socket);

  return 0;
}