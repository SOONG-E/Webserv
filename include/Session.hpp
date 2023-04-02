#ifndef SESSION_HPP_
#define SESSION_HPP_

#include <ctime>
#include <vector>

#include "constant.hpp"

class Session {
 public:
  Session(const std::string& session_id);
  Session(const Session& src);
  ~Session();

  const std::string& getSessionID() const;
  time_t getTimeout() const;
  void setTimeout(std::time_t time = std::time(NULL));
  void addFilename(const std::string& filename);

 private:
  const std::string session_id_;
  time_t timeout_;
  std::vector<std::string> upload_filenames_;
};

#endif