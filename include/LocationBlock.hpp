#ifndef LOCATION_BLOCK_HPP_
#define LOCATION_BLOCK_HPP_

#include <set>
#include <string>

class LocationBlock {
  enum Index {
    CLIENT_MAX_BODY_SIZE,
    ROOT,
    AUTOINDEX,
    INDEX,
  };

  static const std::string DEFAULTS[];

 public:
  LocationBlock();
  LocationBlock(const LocationBlock& origin);
  LocationBlock& operator=(const LocationBlock& origin);
  ~LocationBlock();

  static bool isImplementedMethod(const std::string& method);

  const std::string& getUri(void) const;
  std::size_t getBodyLimit(void) const;
  std::set<std::string>& getAllowedMethods(void);
  const std::string& getReturnUrl(void) const;
  const std::string& getRoot(void) const;
  std::set<std::string>& getIndex(void);
  const std::set<std::string>& getIndex(void) const;

  void setUri(const std::string& uri);
  void setBodyLimit(const std::string& raw);
  void addAllowedMethod(const std::string& method);
  void setReturnUrl(const std::string& return_url);
  void setRoot(const std::string& root);
  void setAutoindex(const std::string& autoindex);
  void addIndex(const std::string& index);

  bool isAllowedMethod(const std::string& method) const;
  void clear(void);

 private:
  std::string uri_;
  std::size_t body_limit_;
  std::set<std::string> allowed_methods_;
  std::string return_url_;
  std::string root_;
  std::string autoindex_;
  std::set<std::string> index_;
};

#endif
