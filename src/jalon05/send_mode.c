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
#include "send_mode.h"

void init_sender_addr(const char* port, struct sockaddr_in *sender_addr) {
  int portno;
  memset(sender_addr, 0, sizeof (struct sockaddr_in));
  portno=atoi(port);
  sender_addr->sin_family = AF_INET;
  sender_addr->sin_addr.s_addr = htonl(INADDR_ANY);
  sender_addr->sin_port = htons(portno);
}


void send_file(char *pseudo_sender, char *path, char *pseudo_receiver)
{
  int boucle=0;

  char info[1000];
  char answer[100];
  char file_to_send[10000];
  struct sockaddr_in sender_addr;
  struct sockaddr_in adresse_c;
  socklen_t addrlen = sizeof(struct sockaddr);

  int s1=do_socket(PF_INET, SOCK_STREAM,0);

  init_sender_addr("8088", &sender_addr);

  do_bind(s1, (struct sockaddr *) &sender_addr, sizeof(sender_addr));

  do_listen(s1,20);

  int socknew = do_accept(s1,(struct sockaddr*)&adresse_c,&addrlen);

  printf("WAIT FOR ANSWER\n\n");
  fflush(stdout);

  int file=open(path, O_RDONLY);

  do_read(file,file_to_send,10000);


  nfds_t nfds=2;
  struct pollfd *fds = malloc(20*sizeof(struct pollfd));
  memset(fds,0,sizeof(fds));
  fds[0].fd = socknew;
  fds[0].events = POLLIN;
  fds[1].fd = STDIN_FILENO;
  fds[1].events = POLLIN;
  strcpy(info,pseudo_sender);
  strcat(info," want to send you a file, do you accept [y/n]\n");
  do_write(socknew,info,1000);
  while(boucle==0)
  {
    memset(answer,0,100);

    int p=poll(fds, nfds, -1);
    if (fds[0].revents == POLLIN)
    {
      do_read(fds[0].fd,answer,100);
      if (strcmp(answer,"y\n")==0 || strcmp(answer,"Y\n")==0)
      {
        printf("has accepted the file.\n");
        do_write(fds[0].fd,file_to_send,10000);
        boucle=1;
      }

      else if (strcmp(answer,"n\n")==0 || strcmp(answer,"N\n")==0)
      {
        printf("has refused the file.\n");
        boucle=1;
      }
      else
      {
        do_write(fds[0].fd,"Select y or n\n",100);
        boucle=1;
      }
    }

  }
  free(fds);
  close(file);
  close(s1);
  close(socknew);

}
