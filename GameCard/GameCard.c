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

		crearMetadata();

		crearDirectorio(punto_montaje,"/Files");

		crearBitmap();

		txt_close_file(metadata);

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

void terminar_programa(void){

	config_destroy(config);

	//liberar_conexion(conexionBroker);
}
