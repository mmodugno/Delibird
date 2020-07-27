/*
 * utils_gamecard.h
 *
 *  Created on: 15 jun. 2020
 *      Author: utnso
 */
/*
 * utils_gamecard.h
 *
 *  Created on: 15 jun. 2020
 *      Author: utnso
 */

#ifndef UTILS_GAMECARD_H_
#define UTILS_GAMECARD_H_
/*
#define ipGamecard "127.0.0.3"
#define puertoGamecard "5001"

#define IP_BROKER "127.0.0.1"
#define PUERTO_BROKER "5003"
*/

char* username;

#include "utils_en_comun.h"
#include "datos_broker.h"

#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/socket.h>
#include<netdb.h>
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
	int cantidad;

} registroDatos;

typedef struct {

	char* nombre;

	registroDatos* registro;

} registroConNombre;

t_log* logArchivoAbierto;
t_log* logFalloConexion;

sem_t sem_conexion;
sem_t sem_escritura;
sem_t sem_mensaje;
sem_t mutex_bit_array;

FILE* metadata;
FILE* metadataFiles;

t_bitarray* bitArray;

t_config* config;

int tiempo_reintento_conexion;
int	tiempo_reintento_operacion;
int	tiempo_retardo_operacion;
char* puertoBroker;
int tamanioBloques;
int cantidadBloques;

char *nombreConfig;
char* punto_montaje;
char* ipBroker;

char* ipGamecard;
char* puertoGamecard ;

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
int verificarExistenciaPokemon(char* );
void procesarNewPokemon(char*, registroDatos* ,int);
void procesarCatchPokemon(char* ,uint32_t , uint32_t ,int);


////////// REGISTROS Y BLOQUES

t_list* obtenerPosiciones(char* nombrePoke);
int tamanioRegistro(registroDatos* );
char* registro_a_string(registroDatos* );
int estaPosicionEnArchivo(uint32_t ,uint32_t ,char* );
int tamanioArchivoDadoPath(char* );
int tamanioArchivo(FILE* );
registroDatos* hacerRegistro(uint32_t ,uint32_t ,uint32_t );
int tamanioRestante(FILE* );
registroDatos* string_a_registro(char* );
int sumarSiEstaEnBloque(t_list* ,registroDatos*);
bool tieneCantidadCero(char*);
void buscarYeliminarCeros(t_list*);
void eliminarBloquesVacios(char* );
void procesarGetPokemon(char* ,int);
bool tieneUnEspacioComoPrimerCaracter(char* );

////////// BITARRAY
void inicializar_bitarray();
void inicializar_bitmap();
void crearFilesAndBlocks() ;
void crearBitmap();
void actualizar_bitmap();


///////////////////////////FUNCIONES AUXILIARES//////////////////////////////

t_list* crear_lista(char** array);
bool entraDatoEnBloque(registroDatos* registro, int nroBloque);
char* obtener_ruta_bloque(int nro_bloque);
bool estaVacioConRuta(char* );
bool estaVacio(FILE* );
int tamanio_array(char** bloque);
bool configConKeyCompleta(char* ,char* ,int );
void llenarConjunto(char* ,char* ,char* ,struct stat ,struct stat );
void cerrarArchivoMetadataPoke(t_config* );
void verificarDirectorioPokemon(char* );
char* listToString(t_list* );
void vaciarArchivo(char* );
void calcularTamanioMetadata(char* );

//////////////////////////CONEXIONES///////////////////////////////////////
int conectarse_con_broker(void);
void esperar_cliente(int);
void serve_client(int* );
void process_request(int , int);
void* recibir_mensaje(int , int*);
int crear_conexion(char *, char*);
registroConNombre* deserializar_new_pokemon_Gamecard(int);
registroConNombre* deserializar_catch_pokemon_Gamecard(int);
void enviar_appeared(int ,char* , int ,int , int );
void enviar_caught(int ,uint32_t , uint32_t );
void enviar_localized(int , char* , uint32_t , uint32_t* , uint32_t* ,uint32_t );
void iniciar_servidor(void);
void actualizar_bitmap();
void inicializar_bitmap();
void inicializar_bitarray();
char* deserializar_get_pokemon_Gamecard(int);


#endif /* UTILS_GAMECARD_H_ */

