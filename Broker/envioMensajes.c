/*
 * envioMensajes.c
 *
 *  Created on: 13 jul. 2020
 *      Author: utnso
 */

#include "envioMensajes.h"

void envioColaNewPokemon() {
	t_list *usersSinACK ;
	char* username;

	//este while esta demas creo

	sem_wait(&colaNew);



	bool esSuscriptor(char* userActual){
		return (!strcmp(userActual,username));
	}


	void llenarUserSinACK(char* userActual){
		username = string_duplicate(userActual);
		if(!list_any_satisfy(usersSinACK,esSuscriptor)){
			list_add(usersSinACK,userActual);
		}
	}


	bool menNewQueFalten(particion *part) {
		if (part->tipoMensaje == NEW_POKEMON) {
			//me fijo que se hayan mandado a todos los suscriptores
			usersSinACK = list_duplicate(part->acknoleged);
			list_iterate(suscriptoresNewPokemon,llenarUserSinACK);
			if(usersSinACK->elements_count!=0){
				return 1;
			}
			list_clean_and_destroy_elements(usersSinACK,free);
			return 0;
		}
		list_clean_and_destroy_elements(usersSinACK,free);
		return 0;
	}

	sem_wait(&usoMemoria);

	//BUSCO UN MENSAJE QUE NO HAYA ENVIADO
	particion *mensajeNewEnMemo = list_find(tablaDeParticiones,menNewQueFalten);



	//conectarme con cada uno que necesite mandarle
	bool esTeam(char* username){
		return string_contains(username,"TEAM");
	}
	bool esGameCard(char* username){
		return string_contains(username,"GAMECARD");
	}
	bool esGameBoy(char* username){
		return string_contains(username,"GAMEBOY");
	}

	if(mensajeNewEnMemo!=NULL){
		if(list_any_satisfy(usersSinACK,esTeam)){

			//conexion con TEAM

		}
		if(list_any_satisfy(usersSinACK,esGameBoy)){

			//conexion con GAMEBOY

		}
		if(list_any_satisfy(usersSinACK,esGameCard)){

			//conexion con GAMECARD

		}

	}


	//nose si list_destroy_and_destroy_elements() eliminaria los username que siguen estando en la de ack de esa part

	//clean si lo voy limpiandocada vez que lo uso
	list_clean(usersSinACK);
	//destroy si lo uso fuera del while
	//list_destroy(usersSinACK);

	free(username);
	sem_post(&usoMemoria);

}

void envioColaGetPokemon() {
	sem_wait(&colaGet);
	sem_wait(&usoMemoria);



	sem_post(&usoMemoria);
}

void envioColaLocalizedPokemon() {
	sem_wait(&colaLocalized);
	sem_wait(&usoMemoria);




	sem_post(&usoMemoria);
}

void envioColaAppearedPokemon() {
	sem_wait(&colaAppeared);
	sem_wait(&usoMemoria);




	sem_post(&usoMemoria);
}

void envioColaCatchPokemon() {
	sem_wait(&colaCatch);
	sem_wait(&usoMemoria);



	sem_post(&usoMemoria);
}

void envioColaCaughtPokemon() {
	sem_wait(&colaCaught);
	sem_wait(&usoMemoria);



	sem_post(&usoMemoria);
}


int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) != 0){
		//printf("error");
		freeaddrinfo(server_info);
		return -1;
	}


	freeaddrinfo(server_info);

	return socket_cliente;
}
