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
#include "rcvd_mode.h"



void receive_file(char *pseudo)
{
  int boucle=0;
  sleep(1);
  struct sockaddr_in sock_host;
  char info[1000];
  char answer[100];
  char file_to_save[10000];
  sock_host=get_addr_info("127.0.0.6", atoi("8088"));

  int s = do_socket(AF_INET, SOCK_STREAM, 0);
  do_connect(s, (const struct sockaddr*)&sock_host, sizeof(struct sockaddr));
  fflush(stdout);
  nfds_t nfds=2;
  struct pollfd *fds = malloc(20*sizeof(struct pollfd));
  memset(fds,0,sizeof(fds));

  fds[0].fd = s;
  fds[0].events = POLLIN;
  fds[1].fd = STDIN_FILENO;
  fds[1].events = POLLIN;

  while(boucle==0)
  {
    memset(info,0,1000);
    memset(answer,0,100);
    int p=poll(fds, nfds, -1);

    if (fds[0].revents == POLLIN)
    {
      do_read(fds[0].fd,info,1000);
      printf("%s",info);
    }

    if (fds[1].revents == POLLIN)
    {
      int r=do_read(fds[1].fd,answer,100);
      if (strcmp(answer,"y\n")==0 || strcmp(answer,"Y\n")==0)
      {
        do_write(fds[0].fd,answer,100);
        do_read(fds[0].fd,file_to_save,10000);
        open("./Download_file/new.txt",O_CREAT,S_IRWXU);
        FILE *file2=fopen("./Download_file/new.txt","w+");
        fputs(file_to_save,file2);
        fclose(file2);
        boucle=1;
      }
      else{
        do_write(fds[0].fd,answer,100);
        boucle=1;
      }
    }

  }
  close(s);
}
