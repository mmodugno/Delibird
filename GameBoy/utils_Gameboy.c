/*
 * conexiones.c
 *
 *  Created on: 2 mar. 2019
 *      Author: the x team
 */

#include "utils_Gameboy.h"


//TODO
/*
 * Recibe un paquete a serializar, y un puntero a un int en el que dejar
 * el tamaño del stream de bytes serializados que devuelve
 */

//TODO
void* serializar_paquete(t_paquete* paquete, int *bytes)
{
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


//TODO
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



//TODO
void enviar_mensaje(char* mensaje, int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = MENSAJE;


	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size=strlen(mensaje)+1;
	buffer->stream= mensaje;

	paquete_a_enviar->buffer= buffer;

	int tamanio_buffer=0;

	void* bufferStream = serializar_paquete(paquete_a_enviar,&tamanio_buffer);
	send(socket_cliente,bufferStream,tamanio_buffer,0);


	free(bufferStream);

	//estos no hacen falta porque no pedimos memoria de stream, el buffer y paquete_a_enviar->buffer son lo mismo
	//free(buffer->stream);
	//free(buffer);
	//free(paquete_a_enviar->buffer->stream);
	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
}

//TODO?
char* recibir_mensaje(int socket_cliente)
{
	t_paquete* paquete_recibido=malloc(sizeof(t_paquete));
	paquete_recibido->buffer= malloc(sizeof(t_buffer));

	char* mensaje;


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
