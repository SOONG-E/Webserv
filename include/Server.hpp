#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "Listen.hpp"
#include "Location.hpp"

class Server {
 public:
  Server();
  Server(const Server& origin);
  Server& operator=(const Server& origin);
  virtual ~Server();

  std::vector<Listen*>& getListens(void);

  void addListen(const std::string& raw);
  void addServerName(const std::string& name);
  void addErrorPage(const std::string& code, const std::string& page);
  void setBodyLimit(const std::string& body_limit);
  void addLocation(Location* location);

  std::set<std::string> keys(void) const;
  void print(const int index) const;

 private:
  std::vector<Listen*> listens_;
  std::set<std::string> server_names_;
  std::map<std::string, std::string> error_pages_;
  std::string body_limit_;
  std::vector<Location*> locations_;
};

#endif
