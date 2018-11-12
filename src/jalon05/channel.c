#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "channel.h"
#include "gestion_pseudo.h"

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

struct liste_channel *initialisation_chan(char *name, int fd)
{
  struct liste_channel *liste = malloc(sizeof(*liste));
  struct channel *chan=malloc(sizeof(*chan));

  if (liste==NULL || chan==NULL)
  {
    exit(EXIT_FAILURE);
  }
  strcpy(chan->nom,name);
  chan->nb_co=1;
  int k;
  for (k=0;k<10;k++)
  {
    chan->liste_co[k]=0;
  }
  chan->liste_co[0] =fd;
  chan->next=NULL;
  liste->premier=chan;

  return liste;
}

void new_channel(struct liste_channel *liste, char *name, int fd)
{
  struct channel *nouveau = malloc(sizeof(*nouveau));
  if (liste==NULL || nouveau==NULL)
  {
    exit(EXIT_FAILURE);
  }
  nouveau->nb_co=1;
  strcpy(nouveau->nom,name);
  int k;
  for (k=0;k<10;k++)
  {
    nouveau->liste_co[k]=0;
  }
  nouveau->liste_co[0] =fd;
  nouveau->next=liste->premier;
  liste->premier=nouveau;
}

int exist(struct liste_channel *liste, char *name)
{
  int a=0;
  struct channel *actuel=liste->premier;

  while (actuel!=NULL)
  {
    if (strcmp(name,actuel->nom)==0)
    {
      a=1;
    }
    actuel=actuel->next;
  }
  return a;
}

void dell_chan(struct liste_channel *liste)
{

  struct channel  *previous=liste->premier;

  if(previous->next==NULL && previous->nb_co==0)
  {
    liste->premier=NULL;
    liste=NULL;
    free(previous);
    return ;
  }

  struct channel  *ptr=previous->next;

  if (liste->premier->nb_co==0)
  {
    liste->premier=previous->next;
    free(previous);
  }
  else
  {
    while(ptr != NULL && ptr->nb_co!=0)
    {
      previous=ptr;
      ptr=previous->next;
    }
    if (ptr!=NULL)
    {
      previous->next=ptr->next;
      free(ptr);
    }
  }
}

void afficher_liste_channel(struct liste_channel *liste, char *str_liste_c)
{
  if (liste==NULL || liste->premier==NULL)
  {
    strcpy(str_liste_c,"There is no channel\n");
    printf("%s",str_liste_c);
    return ;
  }

  struct channel *actuel=liste->premier;
  strcpy(str_liste_c,"Available channel are:\n");
  while (liste!=NULL && actuel!=NULL)
  {
    strcat(str_liste_c,"\t-");
    strcat(str_liste_c,(actuel->nom));
    strcat(str_liste_c,"\n");
    actuel=actuel->next;
  }
  printf("%s",str_liste_c);
}

void new_user_in_chan(struct liste_channel *liste, char *chan_name, int fd)
{
  struct channel *actuel=liste->premier;
  int i;
  while (actuel!=NULL)
  {
    if(strcmp(actuel->nom,chan_name)==0)
    {
      for(i=0;i<10;i++)
      {
        if (actuel->liste_co[i]==0)
        {
          actuel->liste_co[i]=fd;
          actuel->nb_co++;
          break;
        }
      }
    }
    actuel=actuel->next;
  }
}

void dell_user_in_chan(struct liste_channel *liste, char *chan_name, int fd)
{
  struct channel *actuel=liste->premier;
  int i;
  while (actuel!=NULL)
  {
    if(strcmp(actuel->nom,chan_name)==0)
    {
      for(i=0;i<10;i++){
        if (actuel->liste_co[i]==fd)
        {
          actuel->liste_co[i]=0;
          actuel->nb_co--;
          break;
        }
      }
    }
    actuel=actuel->next;
  }
}

int gestion_doublon_c(struct liste_channel *liste, char *channel_check)
{
  if (liste==NULL){
    return 0;
  }
  struct channel *actuel=liste->premier;
  while (actuel!=NULL)
  {
    if ((strcmp(channel_check,actuel->nom)==0))
    {
      return 1;
    }
    actuel=actuel->next;
  }
  return 0;
}

void afficher_user_in_channel(struct liste_pseudo *liste_p,struct liste_channel *liste_c, char *channel_name, char *str_liste_u)
{
  struct channel *actuel=liste_c->premier;
  int i;
  char tmp[20];
  strcpy(str_liste_u,"User in this channel are:\n");

  while (actuel!=NULL)
  {
    if (strcmp(actuel->nom,channel_name)==0)
    {
      for(i=0;i<10;i++)
      {
        if(actuel->liste_co[i]!=0)
        {
          fd_to_pseudo(liste_p,actuel->liste_co[i],tmp);
          strcat(str_liste_u,"\t-");
          strcat(str_liste_u,tmp);
          strcat(str_liste_u,"\n");
        }
      }
    }
    actuel=actuel->next;
  }
  printf("%s",str_liste_u);
}
