#include <map>
#include <string>

#include "Server.hpp"

class Configuration {
 public:
  Configuration();
  Configuration(const Configuration& origin);
  virtual ~Configuration();

  Configuration& operator=(const Configuration& origin);

 private:
  std::map<std::string, Server> servers;
};
