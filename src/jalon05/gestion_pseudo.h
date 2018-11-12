#ifndef GESTION_PSEUDO_
#define GESTION_PSEUDO_

#include <time.h>

struct id_pseudo;

struct liste_pseudo;

int get_fd_user (struct id_pseudo *ps);
struct liste_pseudo *initialisation(char *name, int fd);
void nouveau_user(struct liste_pseudo *liste, char *name, int fd, char *IP2,int port);
void supprimer_user(struct liste_pseudo *liste, int fd2);
int pseudo_to_fd(struct liste_pseudo *liste, char *name);
void fd_to_pseudo(struct liste_pseudo *liste, int fd, char *sortie);
int get_init_ps_user(struct id_pseudo *ps);
int get_port_user(struct id_pseudo *ps);
struct id_pseudo *get_next_user(struct id_pseudo *ps);
char get_name_user(struct id_pseudo *ps);
int gestion_doublon(struct liste_pseudo *liste, char *pseudo_check, int fd);
void afficher_liste_pseudo_co(struct liste_pseudo *liste, char *str_liste);
int conformed_pseudo(char *pseudo, char *erreur);

#endif
