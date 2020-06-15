/*
 ============================================================================
 Name        : GameCard.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include"GameCard.h"


int main(int argc, char* argv[]){

	leer_config();








}



void leer_config(void){

	config = config_create("gameboy.config");

	tiempo_reintento_conexion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_CONEXION");
	tiempo_reintento_operacion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_OPERACION");
	tiempo_retardo_operacion = config_get_int_value(config,"TIEMPO_RETARDO_OPERACION");

	punto_montaje = config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS");

	ipBroker = config_get_string_value(config,"IP_BROKER");
	puertoBroker = config_get_string_value(config,"PUERTO_BROKER");
}

void terminar_programa(void){

	config_destroy(config);

	//liberar_conexion(conexionBroker);
}
