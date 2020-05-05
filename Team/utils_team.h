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
#include<math.h>
#include<commons/collections/queue.h>

typedef enum{ //Para futura planificacion:
    NEW=0,
    READY=1,
    EXEC=2,
    BLOCK=3,
    EXIT=4,
}estadoEntrenador;

typedef struct{
	char* nombre;
    pthread_t hiloDeEntrenador;
    int estado;
    t_list* pokemones;
    t_list* objetivos;
    uint32_t posX;
    uint32_t posY;
    int cuantos_puede_cazar;
}entrenador;

typedef struct{
    char* nombre;
    uint32_t posX;
    uint32_t posY;
    uint32_t  tamanio_nombre;
}pokemon;

t_config* config;

void iniciar_config(void);
t_config* get_config(void);
void destruir_config(void);


int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char mensaje, int socket_cliente);
char* recibir_mensaje(int socket_cliente);
void liberar_conexion(int socket_cliente);

t_config* leer_config(void);
t_list* obtener_lista_posiciones(void);
t_list* obtener_lista_objetivos(void);
t_list* obtener_lista_pokemones(void);


entrenador* configurar_entrenador(char* nombre,char* posicion,char* pokemonsconfig, char* objetivosconfig);
t_list* hacer_entrenadores(void);



int distancia_entrenador_pokemon(entrenador entrenador, pokemon pokemon);

pokemon* hacer_pokemon(char* nombre, uint32_t posX, uint32_t posY);

void mover_entrenador(entrenador* entrenador,pokemon* pokemon);

int leer_retardo_cpu(void);
char* leer_ip_broker(void);


t_log* iniciar_log(char* proceso);
void log_cambio_de_cola(char * razon);
void log_movimiento_de_enntrenador(entrenador* entrenador);
void log_atrapar_pokemon(pokemon* poke);
void log_intercambio(entrenador* entrenador1,entrenador* entrenador2);
void log_reintentar_comunicacion(void);
void log_conexion_exitosa(void);
void log_fallo_de_conexion(void);

#endif /* TEAM_UTILS_TEAMH */
