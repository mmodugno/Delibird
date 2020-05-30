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

	if(argc == 1){
		printf("No hay parametros suficientes\n");
		return 2;
	}
	else if(argc == 2){
		printf("No hay parametros suficientes\n");
		return 2;
	}

	logConexion=iniciar_logger("Conexion");
	logSuscipcion=iniciar_logger("Suscripcion");
	logMensajeNuevo=iniciar_logger("Mensaje Nuevo"); //falta el de recibir mensaje
	logEnviarNuevo= iniciar_logger("Enviar Mensaje");


	//leemos todo el archivo de config
	leer_config();


	//////////////HACER CONEXION DEPENDIENDO QUE NOS PASEN POR PARAMETRO////////////


	if(!strcmp(argv[1],"BROKER")){
		conexionBroker = crear_conexion(ipBroker,puertoBroker);
		if(conexionBroker <= 0){
			log_info(logConexion,"no me pude conectar a Broker");
			return 0;
		}
		else{
			log_info(logConexion,"me conecte a Broker exitosamente");
		}
		if(!strcmp(argv[2],"NEW_POKEMON")){
			if(argc == 7){
				broker_new_pokemon *newPokemon = malloc(sizeof(broker_new_pokemon));
				newPokemon->datos = malloc(sizeof(new_pokemon));
				//estructuras dinamicas
				newPokemon->datos->nombrePokemon = argv[3];
				newPokemon->datos->tamanioNombre = strlen(newPokemon->datos->nombrePokemon)+1;

				//estructuras estaticas
				newPokemon->datos->posX = atoi(argv[4]);
				newPokemon->datos->posY = atoi(argv[5]);
				newPokemon->datos->cantidadPokemon = atoi(argv[6]);


				enviar_Broker_New_Pokemon(newPokemon,conexionBroker);

				log_info(logMensajeNuevo,"Mensaje New Pokemon a Broker");

				free(newPokemon);
			}
		}
		if(!strcmp(argv[2],"APPEARED_POKEMON")){
			if(argc == 7){
				broker_appeared_pokemon *appPokemon = malloc(sizeof(broker_appeared_pokemon));
				//estructuras dinamicas
				appPokemon->datos->nombrePokemon = argv[3];
				appPokemon->datos->tamanioNombre = atoi(appPokemon->datos->nombrePokemon)+1;

				//estructuras estaticas
				appPokemon->datos->posX = atoi(argv[4]);
				appPokemon->datos->posY = atoi(argv[5]);
				appPokemon->id_relativo = atoi(argv[6]);

				enviar_Broker_Appeared_Pokemon(appPokemon,conexionBroker);

				log_info(logMensajeNuevo,"Mensaje Appeared Pokemon a Broker");


				free(appPokemon);
			}
		}
		if(!strcmp(argv[2],"CATCH_POKEMON")){
			if(argc == 6){
				broker_catch_pokemon *catchPoke = malloc(sizeof(broker_catch_pokemon));
				//dinamicas
				catchPoke->datos->nombrePokemon = argv[3];
				catchPoke->datos->tamanioNombre = strlen(catchPoke->datos->nombrePokemon);

				//estaticas
				catchPoke->datos->posX = atoi(argv[4]);
				catchPoke->datos->posX = atoi(argv[5]);

				enviar_Broker_Catch_Pokemon(catchPoke,conexionBroker);

				log_info(logMensajeNuevo,"Mensaje Catch Pokemon a Broker");
				free(catchPoke);
			}
		}
		if(!strcmp(argv[2],"CAUGHT_POKEMON")){
			if(argc == 5){
				broker_caught_pokemon *caughtPoke = malloc(sizeof(broker_caught_pokemon));
				//estaticas
				caughtPoke->id_relativo = atoi(argv[3]);
				caughtPoke->datos->puedoAtraparlo = atoi(argv[4]);

				enviar_Broker_Caught_Pokemon(caughtPoke,conexionBroker);

				log_info(logMensajeNuevo,"Mensaje Caught Pokemon a Broker");
				free(caughtPoke);
			}
		}
		if(!strcmp(argv[2],"GET_POKEMON")){

			if(argc==4){

				broker_get_pokemon *getPoke= malloc(sizeof(broker_get_pokemon));
				//dinamica
				getPoke->datos->nombrePokemon = argv[3];
				getPoke->datos->tamanioNombre = strlen(getPoke->datos->nombrePokemon)+1;

				enviar_Broker_Get_Pokemon(getPoke,conexionBroker);
				log_info(logMensajeNuevo,"Mensaje Get Pokemon a Broker");

				free(getPoke);
			}

		}


	}
	if(!strcmp(argv[1],"TEAM")){

		conexionTeam = crear_conexion(ipTeam,puertoTeam);
		if(conexionTeam <= 0){
			log_info(logConexion,"no me pude conectar a Team");
			return 0;
		}
		else{
			log_info(logConexion,"me conecte a Team exitosamente");
		}


	    if(!strcmp(argv[2],"APPEARED_POKEMON")){
	    	if(argc==6){

		        team_appeared_pokemon *appearedPokemon = malloc(sizeof(team_appeared_pokemon));

		        appearedPokemon->datos->nombrePokemon = argv[3];
		        appearedPokemon->datos->tamanioNombre = strlen(appearedPokemon->datos->nombrePokemon)+1;

		        appearedPokemon->datos->posX = atoi(argv[4]);
		        appearedPokemon->datos->posY = atoi(argv[5]);

		        enviar_Team_Appeared_Pokemon(appearedPokemon,conexionTeam);
		        log_info(logMensajeNuevo,"Mensaje Appeared Pokemon a Team");
		        free(appearedPokemon);
	    	}
	    }


	}

	if(!strcmp(argv[1],"GAMECARD")){

		conexionGamecard = crear_conexion(ipGamecard,puertoGamecard);
		if(conexionGamecard <= 0){
			log_info(logConexion,"no me pude conectar a Gamecard");
			return 0;
		}
		else{
			log_info(logConexion,"me conecte a Gamecard exitosamente");
		}
		if(!strcmp(argv[2],"NEW_POKEMON")){
			if(argc==8){
				gameCard_new_pokemon *newPokemon = malloc(sizeof(gameCard_new_pokemon));

				newPokemon->datos->nombrePokemon = argv[3];
				newPokemon->datos->tamanioNombre = strlen(newPokemon->datos->nombrePokemon)+1;

				newPokemon->datos->posX = atoi(argv[4]);
				newPokemon->datos->posY = atoi(argv[5]);
				newPokemon->datos->cantidadPokemon = atoi(argv[6]);
				newPokemon->id_relativo = atoi(argv[7]);

				enviar_GameCard_New_Pokemon(newPokemon,conexionGamecard);

				log_info(logMensajeNuevo,"Mensaje New Pokemon a GameCard");
				free(newPokemon);
			}

		}

		if(!strcmp(argv[2],"CATCH_POKEMON")){

			if(argc==7){
				gameCard_catch_pokemon *catchPokemon = malloc(sizeof(gameCard_catch_pokemon));

				catchPokemon->datos->nombrePokemon = argv[3];
				catchPokemon->datos->tamanioNombre = strlen(catchPokemon->datos->nombrePokemon)+1;

				catchPokemon->datos->posX = atoi(argv[4]);
				catchPokemon->datos->posY = atoi(argv[5]);
				catchPokemon->id_relativo = atoi(argv[6]);

				enviar_GameCard_Catch_Pokemon(catchPokemon,conexionGamecard);
				log_info(logMensajeNuevo,"Mensaje Catch Pokemon a GameCard");

				free(catchPokemon);
			}


		}


		if(!strcmp(argv[2],"GET_POKEMON")){
			if(argc==4){

				gameCard_get_pokemon *getPokemon = malloc(sizeof(gameCard_get_pokemon));

				getPokemon->datos->nombrePokemon = argv[3];
				getPokemon->datos->tamanioNombre = strlen(getPokemon->datos->nombrePokemon)+1;

				enviar_GameCard_Get_Pokemon(getPokemon,conexionGamecard);

				log_info(logMensajeNuevo,"Mensaje Get Pokemon a GameCard");
				free(getPokemon);
			}

		}



	}

	//TODO
	//Ver el tipo de dato y la funcion enviar
	if(!strcmp(argv[1],"SUSCRIPTOR")){
		conexionBroker = crear_conexion(ipBroker,puertoBroker);
		if(conexionBroker <= 0){
			log_info(logConexion,"no me pude conectar a Broker");
			return 0;
		}
		else{
			log_info(logConexion,"me conecte a Broker exitosamente");
		}
		if(argc==3){
			suscriptor* meSuscribo = malloc(sizeof(suscriptor));
			meSuscribo->nombreDeSuscriptor="GAMEBOY";
			meSuscribo->tamanioNombreSucriptor= strlen(meSuscribo->nombreDeSuscriptor)+1;

			if(!strcmp(argv[2],"NEW_POKEMON")){
				meSuscribo->tipoDeCola = NEW_POKEMON;

				enviar_pedido_suscripcion(meSuscribo,conexionBroker);
				log_info(logSuscipcion,"me conecto como modo suscriptor a New_Pokemon exitosamente");
			}
			if(!strcmp(argv[2],"APPEARED_POKEMON")){
				meSuscribo->tipoDeCola = APPEARED_POKEMON;

				enviar_pedido_suscripcion(meSuscribo,conexionBroker);
				log_info(logSuscipcion,"me conecto como modo suscriptor a Appeared_Pokemon exitosamente");
			}
			if(!strcmp(argv[2],"CATCH_POKEMON")){
				meSuscribo->tipoDeCola = CATCH_POKEMON;

				enviar_pedido_suscripcion(meSuscribo,conexionBroker);
				log_info(logSuscipcion,"me conecto como modo suscriptor a Catch_Pokemon exitosamente");
			}
			if(!strcmp(argv[2],"CAUGHT_POKEMON")){
				meSuscribo->tipoDeCola = CAUGHT_POKEMON;

				enviar_pedido_suscripcion(meSuscribo,conexionBroker);
				log_info(logSuscipcion,"me conecto como modo suscriptor a Caught_Pokemon exitosamente");
			}
			if(!strcmp(argv[2],"GET_POKEMON")){
				meSuscribo->tipoDeCola= GET_POKEMON;

				enviar_pedido_suscripcion(meSuscribo,conexionBroker);
				log_info(logSuscipcion,"me conecto como modo suscriptor a Get_Pokemon exitosamente");
			}
			if(!strcmp(argv[2],"LOCALIZED_POKEMON")){
				meSuscribo->tipoDeCola= LOCALIZED_POKEMON;

				enviar_pedido_suscripcion(meSuscribo,conexionBroker);
				log_info(logSuscipcion,"me conecto como modo suscriptor a Localized_Pokemon exitosamente");
			}
			free(meSuscribo);

		}
	}

	////////////////////////////////////////////////////////////////////////////////



	//terminamos los logs, config y conexiones
	terminar_programa();

	return 1;
}


t_log* iniciar_logger(char* tipoDeProceso){

	//preguntar por el tipo de LOG_LEVEL
	return log_create("gameboy.log",tipoDeProceso,0,LOG_LEVEL_INFO);
}


void leer_config(void){

	config = config_create("gameboy.config");

	ipBroker = config_get_string_value(config,"IP_BROKER");
	ipGamecard = config_get_string_value(config,"IP_GAMECARD");
	ipTeam = config_get_string_value(config,"IP_TEAM");

	puertoBroker = config_get_string_value(config,"PUERTO_BROKER");
	puertoGamecard = config_get_string_value(config,"PUERTO_GAMECARD");
	puertoTeam = config_get_string_value(config,"PUERTO_TEAM");

}

//termino todas las conexiones, logs y archivo de config
//si se agregan mas logs, conexiones o archivos de config, agregar
void terminar_programa(){

	log_destroy(logConexion);
	log_destroy(logSuscipcion);
	log_destroy(logMensajeNuevo);
	log_destroy(logEnviarNuevo);

	config_destroy(config);

	liberar_conexion(conexionBroker);
	liberar_conexion(conexionGamecard);
	liberar_conexion(conexionTeam);


}
