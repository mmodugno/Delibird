/*
 * broker.h
 *
 *  Created on: 1 may. 2020
 *      Author: utnso
 */

#ifndef BROKER_H_
#define BROKER_H_



#include "utils_broker.h"

t_log* iniciar_logger(char*,char* archivoLog);
void leer_config(void);
//void terminar_programa(int,int,int, t_log*,t_log*,t_log*,t_log*, t_config*);



int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
void iniciarmemoria();
//char* recibir_mensaje(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void liberar_conexion(int socket_cliente);
void terminar_programa();

#endif /* BROKER_H_ */
