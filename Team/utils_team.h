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


t_config* config;


t_config* leer_config(void);
t_list* entrenadores;
t_dictionary* objetivo_global;

t_list* pokemones_en_el_mapa;
t_list* pokemones_atrapados;

t_list* entrenadores_en_ready;


typedef enum{
    NEW=0,
    READY=1,
    EXEC=2,
	BLOCK_READY=3,
    BLOCK_ESPERANDO=4, //no sería block_catching?
	BLOCK_DEADLOCK=5,
    EXIT=6
}estadoEntrenador;

typedef struct{
    int estado;
    t_list* pokemones;
    t_list* objetivos;
    uint32_t posX;
    uint32_t posY;
    int cuantos_puede_cazar;
    int id;
    pthread_t hiloDeEntrenador;
}entrenador;

typedef struct{
    char* nombre;
    uint32_t posX;
    uint32_t posY;
    uint32_t  tamanio_nombre;
}pokemon;


typedef enum{
	FIFO=0,
	RR=1,
	SJFCD=2,
	SJFSD=3
};



pokemon* proximo_objetivo;

// FUNCIONES DE LA CONFIG //

t_list* obtener_lista_posiciones(void);
t_list* obtener_lista_objetivos(void);
t_list* obtener_lista_pokemones(void);
int leer_puerto_broker(void);
char* leer_ip_broker(void);
int leer_algoritmo_planificacion(void);
int leer_quantum(void);
int leer_estimacion_inicial(void);
int leer_tiempo_de_reconexion(void);
int leer_retardo_cpu(void);


// FUNCIONES DE LOS LOGS //
t_log* iniciar_log(char* proceso);
void log_algoritmo_de_planificacion(void);
void log_cambio_de_cola(char * razon);
void log_movimiento_de_entrenador(entrenador* entrenador);
void log_atrapar_pokemon(pokemon* poke);
void log_intercambio(entrenador* entrenador1,entrenador* entrenador2);
void log_reintentar_comunicacion(void);
void log_conexion_exitosa(void);
void log_fallo_de_conexion(void);



//globales
void variables_globales();

//entrenador
entrenador* configurar_entrenador(char* posicion,char* pokemonsconfig, char* objetivosconfig,int id);
void hacer_entrenadores(void);
void mover_entrenador(entrenador* entrenador,pokemon* pokemon);

//pokemon
pokemon* hacer_pokemon(char* nombre, uint32_t posX, uint32_t posY);
void aparece_nuevo_pokemon(pokemon* poke);
bool es_de_especie(char* nombre_poke);

//objetivo
void calcular_objetivo_global(void);
void agregar_un_objetivo(char * pokemon_a_agregar);
void quitar_un_objetivo(char* pokemon_a_quitar);

//distancias entre pokemon y entrenador
int distancia_entrenador_pokemon(entrenador* entrenador, pokemon* pokemon);
bool primer_entrenador_mas_cerca_de_pokemon(entrenador* entrenador1, entrenador* entrenador2);

//planificacion
bool se_puede_planificar(entrenador* entrenador);
void planificar_entrenador(pokemon* pokemon);
void procedimiento_de_caza(entrenador* un_entrenador);
void algoritmo_aplicado(void);





#endif /* TEAM_UTILS_TEAMH */
