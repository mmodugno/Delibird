/*
 ============================================================================
 Name        : GameBoy.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "gameboy.h"


int main(int argc, char* argv[]){

	//ESTO ME PARECE QUE SE PUEDE DEJAR COMO INT PORQUE NO SE VA A PASAR POR SOCKET
	int conexionBroker;
	int conexionTeam;
	int conexionGamecard;

	char* ipBroker;
	char* puertoBroker;

	char* ipTeam;
	char* puertoTeam;

	char* ipGamecard;
	char* puertoGamecard;
	////////////////////////////////////////////////////////////////////////////////


	///////////////////////////LOGS OBLIGATRIOS/////////////////////////////////////
	t_log* logConexion=iniciar_logger("Conexion");
	t_log* logSuscipcion=iniciar_logger("Suscripcion");
	t_log* logMensajeNuevo=iniciar_logger("Mensaje Nuevo");
	t_log* logEnviarNuevo= iniciar_logger("Enviar Mensaje");
	////////////////////////////////////////////////////////////////////////////////

	t_config* config = leer_config();

	ipBroker = config_get_string_value(config,"IP_BROKER");
	ipGamecard = config_get_string_value(config,"IP_GAMECARD");
	ipTeam = config_get_string_value(config,"IP_TEAM");

	puertoBroker = config_get_string_value(config,"PUERTO_BROKER");
	puertoGamecard = config_get_string_value(config,"PUERTO_GAMECARD");
	puertoTeam = config_get_string_value(config,"PUERTO_TEAM");

	//conexiones

	conexionBroker = crear_conexion(ipBroker,puertoBroker);
	conexionGamecard = crear_conexion(ipGamecard,puertoGamecard);
	conexionTeam = crear_conexion(ipTeam,puertoTeam);

	//////////////HACER CONEXION DEPENDIENDO QUE NOS PASEN POR PARAMETRO////////////
	//ver
	/*
	switch(*&argv[1]){
		case "BROKER":



			break;

		case "TEAM":

			break;

		case "GAMECARD":

			break;

	}*/



	if(!strcmp(*&argv[1],"BROKER")){
		if(!strcmp(*&argv[2],"NEW_POKEMON")){

			broker_new_pokemon *newPokemon = malloc(sizeof(broker_new_pokemon));
			//estructuras dinamicas
			newPokemon->datos->nombrePokemon = *&argv[3];
			newPokemon->datos->tamanioNombre = strlen(newPokemon->datos->nombrePokemon)+1;

			//estructuras estaticas


			newPokemon->datos->posX = atoi(*&argv[4]);
			newPokemon->datos->posY = atoi(*&argv[5]);
			newPokemon->datos->cantidadPokemon = atoi(*&argv[6]);


			enviar_Broker_New_Pokemon(newPokemon,conexionBroker);
		}
		if(!strcmp(*&argv[2],"APPEARED_POKEMON")){

			broker_appeared_pokemon *appPokemon = malloc(sizeof(broker_appeared_pokemon));
			//estructuras dinamicas
			appPokemon->datos->nombrePokemon = *&argv[3];
			appPokemon->datos->tamanioNombre = atoi(appPokemon->datos->nombrePokemon)+1;

			//estructuras estaticas
			appPokemon->datos->posX = atoi(*&argv[4]);
			appPokemon->datos->posY = atoi(*&argv[5]);
			appPokemon->id = atoi(*&argv[6]);

			enviar_Broker_Appeared_Pokemon(appPokemon,conexionBroker);
		}
		if(!strcmp(*&argv[2],"CATCH_POKEMON")){
			broker_catch_pokemon *catchPoke = malloc(sizeof(broker_catch_pokemon));
			//dinamicas
			catchPoke->datos->nombrePokemon = *&argv[3];
			catchPoke->datos->tamanioNombre = strlen(catchPoke->datos->nombrePokemon);

			//estaticas
			catchPoke->datos->posX = atoi(*&argv[4]);
			catchPoke->datos->posX = atoi(*&argv[5]);

			enviar_Broker_Catch_Pokemon(catchPoke,conexionBroker);

		}
		if(!strcmp(*&argv[2],"CAUGHT_POKEMON")){

			broker_caught_pokemon *caughtPoke = malloc(sizeof(broker_caught_pokemon));
			//estaticas
			caughtPoke->id = atoi(*&argv[3]);
			caughtPoke->datos->puedoAtraparlo = atoi(*&argv[4]);

			enviar_Broker_Caught_Pokemon(caughtPoke,conexionBroker);

		}
		if(!strcmp(*&argv[2],"GET_POKEMON")){

			broker_get_pokemon *getPoke= malloc(sizeof(broker_get_pokemon));
			//dinamica
			getPoke->datos->nombrePokemon = *&argv[3];
			getPoke->datos->tamanioNombre = strlen(getPoke->datos->nombrePokemon)+1;

			enviar_Broker_Get_Pokemon(getPoke,conexionBroker);

		}
	}

	if(!strcmp(*&argv[1],"TEAM")){

	    if(!strcmp(*&argv[2],"APPEARED_POKEMON")){

	        team_appeared_pokemon *appearedPokemon = malloc(sizeof(team_appeared_pokemon));

	        appearedPokemon->datos->nombrePokemon = *&argv[3];
	        appearedPokemon->datos->tamanioNombre = strlen(appearedPokemon->datos->nombrePokemon)+1;

	        appearedPokemon->datos->posX = atoi(*&argv[4]);
	        appearedPokemon->datos->posY = atoi(*&argv[5]);

	        enviar_Team_Appeared_Pokemon(appearedPokemon,conexionTeam);
	    }

	}

	if(!strcmp(*&argv[1],"GAMECARD")){

		if(!strcmp(*&argv[2],"NEW_POKEMON")){

			gameCard_new_pokemon *newPokemon = malloc(sizeof(gameCard_new_pokemon));

			newPokemon->datos->nombrePokemon = *&argv[3];
			newPokemon->datos->tamanioNombre = strlen(newPokemon->datos->nombrePokemon)+1;

			newPokemon->datos->posX = atoi(*&argv[4]);
			newPokemon->datos->posY = atoi(*&argv[5]);
			newPokemon->datos->cantidadPokemon = atoi(*&argv[6]);
			newPokemon->id = atoi(*&argv[7]);

			enviar_GameCard_New_Pokemon(newPokemon,conexionGamecard);

		}

		if(!strcmp(*&argv[2],"CATCH_POKEMON")){

			gameCard_catch_pokemon *catchPokemon = malloc(sizeof(gameCard_catch_pokemon));

			catchPokemon->datos->nombrePokemon = *&argv[3];
			catchPokemon->datos->tamanioNombre = strlen(catchPokemon->datos->nombrePokemon)+1;

			catchPokemon->datos->posX = atoi(*&argv[4]);
			catchPokemon->datos->posY = atoi(*&argv[5]);
			catchPokemon->id = atoi(*&argv[6]);

			enviar_GameCard_Catch_Pokemon(catchPokemon,conexionGamecard);

		}


		if(!strcmp(*&argv[2],"GET_POKEMON")){

			gameCard_get_pokemon *getPokemon = malloc(sizeof(gameCard_new_pokemon));

			getPokemon->datos->nombrePokemon = *&argv[3];
			getPokemon->datos->tamanioNombre = strlen(getPokemon->datos->nombrePokemon)+1;

			enviar_GameCard_Get_Pokemon(getPokemon,conexionGamecard);
		}


	}

	//TODO
	//Ver el tipo de dato y la funcion enviar
	if(!strcmp(*&argv[1],"SUSCRIPTOR")){

	}

	////////////////////////////////////////////////////////////////////////////////

	//respetar el orden de los parametros
	terminar_programa(conexionBroker,conexionTeam,conexionGamecard,logConexion,logSuscipcion,logMensajeNuevo,logEnviarNuevo,config);

}


t_log* iniciar_logger(char* tipoDeProceso){

	//preguntar por el tipo de LOG_LEVEL
	return log_create("gameboy.log",tipoDeProceso,0,LOG_LEVEL_INFO);
}


t_config* leer_config(void){

	return config_create("gameboy.config");

}

//termino todas las conexiones, logs y archivo de config
//si se agregan mas logs, conexiones o archivos de config, agregar
void terminar_programa(int conexBroker,int conexTeam,int conexGamecard, t_log* logConexion,t_log* logSuscripcion,t_log* logMensajeNuevo,t_log* logEnviarMensaje, t_config* config){

	log_destroy(logConexion);
	log_destroy(logSuscripcion);
	log_destroy(logMensajeNuevo);
	log_destroy(logEnviarMensaje);

	config_destroy(config);

	liberar_conexion(conexBroker);
	liberar_conexion(conexGamecard);
	liberar_conexion(conexTeam);
}
