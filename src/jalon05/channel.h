#ifndef CHANNEL_
#define CHANNEL_

#include "gestion_pseudo.h"

struct channel;
struct liste_channel;


struct liste_channel *initialisation_chan(char *name, int fd);
void new_channel(struct liste_channel *liste, char *name, int fd);
void dell_chan(struct liste_channel *liste);
void afficher_liste_channel(struct liste_channel *liste, char *str_liste_c);
void new_user_in_chan(struct liste_channel *liste, char *chan_name, int fd);
void dell_user_in_chan(struct liste_channel *liste, char *chan_name, int fd);
int exist(struct liste_channel *liste, char *name);
int gestion_doublon_c(struct liste_channel *liste, char *channel_check);
void afficher_user_in_channel(struct liste_pseudo *liste_p,struct liste_channel *liste_c, char *channel_name, char *str_liste_u);

#endif
