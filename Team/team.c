/*
 * team.c
 *
 *  Created on: 27 abr. 2020
 *      Author: utnso
 */


#include "team.h"


int main(int argc, char* argv[]){

	printf(" Arrancando \n \n");

	cambioDeCola = iniciar_log("CAMBIO DE COLA");
	//log_info(log,"cambio de cola porque: %s ",razon);

	llegadaDeMensaje = iniciar_log("NUEVO MENSAJE");
	//INDICAR TIPO DEL MENSAJE Y SUS DATOS

	movimiento_entrenador = iniciar_log("MOVIMIENTO DE ENTRENADOR");
	//log_info(log,"entrenador %d: se movio a (%d,%d)",entrenador->id,entrenador->posX,entrenador->posY);

	operacion = iniciar_log("OPERACION");
	//ATRAPAR POKEMON
	//log_info(operacion,"pokemon: %s con posicion (%d, %d)",poke->nombre,poke->posX,poke->posY);

	//INTERCAMBIO
	//log_info(log,"intercambio entre entrenadores %d y %d",entrenador1->id,entrenador2->id);


	deadlock = iniciar_log("DEADLOCK");
	//inicio de algoritmo deadlock
	//resultado de analisis de deadlock


	resultado = iniciar_log("RESULTADO TEAM"); //al final

	comunicacion_broker = iniciar_log("COMUNICACION CON BROKER");
	//error de comunicacion, se realizara la operacion por default
	//inicio de proceso de reconexion
	//resultado de reconexion





	sem_init(&hay_entrenador,0,0);
	sem_init(&planificando,0,1);
	///////////////////////////////////////////
	variables_globales();



	//iniciar_servidor();

 	//sem_init(&(espera_de_movimiento),0,0);




 	pthread_t hilo_principal;
	pthread_create(&hilo_principal,NULL,(void *) algoritmo_aplicado,NULL);


	//algoritmo_aplicado();

 	//PROBANDO TODA LA EJECUCION DE CAZA cdeo este el hilo:

	pokemon* pikachu = hacer_pokemon("Pikachu", 10, 30);
	aparece_nuevo_pokemon(pikachu);

	//pokemon* charmander = hacer_pokemon("Charmander", 0,0);
	//aparece_nuevo_pokemon(charmander);


	//aparece_nuevo_pokemon(pikachu);
	//aparece_nuevo_pokemon(pikachu);




 	pthread_join(hilo_principal,NULL);
	//Hacer hilo server que este esperando un appeared_pokemon, que despues si fije si nos sirve y meterlo en la lista de pokemones en mapa




printf(" \n \n Termino todo ok ");
}


    //aca deberiamos poner terminar_programa
void terminar_programa(void){
	//Y por ultimo, para cerrar, hay que liberar lo que utilizamos (conexion, log y config) con las funciones de las commons y del TP mencionadas en el enunciado
	log_destroy(cambioDeCola);
	log_destroy(llegadaDeMensaje);
	log_destroy(movimiento_entrenador);
	log_destroy(operacion);
	log_destroy(deadlock);
	log_destroy(resultado);
	log_destroy(comunicacion_broker);
	config_destroy(config);
	//liberar_conexion(conexion);
}
