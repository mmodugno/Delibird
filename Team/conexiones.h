/*
 * conexiones.h
 *
 *  Created on: 9 may. 2020
 *      Author: utnso
 */

#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include"utils_team.h"
#include"datos_team.h"
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>




void deserializar_appeared_pokemon(void* stream);

void recibir_mensaje(int socket_cliente);



// FUNCIONES DE LOGS //

t_log* iniciar_log(char* proceso);
void log_algoritmo_de_planificacion(void);
void log_cambio_de_cola(char * razon);
void log_movimiento_de_entrenador(entrenador* entrenador);
void log_atrapar_pokemon(pokemon* poke);
void log_intercambio(entrenador* entrenador1,entrenador* entrenador2);
void log_reintentar_comunicacion(void);
void log_conexion_exitosa(void);
void log_fallo_de_conexion(void);



#endif /* CONEXIONES_H_ */
