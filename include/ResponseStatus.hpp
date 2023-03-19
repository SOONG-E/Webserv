#ifndef RESPONSE_STATUS_HPP_
#define RESPONSE_STATUS_HPP_

#include <string>

struct ResponseStatus {
  enum Index {
    C200,
    C400,
    C404,
    C411,
    C413,
    C505,
  };

  static const std::size_t CODES[];
  static const std::size_t REASONS[];
};

const std::string ResponseStatus::CODES[] = {"200", "400", "404",
                                             "411", "413", "505"};
const std::string ResponseStatus::REASONS[] = {"OK",
                                               "Bad Request",
                                               "Not Found",
                                               "Length Required",
                                               "Payload Too Large",
                                               "HTTP Version Not Supported"};

#endif
