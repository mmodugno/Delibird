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


 	//sem_init(&entrenador_listo,0 ,0);

 	//pthread_t hilo_principal;
 	//pthread_create(&hilo_principal,NULL,(void *) algoritmo_aplicado,NULL);



 	//PROBANDO TODA LA EJECUCION DE CAZA:
 	//pokemon* pikapika = hacer_pokemon("Pikachu", 1, 3);
 	//aparece_nuevo_pokemon(pikapika);



 	entrenador* un_entrenador = list_get(entrenadores,0);

 		  printf(" \n pokemones: \n  ");

 		   for(int i =0; i< list_size(un_entrenador->pokemones);i++){
 			   char * poke = list_get(un_entrenador->pokemones,i);
 			  printf(" %s    " ,poke);
 		   }

 		  printf("\n Objetivos: \n  ");

 		  for(int i =0; i< list_size(un_entrenador->objetivos);i++){
 			  char * obj = list_get(un_entrenador->objetivos,i);
 			  printf(" %s    " ,obj);
 		 }










 	//pthread_join(hilo_principal,NULL);
	//Hacer hilo server que este esperando un appeared_pokemon, que despues si fije si nos sirve y meterlo en la lista de pokemones en mapa




printf(" \n \n Termino todo ok ");
}


    //aca deberiamos poner terminar_programa

