#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <map>
#include <set>
#include <string>
#include <vector>

class Server {
 public:
  struct Location;

  Server();
  Server(const Server& origin);
  Server& operator=(const Server& origin);
  virtual ~Server();

  void addListen(const std::string& raw);
  void addServerName(const std::string& name);
  void addErrorPage(const int code, const std::string& page);
  void setBodyLimit(const std::string& body_limit);
  void addLocation(const Location& location);

  void init(void);
  std::set<std::string> keys(void) const;

 private:
  struct Listen;

  std::vector<Listen> listens_;
  std::set<std::string> server_names_;
  std::map<int, std::string> error_pages_;
  std::string body_limit_;
  std::vector<Location> locations_;
};

struct Server::Listen {
  explicit Listen(const std::string& raw);

  std::string listen;
  std::string host;
  int port;
};

struct Server::Location {
  std::set<std::string> try_files;
};

#endif
