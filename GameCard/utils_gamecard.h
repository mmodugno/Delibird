/*
 * utils_gamecard.h
 *
 *  Created on: 15 jun. 2020
 *      Author: utnso
 */

#ifndef UTILS_GAMECARD_H_
#define UTILS_GAMECARD_H_

#include<commons/config.h>
#include<commons/string.h>


t_config* config;

int tiempo_reintento_conexion;
int	tiempo_reintento_operacion;
int	tiempo_retardo_operacion;

char* punto_montaje;
char* ipBroker;
char* puertoBroker;


#endif /* UTILS_GAMECARD_H_ */
