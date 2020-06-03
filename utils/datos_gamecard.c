/*
 * datos_GameCard.c
 *
 *  Created on: 15 may. 2020
 *      Author: utnso
 */

#include "datos_gamecard.h"
#include <stdlib.h>
#include <string.h>


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
	//ESTO ESTA MAL, NO ES SIZEOF
	offset+=(gameCardNewPokemon->datos->tamanioNombre);

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
	offset+=(gameCardCatchPokemon->datos->tamanioNombre);

	memcpy(buffer->stream+offset,&(gameCardCatchPokemon->datos->posX),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(gameCardCatchPokemon->datos->posY),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(gameCardCatchPokemon->id_relativo),sizeof(uint32_t));
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
	offset+=(gameCardGetPokemon->datos->tamanioNombre);
}
