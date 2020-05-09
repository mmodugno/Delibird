/*
 * conexiones.c
 *
 *  Created on: 9 may. 2020
 *      Author: utnso
 */


#include "conexiones.h"

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



char* recibir_mensaje(int socket_cliente)
{
	t_paquete* paquete_recibido=malloc(sizeof(t_paquete));
	paquete_recibido->buffer= malloc(sizeof(t_buffer));

	char* mensaje;

//TEAM__APPEARED_POKEMON

	recv(socket_cliente,&(paquete_recibido->codigo_operacion),sizeof(op_code),0);
	recv(socket_cliente,&(paquete_recibido->buffer->size),sizeof(int),0);

	if(paquete_recibido->codigo_operacion == MENSAJE){
		recv(socket_cliente,(paquete_recibido->buffer->stream),paquete_recibido->buffer->size,0);
	}

	mensaje = malloc(paquete_recibido->buffer->size);
	memcpy(mensaje,paquete_recibido->buffer->stream,paquete_recibido->buffer->size);

	free(paquete_recibido->buffer);
	free(paquete_recibido);
	return mensaje;


}



void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}
