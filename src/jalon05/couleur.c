#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "couleur.h"

// https://www.commentcamarche.net/forum/affich-2622033-couleur-des-texte-en-langage-c

void put_color(char *msg, char *color)
{
  char msg2[1000];
  memset(msg2,0,sizeof(msg2));
  if (strcmp(color,"red")==0)
  {
  strcpy(msg2,"\033[1;31m");
  strcat(msg2,msg);
  strcat(msg2,"\033[0;m ");
  strcpy(msg,msg2);
  }
  if (strcmp(color,"green")==0)
  {
  strcpy(msg2,"\033[1;32m");
  strcat(msg2,msg);
  strcat(msg2,"\033[0;m ");
  strcpy(msg,msg2);
  }
  else if (strcmp(color,"yellow")==0)
  {
  strcpy(msg2,"\033[1;33m");
  strcat(msg2,msg);
  strcat(msg2,"\033[0;m ");
  strcpy(msg,msg2);
  }
  else if (strcmp(color,"blue")==0)
  {
  strcpy(msg2,"\033[1;34m");
  strcat(msg2,msg);
  strcat(msg2,"\033[0;m ");
  strcpy(msg,msg2);
  }
  else if (strcmp(color,"purple")==0)
  {
  strcpy(msg2,"\033[1;35m");
  strcat(msg2,msg);
  strcat(msg2,"\033[0;m ");
  strcpy(msg,msg2);
  }
  else if (strcmp(color,"cyan")==0)
  {
  strcpy(msg2,"\033[1;36m");
  strcat(msg2,msg);
  strcat(msg2,"\033[0;m ");
  strcpy(msg,msg2);
  }
  else if (strcmp(color,"grey")==0)
  {
  strcpy(msg2,"\033[1;37m");
  strcat(msg2,msg);
  strcat(msg2,"\033[0;m ");
  strcpy(msg,msg2);
  }
}
