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
#include<commons/temporal.h>

//ID	Base	tamaño de mensaje	Mensaje	Fecha(creacion/uso)
typedef struct {
	uint32_t idMensaje;
	uint32_t base;
	uint32_t tamanioMensaje;
	void* mensaje;
	char* tiempo;//no hay un date, en las commons hay un temporal.h que solo hay una funcion que devuelve el tiempo en string
}particion;

typedef struct {

    uint32_t base;
    uint32_t limite;
    uint32_t tamanio;

} particionLibre;

t_list *tablaDeParticiones;

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

t_list* colaNewPokemon;
t_list* colaAppearedPokemon;

t_list* colaCatchPokemon;
t_list* colaCaughtPokemon;
t_list* colaGetPokemon;
t_list* colaLocalizedPokemon;

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


static const char *colasDeEnum[] = {"NEW_POKEMON","APPEARED_POKEMON","CATCH_POKEMON","CAUGHT_POKEMON","GET_POKEMON","LOCALIZED_POKEMON"};


//void algoritmoFirstFit(particion* ,uint32_t ,particion* );
//void algoritmoBestFit(particion* ,uint32_t );
void agregarAMemoria(void* , uint32_t );
void iniciarMemoria();
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
particion* crearEntradaParticionBasica(void * dato, uint32_t idMensaje);
void algoritmoFirstFit(particion *datoAAgregar,uint32_t *desplazamiento,particion *particionEncontrada);
void algoritmoBestFit(particion *datoAAgregar, particion* particionMasChica);

#endif /* UTILS_BROKER_H_ */
