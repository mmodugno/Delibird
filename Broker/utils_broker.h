/*
 * utils_broker.h
 *
 *  Created on: 30 abr. 2020
 *      Author: utnso
 */

#ifndef UTILS_BROKER_H_
#define UTILS_BROKER_H_


#include<readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<commons/collections/queue.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include<pthread.h>
#include "utils_en_comun.h"
#include<commons/collections/list.h>


char* ip_broker;
char* puerto_broker;
uint32_t tamanio_memoria;
uint32_t tamanio_minimo_particion;
char* algoritmo_memoria;
char* algoritmo_reemplazo;
char* algoritmo_particion_libre;
uint32_t frecuencia_compactacion;
char* log_file;

t_config* config;


pthread_t thread;

t_queue* colaNewPokemon;
t_queue* colaAppearedPokemon;

t_queue* colaCatchPokemon;
t_queue* colaCaughtPokemon;
t_queue* colaGetPokemon;
t_queue* colaLocalizedPokemon;

t_queue* suscriptoresNewPokemon;
t_queue* suscriptoresAppearedPokemon;

t_queue* suscriptoresCatchPokemon;
t_queue* suscriptoresCaughtPokemon;
t_queue* suscriptoresGetPokemon;
t_queue* suscriptoresLocalizedPokemon;

t_log* logConexion;
t_log* logSuscipcion;
t_log* logMensajeNuevo;
t_log* logEnviarNuevo;
t_log* confirmacionRecepcion;
t_log* almacenadoMemoria;
t_log* eliminacionMemoria;
t_log* compactacionMemoria;
t_log* dumpCache;


void* recibir_buffer(int*, int);

void iniciar_servidor(void);
void esperar_cliente(int);
void* recibir_mensaje(int socket_cliente, int* size);
int recibir_operacion(int);
void process_request(int cod_op, int cliente_fd);
void serve_client(int *socket);
void devolver_mensaje(void* payload, int size, int socket_cliente);

#endif /* UTILS_BROKER_H_ */
