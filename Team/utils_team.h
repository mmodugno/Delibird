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
#include "datos_broker.h"

#include"datos_team.h"

//LOGS:

t_log* cambioDeCola;
t_log* movimiento_entrenador;
t_log* llegadaDeMensaje;
t_log* resultado;
t_log* comunicacion_broker_error;
t_log* operacion_de_atrapar;
t_log* operacion_de_intercambio;
t_log* inicio_deadlock;
t_log* resultado_deadlock;
t_log* comunicacion_broker_reintento;
t_log* comunicacion_broker_resultado;

char* username;

t_config* config;

#define IP_TEAM "127.0.0.2"
#define PUERTO_TEAM "5002"

#define IP_BROKER "127.0.0.1"
#define PUERTO_BROKER "5003"

t_config* leer_config(void);
t_list* entrenadores;
t_dictionary* objetivo_global;

t_queue* pokemones_en_el_mapa;
t_list* pokemones_atrapados;

t_list* entrenadores_new;
t_list* entrenadores_finalizados;
t_list* entrenadores_en_deadlock;
t_queue* entrenadores_block_ready;
t_queue* entrenadores_blocked;
t_queue* entrenadores_ready;

char* nombre_pokemon;
char* archivo_config;
sem_t en_ejecucion;
sem_t hay_entrenador;
sem_t nuevo_pokemon;

//Metricas
int cant_deadlocks;
int cant_deadlocks_resueltos;
int cambio_contexto;


bool broker_conectado;

int conexionBroker;
int quantum;
int entrenador_deadlock;
int validar_deadlock;

typedef struct{
    char* nombre;
    uint32_t posX;
    uint32_t posY;
    uint32_t  tamanio_nombre;
}pokemon;


typedef struct{
    t_list* pokemones;
    t_list* objetivos;
    uint32_t posX;
    uint32_t posY;
    int cuantos_puede_cazar;
    int id;
    pthread_t hiloDeEntrenador;
    sem_t sem_entrenador;
    sem_t espera_de_catch;
    pokemon* objetivo_proximo;
    int ciclos_cpu;
    //VER:
    sem_t nuevoPoke;

}entrenador;



typedef enum{
	FIFO=0,
	RR=1,
	SJFCD=2,
	SJFSD=3
}algoritmoPlanificacion;


typedef struct{
	entrenador* primero;
	entrenador* segundo;
}par_entrenadores;



entrenador* entrenador_exec;
pokemon* proximo_objetivo;

// FUNCIONES DE LA CONFIG //
t_list* crear_lista(char** array);
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
int leer_alpha(void);


// FUNCIONES DE LOS LOGS //
t_log* iniciar_log(char* proceso);


// FUNCIONES DE CONEXIONES //
void iniciar_servidor(void);
void esperar_cliente(int);
//void* recibir_mensaje(int socket_cliente, int* size);
int recibir_operacion(int);
void process_request(int cod_op, int cliente_fd);
void serve_client(int *socket);
int crear_conexion(char *ip, char* puerto);
int conectarse_con_broker(void);



//globales
void variables_globales();

//ENTRENADOR
entrenador* configurar_entrenador(char* posicion,char* pokemonsconfig, char* objetivosconfig,int id);
void hacer_entrenadores(void);
void mover_entrenador(entrenador* entrenador,int x, int y);
void mover_entrenador_RR(entrenador* entrenador,int x, int y);
void disminuir_cuantos_puede_cazar(entrenador* un_entrenador);
bool puede_cazar(entrenador* entrenador);
bool entrenador_en_exec(entrenador* un_entrenador);
bool cumplio_objetivo(entrenador* un_entrenador);
void analizar_proxima_cola(entrenador* un_entrenador);
void printear_lista_entrenadores(t_list* lista);
void bloquear_entrenador(entrenador* un_entrenador);

//POKEMON
pokemon* hacer_pokemon(char* nombre, uint32_t posX, uint32_t posY, uint32_t tamanio);
void aparece_nuevo_pokemon(pokemon* poke);
bool es_de_especie(char* nombre_poke);
void sacar_pokemones_repetidos(t_list* objetivos, t_list* pokemones);
bool pokemon_repetido(char* nombre);

//OBJETIVO GLOBAL
void calcular_objetivo_global(void);
void agregar_un_objetivo(char * pokemon_a_agregar);
void quitar_un_objetivo(char* pokemon_a_quitar);



//DISTANCIAS
int distancia_entrenador_pokemon(entrenador* entrenador, pokemon* pokemon);
bool primer_entrenador_mas_cerca_de_pokemon(entrenador* entrenador1, entrenador* entrenador2);

//PLANIFICACION
bool se_puede_planificar(entrenador* entrenador);
void planificar_entrenador(void);
void procedimiento_de_caza(entrenador* un_entrenador);
void algoritmo_aplicado(void);
void planifico_con_fifo(void);
void terminar_ejecucion_entrenador(void);
bool validacion_nuevo_pokemon(void);
bool hay_pokemon_y_entrenador(void);
void planifico_con_RR(void);


//DEADLOCK
void planificar_deadlock(entrenador* entrenador0,entrenador* entrenador1);
void manejar_deadlock(void);
bool hay_deadlock(void);
void planificar_deadlock_RR(entrenador* entrenador0,entrenador* entrenador1);
void espera_de_deadlock(void);


//Mensajes
void denegar_catch(entrenador* un_entrenador);
void confirmacion_de_catch(entrenador* un_entrenador);
void esperar_respuesta_catch(entrenador* un_entrenador);
void enviar_catch(entrenador* un_entrenador,broker_catch_pokemon *catchAEnviar);

//	Metricas

void cpu_por_entrenador(void);
void cpu_team(void);




#endif /* TEAM_UTILS_TEAMH */
