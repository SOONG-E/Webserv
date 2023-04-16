#include "ServerManager.hpp"

ServerManager::ServerManager(const Config &config) { registerServer(config); };

ServerManager::~ServerManager() {}

/*======================//
 initialize server
/*======================*/

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
  return new TcpServer(key);
}

/* create new HttpServer and append it to http_servers list */
HttpServer *ServerManager::createHttpServer(const ServerBlock &server_block) {
  HttpServer *http_server = new HttpServer(server_block);
  http_servers_.push_back(http_server);

  return http_server;
}

/*======================//
 set server
/*======================*/

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
  }
}

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

struct addrinfo *ServerManager::getAddrInfo(const std::string ip,
                                            const std::string port) {
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
/*======================*/

ServerManager::acceptNewClient(const int fd) {
  sockaddr client_addr;
  socklen_t client_addrlen;

  int client_fd = ::accept(fd, &client_addr, &client_addrlen);
  if (client_fd == ERROR<int>()) {
    throw std::runtime_error(strerror(errno));
  }

  if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == ERROR<int>()) {
    close(client_fd);
    throw std::runtime_error(strerror(errno));
  }
}
