/*
 * team.c
 *
 *  Created on: 27 abr. 2020
 *      Author: utnso
 */


#include "team.h"


int main(int argc, char* argv[]){

	//POR AHORA DEJAMOS SOLO LA CONEXION CON BROKER PORQUE NO ESTAMOS SEGUROS SI HAY QUE CONECTAR CON GAMEBOY
	int conexionBroker;

	char* ipBroker;
	int puertoBroker;
	char* algoritmoPlanificacion;

	t_config* config = leer_config();

	ipBroker=config_get_string_value(config,"IP_BROKER");
	puertoBroker = config_get_int_value(config,"PUERTO_BROKER");
	algoritmoPlanificacion = config_get_string_value(config,"ALGORITMO_PLANIFICACION");

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



	t_log* iniciar_logger(char* tipoDeProceso){

		//preguntar por el tipo de LOG_LEVEL
		return log_create("team1.log",tipoDeProceso,0,LOG_LEVEL_INFO);
	}


	t_config* leer_config(void){

		return config_create("team1.config");

	}
