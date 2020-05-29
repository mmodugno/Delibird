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

int main(){


	/*
	 *
Ejecución de compactación (para particiones dinámicas) o asociación de bloques (para buddy system). En este último, indicar que particiones se asociaron (indicar posición inicio de ambas particiones).
Ejecución de Dump de cache (solo informar que se solicitó el mismo).

	 *
	 */
		leer_config();


		logConexion = iniciar_logger("Conexion",log_file);//ver bien donde va
		logSuscipcion = iniciar_logger("Suscripcion",log_file);
		logMensajeNuevo = iniciar_logger("Mensaje Nuevo",log_file);
		logEnviarNuevo = iniciar_logger("Enviar Mensaje",log_file);
		confirmacionRecepcion = iniciar_logger("Recepcion Mensaje",log_file); //ver bien donde va, cuando una suscrpicion reciba el mensaje, se tiene que loggear esto
		almacenadoMemoria = iniciar_logger("Almacenado Memoria",log_file); //debe indicar posicion de inicio de particion
		eliminacionMemoria = iniciar_logger("Eliminacion Pariticion Memoria",log_file); //debe indicar posicion de inicio
		/*compactacionMemoria= iniciar_logger("",log_file);
		dumpCache= iniciar_logger("",log_file);*/


		colaNewPokemon = queue_create();
		colaAppearedPokemon = queue_create();
		/*
		colaCatchPokemon;
		colaCaughtPokemon;
		colaGetPokemon;
		colaLocalizedPokemon;*/

		iniciar_servidor();




		//terminar conexiones logs y config
		terminar_programa();
		return EXIT_SUCCESS;

}


t_log* iniciar_logger(char* tipoDeProceso,char* archivoLog){

	//preguntar por el tipo de LOG_LEVEL
	return log_create(archivoLog,tipoDeProceso,0,LOG_LEVEL_INFO);
}


void leer_config(void){

	config = config_create("broker.config");

	ip_broker = config_get_string_value(config,"IP_BROKER");
	puerto_broker = config_get_string_value(config,"PUERTO_BROKER");
	tamanio_memoria = config_get_int_value(config,"TAMANO_MEMORIA");
	tamanio_minimo_particion = config_get_int_value(config,"TAMANO_MINIMO_PARTICION");
	algoritmo_memoria = config_get_string_value(config,"ALGORITMO_MEMORIA");
	algoritmo_particion_libre = config_get_string_value(config, "ALGORITMO_PARTICION_LIBRE");
	algoritmo_reemplazo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	frecuencia_compactacion = config_get_int_value(config,"FRECUENCIA_COMPACTACION");
	log_file = config_get_string_value(config,"LOG_FILE");


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

void terminar_programa(){

	log_destroy(logConexion);
	log_destroy(logSuscipcion);
	log_destroy(logMensajeNuevo);
	log_destroy(logEnviarNuevo);
	log_destroy(confirmacionRecepcion);
	log_destroy(almacenadoMemoria);
	log_destroy(eliminacionMemoria);
	log_destroy(compactacionMemoria);
	log_destroy(dumpCache);


	config_destroy(config);


}
