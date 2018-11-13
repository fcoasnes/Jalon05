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
#include "gestion_pseudo.h"
#include "channel.h"
#include "couleur.h"


struct id_pseudo
{
  char nom[20];   //nom utilisateur
  char channel[20];   //savoir à quelle channel appartient l'utilisateur
  int init_ps;   //variable qui sert à savoir si le pseudo a déja été initialisé!
  int fd;   //son fd associé
  char IP[20];    //adresse Ip
  int port;
  time_t t;
  struct tm *tmp;
  struct id_pseudo *next;
};

struct liste_pseudo
{
  struct id_pseudo *premier;
};

struct channel
{
  struct channel* next;
  char nom[20];
  int nb_co;
  int liste_co[10];
};

struct liste_channel
{
  struct channel *premier;
};


void init_serv_addr(const char* port, struct sockaddr_in *serv_addr) {
  int portno;
  //clean the serv_add structure
  memset(serv_addr, 0, sizeof (struct sockaddr_in));
  //cast the port from a string to an int
  portno=atoi(port);
  //internet family protocol
  serv_addr->sin_family = AF_INET;
  //we bind to any ip form the host
  serv_addr->sin_addr.s_addr = htonl(INADDR_ANY);
  //we bind on the tcp port specified
  serv_addr->sin_port = htons(portno);
}


int comptage_nb_connecte(struct pollfd *fds, int nfds){
  int i=0;
  int c=0;
  for(i=0; i<=nfds; i++){
    if (fds[i].fd==-1){
      c++;
    }
  }
  return (nfds-c-1);
}


int main(int argc, char** argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "usage: RE216_SERVER port\n");
    return 1;
  }
  socklen_t taille;
  struct sockaddr_in adresse_c;
  socklen_t addrlen = sizeof(struct sockaddr);
  size_t maxlen= 1000;          //Taille maximale en octets
  char msg_rcvd[maxlen];
  char copie_str[maxlen];
  char sortie[maxlen];
  char private_msg[1000];
  char broadcast_msg[1000];
  ssize_t len;
  nfds_t nfds=1;
  struct pollfd *fds = malloc(20*sizeof(struct pollfd));
  memset(fds,0,sizeof(fds));

  int init_chan=0;

  struct liste_channel *liste_c;

  struct liste_pseudo *liste_ps;

  char pseud[20];
  char pseudo_ini[20]="";

  struct sockaddr_in serv_addr;

  //create the socket, check for validity!
  int s=do_socket(PF_INET, SOCK_STREAM,0);


  //init the serv_add structure
  init_serv_addr(argv[1], &serv_addr);

  fcntl(s,F_SETFL,O_NONBLOCK);

  //perform the binding
  //we bind on the tcp port specified

  do_bind(s, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

  //specify the socket to be a server socket and listen for at most 20 concurrent client

  do_listen(s,20);

  fds[0].fd = s;
  fds[0].events = POLLIN;

  liste_ps=initialisation("serveur", s);

  int i;

  printf("En attente de connexion ...\n\n");
  while(1)
  {
    memset(sortie,0,maxlen);
    memset(msg_rcvd,0,maxlen);
    memset(copie_str,0,maxlen);
    memset(private_msg,0,maxlen); // char pour envoyer les messages unicast
    memset(broadcast_msg,0,maxlen); // char pour envoyer les messages broadcat
    strcpy(private_msg,"PM from ");
    strcpy(broadcast_msg,"BROADCAST: ");

    int p = poll(fds,20,-1);  //le poll pour detecter qui savoir quelle socket est active

    if (p == -1)
    {
      error("ERREUR lors du poll");
    }

    if (liste_c!=NULL)    //suppression des channels vides
    {
      if (liste_c->premier==NULL)
      {
        liste_c==NULL;
      }
      else{
        dell_chan(liste_c);
      }
    }

    for(i=0; i<=nfds; i++)
    {
      if (fds[i].revents == POLLIN && fds[i].fd == s)
      {
        int a=0;  // variable pour regarder si une place est libre dans la structure pollfd
        int k;

        for(k=0; k<nfds; k++)
        {
          if ((fds[k].fd == -1) && (a!=-1)) //si une place est dans la structure pollfd, le nouvel utilisateur la prendra
          {
            int socknew = do_accept(s,(struct sockaddr*)&adresse_c,&addrlen);
            fds[k].fd = socknew;
            fds[k].events =POLLIN;
            a=-1;
            nouveau_user(liste_ps,"",fds[k].fd,inet_ntoa(adresse_c.sin_addr),adresse_c.sin_port);
          }
        }

        if (a==0)
        {
          int socknew = do_accept(s,(struct sockaddr*)&adresse_c,&addrlen);
          fds[nfds].fd = socknew;
          fds[nfds].events =POLLIN;
          nouveau_user(liste_ps,"",fds[nfds].fd,inet_ntoa(adresse_c.sin_addr),adresse_c.sin_port);
          nfds++;
        }
      }

      else if(fds[i].revents ==POLLIN && fds[i].fd != s)
      {

        struct id_pseudo *actuel=liste_ps->premier;
        while(actuel!=NULL)
        {
          if (get_fd_user(actuel)==fds[i].fd) //trouver la bonne socket qui est en action, ainsi trouver le fd qui est en écoute dans la struct pollfd
          {

            if (actuel->init_ps==0)   //tant que l'utlisateur n'a pas initialisé son pseudo il ne peut rien faire
            {
              memset(pseud,0,sizeof(pseud));
              do_read(fds[i].fd,pseud,maxlen);

              if(strcmp(pseud,"/quit\n")== 0)
              {
                supprimer_user(liste_ps,fds[i].fd);
                fds[i].revents=0;
                fds[i].events =0;
                fds[i].fd=-1;
              }
              else if(strcmp(pseud,"erreur")== 0)
              {
                do_write(fds[i].fd,"[Server] : please logon with /nick <your pseudo>",100);
              }
              else if(conformed_pseudo(pseud,sortie)==1)
              {
                do_write(fds[i].fd,sortie,100);
              }
              else if(gestion_doublon(liste_ps,pseud, actuel->fd)==0)
              {
                strcpy(actuel->nom,pseud);
                do_write(fds[i].fd,"nick_ok",100);
                printf("Le client [%s] est connecté\n\n",actuel->nom);
                actuel->init_ps=1;
              }
              else
              {
                do_write(fds[i].fd,"Pseudo déja pris",100);
              }
            }


            else if(actuel->init_ps==1) //le pseudo a été initialisé donc il peut communiquer avec le chat
            {
              len = do_read(fds[i].fd,msg_rcvd,maxlen);    //read what the client has to say
              if (len<=0)
              {
              }
              else
              {
              if(strcmp(msg_rcvd,"/quit\n")== 0)
              {
                if (strcmp(actuel->channel,"")!=0)
                {
                  dell_user_in_chan(liste_c,actuel->channel,actuel->fd);
                }
                printf("ARRET de connexion avec %s, un nouveau client prendra sa place\n\n",actuel->nom);
                supprimer_user(liste_ps,fds[i].fd);
                fds[i].revents=0;
                fds[i].events =0;
                fds[i].fd=-1;
              }


              else if(strcmp(strtok(strcpy(copie_str,msg_rcvd)," "),"/nick")==0)  // pour changer de pseudo
              {
                char chgt_pseudo[20];
                memcpy(chgt_pseudo,&msg_rcvd[6],20);
                chgt_pseudo[strlen(chgt_pseudo)-1]='\0';
                if(conformed_pseudo(chgt_pseudo,sortie)==1)
                {
                  do_write(fds[i].fd,sortie,100);
                }
                else if (gestion_doublon(liste_ps,chgt_pseudo, actuel->fd)==0)
                {
                  strcpy(actuel->nom,chgt_pseudo);
                  do_write(fds[i].fd,"nick_ok\n",100);
                  do_write(fds[i].fd,"[Serveur] Le changement de pseudo a été pris en compte\n",100);
                }
                else
                {
                  do_write(fds[i].fd,"[Serveur] Pseudo déja utilisé\n",100);
                }
              }

              else if(strcmp(strtok(strcpy(copie_str,msg_rcvd)," "),"/whois")==0)
              {
                char qui_est[20];
                int ver2=0;  //sert a verifier si l'utilisateur existe
                char outstr[200];
                char IP2[50];
                char port2[50];
                memset(qui_est,0,20);
                memset(outstr,0,200);
                memset(IP2,0,50);
                memset(port2,0,50);


                struct id_pseudo *actuel2=liste_ps->premier;
                memcpy(qui_est,&msg_rcvd[7],20);
                qui_est[strlen(qui_est)-1]='\0';

                while(actuel2!=NULL)
                {
                  if (strcmp(actuel2->nom,qui_est)==0)
                  {
                    strftime(outstr, sizeof(outstr), "%c", actuel2->tmp);
                    strcpy(IP2,actuel2->IP);
                    sprintf(port2, "%d", actuel2->port);
                    ver2=1;
                  }
                  actuel2=actuel2->next;
                }
                if (ver2==0)
                {
                  strcpy(sortie,"[Serveur] L'utilisateur ");
                  strcat(sortie,qui_est);
                  strcat(sortie, " n'existe pas.\n");
                }
                else
                {
                  strcpy(sortie,"[Serveur] La date de premiere connexion de ");
                  strcat(sortie, qui_est);
                  strcat(sortie," est: ");
                  strcat(sortie,outstr);
                  strcat(sortie,". Il s'est connecté avec l'adresse IP :");
                  strcat(sortie,IP2);
                  strcat(sortie, " sur le port ");
                  strcat(sortie,port2);
                  strcat(sortie,"\n");
                }
                put_color(sortie,"red");
                do_write(fds[i].fd,sortie,1000);

              }

              else if(strcmp(strtok(strcpy(copie_str,msg_rcvd)," "),"/whoin")==0)
              {
                char qui_dans[20];
                memcpy(qui_dans,&msg_rcvd[7],20);
                qui_dans[strlen(qui_dans)-1]='\0';
                if (exist(liste_c,qui_dans)==1)
                {
                  afficher_user_in_channel(liste_ps , liste_c, qui_dans, sortie);
                  do_write(fds[i].fd,sortie,1000);
                }
                else
                {
                  do_write(fds[i].fd,"This channel do not exist ",1000);
                }
              }


              else if(strcmp(msg_rcvd,"/who\n")== 0)
              {
                afficher_liste_pseudo_co(liste_ps,sortie);
                do_write(fds[i].fd,sortie,maxlen);
              }


              else if(strcmp(strtok(strcpy(copie_str,msg_rcvd)," "),"/msgall")==0) //broadcast
              {
                memcpy(sortie,&msg_rcvd[8],1000);
                strcat(broadcast_msg,actuel->nom);
                strcat(broadcast_msg,"-> ");
                put_color(broadcast_msg, "yellow");
                fflush(stdout);
                strcat(broadcast_msg,sortie);

                struct id_pseudo *actuel3=liste_ps->premier;

                while(actuel3!=NULL)
                {
                  if (actuel3->fd!=fds[i].fd && actuel3->fd!=s)
                  {
                    do_write(actuel3->fd,broadcast_msg,1000);
                  }
                  actuel3=actuel3->next;
                }

              }


              else if(strcmp(strtok(strcpy(copie_str,msg_rcvd)," "),"/create")==0) //créer salon
              {
                char nom_chan[1000];
                memcpy(nom_chan,&msg_rcvd[8],1000);
                nom_chan[strlen(nom_chan)-1]='\0';
                if(gestion_doublon_c(liste_c,nom_chan)==0){
                  if (init_chan==0)
                  {
                    liste_c=initialisation_chan(nom_chan,fds[i].fd);
                    init_chan=1;
                  }
                  else
                  {
                    new_channel(liste_c,nom_chan,fds[i].fd);
                  }
                  strcpy(actuel->channel,nom_chan);
                  do_write(fds[i].fd,"create_ok ",1000);
                  do_write(fds[i].fd,"You have joined a channel ",1000);
                }
                else{
                  do_write(fds[i].fd,"This channel already exist ",1000);
                }
              }


              else if(strcmp(strtok(strcpy(copie_str,msg_rcvd)," "),"/join")==0) //créer salon
              {
                char nom_chan[1000];
                memcpy(nom_chan,&msg_rcvd[6],1000);
                nom_chan[strlen(nom_chan)-1]='\0';

                if (liste_c!=NULL && exist(liste_c,nom_chan)==1)
                {
                  if (strcmp(actuel->channel,nom_chan)!=0)
                  {
                    new_user_in_chan(liste_c,nom_chan,fds[i].fd);
                    strcpy(actuel->channel,nom_chan);
                    do_write(fds[i].fd,"join_ok ",1000);
                    do_write(fds[i].fd,"You have joined a channel ",1000);
                  }
                  else
                  {
                    do_write(fds[i].fd,"You still have joined this channel ",1000);
                  }
                }

                else
                {
                  do_write(fds[i].fd,"Channel doesn't exist ",1000);
                }
              }


              else if(strcmp(msg_rcvd,"/which\n")==0) //connaitre les salons existants
              {
                char msg6[1000];
                afficher_liste_channel(liste_c, msg6);
                do_write(fds[i].fd,msg6,1000);
              }

              else if(strcmp(msg_rcvd,"/quitc\n")==0) //connaitre les salons existants
              {
                if(strcmp(actuel->channel,"")!=0)
                {
                  dell_user_in_chan(liste_c,actuel->channel,actuel->fd);
                  strcpy(actuel->channel,"");
                  do_write(fds[i].fd,"You have quitted your channel ",1000);
                }
                else
                {
                  do_write(fds[i].fd,"You do not have channel ",1000);
                }
              }
              else if(strcmp(strtok(strcpy(copie_str,msg_rcvd)," "),"/send")==0) //créer salon
              {

                char pseudo_send[1000];
                char pseudo_path[1000];
                char path[1000];
                int ver2=0;   //verifie si le pseudo existe
                memcpy(pseudo_path,&msg_rcvd[6],1000);  // ici pseudo_send contient le pseudo + path du fichier a envoyer
                pseudo_path[strlen(pseudo_path)-1]='\0';
                strtok(strcpy(pseudo_send,pseudo_path)," ");  //ici on recupere juste le pseudo
                memcpy(path, &pseudo_path[strlen(pseudo_send)+1],1000);   //ici on recupere juste le chemin
                int file=open(path,O_RDONLY);

                struct id_pseudo *actuel2=liste_ps->premier;
                while(actuel2!=NULL)
                {
                  if (strcmp(actuel->nom,pseudo_send)==0)
                  {
                    ver2=2;
                    break;
                  }
                  else if (strcmp(actuel2->nom,pseudo_send)==0)
                  {
                    ver2=1;
                    break;
                  }
                  actuel2=actuel2->next;
                }
                if (ver2==0)
                {
                  do_write(actuel->fd,"Ce pseudo n'existe pas\n", maxlen);
                }
                else if (ver2==2)
                {
                  do_write(actuel->fd,"Vous ne pouvez pas vous envoyer des fichiers à vous même\n", maxlen);
                }

                else if (file<0)
                {
                  do_write(actuel->fd,"This file do not exist ",100);
                }
                else
                {
                  do_write(actuel->fd,"send_mode ",10);
                  do_write(actuel2->fd,"rcvd_mode ",10);
                }
                close(file);

              }


              else if(msg_rcvd[0]=='/')  //parler en unicast
              {
                strtok(strcpy(copie_str,msg_rcvd)," ");
                char choix_pseudo[20];
                memcpy(choix_pseudo,&copie_str[1],20);
                memcpy(sortie, &msg_rcvd[strlen(choix_pseudo)+2],1000);
                int lien=pseudo_to_fd(liste_ps,choix_pseudo);     //si le pseudo existe, cette fonction renvoie son fd associé

                strcat(private_msg,actuel->nom);
                put_color(private_msg,"purple");
                strcat(private_msg,"> ");
                strcat(private_msg,sortie);

                if(lien!=0)
                {
                  do_write(lien,private_msg,1000);
                }

                else    //si le lien vaut 0 ça veut dire que le pseudo n'existe pas, et donc c'est une commande inconnu
                {
                  do_write(fds[i].fd,"\033[1;31m[Serveur] Commande inconnu \033[0;m ",100);
                }
              }


              else if(strcmp(actuel->channel,"")!=0)  //parler dans le channel auquel on apprtient
              {
                char msg[1000]="[";
                strcat(msg,actuel->channel);
                strcat(msg,"] ");
                strcat(msg, actuel->nom);
                strcat(msg,"> ");
                put_color(msg, "cyan");
                strcat(msg,msg_rcvd);
                struct channel *actuel2=liste_c->premier;
                int k;
                while(actuel2!=NULL && strcmp(actuel2->nom,actuel->channel)!=0)   //on cherche le channel auquel appartient actuel
                {
                  actuel2=actuel2->next;
                }
                for(k=0;k<=actuel2->nb_co;k++)
                {
                  if(actuel2->liste_co[k]!=0)
                  {
                    if(actuel2->liste_co[k]!=actuel->fd)
                    {
                      do_write(actuel2->liste_co[k],msg,1000);
                    }
                  }
                }
              }
              else
              {
                char msg7[2000]="[Serveur] Join or create a channel";
                put_color(msg7,"red");
                do_write(fds[i].fd,msg7,100);
              }
            }
          }
        }
          actuel=actuel->next;
        }
      }
    }
  }

  close(s);

  return 0;
}
