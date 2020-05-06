/*
 * utils_gameboy.c
 *
 *  Created on: 29 abr. 2020
 *      Author: utnso
 */

#include "utils_Gameboy.h"
#include "gameboy.c"


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

	memcpy(buffer->stream+offset,(brokerNewPokemon->datos->nombrePokemon),brokerNewPokemon->datos->tamanioNombre);
	offset+=sizeof(brokerNewPokemon->datos->tamanioNombre);

	memcpy(buffer->stream+offset,&(brokerNewPokemon->datos->posX),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(brokerNewPokemon->datos->posY),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(brokerNewPokemon->datos->cantidadPokemon),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

}

//CONSULTAR CHARLY EN UNA COSA DE MEMCPY y corregir los mios si esta mal
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

	memcpy(buffer->stream+offset,(brokerAppearedPokemon->datos->nombrePokemon), brokerAppearedPokemon->datos->tamanioNombre);
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

	memcpy(buffer->stream+offset,(brokerCatchPokemon->datos->nombrePokemon),brokerCatchPokemon->datos->tamanioNombre);
	offset+=sizeof(brokerCatchPokemon->datos->tamanioNombre);

	memcpy(buffer->stream+offset,&(brokerCatchPokemon->datos->posX),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(brokerCatchPokemon->datos->posY),sizeof(uint32_t));
	offset+=sizeof(uint32_t);
}

void serializar_broker_caught_pokemon(broker_caught_pokemon* brokerCaughtPokemon, t_buffer* buffer)

{
	// serializacion
	//2. uint32_t id;
	//1. uint32_t puedoAtraparlo;

	buffer->size= sizeof(uint32_t) * 2;// size de puedoAtraparlo

	//preguntar
	//supongo que es
	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream+offset,&(brokerCaughtPokemon->id),sizeof(uint32_t));
		offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(brokerCaughtPokemon->datos->puedoAtraparlo),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

}

void serializar_broker_get_pokemon(broker_get_pokemon* brokerGetPokemon, t_buffer* buffer)
{
	//serializacion
	//1. uint32_t tamanioNombre;
	//2. char* nombrePokemon;

	buffer->size= sizeof(uint32_t)
			+ strlen(brokerGetPokemon->datos->nombrePokemon)+1; //longitud del string nombre de pokemon

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream+offset,&(brokerGetPokemon->datos->tamanioNombre),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,(brokerGetPokemon->datos->nombrePokemon),brokerGetPokemon->datos->tamanioNombre);
	offset+=sizeof(brokerGetPokemon->datos->tamanioNombre);

}

void serializar_team_appeared_pokemon(team_appeared_pokemon* teamAppearedPokemon, t_buffer* buffer)
{

	//1. uint32_t tamanioNombre;
	//2. char* nombrePokemon;
	//3. uint32_t posX;
	//4. uint32_t posY;

	buffer->size=
			sizeof(uint32_t)*3 // posX, posY, tamanioNombre
			+ strlen(teamAppearedPokemon->datos->nombrePokemon)+1; //longitud del strind nombre de pokemon

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream+offset,&(teamAppearedPokemon->datos->tamanioNombre),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,(teamAppearedPokemon->datos->nombrePokemon),teamAppearedPokemon->datos->tamanioNombre);
	offset+=sizeof(teamAppearedPokemon->datos->tamanioNombre);

	memcpy(buffer->stream+offset,&(teamAppearedPokemon->datos->posX),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(teamAppearedPokemon->datos->posY),sizeof(uint32_t));
	offset+=sizeof(uint32_t);
}

void serializar_gameCard_new_pokemon(gameCard_new_pokemon* gameCardNewPokemon, t_buffer* buffer)
{
	// serializacion
	//1. uint32_t tamanioNombre;
	//2. char* nombrePokemon;
	//3. uint32_t posX;
	//4. uint32_t posY;
	//5. uint32_t cantidadPokemon;

	buffer->size=strlen(gameCardNewPokemon->datos->nombrePokemon)+1 //longitud de string pokemon + 1 del centinela "/0"
				+sizeof(uint32_t)*3 //posX, posY, cantidad de pokemons
				+ sizeof(uint32_t);// size de longitud del string

	buffer->stream = malloc(buffer->size);
	int offset = 0;


	memcpy(buffer->stream+offset,&(gameCardNewPokemon->datos->tamanioNombre),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,(gameCardNewPokemon->datos->nombrePokemon),gameCardNewPokemon->datos->tamanioNombre);
	offset+=sizeof(gameCardNewPokemon->datos->tamanioNombre);

	memcpy(buffer->stream+offset,&(gameCardNewPokemon->datos->posX),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(gameCardNewPokemon->datos->posY),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(gameCardNewPokemon->datos->cantidadPokemon),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

}

void serializar_gameCard_catch_pokemon(gameCard_catch_pokemon* gameCardCatchPokemon, t_buffer* buffer)
{
	// serializacion
	//1. uint32_t tamanioNombre;
	//2. char* nombrePokemon;
	//3. uint32_t posX;
	//4. uint32_t posY;
	//5. uint32_t idMensaje;

	buffer->size= sizeof(uint32_t)*4 // posX, posY, tamanioNombre, id
			+ strlen(gameCardCatchPokemon->datos->nombrePokemon)+1; //longitud del strind nombre de pokemon +1centinela

	buffer->stream = malloc(buffer->size);
	int offset = 0;



	memcpy(buffer->stream+offset,&(gameCardCatchPokemon->datos->tamanioNombre),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,(gameCardCatchPokemon->datos->nombrePokemon), gameCardCatchPokemon->datos->tamanioNombre);
	offset+=sizeof(gameCardCatchPokemon->datos->tamanioNombre);

	memcpy(buffer->stream+offset,&(gameCardCatchPokemon->datos->posX),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(gameCardCatchPokemon->datos->posY),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(gameCardCatchPokemon->id),sizeof(uint32_t));
	offset+=sizeof(uint32_t);
}

void serializar_gameCard_get_pokemon(gameCard_get_pokemon* gameCardGetPokemon, t_buffer* buffer)		// GET
{
	// serializacion
	//1. uint32_t tamanioNombre;
	//2. char* nombrePokemon;

	buffer->size=strlen(gameCardGetPokemon->datos->nombrePokemon)+1 //longitud de string pokemon + 1 del centinela "/0"
				+sizeof(uint32_t)*3 //posX, posY, cantidad de pokemons
				+ sizeof(uint32_t);// size de longitud del string

	buffer->stream = malloc(buffer->size);
	int offset = 0;


	memcpy(buffer->stream+offset,&(gameCardGetPokemon->datos->tamanioNombre),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,(gameCardGetPokemon->datos->nombrePokemon),gameCardGetPokemon->datos->tamanioNombre);
	offset+=sizeof(gameCardGetPokemon->datos->tamanioNombre);
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

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) != 0){
		printf("error");
		fflush(stdout);
		freeaddrinfo(server_info);
		return -1;
	}


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

void enviar_Broker_Catch_Pokemon(broker_catch_pokemon *brokerCatchPokemon , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__CATCH_POKEMON;


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

void enviar_Broker_Caught_Pokemon(broker_caught_pokemon *brokerCaughtPokemon, int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__CAUGHT_POKEMON;

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

void enviar_Broker_Get_Pokemon(broker_get_pokemon *brokerGetPokemon , int socket_cliente)
{
	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__GET_POKEMON;

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

void enviar_Team_Appeared_Pokemon(team_appeared_pokemon *teamAppearedPokemon , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = TEAM__APPEARED_POKEMON;

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

void enviar_GameCard_New_Pokemon(gameCard_new_pokemon *gameCardNewPokemon , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = GAMECARD__NEW_POKEMON;


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

void enviar_GameCard_Catch_Pokemon(gameCard_catch_pokemon *gameCardCatchPokemon , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = GAMECARD__CATCH_POKEMON;


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

void enviar_GameCard_Get_Pokemon(gameCard_get_pokemon *gameCardGetPokemon , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = GAMECARD__GET_POKEMON;


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
