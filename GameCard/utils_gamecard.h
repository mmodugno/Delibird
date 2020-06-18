/*
 * utils_gamecard.h
 *
 *  Created on: 15 jun. 2020
 *      Author: utnso
 */

#ifndef UTILS_GAMECARD_H_
#define UTILS_GAMECARD_H_

#include "utils_en_comun.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<commons/config.h>
#include<commons/string.h>
#include<commons/txt.h>
#include<commons/bitarray.h>
#include<commons/log.h>

typedef struct {

	uint32_t posX;
	uint32_t posY;
	uint32_t cantidad;

} registroDatos;

t_log* logArchivoAbierto;
t_log* logFalloConexion;


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

void registrarPokemon(char* , registroDatos* );
char* buscarPrimerBloqueLibre(void);
bool estaVacioConRuta(char* );
void crearDirectorio(char*  ,char* );
void crearBitmap(void);
void crearMetadata(void);
void crearFilesAndBlocks(void);
bool estaVacio(FILE* );
void verificarExistenciaPokemon(char* );
t_log* iniciar_logger(char*);


#endif /* UTILS_GAMECARD_H_ */
