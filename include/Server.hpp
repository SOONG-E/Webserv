class Server {
 public:
  Server();
  Server(const Server& origin);
  virtual ~Server();

  Server& operator=(const Server& origin);

 private:
};
