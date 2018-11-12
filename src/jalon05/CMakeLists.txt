cmake_minimum_required(VERSION 2.8.3)

project(RE216_JALON05)

#add here source files you need to compile and link to the client app
SET(SRC_CLIENT
	fctsocket.c
	client.c
	couleur.c
	send_mode.c
	rcvd_mode.c
	)


add_executable(RE216_JALON05_CLIENT ${SRC_CLIENT})


#add here source files you need to compile and link to the server app
SET(SRC_SERVER
	fctsocket.c
	channel.c
	gestion_pseudo.c
	couleur.c
        server.c)


add_executable(RE216_JALON05_SERVER  ${SRC_SERVER})
