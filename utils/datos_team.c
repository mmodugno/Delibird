/*
 * datos_Team.c
 *
 *  Created on: 15 may. 2020
 *      Author: utnso
 */

#include "datos_team.h"
#include <stdlib.h>
#include <string.h>

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


