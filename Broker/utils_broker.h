/*
 * utils_broker.h
 *
 *  Created on: 30 abr. 2020
 *      Author: utnso
 */

#ifndef UTILS_BROKER_H_
#define UTILS_BROKER_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>

typedef enum
{
	MENSAJE = 1,
	BROKER__NEW_POKEMON = 2,
	BROKER__APPEARED_POKEMON = 3,
	BROKER__CATCH_POKEMON = 4,
	BROKER__CAUGHT_POKEMON = 5,
	BROKER__GET_POKEMON = 6,
	TEAM__APPEARED_POKEMON = 7,
	GAMECARD__NEW_POKEMON = 8,
	GAMECARD__CATCH_POKEMON = 9,
	GAMECARD__GET_POKEMON = 10,
	SUBCRIPCION = 11

}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
char* recibir_mensaje(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void liberar_conexion(int socket_cliente);

#endif /* UTILS_BROKER_H_ */
