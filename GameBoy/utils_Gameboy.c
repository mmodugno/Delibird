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

void serializar_broker_new_pokemon(broker_new_pokemon* brokerNewPokemon, t_buffer* buffer)
{
	// serializacion
	//1. uint32_t tamanioNombre;
	//2. char* nombrePokemon;
	//3. uint32_t posX;
	//4. uint32_t posY;
	//5. uint32_t cantidadPokemon;

	buffer->size=strlen(brokerNewPokemon->datos->nombrePokemon)+1 //longitud de string pokemon + 1 del centinela "/0"
				+sizeof(uint32_t)*3 //posX, posY, cantidad de pokemons
				+ sizeof(uint32_t);// size de longitud del string

	buffer->stream = malloc(buffer->size);
	int offset = 0;


	memcpy(buffer->stream+offset,&(brokerNewPokemon->datos->tamanioNombre),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,(brokerNewPokemon->datos->nombrePokemon),sizeof(uint32_t));
	offset+=sizeof(brokerNewPokemon->datos->tamanioNombre);

	memcpy(buffer->stream+offset,&(brokerNewPokemon->datos->posX),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(brokerNewPokemon->datos->posY),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(brokerNewPokemon->datos->cantidadPokemon),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

}

void serializar_broker_appeared_pokemon(broker_appeared_pokemon* brokerAppearedPokemon, t_buffer* buffer)
{
	// serializacion
	//1. uint32_t id;
	//2. uint32_t tamanioNombre;
	//3. char* nombrePokemon;
	//4. uint32_t posX;
	//5. uint32_t posY;

	buffer->size= sizeof(uint32_t) // id
			+ sizeof(uint32_t)*3 // posX, posY, tamanioNombre
			+ strlen(brokerAppearedPokemon->datos->nombrePokemon)+1; //longitud del strind nombre de pokemon

	buffer->stream = malloc(buffer->size);
	int offset = 0;



	memcpy(buffer->stream+offset,&(brokerAppearedPokemon->id),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(brokerAppearedPokemon->datos->tamanioNombre),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,(brokerAppearedPokemon->datos->nombrePokemon),sizeof(uint32_t));
	offset+=sizeof(brokerAppearedPokemon->datos->tamanioNombre);

	memcpy(buffer->stream+offset,&(brokerAppearedPokemon->datos->posX),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(brokerAppearedPokemon->datos->posY),sizeof(uint32_t));
	offset+=sizeof(uint32_t);
}

void serializar_broker_catch_pokemon(broker_catch_pokemon* brokerCatchPokemon, t_buffer* buffer)
{
	// serializacion
	//1. uint32_t tamanioNombre;
	//2. char* nombrePokemon;
	//3. uint32_t posX;
	//4. uint32_t posY;

	buffer->size= sizeof(uint32_t)*3 // posX, posY, tamanioNombre
			+ strlen(brokerCatchPokemon->datos->nombrePokemon)+1; //longitud del strind nombre de pokemon

	buffer->stream = malloc(buffer->size);
	int offset = 0;



	memcpy(buffer->stream+offset,&(brokerCatchPokemon->datos->tamanioNombre),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,(brokerCatchPokemon->datos->nombrePokemon),sizeof(uint32_t));
	offset+=sizeof(brokerCatchPokemon->datos->tamanioNombre);

	memcpy(buffer->stream+offset,&(brokerCatchPokemon->datos->posX),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(brokerCatchPokemon->datos->posY),sizeof(uint32_t));
	offset+=sizeof(uint32_t);
}

//TODO
//usar este?
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

void enviar_Broker_New_Pokemon(broker_new_pokemon *brokerNewPokemon , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__NEW_POKEMON;


	//serializacion de brokerNewPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_broker_new_pokemon(brokerNewPokemon,buffer);


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

void enviar_Broker_Appeared_Pokemon(broker_appeared_pokemon *brokerAppearedPokemon , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__APPEARED_POKEMON;


	//serializacion de brokerNewPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_broker_new_pokemon(brokerAppearedPokemon,buffer);


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

void enviar_Broker_Catch_Pokemon(broker_catch_pokemon *brokerCatchPokemon , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__CATCH_POKEMON;


	//serializacion de brokerNewPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_broker_new_pokemon(brokerCatchPokemon,buffer);


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

//TODO
//se podria usar este para no repetir codigo, pero no se me ocurrio cómo
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
//podriamos modificar este y usar el server que nos dieron para probar los mensajes anteriores
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
