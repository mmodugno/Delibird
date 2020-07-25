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
int conexionTeam2;
int conexionGamecard;
int conexionGameboy;

void liberar_conexion(int socket_cliente);
void enviarASuscriptores(t_paquete* paqueteAEnviar,t_list* usersAEnviar);
void enviarMensaje(t_paquete* paqueteAEnviar,int socket_cliente,char* suscriptorAEnviar);
void enviarPorTipo(particion* partAEnviar,t_list* usersAEnviar);
int crear_conexion(char *ip, char* puerto);

void enviar_cola_Localized_Pokemon(broker_localized_pokemon *brokerLocalizedPokemon, int socket_cliente);
void enviar_cola_Appeared_Pokemon(broker_appeared_pokemon *brokerAppearedPokemon, int socket_cliente);
void enviar_cola_Catch_Pokemon(broker_catch_pokemon *brokerCatchPokemon, int socket_cliente);
void enviar_cola_Caught_Pokemon(broker_caught_pokemon *brokerCaughtPokemon,int socket_cliente);
void enviar_cola_Get_Pokemon(broker_get_pokemon *brokerGetPokemon, int socket_cliente);
void enviar_Cola_New_Pokemon(broker_new_pokemon *brokerNewPokemon, int socket_cliente);


void enviarASuscriptoresNEW(broker_new_pokemon* newAEnviar ,t_list* usersAEnviar);
void enviarASuscriptoresAPPEARED(broker_appeared_pokemon* appAEnviar ,t_list* usersAEnviar);
void enviarASuscriptoresCATCH(broker_catch_pokemon* catchAEnviar ,t_list* usersAEnviar);
void enviarASuscriptoresCAUGHT(broker_caught_pokemon* caughtAEnviar ,t_list* usersAEnviar);
void enviarASuscriptoresGET(broker_get_pokemon* getAEnviar ,t_list* usersAEnviar);
void enviarASuscriptoresLOCALIZED(broker_localized_pokemon* localizedAEnviar ,t_list* usersAEnviar);

void envioColaNewPokemon();

void envioColaLocalizedPokemon();
void envioColaAppearedPokemon() ;
void envioColaCatchPokemon();
void envioColaCaughtPokemon();
void envioColaGetPokemon();


#endif /* ENVIOMENSAJES_H_ */
