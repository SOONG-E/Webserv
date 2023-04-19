#include "AutoIndexHandler.hpp"

struct Response AutoIndexHandler::handle(Client *client) {
  struct Response response;

  response.body = generateBody(client);
  response.headers["content-length"] = toString(response.body.size());

  return response;
}

std::string AutoIndexHandler::generateBody(Client *client) {
  std::set<File> entries = getEntries(client->getFullUri());
  return generateList(entries);
}

std::set<File> AutoIndexHandler::getEntries(const std::string &url) {
  DIR *dir = opendir(url.c_str());
  if (!dir) {
    throw std::runtime_error(strerror(errno));
  }
  std::set<File> entries;
  struct dirent *entry;
  while ((entry = readdir(dir))) {
    std::string name = entry->d_name;
    if (name == "." || name == "..") continue;
    struct stat statbuf;
    if (stat((url + name).c_str(), &statbuf) == ERROR<int>()) {
      throw std::runtime_error(strerror(errno));
    }
    File file = {name, statbuf.st_mtime, statbuf.st_size};
    entries.insert(file);
  }
  closedir(dir);

  return entries;
}

std::string AutoIndexHandler::generateList(const std::set<File> &entries) {
  std::string listing = ::readFile(DIRECTORY_LISTING_PAGE);
  std::string delimeter = "</tr>\n";

  std::size_t boundary = listing.find(delimeter);
  if (boundary == std::string::npos) {
    throw std::runtime_error("");  // error handling
  }
  std::string head = listing.substr(0, boundary);
  std::string tail = listing.substr(boundary + delimeter.size());

  for (std::set<File>::const_iterator entry_iter = entries.begin();
       entry_iter != entries.end(); ++entry_iter) {
    head +=
        "      <tr>"
        "        <td>" +
        entry_iter->name +
        "        </td>"
        "        <td>" +
        formatTime("%Y-%m-%d %H:%M:%S", entry_iter->last_modified) +
        "        </td>"
        "        <td>" +
        toString(entry_iter->size) +
        "        </td>"
        "      </tr>";
  }
  head += tail;
  return head;
}
