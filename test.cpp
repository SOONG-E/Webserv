#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <string>

int main(int argc, char *argv[]) {
  std::map<std::string, std::string> map;
  map["REQUEST_METHOD"] = "GET";
  map["REQUEST_SCHEME"] = "GET";
  map["CONTENT_TYPE"] = "test/file";
  map["QUERY_STRING"] = "asdfasdf";
  map["SERVER_PROTOCOL"] = "HTTP/1.1";
  map["PATH_INFO"] = "/Users/son-yeong-won/webserv/YoupiBanane/youpi.bla";
  map["REQUEST_URI"] = "/Users/son-yeong-won/webserv/YoupiBanane/youpi.bla";
  map["SCRIPT_NAME"] = "/Users/son-yeong-won/webserv/YoupiBanane/youpi.bla";
  map["CONTENT_LENGTH"] = "8";

  char *pythonPath = "./cgi_tester";
  char *pythonScriptPath = "./YoupiBanane/youpi.bla";
  char *args[] = {pythonPath, pythonScriptPath, NULL};

  char **envp = new char *[map.size() + 1]();
  int i = 0;
  for (std::map<std::string, std::string>::const_iterator it = map.begin();
       it != map.end(); ++it) {
    envp[i] =
        strdup(const_cast<char *>((it->first + "=" + it->second).c_str()));
    ++i;
  }
  envp[i] = NULL;
  int status;
  pid_t pid = fork();
  if (pid == 0) {
    execve(pythonPath, args, envp);
  } else {
    wait(&status);
    std::cout << "\ncgi exit code = " << status << '\n';
  }
  return EXIT_SUCCESS;
}
