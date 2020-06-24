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
			printf("No hay parametros suficientes");
			return 2;
		}

		nombreConfig = argv[1];

		leer_config();

		t_config* configCerrar = config_create("/home/utnso/Escritorio/PuntoMontaje/TallGrass/Files/Pikachu/Metadata.bin");

		modificarArchivoComoConfig(configCerrar,"OPEN","N");

		logArchivoAbierto = iniciar_logger("ARCHIVO ABIERTO");
		logFalloConexion = iniciar_logger("FALLO CONEXION");

		crearDirectorio("/home/utnso/Escritorio/","PuntoMontaje");

		leerMetadata();

		crearDirectorio(punto_montaje,"/TallGrass");

		crearFilesAndBlocks();

		crearBitmap();

		registroDatos* registro1 = hacerRegistro(4,1,3);
		registroDatos* registro2 = hacerRegistro(2,7,1);
		registroDatos* registro3 = hacerRegistro(6,4,1);

		procesarNewPokemon("Pikachu",registro1);
		procesarNewPokemon("Pikachu",registro2);
		procesarNewPokemon("Pikachu",registro3);
		//procesarNewPokemon("Pikachu",registro1);
		//procesarNewPokemon("Pikachu",registro2);
		//procesarNewPokemon("Pikachu",registro3);
		//procesarNewPokemon("Pikachu",registro1);
		//procesarNewPokemon("Pikachu",registro2);
		//procesarNewPokemon("Pikachu",registro3);

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
