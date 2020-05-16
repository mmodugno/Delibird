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



 	pthread_t hilo_principal;
 	pthread_create(&hilo_principal,NULL,(void *) algoritmo_aplicado,NULL);

 	pthread_join(hilo_principal,NULL);
	//Hacer hilo server que este esperando un appeared_pokemon, que despues si fije si nos sirve y meterlo en la lista de pokemones en mapa





printf(" \n \n Termino todo ok ");
}


    //aca deberiamos poner terminar_programa

