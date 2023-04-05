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
  std::string session_key = client.getSessionKey();
  std::string session_id = generateSessionID(server_block_key);

  sessions_[server_block_key].insert(
      std::make_pair(session_key, Session(session_id, client)));
  return &sessions_[server_block_key].find(session_key)->second;
}

Session* SessionHandler::findSession(const Client& client) {
  int server_block_key = client.getServerBlockKey();
  std::string session_key = client.getSessionKey();

  sessions_mapped_type& sessions = sessions_[server_block_key];
  sessions_mapped_type::iterator session_iter = sessions.find(session_key);
  std::string request_session_id =
      client.getRequestObj().getCookie("Session-ID");

  if (session_iter == sessions.end()) {
    if (!request_session_id.empty()) {
      deleteSession(sessions, request_session_id);
    }
    return NULL;
  }

  Session& session = session_iter->second;
  if (session.getID() != request_session_id) {
    deleteSession(sessions, request_session_id);
    sessions.erase(session_key);
    return NULL;
  }

  return &session;
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
    sessions_mapped_type& sessions,
    const std::vector<const std::string*>& delete_sessions) {
  for (std::size_t i = 0; i < delete_sessions.size(); ++i) {
    sessions.erase(*delete_sessions[i]);
  }
}

void SessionHandler::deleteSession(sessions_mapped_type& sessions,
                                   const std::string& session_id) {
  for (sessions_mapped_type::iterator it = sessions.begin();
       it != sessions.end(); ++it) {
    if (it->second.getID() == session_id) {
      const std::string& session_key = it->first;
      sessions.erase(session_key);
      return;
    }
  }
}