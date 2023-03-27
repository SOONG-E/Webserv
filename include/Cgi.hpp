#ifndef CGI_HPP
#define CGI_HPP

#include "HttpRequest.hpp"

class Cgi {
 public:
  Cgi();
  Cgi(const Cgi& src);
  Cgi& operator=(const Cgi& src);
  ~Cgi();

 private:
  const HttpRequest&
};

#endif