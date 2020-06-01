/*
 * datos_team.h
 *
 *  Created on: 15 may. 2020
 *      Author: utnso
 */

#ifndef DATOS_TEAM_H_
#define DATOS_TEAM_H_

#include "utils_en_comun.h"
#include<sys/socket.h>

//tipo de dato de Gameboy
typedef struct {
	appeared_pokemon* datos;
} team_appeared_pokemon;



void serializar_team_appeared_pokemon(team_appeared_pokemon* teamAppearedPokemon, t_buffer* buffer);
team_appeared_pokemon* deserealizar_team_appeared_pokemon(int cliente);

#endif /* DATOS_TEAM_H_ */
