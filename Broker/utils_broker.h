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
#include<signal.h>
#include<semaphore.h>
#include <signal.h>
#include <commons/txt.h>

//ID	Base	tamaño de mensaje	Mensaje	Fecha(creacion/uso)
typedef struct {
	uint32_t idMensaje;
	uint32_t base;
	uint32_t tamanioMensaje;
	void* mensaje;
	tipoDeCola tipoMensaje;
	t_list *acknoleged;
	char* timestamp; //no hay un date, en las commons hay un temporal.h que solo hay una funcion que devuelve el tiempo en string
	uint32_t libre;
	uint32_t idCorrelativo;
} particion;

typedef struct {
	particion* particion;
	uint32_t limite;
} buddy;


t_list* colaVictimaBuddy;

t_list *tablaDeParticiones;

uint32_t idGlobales;

char* ip_broker;
char* puerto_broker;
char* ip_team;
char* puerto_team;
char* ip_gameboy;
char* puerto_gameboy;
char* ip_gamecard;
char* puerto_gamecard;

uint32_t tamanio_memoria;
uint32_t tamanio_minimo_particion;
char* algoritmo_memoria;
char* algoritmo_reemplazo;
char* algoritmo_particion_libre;
uint32_t frecuencia_compactacion;
uint32_t frecuencia;
char* log_file;
char *path;

int flagTermino;
int numeroDePARTICION;

t_config* config;

void *memoria;

//hilos para Recibir Mensajes
//pthread_t thread;
pthread_t hiloReciboMensajes;

//hilos para Enviar Mensajes
pthread_t hiloNew_Envio;
pthread_t hiloLocalized_Envio;
pthread_t hiloGet_Envio;
pthread_t hiloAppeared_Envio;
pthread_t hiloCatch_Envio;
pthread_t hiloCaught_Envio;

t_list* suscriptoresNewPokemon;
t_list* suscriptoresAppearedPokemon;
t_list* suscriptoresCatchPokemon;
t_list* suscriptoresCaughtPokemon;
t_list* suscriptoresGetPokemon;
t_list* suscriptoresLocalizedPokemon;

t_log* logConexion;
t_log* logSuscipcion;
t_log* logMensajeNuevo;
t_log* logEnviarNuevo;
t_log* confirmacionRecepcion;
t_log* almacenadoMemoria;
t_log* eliminacionMemoria;
t_log* compactacionMemoria;
t_log* dumpCache;

static const char *colasDeEnum[] = { "LIBRE", "NEW_POKEMON", "APPEARED_POKEMON",
		"CATCH_POKEMON", "CAUGHT_POKEMON", "GET_POKEMON", "LOCALIZED_POKEMON" };

sem_t idsDeMensajes;
sem_t usoMemoria;
sem_t terminoPrograma;

//sem_t llegadaMensajes;

sem_t suscripcionAColaNEW;
sem_t suscripcionAColaGET;
sem_t suscripcionAColaCATCH;
sem_t suscripcionAColaLOCALIZED;
sem_t suscripcionAColaCAUGHT;
sem_t suscripcionAColaAPPEARED;

tipoDeCola colaAEliminar;
suscriptor *susAEliminar;

/*
sem_t colaNew;
sem_t colaAppeared;
sem_t colaCatch;
sem_t colaCaught;
sem_t colaLocalized;
sem_t colaGet;*/


void agregarAMemoria(void* , uint32_t ,tipoDeCola, uint32_t , uint32_t);
int compactarMemoria(void);
void iniciarMemoria();
void* recibir_buffer(int*, int);
void iniciar_servidor(void);
void esperar_cliente(int);
void* recibir_mensaje(int socket_cliente, int* size);
int recibir_operacion(int);
void process_request(int cod_op, int cliente_fd);
void serve_client(int* );
void devolver_mensaje(void* payload, int size, int socket_cliente);
void suscribirACola(suscriptor* suscriptor);
particion* crearEntradaParticionBasica(void * dato, uint32_t idMensaje,
		tipoDeCola, uint32_t, uint32_t);
void algoritmoFirstFit(particion *datoAAgregar, particion *particionEncontrada);
void algoritmoBestFit(particion *datoAAgregar, particion* particionMasChica);
void agregarTablaParticionesYMemoria(particion *datoAAgregar,
		particion *partElegida, uint32_t* baseSig);
void copiarDatos(particion *target, particion * copiado);
void algoritmoReemplazo();
void formarPosiciones(char* ,localized_pokemon* );
void eliminarParticion(particion * part);
void reservarMemoria(char** , size_t );
char* mostrarPosiciones(localized_pokemon* );
//void producirUnMensaje(tipoDeCola tipo);
void terminar_programa();
broker_new_pokemon* leerdeMemoriaNEW(particion* part);
broker_appeared_pokemon* leerdeMemoriaAPPEARED(particion* part);
broker_catch_pokemon* leerdeMemoriaCATCH(particion* part);
broker_caught_pokemon* leerdeMemoriaCAUGHT(particion* part);
broker_get_pokemon* leerdeMemoriaGET(particion* part);
broker_localized_pokemon* leerdeMemoriaLOCALIZED(particion* part);
void desuscribirACola(suscriptor* suscriptor);
void liberar_conexion(int socket_cliente);


/* buddy */
void agregarEnBuddy(buddy* );
int tamanioBuddy(buddy* );
void actualizarComoOcupadoEnLista(buddy* , buddy* );
void dividirEnDos(buddy* );
void actualizarTablaParticionesParaBuddy(buddy* ,buddy* ,buddy* );
t_list* listaTamanioBuddiesPosibles();
uint32_t tamanioIdealBuddy(uint32_t );
int posicionBuddyTabla(buddy* );
void consolidarSiEsPosible(buddy* );
void consolidarSiEsPosibleADerecha(buddy* );
void consolidarSiEsPosibleAIzquierda(buddy* );
bool hayTamanioDisponiblePara(int );
void consolidarSiEsPosible(buddy* );
void consolidarSiEsPosibleADerecha(buddy* );
void consolidarSiEsPosibleAIzquierda(buddy* );
buddy* seleccionarVictima();
void eliminarVictima();
void consolidarBuddySystem();
bool buddyPuedeConsolidar(buddy*);
bool puedeConsolidarDerecha(buddy* );
bool puedeConsolidarIzquierda(buddy* );
bool baseMasChica(particion *, particion* );

#endif /* UTILS_BROKER_H_ */
