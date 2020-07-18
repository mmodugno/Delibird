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

	//3. uint32_t tamanioNombre;
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

	offset+=teamAppearedPokemon->datos->tamanioNombre;

	memcpy(buffer->stream+offset,&(teamAppearedPokemon->datos->posX),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(teamAppearedPokemon->datos->posY),sizeof(uint32_t));
	offset+=sizeof(uint32_t);
}

team_appeared_pokemon* deserializar_team_appeared_pokemon(int socket_cliente){

	//1. uint32_t tamanioUsername;
	//2. char* nombreUser;
	//1. uint32_t tamanioNombre;
	//2. char* nombrePokemon;
	//3. uint32_t posX;
	//4. uint32_t posY;

	appeared_pokemon  *datos2= malloc(sizeof(appeared_pokemon));
	team_appeared_pokemon* appearedPoke = malloc(sizeof(team_appeared_pokemon));
	appearedPoke->datos = datos2 ;

    //void* nombre;
    /////NO HACE FALTA NOMBRE, PODEMOS HACER UN MALLOC DEL NOMBRE DESPUES DE PEDIR EL TAMANIO DEL NOMBRE

    recv(socket_cliente,&(appearedPoke->datos->tamanioNombre),sizeof(uint32_t),0);

    //nombre = malloc(newPoke->datos->tamanioNombre);
    appearedPoke->datos->nombrePokemon = malloc(appearedPoke->datos->tamanioNombre);

    recv(socket_cliente,appearedPoke->datos->nombrePokemon,appearedPoke->datos->tamanioNombre,0);
    //memcpy(newPoke->datos->nombrePokemon,nombre,newPoke->datos->tamanioNombre);

    recv(socket_cliente,&(appearedPoke->datos->posX),sizeof(uint32_t),0);

    recv(socket_cliente,&(appearedPoke->datos->posY),sizeof(uint32_t),0);


    //free(nombre);

    return appearedPoke;

}

team_caught_pokemon* deserializar_team_caught_pokemon(int socket_cliente){

	// deserializacion
	//1. uint32_t id_relativo;
	//2. uint32_t puedoAtraparlo;


	team_caught_pokemon* caughtPokemon = malloc(sizeof(team_caught_pokemon));
	caughtPokemon->datos = malloc(sizeof(caught_pokemon));
	recv(socket_cliente, &(caughtPokemon->id),sizeof(uint32_t),0);
	recv(socket_cliente, &(caughtPokemon->datos->puedoAtraparlo),sizeof(uint32_t),0);
	recv(socket_cliente, &(caughtPokemon->id_relativo),sizeof(uint32_t),0);

	return caughtPokemon;
}

/* todo
void serializar_team_catch_pokemon(team_catch_pokemon* teamCatchPokemon, t_buffer* buffer){
	buffer->size=
				sizeof(uint32_t)*3 // posX, posY, tamanioNombre
				+ strlen(teamCatchPokemon->datos->nombrePokemon)+1; //longitud del strind nombre de pokemon

		buffer->stream = malloc(buffer->size);
}
*/
