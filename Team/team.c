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



    //uint32_t quantum;
    //uint32_t estimacion_inicial;

    t_config* config;
    config = leer_config();





    //conexion=crear_conexion(ipBroker,puertoBroker);


    //log_info(logger,"Comienzo");


    ipBroker=config_get_string_value(config,"IP_BROKER");
    puertoBroker = config_get_int_value(config,"PUERTO_BROKER");
    algoritmoPlanificacion = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
    tiempo_de_reconexion = config_get_int_value(config,"TIEMPO_RECONEXION");


    printf("Recibi estos datos: %s, %d, %s, %d  \n",ipBroker,puertoBroker,algoritmoPlanificacion,tiempo_de_reconexion);





    //log_info( logger, "inicio de programa");


    t_queue * cola_ready = queue_create();
    t_queue* cola_block = queue_create();







}


    //aca deberiamos poner terminar_programa

