/*
 * team.c
 *
 *  Created on: 27 abr. 2020
 *      Author: utnso
 */


#include "team.h"
#include "/home/utnso/workspace/tp-2020-1c-The-X-Team/Game-watch-client/log.h"



int main(int argc, char* argv[]){

	//POR AHORA DEJAMOS SOLO LA CONEXION CON BROKER PORQUE NO ESTAMOS SEGUROS SI HAY QUE CONECTAR CON GAMEBOY


	int pid;
    pid = fork();
    if (pid < 0){
    	fprintf(stderr, "falló");
        exit(-1);
    }else if (pid == 0){
        execlp("/bin/ls","ls",NULL);
    }else {
        wait(NULL);
        printf("Child Complete");
        exit(0);

    }

    //Aca si van las colas de estado de procesos! y el algoritmo de sincronizacion, creo



	////////////////////////////////////////////////////////////////////////////////
	/*Cambio de un entrenador de cola de planificación (indicando la razón del porqué).
	Movimiento de un entrenador (indicando la ubicación a la que se movió).
	Operación de atrapar (indicando la ubicación y el pokemon a atrapar).
	Operación de intercambio (indicando entrenadores involucrados).
	Inicio de algoritmo de detección de deadlock.
	Resultado de algoritmo de detección de deadlock.
	Llegada de un mensaje (indicando el tipo del mismo y sus datos).
	Resultado del Team (especificado anteriormente).
	Errores de comunicación con el Broker (indicando que se realizará la operación por default).
	Inicio de proceso de reintento de comunicación con el Broker.
	Resultado de proceso de reintento de comunicación con el Broker.*/



	///////////////////////////LOGS OBLIGATORIOS/////////////////////////////////////
	t_log* logConexion=iniciar_logger("Conexion");
	t_log* logSuscipcion=iniciar_logger("Suscripcion");
	t_log* logMensajeNuevo=iniciar_logger("Mensaje Nuevo");
	t_log* logEnviarNuevo= iniciar_logger("Enviar Mensaje");
	////////////////////////////////////////////////////////////////////////////////



	t_log* iniciar_logger(char* tipoDeProceso,char* archivoLog){

		//preguntar por el tipo de LOG_LEVEL
		return log_create(archivoLog,tipoDeProceso,0,LOG_LEVEL_INFO);
	}


	t_config* leer_config(char* archivoConfig){

		return config_create(archivoConfig);



	}

	//aca deberiamos poner terminar_programa

