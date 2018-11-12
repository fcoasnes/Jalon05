#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <fcntl.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "fctsocket.h"



void error(const char *msg)
{
  perror(msg);
  exit(1);
}

struct sockaddr_in get_addr_info(const char* address, int port)
{
  struct sockaddr_in hints;
  memset(&hints,0,sizeof(hints));
  hints.sin_family=AF_INET;
  hints.sin_port=htons(port);
  inet_aton(address, &hints.sin_addr);
  return hints;
}

int do_listen(int sock, int a)
{
  int l =listen(sock,a);
  if (l == -1){
    perror("listen");
    exit (EXIT_FAILURE);
  }
  return l;
}


int do_accept(int sock, struct sockaddr *addr, socklen_t *addrlen)
{
  int s2 =accept(sock,addr,addrlen);
  if (s2 == -1){
    perror("accept");
    exit (EXIT_FAILURE);
  }
  return s2;
}



int do_read(int socket, void *str, size_t L)
{
  int r= read(socket, str, L);
  if(r== -1 ){
    perror("read");
    exit(EXIT_FAILURE);
  }
  return r;
}



int do_write(int socket, void *addr, size_t L)
{
  int w=write(socket, addr, L);
  if (w == -1){
    perror("write");
    exit(EXIT_FAILURE);
  }
  return w;
}

int do_bind(int sock, const struct sockaddr *adr, int adrlen)
{
  int b = bind(sock, adr , adrlen);
  if (b == -1){
    perror("bind");
    exit (EXIT_FAILURE);
  }
  return b;
}



int do_socket(int domain, int type, int protocol)
{
  int sockfd;
  int yes = 1;
  //create the socket
  sockfd = socket(domain,type,protocol);
  //check for socket validity
  if (sockfd == -1)
  {
    perror("Socket");
    exit(EXIT_FAILURE);
  }
    // set socket option, to prevent "already in use" issue when rebooting the server right on
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    perror("ERROR setting socket options");
    return sockfd;
  }

  void do_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
  {
    printf("Recherche une connection...\n\n");
    // Connection
    int res = connect(sockfd, addr, addrlen);
    if (res != 0) {
      perror("error connection");
      exit(EXIT_FAILURE);
    }
    printf("Connection réussi!\n \n");
  }
