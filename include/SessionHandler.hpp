#ifndef SESSION_HANDLER_HPP_
#define SESSION_HANDLER_HPP_

#include <map>
#include <queue>

#include "Client.hpp"
#include "Session.hpp"

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
  void deleteTimeoutSessions();

 private:
  std::string generateSessionID(int server_block_key);
  bool isDuplicatedId(int server_block_key, const std::string& session_id);
  void deleteSessions(sessions_mapped_type& sessions,
                      std::queue<std::string>& delete_sessions);

  sessions_type sessions_;
};

#endif