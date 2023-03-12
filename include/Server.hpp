#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <set>
#include <string>

class Server {
 public:
  Server();
  Server(const Server& origin);
  virtual ~Server();

  Server& operator=(const Server& origin);

 private:
  std::set<int> ports_;
  std::set<std::string> hosts_;
  std::set<std::string> server_names_;
  std::string error_page_;
  std::size_t body_limit_;
};

#endif
