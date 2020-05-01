/*
 * utils_en_comun.h
 *
 *  Created on: 29 abr. 2020
 *      Author: utnso
 */

#ifndef UTILS_EN_COMUN_H_
#define UTILS_EN_COMUN_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "stdint.h"


// //// ESTRUCTURAS DE MENSAJES //////
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
//////////////////////////////////////////////////

#endif /* UTILS_EN_COMUN_H_ */
