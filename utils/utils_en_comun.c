/*
 * Utils_en_comun.c
 *
 *  Created on: 6 may. 2020
 *      Author: utnso
 */
#include <stdlib.h>
#include <string.h>
#include<sys/socket.h>
#include "utils_en_comun.h"

//TODO
void* serializar_paquete(t_paquete* paquete, int *bytes){
	(*bytes) = sizeof(op_code)+sizeof(int)+(paquete->buffer->size);
	int offset = 0;
	void* stream_a_enviar=malloc(*bytes);

	memcpy(stream_a_enviar+offset,&(paquete->codigo_operacion),sizeof(op_code));
	offset+=sizeof(op_code);

	memcpy(stream_a_enviar+offset,&(paquete->buffer->size),sizeof(int));
	offset+=sizeof(int);


	memcpy(stream_a_enviar+offset,(paquete->buffer->stream),paquete->buffer->size);

	return stream_a_enviar;
}


void enviar_pedido_suscripcion(suscriptor* suscriptor,int socketDeBroker){
	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = SUSCRIPCION;


	//serializacion de suscriptor
	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_suscriptor(suscriptor,buffer);

	paquete_a_enviar->buffer= buffer;

	int tamanio_buffer=0;

	void* bufferStream = serializar_paquete(paquete_a_enviar,&tamanio_buffer);
	send(socketDeBroker,bufferStream,tamanio_buffer,0);


	free(bufferStream);

	//estos no hacen falta porque no pedimos memoria de stream, el buffer y paquete_a_enviar->buffer son lo mismo
	//free(buffer->stream);
	//free(buffer);
	//free(paquete_a_enviar->buffer->stream);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
}

void serializar_suscriptor(suscriptor* suscriptor, t_buffer* buffer)
{
	// serializacion
	//1. uint32_t tamanioNombre;
	//2. char* nombreDeSUSCRIPTOR;
	//3. tipoDeCola colaASuscribirse;

	buffer->size=strlen(suscriptor->nombreDeSuscriptor)+1 //longitud de string pokemon + 1 del centinela "/0"
				+sizeof(uint32_t)+sizeof(tipoDeCola); //tamanioNombre, enum de la cola a suscribirse

	buffer->stream = malloc(buffer->size);
	int offset = 0;


	memcpy(buffer->stream+offset,&(suscriptor->tamanioNombreSucriptor),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,(suscriptor->nombreDeSuscriptor),suscriptor->tamanioNombreSucriptor);
	offset+=sizeof(suscriptor->tamanioNombreSucriptor);

	memcpy(buffer->stream+offset,&(suscriptor->tipoDeCola),sizeof(tipoDeCola));
	offset+=sizeof(tipoDeCola);

}


suscriptor* deserializar_suscripcion(int socket_cliente, int* size){
	// deserializacion
	//1. uint32_t tamanioNombre;
	//2. char* nombreDeSUSCRIPTOR;
	//3. uint32_t colaASuscribirse;
	suscriptor* suscriptor = malloc(sizeof(suscriptor));
	void* nombre;

	recv(socket_cliente,&(suscriptor->tamanioNombreSucriptor),sizeof(uint32_t),0);
	nombre = malloc(suscriptor->tamanioNombreSucriptor);
	recv(socket_cliente , nombre , suscriptor->tamanioNombreSucriptor,0);
	recv(socket_cliente , &(suscriptor->tipoDeCola) , sizeof(tipoDeCola),0);

	memcpy(suscriptor->nombreDeSuscriptor,nombre,suscriptor->tamanioNombreSucriptor);

	free(nombre);

	return suscriptor;


}

get_pokemon* deserializar_get_pokemon(int socket_cliente, int* size){
	// deserializacion
	//1. uint32_t tamanioNombre;
	//2. char* nombrePokemon
	get_pokemon* getPoke = malloc(sizeof(get_pokemon));
	void* nombre;

	recv(socket_cliente,&(getPoke->tamanioNombre),sizeof(uint32_t),0);

	//faltaba pedir memoria para nombre y liberarla
	nombre = malloc(getPoke->tamanioNombre);
	recv(socket_cliente , nombre , getPoke->tamanioNombre,0);

	memcpy(getPoke->nombrePokemon,nombre,getPoke->tamanioNombre);

	free(nombre);

	return getPoke;


}

