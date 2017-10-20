#include "SimpleServer.hpp"
#include "util/debug.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>

SimpleServer::SimpleServer(): mFd(0) {
}

SimpleServer::~SimpleServer() {
  if (mFd > 0) {
    ::close(mFd);
  }
}

int
SimpleServer::start(const char* sockname) {
  //int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  int sfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sfd < 0) {
    return sfd;
  }

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(&addr.sun_path[1], sockname, strlen(sockname));

  struct sockaddr_in sin;

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(8899);
  sin.sin_addr.s_addr = htonl(INADDR_ANY);


  if (::bind(sfd, (struct sockaddr*) &sin,
      sizeof(sin)) < 0) {
    ::close(sfd);
  MCINFO("bind error");
    return -1;
  }

  int ret = ::listen(sfd, 1);

  mFd = sfd;

  return mFd;
}

int
SimpleServer::accept() {
  struct sockaddr_un addr;
  socklen_t addr_len = sizeof(addr);
  return ::accept(mFd, (struct sockaddr *) &addr, &addr_len);
}
