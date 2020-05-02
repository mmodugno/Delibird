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
#include<pthread.h>
#include<commons/collections/list.h>
#include<commons/collections/dictionary.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/string.h>


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
    uint32_t  tamanio_nombre;
}pokemon;


int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char mensaje, int socket_cliente);
char* recibir_mensaje(int socket_cliente);
//void eliminar_paquete(t_paquete* paquete);
void liberar_conexion(int socket_cliente);
t_config* leer_config(void);
t_list* obtener_lista_posiciones(void);
t_list* obtener_lista_objetivos(void);
t_list* obtener_lista_pokemones(void);

void imprimirElementosLista (t_list *a);
entrenador* configurar_entrenador(char* posicion,char* pokemonsconfig, char* objetivosconfig);

t_list* hacer_entrenadores(void);


#endif /* TEAM_UTILS_TEAMH */
