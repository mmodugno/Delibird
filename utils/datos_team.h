/*
 * datos_team.h
 *
 *  Created on: 15 may. 2020
 *      Author: utnso
 */

#ifndef DATOS_TEAM_H_
#define DATOS_TEAM_H_

#include "utils_en_comun.h"

typedef struct {
	appeared_pokemon* datos;
} team_appeared_pokemon;

void serializar_team_appeared_pokemon(team_appeared_pokemon* teamAppearedPokemon, t_buffer* buffer);

#endif /* DATOS_TEAM_H_ */
