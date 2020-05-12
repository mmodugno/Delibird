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

t_list* pokemones_sueltos;
t_config* config;
t_config* leer_config(void);



t_list* entrenadores;
t_dictionary* objetivo_global;

typedef enum{
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
    int cuantos_puede_cazar;
    int id;
}entrenador;

typedef struct{
    char* nombre;
    uint32_t posX;
    uint32_t posY;
    uint32_t  tamanio_nombre;
    uint32_t cantidad;
}pokemon;


/* BORRAR?
 *
typedef struct{
	char* especie;
	int cantidad;
}pokemon_objetivo;

*/






// FUNCIONES DE LA CONFIG //

t_list* obtener_lista_posiciones(void);
t_list* obtener_lista_objetivos(void);
t_list* obtener_lista_pokemones(void);
int leer_puerto_broker(void);
char* leer_ip_broker(void);
char* leer_algoritmo_planificacion(void);
int leer_quantum(void);
int leer_estimacion_inicial(void);
int leer_tiempo_de_reconexion(void);





void variables_globales();




entrenador* configurar_entrenador(char* posicion,char* pokemonsconfig, char* objetivosconfig,int id);
void hacer_entrenadores(void);


int distancia_entrenador_pokemon(entrenador entrenador, pokemon pokemon);

pokemon* hacer_pokemon(char* nombre, uint32_t posX, uint32_t posY);

void mover_entrenador(entrenador* entrenador,pokemon* pokemon);

void calcular_objetivo_global(void);
void agregar_un_objetivo(char * pokemon_a_agregar);

bool se_puede_planificar(entrenador* entrenador);

void planificar_entrenador(t_list* entrenadores);


#endif /* TEAM_UTILS_TEAMH */
