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
#include<semaphore.h>

#include"datos_team.h"

//LOGS:

t_log* cambioDeCola;
t_log* movimiento_entrenador;
t_log* operacion;
t_log* deadlock;
t_log* llegadaDeMensaje;
t_log* resultado;
t_log* comunicacion_broker;


t_config* config;

#define IP_TEAM "127.0.0.2"
#define PUERTO_TEAM "5002"



t_config* leer_config(void);
t_list* entrenadores;
t_dictionary* objetivo_global;

t_list* pokemones_en_el_mapa;
t_list* pokemones_atrapados;

t_list* entrenadores_en_ready;
t_list* entrenadores_finalizados;
t_list* entrenadores_en_deadlock;

char* nobmre_objetivoconfig;


sem_t entrenador_listo;
sem_t en_ejecucion;
sem_t hay_entrenador;
sem_t planificando;




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
    sem_t sem_entrenador;
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
}algoritmoPlanificacion;


entrenador* entrenador_exec;
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


// FUNCIONES DE CONEXIONES //
void iniciar_servidor(void);
void esperar_cliente(int);
//void* recibir_mensaje(int socket_cliente, int* size);
int recibir_operacion(int);
void process_request(int cod_op, int cliente_fd);
void serve_client(int *socket);


//globales
void variables_globales();

//entrenador
entrenador* configurar_entrenador(char* posicion,char* pokemonsconfig, char* objetivosconfig,int id);
void hacer_entrenadores(void);
void mover_entrenador(entrenador* entrenador,pokemon* pokemon);
void disminuir_cuantos_puede_cazar(entrenador* un_entrenador);
bool puede_cazar(entrenador* entrenador);
bool entrenador_en_exec(entrenador* un_entrenador);
bool cumplio_objetivo(entrenador* un_entrenador);

//pokemon
pokemon* hacer_pokemon(char* nombre, uint32_t posX, uint32_t posY);
void aparece_nuevo_pokemon(pokemon* poke);
bool es_de_especie(char* nombre_poke);
void sacar_pokemones_repetidos(t_list* objetivos, t_list* pokemones);
bool pokemon_repetido(char* nombre);

//objetivo
void calcular_objetivo_global(void);
void agregar_un_objetivo(char * pokemon_a_agregar);
void quitar_un_objetivo(char* pokemon_a_quitar);
void mostrar_objetivo_global(char* key, void*value);
void mostrar(void);

//distancias entre pokemon y entrenador
int distancia_entrenador_pokemon(entrenador* entrenador, pokemon* pokemon);
bool primer_entrenador_mas_cerca_de_pokemon(entrenador* entrenador1, entrenador* entrenador2);

//planificacion
bool se_puede_planificar(entrenador* entrenador);
void planificar_entrenador(pokemon* pokemon);
void procedimiento_de_caza(entrenador* un_entrenador);
void algoritmo_aplicado(void);
void planifico_con_fifo(void);

//Mensajes
void denegar_catch(void);
void confirmacion_de_catch(void);


#endif /* TEAM_UTILS_TEAMH */
