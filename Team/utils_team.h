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

int crear_conexion(char* ip, char* puerto);
/*void enviar_mensaje(char* mensaje, int socket_cliente);
char* recibir_mensaje(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);*/
void liberar_conexion(int socket_cliente);




#endif /* TEAM_UTILS_TEAM_H_ */
