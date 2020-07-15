/*
 * team.c
 *
 *  Created on: 27 abr. 2020
 *      Author: utnso
 */


#include "team.h"


int main(int argc, char* argv[]){

	if(argc == 1){
			printf("No hay parametros suficientes\n");
			return 2;
		}
	if(argc == 2){
			archivo_config = argv[1];
			}

	username = malloc(strlen("TEAM")+1);
	username = "TEAM";


	printf(" Arrancando \n \n");


	iniciar_logs();


	sem_init(&hay_entrenador,0,0);
	sem_init(&nuevo_pokemon,0,0);

	sem_init(&en_ejecucion,0,1);
	sem_init(&semaforo_mensaje,0,1);





	variables_globales();




	char* algoritmo = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
	printf("Algoritmo de planificacion = %s \n \n",algoritmo);

	conectarse_con_broker();


	pthread_t hilo_principal;


	pthread_create(&hilo_principal,NULL,(void *) algoritmo_aplicado,NULL);


	pthread_create(&hilo_servidor,NULL,(void *) iniciar_servidor,NULL);
	//iniciar_servidor();


 //PRUEBAS DE TP:

/*
	pokemon* pikachu = hacer_pokemon("Pikachu",9, 9,sizeof("Pikachu"));
		aparece_nuevo_pokemon(pikachu);

	pokemon* squirte = hacer_pokemon("Squirtle",5,2,sizeof("Squirtle"));
		aparece_nuevo_pokemon(squirte);

	pokemon* onix = hacer_pokemon("Onix",2,8,sizeof("Onix"));
		aparece_nuevo_pokemon(onix);

	pokemon* gengar = hacer_pokemon("Gengar",6,6,sizeof("Gengar"));
		aparece_nuevo_pokemon(gengar);

	pokemon* squirte2 = hacer_pokemon("Squirtle",5,5,sizeof("Squirtle"));
		aparece_nuevo_pokemon(squirte2);

*/



 	pthread_join(hilo_principal,NULL);



 	imprimir_metricas();
 	//loggear_metricas();

	//terminar_programa();

}


    //aca deberiamos poner terminar_programa
void terminar_programa(void){
	//Y por ultimo, para cerrar, hay que liberar lo que utilizamos (conexion, log y config) con las funciones de las commons y del TP mencionadas en el enunciado
	log_destroy(cambioDeCola);
	log_destroy(llegadaDeMensaje);
	log_destroy(movimiento_entrenador);
	log_destroy(operacion_de_atrapar);
	log_destroy(operacion_de_intercambio);
	log_destroy(resultado);
	log_destroy(inicio_deadlock);
	log_destroy(resultado_deadlock);
	log_destroy(comunicacion_broker_error);
	log_destroy(comunicacion_broker_reintento);
	log_destroy(comunicacion_broker_resultado);

	list_destroy(entrenadores);
	list_destroy(pokemones_atrapados);
	list_destroy(entrenadores_finalizados);
	list_destroy(entrenadores_en_deadlock);

	queue_destroy(pokemones_en_el_mapa);
	queue_destroy(entrenadores_ready);
	queue_destroy(entrenadores_block_ready);
	queue_destroy(entrenadores_blocked);

	config_destroy(config);
	//liberar_conexion(conexion);
}

void iniciar_logs(void){
	cambioDeCola = iniciar_log("CAMBIO DE COLA"); //Detallar razon
	movimiento_entrenador = iniciar_log("MOVIMIENTO DE ENTRENADOR");//indicando la ubicación a la que se movió).
	operacion_de_atrapar = iniciar_log("OPERACION ATRAPAR"); //log_info(operacion_de_atrapar,"pokemon: %s con posicion (%d, %d)",poke->nombre,poke->posX,poke->posY);
	operacion_de_intercambio = iniciar_log("OPERACION INTERCAMBIO"); //log_info(log,"intercambio entre entrenadores %d y %d",entrenador1->id,entrenador2->id);
	inicio_deadlock = iniciar_log("DETECCION_DEADLOCK");
	resultado_deadlock = iniciar_log("RESULTADO_DE_DEADLOCK");
	llegadaDeMensaje = iniciar_log("NUEVO MENSAJE");//INDICAR TIPO DEL MENSAJE Y SUS DATOS
	resultado = iniciar_log("RESULTADO TEAM");
	comunicacion_broker_error = iniciar_log("ERROR EN COMUNICACION"); //se realizara los resultados por default
	comunicacion_broker_reintento = iniciar_log("REINTENTO DE COMUNICACION");
	comunicacion_broker_resultado = iniciar_log("RESULTADO DE COMUNICACION");

}



void imprimir_metricas(void){
	cpu_por_entrenador();

	 cpu_team();

	 printf("Cantidad de deadlocks producidos: %d \n", cant_deadlocks);

	 printf("Cantidad de deadlocks resueltos: %d \n", cant_deadlocks_resueltos);

	 printf("Cantidad de cambios de contexto: %d \n", cambio_contexto);

}

void loggear_metricas(void){


	for(int i= 0; i < list_size(entrenadores); i++){
			entrenador* entrenador = list_get(entrenadores, i);
			log_info(resultado,"Ciclos de cpu entrenador %d: %d", entrenador->id, entrenador->ciclos_cpu);
		}

	int cpu_totales = 0;
		for(int i= 0; i < list_size(entrenadores); i++){
		entrenador* entrenador = list_get(entrenadores, i);
		cpu_totales += entrenador->ciclos_cpu;
		}

	log_info(resultado,"Ciclos de cpu totales: %d", cpu_totales);
	log_info(resultado,"Cantidad de deadlocks producidos: %d", cant_deadlocks);
	log_info(resultado,"Cantidad de deadlocks resueltos: %d", cant_deadlocks_resueltos);
}
