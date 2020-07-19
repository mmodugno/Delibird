/*
 * utils_Gameboy.h
 *
 *  Created on: 29 abr. 2020
 *      Author: utnso
 */

#ifndef UTILS_GAMEBOY_H_
#define UTILS_GAMEBOY_H_

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/time.h>
#include<netdb.h>
#include<string.h>
#include <datos_broker.h>
#include "datos_gamecard.h"
#include "datos_team.h"
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>


//ESTO ME PARECE QUE SE PUEDE DEJAR COMO INT PORQUE NO SE VA A PASAR POR SOCKET
int conexionBroker;
int conexionTeam;
int conexionGamecard;
int segundosSuscripcion;
int flagTerminoSuscripcion;


char* ipBroker;
char* puertoBroker;

char* ipTeam;
char* puertoTeam;

char* ipGamecard;
char* puertoGamecard;
////////////////////////////////////////////////////////////////////////////////



///////////////////////////LOGS OBLIGATRIOS/////////////////////////////////////
t_log* logConexion;
t_log* logSuscipcion;
t_log* logMensajeNuevo; //falta el de recibir mensaje
//CON LA SEGUNDA REVISION DEL TP NO HACE FALTA EL LOG DE ENVIARNUEVO
//t_log* logEnviarNuevo;
////////////////////////////////////////////////////////////////////////////////

t_config* config;


pthread_t hiloConexion;
pthread_t hiloTimeout;
int socket_servidor_gameboy;

sem_t llegadaMensajes;


int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
char* recibir_mensaje(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void liberar_conexion(int socket_cliente);
void enviar_Broker_New_Pokemon(broker_new_pokemon *brokerNewPokemon,char* username , int socket_cliente);
void enviar_Broker_Appeared_Pokemon(broker_appeared_pokemon *brokerAppearedPokemon,char* username , int socket_cliente);
void enviar_Broker_Catch_Pokemon(broker_catch_pokemon *brokerCatchPokemon ,char* username, int socket_cliente);
void enviar_Broker_Caught_Pokemon(broker_caught_pokemon *brokerCaughtPokemon,char* username, int socket_cliente);
void enviar_Broker_Get_Pokemon(broker_get_pokemon *brokerGetPokemon ,char* username, int socket_cliente);
void enviar_Team_Appeared_Pokemon(team_appeared_pokemon *teamAppearedPokemon ,char* username, int socket_cliente);
void enviar_GameCard_New_Pokemon(gameCard_new_pokemon *gameCardNewPokemon ,char* username, int socket_cliente);
void enviar_GameCard_Catch_Pokemon(gameCard_catch_pokemon *gameCardCatchPokemon ,char* username, int socket_cliente);
void enviar_GameCard_Get_Pokemon(gameCard_get_pokemon *gameCardGetPokemon ,char* username, int socket_cliente);
void serve_client_gameboy(int*);
void iniciar_servidor_gameboy(void);
//int recvTimeout(int , int* , int , int );
void process_request_gameboy(int , int );
//int acceptConTimeOut(int socket_servidor_gameboy, __SOCKADDR_ARG dir_cliente, socklen_t *__restrict tam_direccion, int timeOut);
void esperar_cliente_gameboy(int socket_servidor_gameboy) ;
void timeOut(int timeOut);

#endif /* UTILS_GAMEBOY_H_ */
