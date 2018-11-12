#ifndef FCTSOCKET_
#define FCTSOCKET_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

struct sockaddr_in get_addr_info(const char* address, int port);
int do_listen(int sock, int a);
int do_accept(int sock, struct sockaddr *addr, socklen_t *addrlen);
int do_read(int socket, void *str, size_t L);
int do_write(int socket, void *addr, size_t L);
int do_socket(int domain, int type, int protocol);
void error(const char *msg);
int do_bind(int sock, const struct sockaddr *adr, int adrlen);
void do_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

#endif
