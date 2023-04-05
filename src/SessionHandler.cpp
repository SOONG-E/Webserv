#include "SessionHandler.hpp"

#include <cstdlib>
#include <ctime>

SessionHandler::SessionHandler() { std::srand(std::time(NULL)); }

SessionHandler::SessionHandler(const SessionHandler& src)
    : sessions_(src.sessions_) {}
SessionHandler& SessionHandler::operator=(const SessionHandler& src) {
  if (this != &src) {
    sessions_ = src.sessions_;
  }
  return *this;
}

SessionHandler::~SessionHandler() {}

Session* SessionHandler::generateSession(Client& client) {
  int server_block_key = client.getServerBlockKey();
  std::string session_id = generateSessionID(server_block_key);
  std::string session_key = session_id + ":" +
                            client.getClientAddress().getIP() + ":" +
                            client.getRequestObj().getHeader("USER-AGENT");

  sessions_[server_block_key].insert(
      std::make_pair(session_key, Session(session_id, client)));
  return &sessions_[server_block_key].find(session_key)->second;
}

Session* SessionHandler::findSession(const Client& client) {
  int server_block_key = client.getServerBlockKey();
  std::string session_key = client.getSessionKey();

  return &sessions_[server_block_key].find(session_key)->second;
}

bool SessionHandler::isValidSessionID(const Client& client) {
  int server_block_key = client.getServerBlockKey();
  std::string session_key = client.getSessionKey();

  try {
    sessions_[server_block_key].at(session_key);
  } catch (const std::out_of_range& e) {
    return false;
  }
  return true;
}

void SessionHandler::deleteTimeoutSessions() {
  Session* session;
  std::vector<const std::string*> delete_sessions;

  for (sessions_type::iterator sessions_it = sessions_.begin();
       sessions_it != sessions_.end(); ++sessions_it) {
    delete_sessions.reserve(sessions_it->second.size());

    for (sessions_mapped_type::iterator mapped_it = sessions_it->second.begin();
         mapped_it != sessions_it->second.end(); ++mapped_it) {
      session = &mapped_it->second;
      if (session->getTimeout() < std::time(NULL)) {
        delete_sessions.push_back(&mapped_it->first);
        std::string path = "rm -rf ./upload_file/" +
                           toString(sessions_it->first) + "/" +
                           session->getID();
        std::system(path.c_str());
        session->getClient().getResponseObj().setSession(NULL);
      }
    }
    if (!delete_sessions.empty()) {
      deleteSessions(sessions_it->second, delete_sessions);
    }
    delete_sessions.clear();
  }
}

std::string SessionHandler::generateSessionID(int server_block_key) {
  std::string session_id;

  do {
    session_id = toString(std::rand());
  } while (isDuplicatedId(server_block_key, session_id));
  return session_id;
}

bool SessionHandler::isDuplicatedId(int server_block_key,
                                    const std::string& session_id) {
  for (sessions_mapped_type::iterator it = sessions_[server_block_key].begin();
       it != sessions_[server_block_key].end(); ++it) {
    if (it->second.getID() == session_id) {
      return true;
    }
  }
  return false;
}

void SessionHandler::deleteSessions(
    sessions_mapped_type& dest,
    const std::vector<const std::string*>& delete_sessions) {
  for (std::size_t i = 0; i < delete_sessions.size(); ++i) {
    dest.erase(*delete_sessions[i]);
  }
}