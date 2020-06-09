/*
 * datos_broker.c
 *
 *  Created on: 15 may. 2020
 *      Author: utnso
 */

#include "datos_broker.h"
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include<sys/socket.h>

broker_get_pokemon* deserializar_get_pokemon(int socket_cliente){
	// deserializacion
	//1. uint32_t tamanioNombre;
	//2. char* nombrePokemon
	broker_get_pokemon* getPoke = malloc(sizeof(broker_get_pokemon));
	getPoke->datos = malloc(sizeof(get_pokemon));
	//void* nombre;
	/////NO HACE FALTA NOMBRE, PODEMOS HACER UN MALLOC DEL NOMBRE DESPUES DE PEDIR EL TAMANIO DEL NOMBRE

	recv(socket_cliente,&(getPoke->datos->tamanioNombre),sizeof(uint32_t),0);

	//faltaba pedir memoria para nombre y liberarla
	getPoke->datos->nombrePokemon = malloc(getPoke->datos->tamanioNombre);
	recv(socket_cliente , getPoke->datos->nombrePokemon , getPoke->datos->tamanioNombre,0);



	return getPoke;


}

broker_new_pokemon* deserializar_new_pokemon(int socket_cliente){

    //uint32_t tamanioNombre;
    //char* nombrePokemon;
    //uint32_t posX;
    //uint32_t posY;
    //uint32_t cantidadPokemon;
	new_pokemon  *datos2= malloc(sizeof(new_pokemon));
    broker_new_pokemon* newPoke = malloc(sizeof(broker_new_pokemon));
    newPoke->datos = datos2 ;

    //void* nombre;
    /////NO HACE FALTA NOMBRE, PODEMOS HACER UN MALLOC DEL NOMBRE DESPUES DE PEDIR EL TAMANIO DEL NOMBRE

    recv(socket_cliente,&(newPoke->datos->tamanioNombre),sizeof(uint32_t),0);

    //nombre = malloc(newPoke->datos->tamanioNombre);
    newPoke->datos->nombrePokemon = malloc(newPoke->datos->tamanioNombre);

    recv(socket_cliente,newPoke->datos->nombrePokemon,newPoke->datos->tamanioNombre,0);
    //memcpy(newPoke->datos->nombrePokemon,nombre,newPoke->datos->tamanioNombre);

    recv(socket_cliente,&(newPoke->datos->posX),sizeof(uint32_t),0);

    recv(socket_cliente,&(newPoke->datos->posY),sizeof(uint32_t),0);

    recv(socket_cliente,&(newPoke->datos->cantidadPokemon),sizeof(uint32_t),0);

    //free(nombre);

    return newPoke;

}

broker_appeared_pokemon* deserializar_appeared_pokemon(int socket_cliente){

	//uint32_t id_relativo;
    //uint32_t tamanioNombre;
    //char* nombrePokemon;
    //uint32_t posX;
    //uint32_t posY;

    broker_appeared_pokemon* appearedPoke = malloc(sizeof(broker_appeared_pokemon));
    appearedPoke->datos = malloc(sizeof(appeared_pokemon));
    //void* nombre;
    /////NO HACE FALTA NOMBRE, PODEMOS HACER UN MALLOC DEL NOMBRE DESPUES DE PEDIR EL TAMANIO DEL NOMBRE

    recv(socket_cliente,&(appearedPoke->id_relativo),sizeof(uint32_t),0);

    recv(socket_cliente,&(appearedPoke->datos->tamanioNombre),sizeof(uint32_t),0);

    appearedPoke->datos->nombrePokemon = malloc(appearedPoke->datos->tamanioNombre);

    recv(socket_cliente,appearedPoke->datos->nombrePokemon,appearedPoke->datos->tamanioNombre,0);



    recv(socket_cliente,&(appearedPoke->datos->posX),sizeof(uint32_t),0);

    recv(socket_cliente,&(appearedPoke->datos->posY),sizeof(uint32_t),0);

    return appearedPoke;

}

broker_catch_pokemon* deserializar_catch_pokemon(int socket_cliente){
	// deserializacion
	//1. uint32_t tamanioNombre;
	//2. char* nombrePokemon;
	//3. uint32_t posX;
	//4. uint32_t posY;

	broker_catch_pokemon* catchPokemon = malloc(sizeof(broker_catch_pokemon));
	catchPokemon->datos = malloc(sizeof(catch_pokemon));
	//void* nombre;
	/////NO HACE FALTA NOMBRE, PODEMOS HACER UN MALLOC DEL NOMBRE DESPUES DE PEDIR EL TAMANIO DEL NOMBRE

	recv(socket_cliente, &(catchPokemon->datos->tamanioNombre),sizeof(uint32_t),0);

	catchPokemon->datos->nombrePokemon = malloc(catchPokemon->datos->tamanioNombre);
	recv(socket_cliente, catchPokemon->datos->nombrePokemon , catchPokemon->datos->tamanioNombre, 0);
	recv(socket_cliente, &(catchPokemon->datos->posX), sizeof(uint32_t),0);
	recv(socket_cliente, &(catchPokemon->datos->posY), sizeof(uint32_t),0);


	return catchPokemon;
}

broker_caught_pokemon* deserializar_caught_pokemon(int socket_cliente){

	// deserializacion
	//1. uint32_t id_relativo;
	//2. uint32_t puedoAtraparlo;


	broker_caught_pokemon* caughtPokemon = malloc(sizeof(broker_caught_pokemon));
	caughtPokemon->datos = malloc(sizeof(caught_pokemon));

	recv(socket_cliente, &(caughtPokemon->id_relativo),sizeof(uint32_t),0);
	recv(socket_cliente, &(caughtPokemon->datos->puedoAtraparlo),sizeof(uint32_t),0);

	return caughtPokemon;
}

void serializar_broker_new_pokemon(broker_new_pokemon* brokerNewPokemon, t_buffer* buffer)
{
	// serializacion
	//1. uint32_t tamanioNombre;
	//2. char* nombrePokemon;
	//3. uint32_t posX;
	//4. uint32_t posY;
	//5. uint32_t cantidadPokemon;

	buffer->size=strlen(brokerNewPokemon->datos->nombrePokemon)+1 //longitud de string pokemon + 1 del centinela "/0"
				+sizeof(uint32_t)*3 //posX, posY, cantidad de pokemons
				+ sizeof(uint32_t);// size de longitud del string

	buffer->stream = malloc(buffer->size);
	int offset = 0;


	memcpy(buffer->stream+offset,&(brokerNewPokemon->datos->tamanioNombre),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,(brokerNewPokemon->datos->nombrePokemon),brokerNewPokemon->datos->tamanioNombre);
	//ESTO ESTA MAL, NO ES SIZEOF
	//offset+=sizeof(brokerNewPokemon->datos->tamanioNombre);
	offset+=brokerNewPokemon->datos->tamanioNombre;

	memcpy(buffer->stream+offset,&(brokerNewPokemon->datos->posX),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(brokerNewPokemon->datos->posY),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(brokerNewPokemon->datos->cantidadPokemon),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

}

void serializar_broker_appeared_pokemon(broker_appeared_pokemon* brokerAppearedPokemon, t_buffer* buffer)
{
	// serializacion
	//1. uint32_t id;
	//2. uint32_t tamanioNombre;
	//3. char* nombrePokemon;
	//4. uint32_t posX;
	//5. uint32_t posY;

	buffer->size= sizeof(uint32_t) // id
			+ sizeof(uint32_t)*3 // posX, posY, tamanioNombre
			+ strlen(brokerAppearedPokemon->datos->nombrePokemon)+1; //longitud del strind nombre de pokemon

	buffer->stream = malloc(buffer->size);
	int offset = 0;


	memcpy(buffer->stream+offset,&(brokerAppearedPokemon->id_relativo),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(brokerAppearedPokemon->datos->tamanioNombre),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,(brokerAppearedPokemon->datos->nombrePokemon), brokerAppearedPokemon->datos->tamanioNombre);
	//ESTO ESTA MAL, NO ES SIZEOF
	offset+=brokerAppearedPokemon->datos->tamanioNombre;

	memcpy(buffer->stream+offset,&(brokerAppearedPokemon->datos->posX),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(brokerAppearedPokemon->datos->posY),sizeof(uint32_t));
	offset+=sizeof(uint32_t);
}

void serializar_broker_catch_pokemon(broker_catch_pokemon* brokerCatchPokemon, t_buffer* buffer)
{
	// serializacion
	//1. uint32_t tamanioNombre;
	//2. char* nombrePokemon;
	//3. uint32_t posX;
	//4. uint32_t posY;

	buffer->size= sizeof(uint32_t)*3 // posX, posY, tamanioNombre
			+ strlen(brokerCatchPokemon->datos->nombrePokemon)+1; //longitud del strind nombre de pokemon

	buffer->stream = malloc(buffer->size);
	int offset = 0;



	memcpy(buffer->stream+offset,&(brokerCatchPokemon->datos->tamanioNombre),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,(brokerCatchPokemon->datos->nombrePokemon),brokerCatchPokemon->datos->tamanioNombre);
	//ESTO ESTA MAL, NO ES SIZEOF
	offset+=(brokerCatchPokemon->datos->tamanioNombre);

	memcpy(buffer->stream+offset,&(brokerCatchPokemon->datos->posX),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(brokerCatchPokemon->datos->posY),sizeof(uint32_t));
	offset+=sizeof(uint32_t);
}

void serializar_broker_caught_pokemon(broker_caught_pokemon* brokerCaughtPokemon, t_buffer* buffer)
{
	// serializacion
	//2. uint32_t id;
	//1. uint32_t puedoAtraparlo;

	buffer->size= sizeof(uint32_t) * 2;// size de puedoAtraparlo


	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream+offset,&(brokerCaughtPokemon->id_relativo),sizeof(uint32_t));
		offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(brokerCaughtPokemon->datos->puedoAtraparlo),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

}

void serializar_broker_get_pokemon(broker_get_pokemon* brokerGetPokemon, t_buffer* buffer)
{
	//serializacion
	//1. uint32_t tamanioNombre;
	//2. char* nombrePokemon;

	buffer->size= sizeof(uint32_t)
			+ strlen(brokerGetPokemon->datos->nombrePokemon)+1; //longitud del string nombre de pokemon

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream+offset,&(brokerGetPokemon->datos->tamanioNombre),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,(brokerGetPokemon->datos->nombrePokemon),brokerGetPokemon->datos->tamanioNombre);
	//ESTO ESTA MAL, NO ES SIZEOF
	offset+=brokerGetPokemon->datos->tamanioNombre;

}

new_pokemon* transformarBrokerNewPokemon(broker_new_pokemon *newRecibido){
	new_pokemon *pokeTransformado = malloc(sizeof(new_pokemon));

	pokeTransformado->tamanioNombre = newRecibido->datos->tamanioNombre;
	pokeTransformado->nombrePokemon = malloc(sizeof(pokeTransformado-> tamanioNombre));
	pokeTransformado->nombrePokemon = newRecibido->datos->nombrePokemon;
	pokeTransformado->posX = newRecibido->datos->posX;
	pokeTransformado->posY = newRecibido->datos->posY;
	pokeTransformado->cantidadPokemon = newRecibido->datos->cantidadPokemon;

	return pokeTransformado;
}

appeared_pokemon* transformarBrokerAppearedPokemon(broker_appeared_pokemon *appRecibido){
	appeared_pokemon *pokeTransformado = malloc(sizeof(appeared_pokemon));

	pokeTransformado->tamanioNombre = appRecibido->datos->tamanioNombre;
	pokeTransformado->nombrePokemon = malloc(sizeof(pokeTransformado-> tamanioNombre));
	pokeTransformado->nombrePokemon = appRecibido->datos->nombrePokemon;
	pokeTransformado->posX = appRecibido->datos->posX;
	pokeTransformado->posY = appRecibido->datos->posY;

	return pokeTransformado;
}

get_pokemon* transformarBrokerGetPokemon(broker_get_pokemon *getRecibido){
	get_pokemon *pokeTransformado = malloc(sizeof(get_pokemon));

	pokeTransformado->tamanioNombre = getRecibido->datos->tamanioNombre;
	pokeTransformado->nombrePokemon = malloc(sizeof(pokeTransformado-> tamanioNombre));
	pokeTransformado->nombrePokemon = getRecibido->datos->nombrePokemon;

	return pokeTransformado;
}

catch_pokemon* transformarBrokerCatchPokemon(broker_catch_pokemon *catchRecibido){
	catch_pokemon *pokeTransformado = malloc(sizeof(catchRecibido));

	pokeTransformado->tamanioNombre = catchRecibido->datos->tamanioNombre;
	pokeTransformado->nombrePokemon = malloc(sizeof(pokeTransformado-> tamanioNombre));
	pokeTransformado->nombrePokemon = catchRecibido->datos->nombrePokemon;
	pokeTransformado->posX = catchRecibido->datos->posX;
	pokeTransformado->posY = catchRecibido->datos->posY;

	return pokeTransformado;
}

caught_pokemon* transformarBrokerCaughtPokemon(broker_caught_pokemon *caughtRecibido){
	caught_pokemon *pokeTransformado = malloc(sizeof(caught_pokemon));

	pokeTransformado->puedoAtraparlo = caughtRecibido->datos->puedoAtraparlo;

	return pokeTransformado;
}

