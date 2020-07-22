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


	iniciar_logs();


	sem_init(&hay_entrenador,0,0);
	sem_init(&nuevo_pokemon,0,0);

	sem_init(&mutex_lista,0,1);
	sem_init(&en_ejecucion,0,1);
	sem_init(&semaforo_mensaje,0,1);

	lista_ids_get = list_create();

	variables_globales();



	char* algoritmo = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
	printf("Algoritmo de planificacion = %s \n \n",algoritmo);

	//conectarse_con_broker();

	pthread_t hilo_de_conexion_con_broker;
	pthread_create(&hilo_de_conexion_con_broker,NULL,(void *) conexion_broker,NULL);


	pthread_t hilo_principal;

	pthread_create(&hilo_principal,NULL,(void *) algoritmo_aplicado,NULL);

	conexionBroker = crear_conexion(IP_BROKER,PUERTO_BROKER);

	if(conexionBroker){


		suscribirnos_cola_caught();
		suscribirnos_cola_localized();
		suscribirnos_cola_appeared();

		//int j;

		//Mando un get por cada uno de mis objetivos globales.
		dictionary_iterator(objetivo_global,enviar_get_por_objetivo); //(char*,void*)

	}



	pthread_create(&hilo_servidor,NULL,(void *) iniciar_servidor,NULL);

	//SUSCRIPCION A COLAS SI NOS CONECTAMOS AL BROKER
	//Intentar conectarse al broker TODO


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

	//	PRUEBAS COMPLETAS DE TP

/*

	pokemon* jolteon = hacer_pokemon("Jolteon",2,2,sizeof("Jolteon"));
		aparece_nuevo_pokemon(jolteon);

	pokemon* flareon = hacer_pokemon("Flareon",4,6,sizeof("Flareon"));
		aparece_nuevo_pokemon(flareon);

	pokemon* umbreon = hacer_pokemon("Umbreon",10,6,sizeof("Umbreon"));
		aparece_nuevo_pokemon(umbreon);

	pokemon* espeon = hacer_pokemon("Espeon",7,1,sizeof("Espeon"));
		aparece_nuevo_pokemon(espeon);

	pokemon* vaporeon = hacer_pokemon("Vaporeon",4,10,sizeof("Vaporeon"));
		aparece_nuevo_pokemon(vaporeon);
*/


 	pthread_join(hilo_principal,NULL);

 	//imprimir_metricas();

 	loggear_metricas();

	terminar_programa();
}

void enviar_get_por_objetivo(char* nombrePoke,void* cantidad){

	//todo ver con chicos
	broker_get_pokemon* getAEnviar= malloc(sizeof(broker_get_pokemon));
	getAEnviar->datos = malloc(sizeof(get_pokemon));
	enviar_get(nombrePoke,getAEnviar);

}

void suscribirnos_cola_caught() {
	int suscripcionCaught;
	meSuscriboCaught = malloc(sizeof(suscriptor));

	meSuscriboCaught->nombreDeSuscriptor = "TEAM";
	meSuscriboCaught->tamanioNombreSucriptor = strlen(meSuscriboCaught->nombreDeSuscriptor) + 1;

	meSuscriboCaught->tipoDeCola = CAUGHT_POKEMON;

	suscripcionCaught = crear_conexion(IP_BROKER, PUERTO_BROKER);

	if (suscripcionCaught != -1) {
		enviar_pedido_suscripcion(meSuscriboCaught, suscripcionCaught);
		liberar_conexion(suscripcionCaught);
	}

	free(meSuscriboCaught);
}

void suscribirnos_cola_localized(){
	 meSuscriboLocalized = malloc(sizeof(suscriptor));

	int suscripcionLocalized;

	meSuscriboLocalized->nombreDeSuscriptor = "TEAM";
	meSuscriboLocalized->tamanioNombreSucriptor = strlen(meSuscriboLocalized->nombreDeSuscriptor) + 1;

	meSuscriboLocalized->tipoDeCola = LOCALIZED_POKEMON;

	suscripcionLocalized = crear_conexion(IP_BROKER,PUERTO_BROKER);

	if(suscripcionLocalized != -1){
		enviar_pedido_suscripcion(meSuscriboLocalized, suscripcionLocalized);
		liberar_conexion(suscripcionLocalized);
	}
	free(meSuscriboLocalized);
}

void suscribirnos_cola_appeared() {

	int suscripcionAppeared;

	meSuscriboAppeared = malloc(sizeof(suscriptor));

	meSuscriboAppeared->nombreDeSuscriptor = "TEAM";
	meSuscriboAppeared->tamanioNombreSucriptor = strlen(meSuscriboAppeared->nombreDeSuscriptor) + 1;

	meSuscriboAppeared->tipoDeCola = APPEARED_POKEMON;

	suscripcionAppeared = crear_conexion(IP_BROKER, PUERTO_BROKER);

	if (suscripcionAppeared != -1) {
		enviar_pedido_suscripcion(meSuscriboAppeared, suscripcionAppeared);
		liberar_conexion(suscripcionAppeared);
	}
	free(meSuscriboAppeared);
}


void terminar_programa(void){

	int conexion = conectarse_con_broker();

	if(conexion > 0){
		enviar_pedido_desuscripcion( meSuscriboCaught,conexion);
		liberar_conexion(conexion);
	}

	int conexion = conectarse_con_broker();
	if(conexion > 0){
		enviar_pedido_desuscripcion( meSuscriboLocalized,conexion);
		liberar_conexion(conexion);
	}

	int conexion = conectarse_con_broker();
	if(conexion > 0){
		enviar_pedido_desuscripcion( meSuscriboAppeared,conexion);
		liberar_conexion(conexion);
	}


	destruir_logs();

	if(leer_algoritmo_planificacion() == FIFO || leer_algoritmo_planificacion() == RR){

		queue_destroy(entrenadores_block_ready);
	}
	else{
		list_destroy(lista_entrenadores_block_ready);
		list_destroy(lista_entrenadores_ready);
	}
	queue_destroy(entrenadores_ready);
	list_destroy(entrenadores);
	list_destroy(pokemones_atrapados);
	list_destroy(entrenadores_finalizados);
	list_destroy(entrenadores_en_deadlock);

	queue_destroy(pokemones_en_el_mapa);

	list_destroy(entrenadores_blocked);

	config_destroy(config);



}



void destruir_logs(void){
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

int i;
	for( i= 0; i < list_size(entrenadores); i++){
			entrenador* entrenador = list_get(entrenadores, i);
			log_info(resultado,"Ciclos de cpu entrenador %d: %d", entrenador->id, entrenador->ciclos_cpu);
		}
int j;
	int cpu_totales = 0;
		for(j= 0; i < list_size(entrenadores); j++){
		entrenador* entrenador = list_get(entrenadores, j);
		cpu_totales += entrenador->ciclos_cpu;
		}

	log_info(resultado,"Ciclos de cpu totales: %d", cpu_totales);
	log_info(resultado,"Cantidad de deadlocks producidos: %d", cant_deadlocks);
	log_info(resultado,"Cantidad de deadlocks resueltos: %d", cant_deadlocks_resueltos);
}
