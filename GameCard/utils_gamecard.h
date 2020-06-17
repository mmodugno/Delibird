/*
 * utils_gamecard.h
 *
 *  Created on: 15 jun. 2020
 *      Author: utnso
 */

#ifndef UTILS_GAMECARD_H_
#define UTILS_GAMECARD_H_


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<commons/config.h>
#include<commons/string.h>
#include<commons/txt.h>
#include<commons/bitarray.h>
#include<commons/log.h>

FILE* metadata;
FILE* metadataFiles;

t_bitarray* bitArray;

t_config* config;

int tiempo_reintento_conexion;
int	tiempo_reintento_operacion;
int	tiempo_retardo_operacion;
int puertoBroker;
int tamanioBloques;
int cantidadBloques;

char *nombreConfig;
char* punto_montaje;
char* ipBroker;


void crearDirectorio(char*  ,char* );
void crearBitmap(void);
void crearMetadata(void);
void crearFilesAndBlocks(void);
bool estaVacio(FILE* );
void verificarExistenciaPokemon(char* );

#endif /* UTILS_GAMECARD_H_ */
