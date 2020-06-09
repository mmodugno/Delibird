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

typedef struct {
	uint32_t id;
	catch_pokemon* datos;
} team_catch_pokemon;

void serializar_team_appeared_pokemon(team_appeared_pokemon* teamAppearedPokemon, t_buffer* buffer);
team_appeared_pokemon* deserializar_team_appeared_pokemon(int socket_cliente);

#endif /* DATOS_TEAM_H_ */
