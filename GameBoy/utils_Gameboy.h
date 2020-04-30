/*
 * utils_Gameboy.h
 *
 *  Created on: 29 abr. 2020
 *      Author: utnso
 */

#ifndef UTILS_GAMEBOY_H_
#define UTILS_GAMEBOY_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include "/home/utnso/Escritorio/TP_SO/tp-2020-1c-The-X-Team/utils_en_comun/src/utils_en_comun.h"

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

typedef struct {
	new_pokemon* datos;
} broker_new_pokemon;

typedef struct {
	uint32_t id;
	appeared_pokemon* datos;
} broker_appeared_pokemon;

typedef struct {
	catch_pokemon* datos;
} broker_catch_pokemon;

typedef struct {
	uint32_t id;
	caught_pokemon* datos;
} broker_caught_pokemon;

typedef struct {
	get_pokemon* datos;
} broker_get_pokemon;

typedef struct {
	appeared_pokemon* datos;
} team_appeared_pokemon;

typedef struct {
	uint32_t id;
	new_pokemon* datos;
} gameCard_new_pokemon;

typedef struct {
	uint32_t id;
	catch_pokemon* datos;
} gameCard_catch_pokemon;

typedef struct {
	get_pokemon* datos;
} gameCard_get_pokemon;





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
void enviar_Broker_New_Pokemon(broker_new_pokemon *brokerNewPokemon , int socket_cliente);
void enviar_Broker_Appeared_Pokemon(broker_appeared_pokemon *brokerAppearedPokemon , int socket_cliente);
void enviar_Broker_Catch_Pokemon(broker_catch_pokemon *brokerCatchPokemon , int socket_cliente);
void enviar_Broker_Caught_Pokemon(broker_caught_pokemon *brokerCaughtPokemon, int socket_cliente);
void enviar_Broker_Get_Pokemon(broker_get_pokemon *brokerGetPokemon , int socket_cliente);
void enviar_Team_Appeared_Pokemon(team_appeared_pokemon *teamAppearedPokemon , int socket_cliente);
void enviar_GameCard_New_Pokemon(gameCard_new_pokemon *gameCardNewPokemon , int socket_cliente);
void enviar_GameCard_Catch_Pokemon(gameCard_catch_pokemon *gameCardCatchPokemon , int socket_cliente);
void enviar_GameCard_Get_Pokemon(gameCard_get_pokemon *gameCardGetPokemon , int socket_cliente);


#endif /* UTILS_GAMEBOY_H_ */
