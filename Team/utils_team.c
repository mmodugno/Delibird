/*
 * utils_team.c
 *
 *  Created on: 28 abr. 2020
 *      Author: utnso
 */

#include "utils_team.h"

//esto va?
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

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("error");

	freeaddrinfo(server_info);

	return socket_cliente;
}


///

//Ver que devuelve
//cambiar nombre
int crear_proceso(char* archivoConfig){ //team1.config
	t_config* config = leer_config(archivoConfig); //devuelve la config

		//
	int conexionBroker; //ndea

	char* ipBroker;
	int puertoBroker;
	char* algoritmoPlanificacion;

	ipBroker=config_get_string_value(config,"IP_BROKER");
	puertoBroker = config_get_int_value(config,"PUERTO_BROKER");
	algoritmoPlanificacion = config_get_string_value(config,"ALGORITMO_PLANIFICACION");




}
