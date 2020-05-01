/*
 * team.c
 *
 *  Created on: 27 abr. 2020
 *      Author: utnso
 */


#include "utils_team.h"



int main(int argc, char* argv[]){

    //POR AHORA DEJAMOS SOLO LA CONEXION CON BROKER PORQUE NO ESTAMOS SEGUROS SI HAY QUE CONECTAR CON GAMEBOY

	int tiempo_de_reconexion;
	char* ipBroker;
    int puertoBroker;
    char* algoritmoPlanificacion;


    //uint32_t retardo_ciclo_cpu;
    //uint32_t quantum;
    //uint32_t estimacion_inicial;
    char* archivo_log; //   --->           LOG_FILE=/home/utnso/Team/log_team1.txt -> separar por / y agarrar el ultimo elemento


    t_config* config;
    config = leer_config();

    //t_log* logger;
    //logger = iniciar_logger();
    //conexion=crear_conexion(ipBroker,puertoBroker);


    //log_info(logger,"Comienzo");


    ipBroker=config_get_string_value(config,"IP_BROKER");
    puertoBroker = config_get_int_value(config,"PUERTO_BROKER");
    algoritmoPlanificacion = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
    tiempo_de_reconexion = config_get_int_value(config,"TIEMPO_RECONEXION");

    printf("Recibi estos datos: %s, %d, %s, %d",ipBroker,puertoBroker,algoritmoPlanificacion,tiempo_de_reconexion);






    //Aca si van las colas de estado de procesos y el algoritmo de sincronizacion, creo



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
    //Poner nuestros logs aca:
    //t_log* logConexion=iniciar_logger("Conexion");
    ////////////////////////////////////////////////////////////////////////////////



    t_log* iniciar_logger(char* tipoDeProceso,char* archivoLog){
        return log_create(archivoLog,tipoDeProceso,0,LOG_LEVEL_INFO);
    }


    /*t_config* leer_config(char* archivoConfig){
        return config_create(archivoConfig);
    }
    */

}


    //aca deberiamos poner terminar_programa

