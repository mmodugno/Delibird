/*
 ============================================================================
 Name        : Broker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "broker.h"

int main(){


	/*
	 *
Ejecución de compactación (para particiones dinámicas) o asociación de bloques (para buddy system). En este último, indicar que particiones se asociaron (indicar posición inicio de ambas particiones).
Ejecución de Dump de cache (solo informar que se solicitó el mismo).

	 *
	 */



		t_log* logConexion=iniciar_logger("Conexion");//ver bien donde va
		t_log* logSuscipcion=iniciar_logger("Suscripcion");
		t_log* logMensajeNuevo=iniciar_logger("Mensaje Nuevo");
		t_log* logEnviarNuevo= iniciar_logger("Enviar Mensaje");
		t_log* confirmacionRecepcion= iniciar_logger("Recepcion Mensaje"); //ver bien donde va, cuando una suscrpicion reciba el mensaje, se tiene que loggear esto
		t_log* almacenadoMemoria= iniciar_logger("Almacenado Memoria"); //debe indicar posicion de inicio de particion
		t_log* eliminacionMemoria= iniciar_logger("Eliminacion Pariticion Memoria"); //debe indicar posicion de inicio
		/*t_log* compactacionMemoria= iniciar_logger("");
		t_log* dumpCache= iniciar_logger("");*/

}
