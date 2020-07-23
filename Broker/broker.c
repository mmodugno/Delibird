/*
 ============================================================================
 Name        : Broker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "broker.h"
#include <stdio.h>
#include <stdlib.h>



int main() {

	/*
	 *
	 Ejecución de compactación (para particiones dinámicas) o asociación de bloques (para buddy system). En este último, indicar que particiones se asociaron (indicar posición inicio de ambas particiones).
	 Ejecución de Dump de cache (solo informar que se solicitó el mismo).

	 *
	 */

	leer_config();

	signal(SIGUSR1, my_handler);
	//signal(SIGINT, my_handler);

	//mutex
	sem_init(&idsDeMensajes, 0, 1);
	sem_init(&usoMemoria, 0, 1);
	//sem_init(&llegadaMensajes,0,1);
	sem_init(&suscripcionAColaAPPEARED,0,1);
	sem_init(&suscripcionAColaCATCH,0,1);
	sem_init(&suscripcionAColaCAUGHT,0,1);
	sem_init(&suscripcionAColaGET,0,1);
	sem_init(&suscripcionAColaLOCALIZED,0,1);
	sem_init(&suscripcionAColaNEW,0,1);




	//int prueba = crear_conexion(ip_gameboy,puerto_gameboy);
	//productor-consumidor
	/*
	sem_init(&colaNew, 0, 0);
	sem_init(&colaAppeared, 0, 0);
	sem_init(&colaCatch, 0, 0);
	sem_init(&colaCaught, 0, 0);
	sem_init(&colaLocalized, 0, 0);
	sem_init(&colaGet, 0, 0);*/


	//logs implementados
	logSuscipcion = iniciar_logger("Suscripcion", log_file);
	logMensajeNuevo = iniciar_logger("Mensaje Nuevo", log_file);
	confirmacionRecepcion = iniciar_logger("Recepcion Mensaje", log_file); //ver bien donde va, cuando una suscrpicion reciba el mensaje, se tiene que loggear esto
	almacenadoMemoria = iniciar_logger("Almacenado Memoria", log_file); //debe indicar posicion de inicio de particion
	eliminacionMemoria = iniciar_logger("Eliminacion Pariticion Memoria",log_file); //debe indicar posicion de inicio
	compactacionMemoria = iniciar_logger("compactacionMemoria", log_file);
	logEnviarNuevo = iniciar_logger("Enviar Mensaje", log_file);
	logConexion = iniciar_logger("Conexion", log_file); //ver bien donde va
	dumpCache = iniciar_logger("dumpCache", log_file);

	iniciarMemoria();

	suscriptoresAppearedPokemon = list_create();
	suscriptoresCatchPokemon = list_create();
	suscriptoresCaughtPokemon = list_create();
	suscriptoresGetPokemon = list_create();
	suscriptoresLocalizedPokemon = list_create();
	suscriptoresNewPokemon = list_create();

	pthread_create(&hiloReciboMensajes, NULL, (void*) iniciar_servidor, NULL);

	pthread_create(&hiloNew_Envio, NULL, (void*) envioColaNewPokemon, NULL);
	pthread_create(&hiloAppeared_Envio, NULL, (void*) envioColaAppearedPokemon, NULL);
	pthread_create(&hiloCatch_Envio, NULL, (void*) envioColaCatchPokemon, NULL);
	pthread_create(&hiloCaught_Envio, NULL, (void*) envioColaCaughtPokemon, NULL);
	pthread_create(&hiloGet_Envio, NULL, (void*) envioColaGetPokemon, NULL);
	pthread_create(&hiloLocalized_Envio, NULL, (void*) envioColaLocalizedPokemon, NULL);

	pthread_detach(hiloReciboMensajes);

	pthread_detach(hiloNew_Envio);
	pthread_detach(hiloAppeared_Envio);
	pthread_detach(hiloCatch_Envio);
	pthread_detach(hiloCaught_Envio);
	pthread_detach(hiloGet_Envio);
	pthread_detach(hiloLocalized_Envio);

	//iniciar_servidor();


	sem_init(&terminoPrograma,0,0);

	sem_wait(&terminoPrograma);
/*
	*/

	//terminar conexiones logs y config
	//terminar_programa();
	return EXIT_SUCCESS;

}

t_log* iniciar_logger(char* tipoDeProceso, char* archivoLog) {

	//preguntar por el tipo de LOG_LEVEL
	return log_create(archivoLog, tipoDeProceso, 0, LOG_LEVEL_INFO);
}

void leer_config(void) {

	config = config_create("broker.config");

	ip_broker = config_get_string_value(config, "IP_BROKER");
	puerto_broker = config_get_string_value(config, "PUERTO_BROKER");

	ip_team = config_get_string_value(config, "IP_TEAM");
	puerto_team = config_get_string_value(config, "PUERTO_TEAM");

	ip_gameboy = config_get_string_value(config, "IP_GAMEBOY");
	puerto_gameboy = config_get_string_value(config, "PUERTO_GAMEBOY");

	ip_gamecard = config_get_string_value(config, "IP_GAMECARD");
	puerto_gamecard = config_get_string_value(config, "PUERTO_GAMECARD");


	tamanio_memoria = config_get_int_value(config, "TAMANO_MEMORIA");
	tamanio_minimo_particion = config_get_int_value(config,
			"TAMANO_MINIMO_PARTICION");
	algoritmo_memoria = config_get_string_value(config, "ALGORITMO_MEMORIA");
	algoritmo_particion_libre = config_get_string_value(config,
			"ALGORITMO_PARTICION_LIBRE");
	algoritmo_reemplazo = config_get_string_value(config,
			"ALGORITMO_REEMPLAZO");
	frecuencia_compactacion = config_get_int_value(config,
			"FRECUENCIA_COMPACTACION");
	log_file = config_get_string_value(config, "LOG_FILE");

}


void my_handler(int signum)
{
    if (signum == SIGUSR1)
    {
    	sem_wait(&usoMemoria);
        log_info(dumpCache,"voy a escribir un dump de la cache!");

        //TODO escribir archivo

        sem_post(&usoMemoria);
    }
    /*
    if(signum == SIGINT){
    	sem_wait(&usoMemoria);
    	flagTermino =0;
    	terminar_programa();

    	sem_post(&terminoPrograma);
    	sem_post(&usoMemoria);
    }*/
}
/*
 * t_log* logConexion;
 t_log* logSuscipcion;
 t_log* logMensajeNuevo;
 t_log* logEnviarNuevo;
 t_log* confirmacionRecepcion;
 t_log* almacenadoMemoria;
 t_log* eliminacionMemoria;
 t_log* compactacionMemoria;
 t_log* dumpCache;
 */


