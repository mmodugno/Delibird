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
	//logEnviarNuevo= iniciar_logger("Enviar Mensaje");


	//leemos todo el archivo de config
	leer_config();

	//sem_init(&recibiConexion,0,0);

	/*
	void analizadorTime(){
		//ponerSemaforoPAraCuandoSeCOnecteBroker
		sem_wait(&recibiConexion);
		sleep(segundosSuscripcion);
		pthread_kill(hiloConexion,0);
	}

	pthread_t analizador;*/

	//TODO porque esta esto?
	//log_info(logConexion,"Me desconecte broker");


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


				enviar_Broker_New_Pokemon(newPokemon,"GAMEBOY",conexionBroker);

				//log_info(logEnviarNuevo,"Mensaje New Pokemon a Broker");
				/*log_info(logEnviarNuevo,"envie mensaje de NEW_POKEMON \n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d \n cantidad de pokemones: %d"
									,newPokemon->datos->tamanioNombre,
									newPokemon->datos->nombrePokemon,
									newPokemon->datos->posX,
									newPokemon->datos->posY,
									newPokemon->datos->cantidadPokemon);*/

				free(newPokemon);
			}
		}
		if(!strcmp(argv[2],"APPEARED_POKEMON")){
			if(argc == 7){
				broker_appeared_pokemon *appPokemon = malloc(sizeof(broker_appeared_pokemon));
				appPokemon->datos = malloc(sizeof(appeared_pokemon));
				//estructuras dinamicas
				appPokemon->datos->nombrePokemon = argv[3];
				appPokemon->datos->tamanioNombre = strlen(appPokemon->datos->nombrePokemon)+1;

				//estructuras estaticas
				appPokemon->datos->posX = atoi(argv[4]);
				appPokemon->datos->posY = atoi(argv[5]);
				appPokemon->id_relativo = atoi(argv[6]);

				enviar_Broker_Appeared_Pokemon(appPokemon,"GAMEBOY",conexionBroker);

				/*log_info(logEnviarNuevo,"Mensaje Appeared Pokemon \n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d y ID_realativo: %d"
									,appPokemon->datos->tamanioNombre,
									appPokemon->datos->nombrePokemon,
									appPokemon->datos->posX,
									appPokemon->datos->posY,appPokemon->id_relativo);*/


				free(appPokemon);
			}
		}
		if(!strcmp(argv[2],"CATCH_POKEMON")){
			if(argc == 6){
				broker_catch_pokemon *catchPoke = malloc(sizeof(broker_catch_pokemon));
				catchPoke->datos = malloc(sizeof(catch_pokemon));
				//dinamicas
				catchPoke->datos->nombrePokemon = argv[3];
				catchPoke->datos->tamanioNombre = strlen(catchPoke->datos->nombrePokemon)+1;

				//estaticas
				catchPoke->datos->posX = atoi(argv[4]);
				catchPoke->datos->posY = atoi(argv[5]);

				enviar_Broker_Catch_Pokemon(catchPoke,"GAMEBOY",conexionBroker);

				/*log_info(logEnviarNuevo,"Mensaje Catch Pokemon \n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d "
									,catchPoke->datos->tamanioNombre,
									catchPoke->datos->nombrePokemon,
									catchPoke->datos->posX,
									catchPoke->datos->posY);*/
				free(catchPoke);
			}
		}
		if(!strcmp(argv[2],"CAUGHT_POKEMON")){
			if(argc == 5){
				broker_caught_pokemon *caughtPoke = malloc(sizeof(broker_caught_pokemon));
				caughtPoke->datos = malloc(sizeof(caughtPoke));
				//estaticas
				caughtPoke->id_relativo = atoi(argv[3]);
				if(!strcmp(argv[4],"OK")){
					caughtPoke->datos->puedoAtraparlo = 1;
				}
				if(!strcmp(argv[4],"FAIL")){
					caughtPoke->datos->puedoAtraparlo = 0;
				}

				enviar_Broker_Caught_Pokemon(caughtPoke,"GAMEBOY",conexionBroker);

				/*log_info(logEnviarNuevo,"Mensaje Caught Pokemon \n con ID_relativo: %d \n puedoAtraparlo: %d "
									,caughtPoke->id_relativo,
									caughtPoke->datos->puedoAtraparlo);*/
				free(caughtPoke);
			}
		}
		if(!strcmp(argv[2],"GET_POKEMON")){

			if(argc==4){

				broker_get_pokemon *getPoke= malloc(sizeof(broker_get_pokemon));
				getPoke->datos = malloc(sizeof(get_pokemon));
				//dinamica
				getPoke->datos->nombrePokemon = argv[3];
				getPoke->datos->tamanioNombre = strlen(getPoke->datos->nombrePokemon)+1;

				enviar_Broker_Get_Pokemon(getPoke,"GAMEBOY",conexionBroker);
				/*log_info(logEnviarNuevo,"Mensaje Get Pokemon \n con tamanio: %d \n nombre: %s \n"
									,getPoke->datos->tamanioNombre,
									getPoke->datos->nombrePokemon);*/

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
		        appearedPokemon->datos = malloc(sizeof(appeared_pokemon));

		        appearedPokemon->datos->nombrePokemon = argv[3];
		        appearedPokemon->datos->tamanioNombre = strlen(appearedPokemon->datos->nombrePokemon)+1;

		        appearedPokemon->datos->posX = atoi(argv[4]);
		        appearedPokemon->datos->posY = atoi(argv[5]);

		        enviar_Team_Appeared_Pokemon(appearedPokemon,"GAMEBOY",conexionTeam);
		        //log_info(logEnviarNuevo,"Mensaje Appeared Pokemon a Team");
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
				newPokemon->datos = malloc(sizeof(new_pokemon));

				newPokemon->datos->nombrePokemon = argv[3];
				newPokemon->datos->tamanioNombre = strlen(newPokemon->datos->nombrePokemon)+1;

				newPokemon->datos->posX = atoi(argv[4]);
				newPokemon->datos->posY = atoi(argv[5]);
				newPokemon->datos->cantidadPokemon = atoi(argv[6]);
				newPokemon->id_relativo = atoi(argv[7]);

				enviar_GameCard_New_Pokemon(newPokemon,"GAMEBOY",conexionGamecard);

				//log_info(logEnviarNuevo,"Mensaje New Pokemon a GameCard");
				free(newPokemon);
			}

		}

		if(!strcmp(argv[2],"CATCH_POKEMON")){

			if(argc==7){
				gameCard_catch_pokemon *catchPokemon = malloc(sizeof(gameCard_catch_pokemon));
				catchPokemon->datos = malloc(sizeof(catch_pokemon));

				catchPokemon->datos->nombrePokemon = argv[3];
				catchPokemon->datos->tamanioNombre = strlen(catchPokemon->datos->nombrePokemon)+1;

				catchPokemon->datos->posX = atoi(argv[4]);
				catchPokemon->datos->posY = atoi(argv[5]);
				catchPokemon->id_relativo = atoi(argv[6]);

				enviar_GameCard_Catch_Pokemon(catchPokemon,"GAMEBOY",conexionGamecard);
				//log_info(logEnviarNuevo,"Mensaje Catch Pokemon a GameCard");

				free(catchPokemon);
			}


		}


		if(!strcmp(argv[2],"GET_POKEMON")){
			if(argc==5){

				gameCard_get_pokemon *getPokemon = malloc(sizeof(gameCard_get_pokemon));
				getPokemon->datos = malloc(sizeof(get_pokemon));

				getPokemon->datos->nombrePokemon = argv[3];
				getPokemon->datos->tamanioNombre = strlen(getPokemon->datos->nombrePokemon)+1;

				enviar_GameCard_Get_Pokemon(getPokemon,"GAMEBOY",conexionGamecard);

				//log_info(logEnviarNuevo,"Mensaje Get Pokemon a GameCard");
				free(getPokemon);
			}

		}



	}

	if(!strcmp(argv[1],"SUSCRIPTOR")){

		segundosSuscripcion = atoi(argv[3]);

		conexionBroker = crear_conexion(ipBroker,puertoBroker);

		if(conexionBroker <= 0){
			log_info(logConexion,"no me pude conectar a Broker");
			return 0;
		} else{
			log_info(logConexion,"me conecte a Broker exitosamente");
			if (argc == 4) {

				sem_init(&llegadaMensajes,0,1);

				//pthread_create(&analizador,NULL,(void*)analizadorTime,NULL);

				//pthread_create(&hiloReciboMensajes, NULL, (void*) iniciar_servidor_gameboy, NULL);

				suscriptor* meSuscribo = malloc(sizeof(suscriptor));
				meSuscribo->nombreDeSuscriptor = "GAMEBOY";
				meSuscribo->tamanioNombreSucriptor = strlen(meSuscribo->nombreDeSuscriptor) + 1;

				if (!strcmp(argv[2], "NEW_POKEMON")) {
					meSuscribo->tipoDeCola = NEW_POKEMON;

					enviar_pedido_suscripcion(meSuscribo, conexionBroker);
					log_info(logSuscipcion,
							"me conecto como modo suscriptor por %d segundos a New_Pokemon exitosamente",
							atoi(argv[3]));
				}
				if (!strcmp(argv[2], "APPEARED_POKEMON")) {
					meSuscribo->tipoDeCola = APPEARED_POKEMON;

					enviar_pedido_suscripcion(meSuscribo, conexionBroker);
					log_info(logSuscipcion,
							"me conecto como modo suscriptor por %d segundos a Appeared_Pokemon exitosamente",
							atoi(argv[3]));
				}
				if (!strcmp(argv[2], "CATCH_POKEMON")) {
					meSuscribo->tipoDeCola = CATCH_POKEMON;

					enviar_pedido_suscripcion(meSuscribo, conexionBroker);
					log_info(logSuscipcion,
							"me conecto como modo suscriptor por %d segundos a Catch_Pokemon exitosamente",
							atoi(argv[3]));
				}
				if (!strcmp(argv[2], "CAUGHT_POKEMON")) {
					meSuscribo->tipoDeCola = CAUGHT_POKEMON;

					enviar_pedido_suscripcion(meSuscribo, conexionBroker);
					log_info(logSuscipcion,
							"me conecto como modo suscriptor por %d segundos a Caught_Pokemon exitosamente",
							atoi(argv[3]));
				}
				if (!strcmp(argv[2], "GET_POKEMON")) {
					meSuscribo->tipoDeCola = GET_POKEMON;

					enviar_pedido_suscripcion(meSuscribo, conexionBroker);
					log_info(logSuscipcion,
							"me conecto como modo suscriptor por %d segundos a Get_Pokemon exitosamente",
							atoi(argv[3]));
				}
				if (!strcmp(argv[2], "LOCALIZED_POKEMON")) {
					meSuscribo->tipoDeCola = LOCALIZED_POKEMON;

					enviar_pedido_suscripcion(meSuscribo, conexionBroker);

					log_info(logSuscipcion,
							"me conecto como modo suscriptor por %d segundos a Localized_Pokemon exitosamente",
							atoi(argv[3]));
				}


				//Abro el server de Gameboy para escuchar los msj q me manda Broker
				liberar_conexion(conexionBroker);

				flagTerminoSuscripcion = 0;

				iniciar_servidor_gameboy();

				//pthread_join(analizador,NULL);

				//pthread_join(hiloReciboMensajes,NULL);

				//conexionBroker = crear_conexion(ipBroker,puertoBroker);

				//enviar_pedido_desuscripcion(meSuscribo,conexionBroker);

				conexionBroker = crear_conexion(ipBroker,puertoBroker);

				if(conexionBroker <= 0){
					log_info(logConexion,"no me pude conectar a Broker para pedir la desucripcion");

				} else{
					enviar_pedido_desuscripcion(meSuscribo,conexionBroker);
				}

				free(meSuscribo);

			}
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
	//log_destroy(logEnviarNuevo);

	config_destroy(config);

	liberar_conexion(conexionBroker);
	liberar_conexion(conexionGamecard);
	liberar_conexion(conexionTeam);
}
