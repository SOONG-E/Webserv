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

 private:
  std::string listen_;
  std::set<std::string> server_names_;
  std::map<std::string, std::string> error_pages_;
  std::string body_limit_;
  std::vector<Location> locations_;
};

#endif
