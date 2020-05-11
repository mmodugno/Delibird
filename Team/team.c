/*
 * team.c
 *
 *  Created on: 27 abr. 2020
 *      Author: utnso
 */


#include "utils_team.h"



int main(int argc, char* argv[]){

	t_queue* pokemones_sueltos = queue_create();

	//Hacer hilo server que este esperando un appeared_pokemon, que despues si fije si nos sirve:
	//si nos sirve, lo agrega a la cola de pokemones_sueltos, sino, lo descarta.

	 hacer_entrenadores();


	pokemon* charmander = hacer_pokemon("Charmander", 1, 6);
	queue_push(pokemones_sueltos,charmander);


    //t_queue * cola_ready = queue_create();
   // t_queue* cola_block = queue_create();


}


    //aca deberiamos poner terminar_programa

