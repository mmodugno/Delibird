/*
 * envioMensajes.h
 *
 *  Created on: 13 jul. 2020
 *      Author: utnso
 */

#ifndef ENVIOMENSAJES_H_
#define ENVIOMENSAJES_H_

#include "utils_broker.h"

int conexionTeam;
int conexionGamecard;
int conexionGameboy;


void liberar_conexion(int socket_cliente);
void enviarASuscriptores(t_paquete* paqueteAEnviar,t_list* usersAEnviar);
void enviarMensaje(t_paquete* paqueteAEnviar,int socket_cliente,char* suscriptorAEnviar);
void enviarPorTipo(particion* partAEnviar,t_list* usersAEnviar);
int crear_conexion(char *ip, char* puerto);

#endif /* ENVIOMENSAJES_H_ */
