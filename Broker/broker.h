/*
 * broker.h
 *
 *  Created on: 1 may. 2020
 *      Author: utnso
 */

#ifndef BROKER_H_
#define BROKER_H_

#include "envioMensajes.h"

t_log* iniciar_logger(char*, char* );
void leer_config(void);
//void terminar_programa(int,int,int, t_log*,t_log*,t_log*,t_log*, t_config*);

int crear_conexion(char* , char* );
void my_handler(int );
//void enviar_mensaje(char* mensaje, int socket_cliente);
//void iniciarmemoria();
//char* recibir_mensaje(int socket_cliente);
//void eliminar_paquete(t_paquete* paquete);


#endif /* BROKER_H_ */
