/*
 * team.c
 *
 *  Created on: 27 abr. 2020
 *      Author: utnso
 */


#include "utils_team.h"



int main(int argc, char* argv[]){


	variables_globales();

	//Hacer hilo server que este esperando un appeared_pokemon, que despues si fije si nos sirve:
	//si nos sirve, lo agrega a la cola de pokemones_sueltos, sino, lo descarta.


	 //iniciar_team();

	int tiempo_rec = leer_tiempo_de_reconexion();
	printf("tiempo: %d",tiempo_rec);

	//entrenador* entrenado = list_get(entrenadores,0);
	//printf("nombre: %d",entrenado->id);




}


    //aca deberiamos poner terminar_programa

