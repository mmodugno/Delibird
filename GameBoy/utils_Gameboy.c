/*
 * utils_gameboy.c
 *
 *  Created on: 29 abr. 2020
 *      Author: utnso
 */

#include "utils_Gameboy.h"


//TODO
/*
 * Recibe un paquete a serializar, y un puntero a un int en el que dejar
 * el tamaño del stream de bytes serializados que devuelve
 */




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

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) != 0){
		//printf("error");
		freeaddrinfo(server_info);
		return -1;
	}


	freeaddrinfo(server_info);

	return socket_cliente;
}

void enviar_Broker_New_Pokemon(broker_new_pokemon *brokerNewPokemon,char* username , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__NEW_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username)+1;
	paquete_a_enviar->username = username;

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

void enviar_Broker_Appeared_Pokemon(broker_appeared_pokemon *brokerAppearedPokemon,char* username , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__APPEARED_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username)+1;
	paquete_a_enviar->username = username;

	//serializacion de brokerNewPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_broker_appeared_pokemon(brokerAppearedPokemon,buffer);


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

void enviar_Broker_Catch_Pokemon(broker_catch_pokemon *brokerCatchPokemon,char* username , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__CATCH_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username)+1;
	paquete_a_enviar->username = username;

	//serializacion de brokerNewPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_broker_catch_pokemon(brokerCatchPokemon,buffer);


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

void enviar_Broker_Caught_Pokemon(broker_caught_pokemon *brokerCaughtPokemon,char* username, int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__CAUGHT_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username)+1;
	paquete_a_enviar->username = username;
	//serializacion de brokerCaughtPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_broker_caught_pokemon(brokerCaughtPokemon,buffer);

	paquete_a_enviar->buffer = buffer;

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

void enviar_Broker_Get_Pokemon(broker_get_pokemon *brokerGetPokemon,char* username , int socket_cliente)
{
	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__GET_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username)+1;
	paquete_a_enviar->username = username;
	//serializacion de brokerGetPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_broker_get_pokemon(brokerGetPokemon,buffer);

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

void enviar_Team_Appeared_Pokemon(team_appeared_pokemon *teamAppearedPokemon,char* username , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = TEAM__APPEARED_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username)+1;
	paquete_a_enviar->username = username;

	//serializacion de teamNewPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_team_appeared_pokemon(teamAppearedPokemon,buffer);

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

void enviar_GameCard_New_Pokemon(gameCard_new_pokemon *gameCardNewPokemon,char* username , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = GAMECARD__NEW_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username)+1;
	paquete_a_enviar->username = username;

	//serializacion de gamecardNewPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_gameCard_new_pokemon(gameCardNewPokemon,buffer);


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

void enviar_GameCard_Catch_Pokemon(gameCard_catch_pokemon *gameCardCatchPokemon,char* username , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = GAMECARD__CATCH_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username)+1;
	paquete_a_enviar->username = username;

	//serializacion de brokerNewPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_gameCard_catch_pokemon(gameCardCatchPokemon,buffer);

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

void enviar_GameCard_Get_Pokemon(gameCard_get_pokemon *gameCardGetPokemon,char* username , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = GAMECARD__GET_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username)+1;
	paquete_a_enviar->username = username;

	//serializacion de brokerNewPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_gameCard_get_pokemon(gameCardGetPokemon,buffer);

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
