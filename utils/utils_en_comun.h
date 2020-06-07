/*
 * utils_en_comun.h
 *
 *  Created on: 29 abr. 2020
 *      Author: utnso
 */

#ifndef UTILS_EN_COMUN_H_
#define UTILS_EN_COMUN_H_

#include <stdio.h>
#include<commons/collections/queue.h>

#include "stdint.h"


// //// ESTRUCTURAS DE MENSAJES //////

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
	SUSCRIPCION = 11
	//despues vamos a tener un tipo de mensaje de Gamecard del tipo BROKER_LOCALIZED_POKEMON

}op_code;

typedef enum{
	NEW_POKEMON=1,
	APPEARED_POKEMON=2,
	CATCH_POKEMON=3,
	CAUGHT_POKEMON=4,
	GET_POKEMON=5,
	LOCALIZED_POKEMON=6
}tipoDeCola;

typedef struct {
	uint32_t tamanioNombre;
	char* nombrePokemon;
	uint32_t posX;
	uint32_t posY;
	uint32_t cantidadPokemon;
} new_pokemon;

typedef struct {
	uint32_t tamanioNombre;
	char* nombrePokemon;
	uint32_t cantidadPosiciones;
	//preguntar []

	uint32_t* posX;
	uint32_t* posY;
} localize_pokemon;

typedef struct {
	uint32_t tamanioNombre;
	char* nombrePokemon;
} get_pokemon;

typedef struct {
	uint32_t tamanioNombre;
	char* nombrePokemon;
	uint32_t posX;
	uint32_t posY;
} appeared_pokemon;

typedef struct {
	uint32_t tamanioNombre;
	char* nombrePokemon;
	uint32_t posX;
	uint32_t posY;
} catch_pokemon;

typedef struct {
	uint32_t puedoAtraparlo;
} caught_pokemon;

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

typedef struct{
	uint32_t tamanioNombreSucriptor;
	char* nombreDeSuscriptor;
	tipoDeCola tipoDeCola;
}suscriptor;
//////////////////////////////////////////////////

void* serializar_paquete(t_paquete* paquete, int *bytes);
void serializar_suscriptor(suscriptor* suscriptor, t_buffer* buffer);
void enviar_pedido_suscripcion(suscriptor* suscriptor,int socketDeBroker);
suscriptor* deserializar_suscripcion(int socket_cliente);

#endif /* UTILS_EN_COMUN_H_ */
