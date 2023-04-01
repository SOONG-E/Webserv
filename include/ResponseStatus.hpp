#ifndef RESPONSE_STATUS_HPP_
#define RESPONSE_STATUS_HPP_

#include <map>
#include <string>

enum StatusIndex {
  C200,
  C204,
  C400,
  C404,
  C405,
  C411,
  C413,
  C500,
  C501,
  C505,
};

struct ResponseStatus {
  static const std::string CODES[];
  static const std::string REASONS[];

  ResponseStatus() {
    reason.insert(std::make_pair(C200, REASONS[C200]));
    reason.insert(std::make_pair(C204, REASONS[C204]));
    reason.insert(std::make_pair(C400, REASONS[C400]));
    reason.insert(std::make_pair(C404, REASONS[C404]));
    reason.insert(std::make_pair(C405, REASONS[C405]));
    reason.insert(std::make_pair(C411, REASONS[C411]));
    reason.insert(std::make_pair(C413, REASONS[C413]));
    reason.insert(std::make_pair(C500, REASONS[C500]));
    reason.insert(std::make_pair(C501, REASONS[C501]));
    reason.insert(std::make_pair(C505, REASONS[C505]));
  }

  std::map<int, std::string> reason;
};

#endif
