/*
 * utils_team.h
 *
 *  Created on: 27 abr. 2020
 *      Author: utnso
 */


#ifndef TEAM_UTILS_TEAMH
#define TEAM_UTILS_TEAMH
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<p.thread>

//ES ASI?
#include "/home/utnso/tp-2020-1c-The-X-Team/utils_en_comun/utils_en_comun.h"

typedef enum{ //Para futura planificacion:
    NEW=0,
    READY=1,
    EXEC=2,
    BLOCK=3,
    EXIT=4,
}estadoEntrenador;

typedef struct{
    pthread_t hiloDeEntrenador;
    int estado;
    t_list* pokemones;
    t_list* objetivos;
    uint32_t posX;
    uint32_t posY;
    //hace falta agregar su maximo de pokemones o podria ser una funcion?
}entrenador;

typedef struct{
    char* nombre;
    uint32_t posX;
    uint32_t posY;
}pokemon;


int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char mensaje, int socket_cliente);
char* recibir_mensaje(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void liberar_conexion(int socket_cliente);




#endif / TEAM_UTILS_TEAMH */
