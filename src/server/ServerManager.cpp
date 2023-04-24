#include "ServerManager.hpp"

ServerManager::ServerManager(const Config &config) : kq_(kqueue()) {
  registerServer(config);
  if (kq_ == -1) {
    throw std::runtime_error(strerror(errno));
  }
};

ServerManager::~ServerManager() {}

/*======================//
 initialize server
========================*/

/* register TcpServer, HttpServer */
void ServerManager::registerServer(const Config &config) {
  std::vector<ServerBlock> servers = config.getServerBlocks();
  std::vector<Listen> listens;
  TcpServer *tcp_server;
  HttpServer *http_server;

  for (std::vector<ServerBlock>::iterator it = servers.begin();
       it != servers.end(); ++it) {
    listens = it->listens;
    http_server = createHttpServer(*it);

    for (std::vector<Listen>::iterator lit = listens.begin();
         lit != listens.end(); ++lit) {
      tcp_server = seekTcpServer(lit->server_socket_key);
      tcp_server->appendServer(*it, http_server);
    }
  }
}

/* seek the TcpServer using key and return
unless there is TcpServer look forward, then create new on and return */
TcpServer *ServerManager::seekTcpServer(const std::string &key) {
  TcpServerType::iterator tcp_server = tcp_servers_.find(key);

  if (tcp_server == tcp_servers_.end()) {
    return createTcpServer(key);
  }
  return tcp_server->second;
}

/* create new TcpServer */
TcpServer *ServerManager::createTcpServer(const std::string &key) {
  TcpServer *new_server = new TcpServer(key);
  tcp_servers_[key] = new_server;
  return new_server;
}

/* create new HttpServer and append it to http_servers list */
HttpServer *ServerManager::createHttpServer(const ServerBlock &server_block) {
  HttpServer *http_server = new HttpServer(server_block);
  http_servers_.push_back(http_server);

  return http_server;
}

/*======================//
 set server
========================*/

void ServerManager::setServer(void) { bindServers(); }

/* bind each server to listen socket,
append the listen socket on chanege list */
void ServerManager::bindServers(void) {
  int fd;
  struct addrinfo *addr_info;

  for (TcpServerType::iterator it = tcp_servers_.begin();
       it != tcp_servers_.end(); ++it) {
    fd = createListenSocket();
    listen_sockets_.insert(fd);
    addr_info = getAddrInfo(it->second->getIp(), it->second->getPort());
    if (::bind(fd, addr_info->ai_addr, addr_info->ai_addrlen) == -1) {
      throw std::runtime_error(strerror(errno));
    }
    freeaddrinfo(addr_info);
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
      throw std::runtime_error(strerror(errno));
    }
    if (listen(fd, 128) == -1) {
      throw std::runtime_error(strerror(errno));
    }
    createListenEvent(fd, it->second);
  }
}

/* create and set a listen socket for each server */
int ServerManager::createListenSocket(void) const {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    throw std::runtime_error(strerror(errno));
  }

  const int enable = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
    throw std::runtime_error(strerror(errno));
  }
  return fd;
}

/* get addrinfo using information of server */
struct addrinfo *ServerManager::getAddrInfo(const std::string &ip,
                                            const std::string &port) {
  struct addrinfo hints, *addr_info;

  std::memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  int result = getaddrinfo(ip.c_str(), port.c_str(), &hints, &addr_info);
  if (result != 0) {
    throw std::runtime_error(gai_strerror(result));
  }

  return addr_info;
}

/*======================//
 server run
========================*/

/* run Server with main loop */
void ServerManager::runServer(void) {
  int events;

  while (true) {
    events = kevent(kq_, &change_event_list[0], change_event_list.size(),
                    event_list, CAPABLE_EVENT_SIZE, 0);  // timeout 추가할 지
    if (events == -1) {
      throw std::runtime_error(strerror(errno));
    }
    change_event_list.clear();
    processEventOnQueue(events);
  }
}

/* recognize where is event occurred */
void ServerManager::processEventOnQueue(const int events) {
  struct kevent event;
  Client *client;

  for (int i = 0; i < events; ++i) {
    event = event_list[i];
    if (listen_sockets_.find(event.ident) != listen_sockets_.end()) {
      acceptNewClient(event.ident, static_cast<TcpServer *>(event.udata));
      continue;
    }
    client = static_cast<Client *>(event.udata);
    try {
      client->processEvent(event.filter);
    } catch (const ConnectionClosedException &e) {
      unconnectClient(e.client_fd);
    } catch (const std::runtime_error &e) {
      continue;
    }
  }
}

/* accept client, create Client instance with fd, tcp server */
void ServerManager::acceptNewClient(const int server_socker,
                                    const TcpServer *tcp_server) {
  sockaddr client_addr;
  socklen_t client_addrlen;

  int client_fd = ::accept(server_socker, &client_addr, &client_addrlen);
  if (client_fd == -1) {
    throw std::runtime_error(strerror(errno));
  }

  if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
    close(client_fd);
    throw std::runtime_error(strerror(errno));
  }
  createClient(client_fd, tcp_server,
               SocketAddress(client_addr, client_addrlen));
}

/* create Client instance, register event and append client in client list*/
void ServerManager::createClient(const int client_fd,
                                 const TcpServer *tcp_server,
                                 const SocketAddress socket_address) {
  Client *new_client = new Client(client_fd, tcp_server, socket_address, this);
  createEvent(client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, new_client);
  clients_[client_fd] = new_client;
}

void ServerManager::unconnectClient(const int client_fd) {
  close(client_fd);
  clients_.find(client_fd)->second->clear();
  clients_.erase(client_fd);
}

/*======================//
 manage event
========================*/

/* EV_SET interface */
void ServerManager::createEvent(uintptr_t ident, int16_t filter, uint16_t flags,
                                uint32_t fflags, intptr_t data, void *udata) {
  struct kevent event;

  EV_SET(&event, ident, filter, flags, fflags, data, udata);
  change_event_list.push_back(event);
}

void ServerManager::createListenEvent(int fd, TcpServer *server) {
  createEvent(fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, server);
}
