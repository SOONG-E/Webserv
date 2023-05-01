#ifndef CGI_HANDLER_HPP_
#define CGI_HANDLER_HPP_

#include <signal.h>

#include <cstdlib>

#include "Client.hpp"

enum PipeFD { READ = 0, WRITE = 1 };
enum Phase { P_UNSTARTED = 0, P_WRITE, P_WAIT, P_READ, P_DONE, P_RESET };

class CgiHandler {
 public:
  static void execute(Client *client);
  static void handle(Client *client, int event_type);
  static struct Response getResponse(Client *client);

  static void setPhase(Client *client, int phase);

 private:
  CgiHandler(){};
  ~CgiHandler(){};

  static void sendToCgi(Client *client);
  static void readFromCgi(Client *client);

  static std::map<std::string, std::string> generateHeader(
      const std::string &headers);

  static void setTimer(Client *client);

  static char **generateEnvp(const Client *client);

  static void deleteEnvp(char **envp);
  static std::string getAbsolutePath(const std::string &uri);
  static void cleanUp(Client *client);
};

#endif