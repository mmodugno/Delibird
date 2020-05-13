/*
 * team.c
 *
 *  Created on: 27 abr. 2020
 *      Author: utnso
 */


#include "utils_team.h"



int main(int argc, char* argv[]){

	printf(" Arrancando \n \n");

	variables_globales();



    pokemon* blacky = hacer_pokemon("blacky", 1, 6);
	planificar_entrenador(blacky);


	entrenador* ent_ready = list_get(entrenadores_en_ready,0);
	printf("entrenador n: %d, posicion (%d,%d",ent_ready->id,ent_ready->posX,ent_ready->posY);
	//Hacer hilo server que este esperando un appeared_pokemon, que despues si fije si nos sirve y meterlo en la lista de pokemones en mapa








printf(" \n Termino todo ok ");
}


    //aca deberiamos poner terminar_programa

