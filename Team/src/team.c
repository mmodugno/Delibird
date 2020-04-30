/*
 * team.c
 *
 *  Created on: 27 abr. 2020
 *      Author: utnso
 */


#include "team.h"
#include "/home/utnso/workspace/tp-2020-1c-The-X-Team/Game-watch-client/log.h"



int main(int argc, char argv[]){

    //POR AHORA DEJAMOS SOLO LA CONEXION CON BROKER PORQUE NO ESTAMOS SEGUROS SI HAY QUE CONECTAR CON GAMEBOY
    char* ipBroker;
    int puertoBroker;
    char* algoritmoPlanificacion;

    t_config config = leer_config();

    ipBroker=config_get_string_value(config,"IP_BROKER");
    puertoBroker = config_get_int_value(config,"PUERTO_BROKER");
    algoritmoPlanificacion = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
    //leer todo de config




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


    t_config* leer_config(char* archivoConfig){
        return config_create(archivoConfig);
    }
    t_config* leer_config(void){
            return config_create("team1.config");
    }

    //aca deberiamos poner terminar_programa
