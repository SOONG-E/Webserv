#include "HttpServer.hpp"

HttpServer::HttpServer(ServerBlock &server_block)
    : locations_(server_block.locations),
      error_pages_(server_block.error_pages) {}

HttpServer::~HttpServer() {}
