#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <string>

int main(int argc, char *argv[]) {
  std::map<std::string, std::string> map;
  map["REQUEST_METHOD"] = "GET";
  map["SERVER_PROTOCOL"] = "HTTP/1.1";
  map["PATH_INFO"] = "/Users/yoson/Desktop/Webserv/cgi-bin/index.php";
  map["REQUEST_URI"] = "/Users/yoson/Desktop/Webserv/cgi-bin/index.php";
  map["SCRIPT_NAME"] = "/Users/yoson/Desktop/Webserv/cgi-bin/index.php";
  map["NAME"] = "asdasd";

  char *pythonPath = "/usr/bin/python3";
  char *pythonScriptPath = "./test.py";
  char *args[] = {pythonPath, pythonScriptPath, "NAME=qwe;FIELD=qqqq", NULL};

  char **envp = new char *[map.size() + 1]();
  int i = 0;
  for (std::map<std::string, std::string>::const_iterator it = map.begin();
       it != map.end(); ++it) {
    envp[i] =
        strdup(const_cast<char *>((it->first + "=" + it->second).c_str()));
    ++i;
  }
  envp[i] = NULL;
  pid_t pid = fork();
  if (pid == 0) {
    std::cout << "FORK" << std::endl;
    execve(pythonPath, args, envp);
  } else {
    std::cout << "qqq" << std::endl;
    wait(0);
  }
  return EXIT_SUCCESS;
}