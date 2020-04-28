/*
 * utils_team.h
 *
 *  Created on: 27 abr. 2020
 *      Author: utnso
 */

#ifndef TEAM_UTILS_TEAM_H_
#define TEAM_UTILS_TEAM_H_
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<p.thread>
#include "/home/utnso/tp-2020-1c-The-X-Team/utils_en_comun/utils_en_comun.h"


typedef struct{
	char pokemons[];
	char objetivos[];
	uint32_t posX;
	uint32_t posY;
}entrenador;




#endif /* TEAM_UTILS_TEAM_H_ */
