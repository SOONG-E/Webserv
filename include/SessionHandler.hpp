#ifndef SESSION_HANDLER_HPP_
#define SESSION_HANDLER_HPP_

#include <map>
#include <vector>

#include "Client.hpp"
#include "Session.hpp"
#include "utility.hpp"

class SessionHandler {
 public:
  typedef std::map<std::string, Session> sessions_mapped_type;
  typedef std::map<int, sessions_mapped_type> sessions_type;

  SessionHandler();
  SessionHandler(const SessionHandler& src);
  SessionHandler& operator=(const SessionHandler& src);
  ~SessionHandler();

  Session* generateSession(Client& client);
  Session* findSession(const Client& client);
  bool isValidSessionID(const Client& client);
  void deleteTimeoutSessions();

 private:
  std::string generateSessionID(int server_block_key);
  bool isDuplicatedId(int server_block_key, const std::string& session_id);
  void deleteSessions(sessions_mapped_type& dest,
                      const std::vector<const std::string*>& delete_sessions);

  sessions_type sessions_;
};

#endif