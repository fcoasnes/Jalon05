#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "fctsocket.h"
#include <signal.h>
#include "couleur.h"
#include "rcvd_mode.h"
#include "send_mode.h"

volatile int gest_signal;


int gestion_commande(char *str){
  int a=0;
  const char s[2] = " ";
  char *tmp;
  tmp = strtok(str, s);
  if (strcmp(tmp,"/nick")==0){
    a=1;
  }
  if (strcmp(tmp,"/whois")==0){
    a=2;
  }
  if (strcmp(tmp,"/who")==0){
    a=3;
  }
  if (strcmp(tmp,"/quit\n")==0){
    a=4;
  }
  return a;
}


void signal_handler(int signo)
{
  gest_signal=1;
}

int main(int argc,char** argv)
{
  if (argc != 3)
  {
    fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
    return 1;
  }
  int have_chan=0; //savoir s'il appartien à un channel
  int pseudo_max=20;
  char msg[1000];
  char msg2[1000];
  char buffer[1000];
  char path[1000];
  char channame[100];
  char channameps[120];
  char pseudo[pseudo_max];
  char pseudo2[pseudo_max];
  char pseudo_send[1000];
  char pseudo_path[1000];
  gest_signal=0;
  struct sockaddr_in sock_host;
  char yolo[100]="[";

  //get address info from the server
  sock_host=get_addr_info(argv[1], atoi(argv[2]));

  //get the socket
  int s = do_socket(AF_INET, SOCK_STREAM, 0);

  //connect to remote socket
  do_connect(s, (const struct sockaddr*)&sock_host, sizeof(struct sockaddr));

  nfds_t nfds=2;
  struct pollfd *fds = malloc(20*sizeof(struct pollfd));
  memset(fds,0,sizeof(fds));
  fds[0].fd = s;
  fds[0].events = POLLIN;
  fds[1].fd = STDIN_FILENO;
  fds[1].events = POLLIN;

  struct sigaction sig;
  sig.sa_handler = signal_handler;
  sigaction(SIGINT, &sig, NULL);
  sigaction(SIGSEGV, &sig, NULL);

  printf("------Welcome to the chat------\n\n");

  printf("Please, check all the command and rule by typing /help\n");
  printf("[Server] : please logon with /nick <your pseudo>\n");

  int init_pseudo=0; // variable pour detecter si l'utilisateur à entrer son pseudo, tant que cette variable vaut 0, l'utilisateur ne peut rien faire
  while (init_pseudo==0)
  {
    memset(buffer,0,sizeof(buffer));
    int p=poll(fds, nfds, -1);


    if (gest_signal==1)
    {
      printf("Connection terminé\n");
      do_write(s,"/quit\n",sizeof("/quit\n"));
      close(s);
      return 0;
    }

    if (fds[1].revents == POLLIN)
    {
      do_read(0,buffer,1000);
      if (gestion_commande(buffer)==1)
      {
        memcpy(pseudo,&buffer[6],pseudo_max);
        pseudo[strlen(pseudo)-1]='\0';
        do_write(s,pseudo,sizeof(pseudo));
      }

      else if (strcmp(buffer,"/help\n")==0)
      {
        char *msg=malloc(100000);
        char *path="./help.txt";
        int file=open(path, O_RDONLY);
        read(file,msg, 10000);
        printf("%s\n",msg);
        printf("[Server] : please logon with /nick <your pseudo>\n");
        close(file);
      }

      else{
        do_write(s,"erreur",10);
      }
    }

    if(fds[0].revents==POLLIN)
    {
      do_read(s, buffer, 1000);
      if (strcmp(buffer,"nick_ok")==0)
      {
        init_pseudo=1;
      }

      else
      {
        printf("%s\n",buffer);
        fflush(stdout);
      }
    }
  }

  printf("Connecté! Vous pouvez désormais communiquer avec le serveur! \n\n");

  strcpy(channameps,pseudo);
  while(1)
  {
    if (have_chan==0)   //s'il le client n'a pas de channel, on affiche juste son pseudo
    {
      printf("\033[1;32m%s>\033[0;m ",pseudo);
      fflush(stdout);
    }
    if (have_chan==1)   //si le client est dan sun channel on affiche son channel+pseudo
    {
      strcpy(yolo,"[");
      strcat(yolo,channame);
      strcat(yolo,"] ");
      strcat(yolo,pseudo);
      strcat(yolo,"> ");
      put_color(yolo,"green");
      printf("%s",yolo);
      fflush(stdout);
    }
    int p=poll(fds, nfds, -1);
    int i;
    //get user input


    if (gest_signal==1)
    {
      printf("Connection terminé\n");
      do_write(s,"/quit\n",sizeof("/quit\n"));
      close(s);
      return 0;
    }


    if (p == -1)
    {
      error("ERREUR lors du poll");
    }


    if (fds[1].revents == POLLIN)
    {
      memset(msg, 0, 1000);
      do_read(0,msg,1000);

      if (gestion_commande(strcpy(msg2,msg))==4)
      {
        printf("Connection terminé\n");
        do_write(s,msg2,sizeof(msg2));
        close(s);
        return 0;
      }

      else if (strcmp(strtok(strcpy(msg2,msg)," "),"/create")==0)
      {
        do_write(s,msg,sizeof(msg));
      }

      else if (strcmp(msg,"/quitc\n")==0)
      {
        if(have_chan==1)
        {
          do_write(s,msg,sizeof(msg));
          have_chan=0;
        }
        else
        {
          do_write(s,msg,sizeof(msg));
        }

      }

      else if (strcmp(msg,"/help\n")==0)
      {
        char *msg=malloc(100000);
        char *path="./help.txt";
        int file=open(path, O_RDONLY);
        read(file,msg, 10000);
        printf("%s\n",msg);
        close(file);
      }

      else if (strcmp(strtok(strcpy(msg2,msg)," "),"/join")==0)
      {
        do_write(s,msg,sizeof(msg));
      }

      else if (strcmp(strtok(strcpy(msg2,msg)," "),"/send")==0)
      {
        char pseudo_send[1000];
        char pseudo_path[1000];
        memcpy(pseudo_path,&msg2[6],1000);  // ici pseudo_send contient le pseudo + path du fichier a envoyer
        pseudo_path[strlen(pseudo_path)-1]='\0';
        strtok(strcpy(pseudo_send,pseudo_path)," ");  //ici on recupere juste le pseudo
        memcpy(path, &pseudo_path[strlen(pseudo_send)+1],1000);   //ici on recupere juste le chemin
        do_write(s,msg,sizeof(msg));
      }

      else if (strcmp(strtok(strcpy(msg2,msg)," "),"/whoin")==0)
      {
        do_write(s,msg,sizeof(msg));
      }

      else if (gestion_commande(strcpy(msg2,msg))==1)
      {

        memcpy(pseudo2,&msg2[6],pseudo_max);
        pseudo2[strlen(pseudo2)-1]='\0';
        do_write(s,msg,sizeof(msg));
      }
      //send message to the server
      else
      {
        do_write(s,msg,sizeof(msg));
      }
    }


    if(fds[0].revents==POLLIN)
    {
      printf("\x0d"); //fait revenir le curseur au debut de la ligne
      printf("\033[K"); //efface ligne
      memset(buffer,0,sizeof(buffer));
      int r=do_read(s, buffer, 1000);
      buffer[strlen(buffer)-1]='\0';

      if (r==0)  //si le serveur fait un ctr-C
      {
        printf("The servor has been disconnected.\n");
        close(s);
        return 0;
      }

      else if (strcmp(buffer,"join_ok")==0) //vérfie s'il peut bien rejoindre ce channel
      {
        memcpy(channame,&msg[6],pseudo_max);
        channame[strlen(channame)-1]='\0';
        have_chan=1;
      }

      else if (strcmp(buffer,"create_ok")==0) // vérifie s'il peut créer ce channel
      {
        memcpy(channame,&msg[8],pseudo_max);
        channame[strlen(channame)-1]='\0';
        have_chan=1;
      }
      else if (strcmp(buffer,"nick_ok")==0)
      {
        strcpy(pseudo,pseudo2);
      }

      else if (strcmp(buffer,"send_mode")==0)
      {
        send_file(pseudo,path,pseudo_send);
      }

      else if (strcmp(buffer,"rcvd_mode")==0)
      {
        receive_file(pseudo);
      }
      else
      {
        printf("%s\n",buffer);
        fflush(stdout);
      }
    }

  }

  return 0;


}
