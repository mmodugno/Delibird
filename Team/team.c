/*
 * team.c
 *
 *  Created on: 27 abr. 2020
 *      Author: utnso
 */


#include "team.h"


int main(int argc, char* argv[]){

	printf(" Arrancando \n \n");

	iniciar_servidor();


 	variables_globales();

sem_init(&entrenador_listo,0 ,0);


 	//sem_init(&(espera_de_movimiento),0,0);


 	//pthread_t hilo_principal;
 	//pthread_create(&hilo_principal,NULL,(void *) algoritmo_aplicado,NULL);



 	//PROBANDO TODA LA EJECUCION DE CAZA:
 	pokemon* pikapika = hacer_pokemon("Pikachu", 10, 30);
 	aparece_nuevo_pokemon(pikapika);





 	planifico_con_fifo();






 	//pthread_join(hilo_principal,NULL);
	//Hacer hilo server que este esperando un appeared_pokemon, que despues si fije si nos sirve y meterlo en la lista de pokemones en mapa




printf(" \n \n Termino todo ok ");
}


    //aca deberiamos poner terminar_programa

