#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <map>
#include <set>
#include <string>
#include <vector>

struct Location {
  std::set<std::string> try_files;
};

class Server {
 public:
  Server();
  Server(const Server& origin);
  Server& operator=(const Server& origin);

  virtual ~Server();

  const std::set<std::string>& getServerNames(void) const;

  void setBodyLimit(const int body_limit);

  void addListen(const std::string& host, const int port);
  void addServerName(const std::string& name);
  void addErrorPage(const int code, const std::string& page);
  void addLocation(const Location& location);

 private:
  std::map<std::string, int> listens_;
  std::set<std::string> server_names_;
  std::map<int, std::string> error_pages_;
  int body_limit_;
  std::vector<Location> locations_;
};

#endif
