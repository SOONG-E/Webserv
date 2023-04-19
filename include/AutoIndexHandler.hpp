#ifndef AUTO_INDEX_HANDLER_HPP_
#define AUTO_INDEX_HANDLER_HPP_

#include <dirent.h>
#include <sys/stat.h>

#include "Client.hpp"
#include "File.hpp"

class AutoIndexHandler {
 public:
  static struct Response handle(Client *client);

 private:
  AutoIndexHandler(){};
  ~AutoIndexHandler(){};

  static std::string generateBody(Client *client);
  static std::set<File> getEntries(const std::string &url);
  static std::string generateList(const std::set<File> &entries);
};

#endif