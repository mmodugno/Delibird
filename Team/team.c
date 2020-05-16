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


/*
 	pthread_t hilo_principal;
 	pthread_create(&hilo_principal,NULL,(void *) algoritmo_aplicado,NULL);

 	pthread_join(hilo_principal,NULL);
	//Hacer hilo server que este esperando un appeared_pokemon, que despues si fije si nos sirve y meterlo en la lista de pokemones en mapa

*/

 	//POSICIONES_ENTRENADORES=[1|2,3|7,5|5]

 	pokemon* blacky = hacer_pokemon("blacky", 3, 8);

 	planificar_entrenador(blacky);

 	entrenador* un_entrenador = list_get(entrenadores_en_ready,0);

 	printf("\n  nro de entrenador: %d y posicion: (%d,%d)", un_entrenador->id,un_entrenador->posX, un_entrenador->posY);




printf(" \n \n Termino todo ok ");
}


    //aca deberiamos poner terminar_programa

