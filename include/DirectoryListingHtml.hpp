#ifndef DIRECTORY_LISTING_HPP_
#define DIRECTORY_LISTING_HPP_

#include <set>
#include <string>

#include "File.hpp"

struct DirectoryListingHtml {
  static std::string generate(const std::string& url,
                              const std::set<File>& entries);
  static std::string head(const std::string& url);
  static std::string body(const std::string& url,
                          const std::set<File>& entries);
};

#endif
