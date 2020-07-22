/*
 * datos_gamecard.h
 *
 *  Created on: 15 may. 2020
 *      Author: utnso
 */

#ifndef DATOS_GAMECARD_H_
#define DATOS_GAMECARD_H_

#include "utils_en_comun.h"

typedef struct {
	uint32_t id;
	new_pokemon* datos;
} gameCard_new_pokemon;

typedef struct {
	uint32_t id;
	catch_pokemon* datos;
} gameCard_catch_pokemon;

typedef struct {
	uint32_t id;
	get_pokemon* datos;
} gameCard_get_pokemon;

void serializar_gameCard_new_pokemon(gameCard_new_pokemon* gameCardNewPokemon, t_buffer* buffer);
void serializar_gameCard_catch_pokemon(gameCard_catch_pokemon* gameCardCatchPokemon, t_buffer* buffer);
void serializar_gameCard_get_pokemon(gameCard_get_pokemon* gameCardGetPokemon, t_buffer* buffer);

#endif /* DATOS_GAMECARD_H_ */
