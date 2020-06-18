/*
 ============================================================================
 Name        : GameCard.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include"GameCard.h"


int main(int argc, char* argv[]){

	if(argc == 1){
				printf("No hay parametros suficientes\n");
				return 2;
			}
		if(argc == 4){

			nombreConfig = argv[1];
			tamanioBloques = atoi(argv[2]);
			cantidadBloques = atoi(argv[3]);

		}

		leer_config();

		logArchivoAbierto = iniciar_logger("ARCHIVO ABIERTO");
		logFalloConexion = iniciar_logger("FALLO CONEXION");

		crearDirectorio("/home/utnso/Escritorio/","PuntoMontaje");

		crearMetadata();

		crearDirectorio(punto_montaje,"/TallGrass");

		crearFilesAndBlocks();

		crearBitmap();

		verificarAperturaArchivo("/home/utnso/Escritorio/PuntoMontaje/TallGrass/Files/Pikachu/Metadata.bin");

		char* launch = buscarPrimerBloqueLibre();

		printf("\n bloque libre en: %s", launch);

		printf(" \n Terminado  \n" );

		terminar_programa();

}



void leer_config(void){

	config = config_create(nombreConfig);

	tiempo_reintento_conexion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_CONEXION");
	tiempo_reintento_operacion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_OPERACION");
	tiempo_retardo_operacion = config_get_int_value(config,"TIEMPO_RETARDO_OPERACION");

	punto_montaje = config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS");

	ipBroker = config_get_string_value(config,"IP_BROKER");
	puertoBroker = config_get_int_value(config,"PUERTO_BROKER");
}


t_log* iniciar_logger(char* tipoDeProceso){

	//preguntar por el tipo de LOG_LEVEL
	return log_create("gameboy.log",tipoDeProceso,0,LOG_LEVEL_INFO);
}


void terminar_programa(void){
	log_destroy(logArchivoAbierto);
	log_destroy(logFalloConexion);
	config_destroy(config);

	//liberar_conexion(conexionBroker);
}
