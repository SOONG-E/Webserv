#ifndef LOCATION_BLOCK_HPP_
#define LOCATION_BLOCK_HPP_

#include <map>
#include <set>
#include <string>
#include <vector>

class Location {
  enum Index {
    CLIENT_MAX_BODY_SIZE,
    ROOT,
    AUTOINDEX,
    INDEX,
  };

  static const std::string DEFAULTS[];

 public:
  Location();
  Location(const Location& origin);
  Location& operator=(const Location& origin);
  ~Location();

  static bool isImplementedMethod(const std::string& method);

  const std::string& getUri(void) const;
  std::size_t getBodyLimit(void) const;
  std::set<std::string>& getAllowedMethods(void);
  const std::set<std::string>& getAllowedMethods(void) const;
  const std::string& getReturnUrl(void) const;
  const std::string& getRoot(void) const;
  bool getAutoindex(void) const;
  std::vector<std::string>& getIndex(void);
  const std::vector<std::string>& getIndex(void) const;
  std::string getCgiParam(const std::string& key) const;

  void setUri(const std::string& uri);
  void setBodyLimit(const std::string& raw);
  void addAllowedMethod(const std::string& method);
  void setReturnUrl(const std::string& return_url);
  void setRoot(const std::string& root);
  void setAutoindex(const std::string& raw);
  void addIndex(const std::string& index);
  void addCgiParam(const std::string& key, const std::string& value);

  bool isAllowedMethod(const std::string& method) const;
  bool isCgi(void);
  void clear(void);

 private:
  std::string uri_;
  std::size_t body_limit_;
  std::set<std::string> allowed_methods_;
  std::string return_url_;
  std::string root_;
  bool autoindex_;
  std::vector<std::string> index_;
  std::map<std::string, std::string> cgi_param_;
  bool is_cgi_;
};

#endif
