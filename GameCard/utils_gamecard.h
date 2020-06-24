/*
 * utils_gamecard.h
 *
 *  Created on: 15 jun. 2020
 *      Author: utnso
 */

#ifndef UTILS_GAMECARD_H_
#define UTILS_GAMECARD_H_

#include "utils_en_comun.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

////////// CONFIGURACIONES INICIALES
void crearDirectorio(char*  ,char* );
void crearBitmap(void);
void leerMetadata(void);
void crearFilesAndBlocks(void);
t_log* iniciar_logger(char*);
void modificarArchivoComoConfig(t_config* ,char* ,char* );

////////// CONFIGURACIONES POKEMONES

void registrarPokemon(char* , registroDatos* );
void agregarBloqueParaPokemon(char* ,int );
void verificarExistenciaPokemon(char* );
void procesarNewPokemon(char*, registroDatos* );

////////// REGISTROS Y BLOQUES

int tamanioRegistro(registroDatos* );
char* registro_a_string(registroDatos* );
char* buscarPrimerBloqueLibre(void);
int estaPosicionEnArchivo(uint32_t ,uint32_t ,char* );
int tamanioArchivoDadoPath(char* );
int tamanioArchivo(FILE* );
registroDatos* hacerRegistro(uint32_t ,uint32_t ,uint32_t );
int tamanioRestante(FILE* );
registroDatos* string_a_registro(char* );

///////////////////////////FUNCIONES AUXILIARES//////////////////////////////

t_list* crear_lista(char** array);
bool entraDatoEnBloque(registroDatos* registro, int nroBloque);
char* obtener_ruta_bloque(int nro_bloque);
bool estaVacioConRuta(char* );
bool estaVacio(FILE* );
int tamanio_array(char** bloque);
bool configConKeyCompleta(char* rutaConfig,char* key);

#endif /* UTILS_GAMECARD_H_ */
