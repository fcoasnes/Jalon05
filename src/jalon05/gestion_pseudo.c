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
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "gestion_pseudo.h"


struct id_pseudo
{
  char nom[20];
  char channel[20];
  int init_ps;   //variable qui sert à savoir si le pseudo a déja été initialisé!
  int fd;
  char IP[20];
  int port;
  time_t t;
  struct tm *tmp;
  struct id_pseudo *next;
};

struct liste_pseudo
{
  struct id_pseudo *premier;
};


int conformed_pseudo(char *pseudo, char *erreur)
{
  int i;
  if (strlen(pseudo)>20)
  {
    strcpy(erreur,"Pseudo trop long ");
    return 1;
  }

  if (strcmp(pseudo,"")==0)
  {
    strcpy(erreur,"Pseudo invalide ");
    return 1;
  }
  for (i=0;i<strlen(pseudo);i++)
  {
    if (pseudo[i]=='_')
    {

    }
    else if (isalnum(pseudo[i])==0)   //autorisation des lettres, chiffrs et _ dans le pseudo.
    {
      strcpy(erreur,"Caractère invalide ");
      return 1;
    }
  }
  return 0;
}


struct liste_pseudo *initialisation(char *name, int fd)
{
  struct liste_pseudo *liste = malloc(sizeof(*liste));
  struct id_pseudo *pseudo=malloc(sizeof(*pseudo));
  if (liste==NULL || pseudo==NULL)
  {
    exit(EXIT_FAILURE);
  }
  strcpy(pseudo->nom,name);
  strcpy(pseudo->channel,"");
  pseudo->t = time(NULL);
  pseudo->tmp = localtime(&(pseudo->t));
  strcpy(pseudo->IP,"");
  pseudo->fd=fd;
  pseudo->port=0;
  pseudo->init_ps=0;
  pseudo->next=NULL;
  liste->premier=pseudo;

  return liste;
}

void nouveau_user(struct liste_pseudo *liste, char *name, int fd, char *IP2,int port)
{
  struct id_pseudo *nouveau = malloc(sizeof(*nouveau));

  if (liste==NULL || nouveau==NULL)
  {
    exit(EXIT_FAILURE);
  }

  strcpy(nouveau->nom,name);
  strcpy(nouveau->IP,IP2);
  strcpy(nouveau->channel,"");
  nouveau->t = time(NULL);
  nouveau->tmp = localtime(&(nouveau->t));
  nouveau->fd=fd;
  nouveau->init_ps=0;
  nouveau->port=port;
  nouveau->next=liste->premier;
  liste->premier=nouveau;

}

void supprimer_user(struct liste_pseudo *liste, int fd2)
{

  struct id_pseudo  *previous=liste->premier;
  struct id_pseudo  *ptr=previous->next;

  if (liste->premier->fd==fd2)
  {
    liste->premier=previous->next;
    free(previous);
  }
  else{
    while(ptr != NULL && ptr->fd!=fd2)
    {
      previous=ptr;
      ptr=previous->next;
    }
    if (ptr!=NULL)
    {
      previous->next=ptr->next;
      free(ptr);
    }
    else
    {
      printf("Erreur user non trouvable\n");
    }
  }
}

int pseudo_to_fd(struct liste_pseudo *liste, char *name)  //trouver un fd à partir d'un nom, retourne 0 si le pseudo n'est pas dans la liste des pseudo connectés
{
  struct id_pseudo *actuel=liste->premier;
  int a=0;
  while (actuel->next!=NULL)
  {
    if (strcmp(actuel->nom,name)==0)
    {
      a=actuel->fd;
    }
    actuel=actuel->next;
  }
  return a;
}

void fd_to_pseudo(struct liste_pseudo *liste, int fd, char *sortie)  //trouver un nom à partir d'un fd et l'enregistrer gràce à sortie
{
  struct id_pseudo *actuel=liste->premier;
  while (actuel->next!=NULL)
  {
    if (actuel->fd==fd)
    {
      strcpy(sortie,actuel->nom);
      break;
    }
    actuel=actuel->next;
  }
}

void afficher_liste_pseudo_co(struct liste_pseudo *liste, char *str_liste)  //permet d'afficher tout les utilisateurs connectés au chat
{
  struct id_pseudo *actuel=liste->premier;
  strcpy(str_liste,"Online users are:\n");
  while (actuel->next!=NULL)
  {
    if (strcmp(actuel->nom,"")!=0)
    {
      strcat(str_liste,"\t-");
      strcat(str_liste,(actuel->nom));
      strcat(str_liste,"\n");
    }
    actuel=actuel->next;
  }
  printf("%s",str_liste);
}


int gestion_doublon(struct liste_pseudo *liste, char *pseudo_check, int fd) //fct qui renvoie 1 si pseudo_check existe deja dans la liste des pseudo co
{
  struct id_pseudo *actuel=liste->premier;
  int a=0;
  while (actuel!=NULL)
  {
    if ((strcmp(pseudo_check,actuel->nom)==0) && (fd!=actuel->fd))
    {
      a=1;
    }
    actuel=actuel->next;
  }
  return a;
}

int get_fd_user(struct id_pseudo *ps)
{
  return ps->fd;
}

// int get_init_ps_user(struct id_pseudo *ps)
// {
//   return ps->init_ps;
// }
//
// int get_port_user(struct id_pseudo *ps)
// {
//   return ps->port;
// }
//
// struct id_pseudo *get_next_user(struct id_pseudo *ps)
// {
//   return ps->next;
// }
//
// char get_name_user(struct id_pseudo *ps)
// {
//   return ps->nom;
// }
//
// char get_IP_user(struct id_pseudo *ps)
// {
//   return ps->IP;
// }
//
// struct id_pseudo *get_first_user(struct liste_pseudo *liste)
// {
//   return liste->premier;
// }
