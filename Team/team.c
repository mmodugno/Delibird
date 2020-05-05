/*
 * team.c
 *
 *  Created on: 27 abr. 2020
 *      Author: utnso
 */


#include "utils_team.h"



int main(int argc, char* argv[]){

    //POR AHORA DEJAMOS SOLO LA CONEXION CON BROKER PORQUE NO ESTAMOS SEGUROS SI HAY QUE CONECTAR CON GAMEBOY




    //conexion=crear_conexion(ipBroker,puertoBroker);


    int retardo;
    retardo = leer_retardo_cpu();


    t_log* log_de_team = iniciar_log();

    log_info(log_de_team ,"inicio de programa");



    t_queue * cola_ready = queue_create();
    t_queue* cola_block = queue_create();







}


    //aca deberiamos poner terminar_programa

