/*
 * utils_broker.h
 *
 *  Created on: 30 abr. 2020
 *      Author: utnso
 */

#ifndef UTILS_BROKER_H_
#define UTILS_BROKER_H_


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
#include<pthread.h>
#include "datos_broker.h"
#include<commons/collections/list.h>

uint32_t idGlobales;

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

void *memoria;

pthread_t thread;

typedef struct {
	uint32_t id;
	uint32_t base;
	uint32_t tamanioMensaje;
	void* mensaje;
} particiones;

t_list* tablaAcknowledged;

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


static const char *colasDeEnum[] = {
		"NEW_POKEMON","APPEARED_POKEMON","CATCH_POKEMON","CAUGHT_POKEMON","GET_POKEMON","LOCALIZED_POKEMON",
};




void* recibir_buffer(int*, int);
void iniciar_servidor(void);
void esperar_cliente(int);
void* recibir_mensaje(int socket_cliente, int* size);
int recibir_operacion(int);
void process_request(int cod_op, int cliente_fd);
void serve_client(int *socket);
void devolver_mensaje(void* payload, int size, int socket_cliente);
void agregarACola(tipoDeCola tipo_de_Cola, void* mensaje);
void suscribirACola(suscriptor* suscriptor);

#endif /* UTILS_BROKER_H_ */
