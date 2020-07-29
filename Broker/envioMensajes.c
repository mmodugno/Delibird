/*
 * envioMensajes.c
 *
 *  Created on: 13 jul. 2020
 *      Author: utnso
 */

#include "envioMensajes.h"

void envioColaNewPokemon() {
	t_list *usersConACK = list_create();
	t_list *usersSinACK = list_create();
	char* username;
	particion *mensajeNewEnMemo;
	buddy* mensajeNewEnMemoBuddy;

	bool esSuscriptor(char* userActual) {
		return (!strcmp(userActual, username));
	}

	void llenarUserSinACK(char* userActual) {
		username = string_duplicate(userActual);
		if (!list_any_satisfy(usersConACK, (void*) esSuscriptor)) {
			list_add(usersSinACK, userActual);
		}
	}

	bool menNewQueFalten(particion *part) {
		if (part->tipoMensaje == NEW_POKEMON) {
			//me fijo que se hayan mandado a todos los suscriptores
			usersConACK = list_duplicate(part->acknoleged);

			list_iterate(suscriptoresNewPokemon, (void*) llenarUserSinACK);

			if (usersSinACK->elements_count) {
				return 1;
			}
			list_clean(usersSinACK);
			return 0;
		}
		list_clean(usersSinACK);
		return 0;
	}

	bool menNewQueFaltenEnBuddy(buddy* unBuddy) {
		return menNewQueFalten(unBuddy->particion);
	}

	while (1) {

		sem_wait(&suscripcionAColaNEW);

		//cambio a un if para que mande la señal al semaforo suscripcionACola porque sino se iba a quedar ahi
		if (suscriptoresNewPokemon->elements_count > 0) {

			sem_wait(&usoMemoria);


			//BUSCO UN MENSAJE QUE NO HAYA ENVIADO
			if (!strcmp(algoritmo_memoria, "PARTICIONES")) {
				if (suscriptoresNewPokemon->elements_count) {
					list_sort(tablaDeParticiones, (void*) baseMasChica);
					mensajeNewEnMemo = list_find(tablaDeParticiones,
							(void*) menNewQueFalten);
					enviarPorTipo(mensajeNewEnMemo, usersSinACK);
					//list_clean_and_destroy_elements(usersSinACK, free);
				}
			}

			if (!strcmp(algoritmo_memoria, "BS")) {
				//sem_wait(&suscripcionACola);
				if (suscriptoresNewPokemon->elements_count) {

					mensajeNewEnMemoBuddy = list_find(tablaDeParticiones,
							(void*) menNewQueFaltenEnBuddy);

					if (mensajeNewEnMemoBuddy == NULL) {

						particion* particionVacia = NULL;

						enviarPorTipo(particionVacia, usersSinACK);
					} else {
						enviarPorTipo(mensajeNewEnMemoBuddy->particion,
								usersSinACK);
					}

				}
				//list_clean_and_destroy_elements(usersSinACK, free);
			}

			//nose si list_destroy_and_destroy_elements() eliminaria los username que siguen estando en la de ack de esa part

			//clean si lo voy limpiandocada vez que lo uso
			list_clean(usersSinACK);
			list_clean(usersConACK);

			//free(username);
			sem_post(&usoMemoria);
		}
		sem_post(&suscripcionAColaNEW);
		sleep(2);
	}
}

//conectarme con cada uno que necesite mandarle
bool esTeam(char* username) {
	return !strcmp(username, "TEAM");
}
bool esGameCard(char* username) {
	return !strcmp(username, "GAMECARD");
}
bool esGameBoy(char* username) {
	return !strcmp(username, "GAMEBOY");
}

void enviarPorTipo(particion* partAEnviar, t_list* usersAEnviar) {
	broker_new_pokemon* newEnMemo;
	broker_get_pokemon* getEnMemo;
	broker_appeared_pokemon* appEnMemo;
	broker_localized_pokemon* localizedEnMemo;
	broker_catch_pokemon* catchEnMemo;
	broker_caught_pokemon* caughtEnMemo;

	if (partAEnviar != NULL && !list_is_empty(usersAEnviar)) {
		switch (partAEnviar->tipoMensaje) {
		case NEW_POKEMON:
			newEnMemo = leerdeMemoriaNEW(partAEnviar);
			enviarASuscriptoresNEW(newEnMemo, usersAEnviar);

			free(newEnMemo->datos->nombrePokemon);
			free(newEnMemo);
			break;
		case GET_POKEMON:
			getEnMemo = leerdeMemoriaGET(partAEnviar);
			enviarASuscriptoresGET(getEnMemo, usersAEnviar);

			free(getEnMemo->datos->nombrePokemon);
			free(getEnMemo);
			break;
		case APPEARED_POKEMON:
			appEnMemo = leerdeMemoriaAPPEARED(partAEnviar);
			enviarASuscriptoresAPPEARED(appEnMemo, usersAEnviar);

			free(appEnMemo->datos->nombrePokemon);
			free(appEnMemo);
			break;
		case LOCALIZED_POKEMON:
			localizedEnMemo = leerdeMemoriaLOCALIZED(partAEnviar);
			enviarASuscriptoresLOCALIZED(localizedEnMemo, usersAEnviar);

			free(localizedEnMemo->datos->posX);
			free(localizedEnMemo->datos->posY);
			free(localizedEnMemo->datos->nombrePokemon);
			free(localizedEnMemo);
			break;
		case CATCH_POKEMON:
			catchEnMemo = leerdeMemoriaCATCH(partAEnviar);
			enviarASuscriptoresCATCH(catchEnMemo, usersAEnviar);

			free(catchEnMemo->datos->nombrePokemon);
			free(catchEnMemo);
			break;
		case CAUGHT_POKEMON:
			caughtEnMemo = leerdeMemoriaCAUGHT(partAEnviar);
			enviarASuscriptoresCAUGHT(caughtEnMemo, usersAEnviar);

			free(caughtEnMemo);
			break;
		}
	}
}

void enviarASuscriptoresNEW(broker_new_pokemon* newAEnviar,t_list* usersAEnviar) {
	//uint32_t noSeConecto = 0;

	if (list_any_satisfy(usersAEnviar, (void*) esTeam)) {
		//conexion con TEAM
		conexionTeam = crear_conexion(ip_team, puerto_team);

		if (conexionTeam != -1) {
			enviar_Cola_New_Pokemon(newAEnviar, conexionTeam);
			log_info(logEnviarNuevo,
					"Envie a TEAM el mensaje de la cola %s con id: %d",
					"NEW_POKEMON", newAEnviar->id);
			liberar_conexion(conexionTeam);
		}
		conexionTeam2 = crear_conexion(ip_team, puerto_team2);
		if (conexionTeam2 != -1) {
			enviar_Cola_New_Pokemon(newAEnviar, conexionTeam2);
			log_info(logEnviarNuevo,
					"Envie a TEAM2 el mensaje de la cola %s con id: %d",
					"NEW_POKEMON", newAEnviar->id);
			liberar_conexion(conexionTeam2);
		}
	}
	if (list_any_satisfy(usersAEnviar, (void*) esGameBoy)) {
		//conexion con GAMEBOY
		conexionGameboy = crear_conexion(ip_gameboy, puerto_gameboy);
		if (conexionGameboy != -1) {
			enviar_Cola_New_Pokemon(newAEnviar, conexionGameboy);
			log_info(logEnviarNuevo,
					"Envie a GAMEBOY el mensaje de la cola %s con id: %d",
					"NEW_POKEMON", newAEnviar->id);
			liberar_conexion(conexionGameboy);
		}
	}
	if (list_any_satisfy(usersAEnviar, (void*) esGameCard)) {
		//conexion con GAMECARD
		int conexion = crear_conexion(ip_gamecard, puerto_gamecard);
		if (conexion != -1) {
			enviar_Cola_New_Pokemon(newAEnviar, conexion);
			log_info(logEnviarNuevo,
					"Envie a GAMECARD el mensaje  de la cola %s con id: %d",
					"NEW_POKEMON", newAEnviar->id);
			liberar_conexion(conexion);
		}
	}
}

void enviarASuscriptoresAPPEARED(broker_appeared_pokemon* appAEnviar,
		t_list* usersAEnviar) {
	if (list_any_satisfy(usersAEnviar, (void*) esTeam)) {
		//conexion con TEAM
		conexionTeam = crear_conexion(ip_team, puerto_team);
		if (conexionTeam != -1) {
			enviar_cola_Appeared_Pokemon(appAEnviar, conexionTeam);
			log_info(logEnviarNuevo,
					"Envie a TEAM el mensaje de la cola %s con id: %d",
					"APPEARED_POKEMON", appAEnviar->id);
			liberar_conexion(conexionTeam);
		}
		conexionTeam2 = crear_conexion(ip_team, puerto_team2);
		if (conexionTeam2 != -1) {
			enviar_cola_Appeared_Pokemon(appAEnviar, conexionTeam2);
			log_info(logEnviarNuevo,
					"Envie a TEAM2 el mensaje de la cola %s con id: %d",
					"APPEARED_POKEMON", appAEnviar->id);
			liberar_conexion(conexionTeam2);
		}
	}
	if (list_any_satisfy(usersAEnviar, (void*) esGameBoy)) {
		//conexion con GAMEBOY
		conexionGameboy = crear_conexion(ip_gameboy, puerto_gameboy);
		if (conexionGameboy != -1) {
			enviar_cola_Appeared_Pokemon(appAEnviar, conexionGameboy);
			log_info(logEnviarNuevo,
					"Envie a GAMEBOY el mensaje de la cola %s con id: %d",
					"APPEARED_POKEMON", appAEnviar->id);
			liberar_conexion(conexionGameboy);
		}

	}
	if (list_any_satisfy(usersAEnviar, (void*) esGameCard)) {
		//conexion con GAMECARD
		int conexion = crear_conexion(ip_gamecard, puerto_gamecard);
		if (conexion != -1) {
			enviar_cola_Appeared_Pokemon(appAEnviar, conexion);
			log_info(logEnviarNuevo,
					"Envie a GAMECARD el mensaje de la cola APPEARED_POKEMON con id: %d",
					appAEnviar->id);
			liberar_conexion(conexion);
		}
	}
}

void enviarASuscriptoresCATCH(broker_catch_pokemon* catchAEnviar,
		t_list* usersAEnviar) {
	if (list_any_satisfy(usersAEnviar, (void*) esTeam)) {
		//conexion con TEAM
		conexionTeam = crear_conexion(ip_team, puerto_team);
		if (conexionTeam != -1) {
			enviar_cola_Catch_Pokemon(catchAEnviar, conexionTeam);
			log_info(logEnviarNuevo,
					"Envie a TEAM el mensaje de la cola CATCH_POKEMON con id: %d",
					catchAEnviar->id);
			liberar_conexion(conexionTeam);
		}
		conexionTeam2 = crear_conexion(ip_team, puerto_team2);
		if (conexionTeam2 != -1) {
			enviar_cola_Catch_Pokemon(catchAEnviar, conexionTeam2);
			log_info(logEnviarNuevo,
					"Envie a TEAM2 el mensaje de la cola %s con id: %d",
					"CATCH_POKEMON", catchAEnviar->id);
			liberar_conexion(conexionTeam2);
		}
	}
	if (list_any_satisfy(usersAEnviar, (void*) esGameBoy)) {
		//conexion con GAMEBOY
		conexionGameboy = crear_conexion(ip_gameboy, puerto_gameboy);
		if (conexionGameboy != -1) {
			enviar_cola_Catch_Pokemon(catchAEnviar, conexionGameboy);
			log_info(logEnviarNuevo,
					"Envie a GAMEBOY el mensaje de la cola CATCH_POKEMON con id: %d",
					catchAEnviar->id);
			liberar_conexion(conexionGameboy);
		}
	}
	if (list_any_satisfy(usersAEnviar, (void*) esGameCard)) {
		//conexion con GAMECARD
		int conexion = crear_conexion(ip_gamecard, puerto_gamecard);
		if (conexion != -1) {
			enviar_cola_Catch_Pokemon(catchAEnviar, conexion);
			log_info(logEnviarNuevo,
					"Envie a GAMECARD el mensaje de la cola CATCH_POKEMON con id: %d",
					catchAEnviar->id);
			liberar_conexion(conexion);
		}
	}
}

void enviarASuscriptoresCAUGHT(broker_caught_pokemon* caughtAEnviar,
		t_list* usersAEnviar) {
	if (list_any_satisfy(usersAEnviar, (void*) esTeam)) {
		//conexion con TEAM
		conexionTeam = crear_conexion(ip_team, puerto_team);
		if (conexionTeam != -1) {
			enviar_cola_Caught_Pokemon(caughtAEnviar, conexionTeam);
			log_info(logEnviarNuevo,
					"Envie a TEAM el mensaje de la cola CAUGHT_POKEMON con id: %d",
					caughtAEnviar->id);
			liberar_conexion(conexionTeam);
		}
		conexionTeam2 = crear_conexion(ip_team, puerto_team2);
		if (conexionTeam2 != -1) {
			enviar_cola_Caught_Pokemon(caughtAEnviar, conexionTeam2);
			log_info(logEnviarNuevo,
					"Envie a TEAM2 el mensaje de la cola %s con id: %d",
					"CAUGHT_POKEMON", caughtAEnviar->id);
			liberar_conexion(conexionTeam2);
		}
	}
	if (list_any_satisfy(usersAEnviar, (void*) esGameBoy)) {
		//conexion con GAMEBOY
		conexionGameboy = crear_conexion(ip_gameboy, puerto_gameboy);
		if (conexionGameboy != -1) {
			enviar_cola_Caught_Pokemon(caughtAEnviar, conexionGameboy);
			log_info(logEnviarNuevo,
					"Envie a GAMEBOY el mensaje de la cola CAUGHT_POKEMON con id: %d",
					caughtAEnviar->id);
			liberar_conexion(conexionGameboy);
		}
	}
	if (list_any_satisfy(usersAEnviar, (void*) esGameCard)) {
		//conexion con GAMECARD
		int conexion = crear_conexion(ip_gamecard, puerto_gamecard);
		if (conexion != -1) {
			enviar_cola_Caught_Pokemon(caughtAEnviar, conexion);
			log_info(logEnviarNuevo,
					"Envie a GAMECARD el mensaje de la cola CAUGHT_POKEMON con id: %d",
					caughtAEnviar->id);
			liberar_conexion(conexion);
		}
	}
}

void enviarASuscriptoresGET(broker_get_pokemon* getAEnviar,
		t_list* usersAEnviar) {
	if (list_any_satisfy(usersAEnviar, (void*) esTeam)) {
		//conexion con TEAM
		conexionTeam = crear_conexion(ip_team, puerto_team);
		if (conexionTeam != -1) {
			enviar_cola_Get_Pokemon(getAEnviar, conexionTeam);
			log_info(logEnviarNuevo,
					"Envie a TEAM el mensaje de la cola GET_POKEMON con id: %d",
					getAEnviar->id);
			liberar_conexion(conexionTeam);
		}
		conexionTeam2 = crear_conexion(ip_team, puerto_team2);
		if (conexionTeam2 != -1) {
			enviar_cola_Get_Pokemon(getAEnviar, conexionTeam2);
			log_info(logEnviarNuevo,
					"Envie a TEAM2 el mensaje de la cola %s con id: %d",
					"GET_POKEMON", getAEnviar->id);
			liberar_conexion(conexionTeam2);
		}
	}
	if (list_any_satisfy(usersAEnviar, (void*) esGameBoy)) {
		//conexion con GAMEBOY
		conexionGameboy = crear_conexion(ip_gameboy, puerto_gameboy);
		if (conexionGameboy != -1) {
			enviar_cola_Get_Pokemon(getAEnviar, conexionGameboy);
			log_info(logEnviarNuevo,
					"Envie a GAMEBOY el mensaje de la cola GET_POKEMON con id: %d",
					getAEnviar->id);
			liberar_conexion(conexionGameboy);
		}
	}
	if (list_any_satisfy(usersAEnviar, (void*) esGameCard)) {
		//conexion con GAMECARD
		int conexion = crear_conexion(ip_gamecard, puerto_gamecard);
		if (conexion != -1) {
			enviar_cola_Get_Pokemon(getAEnviar, conexion);
			log_info(logEnviarNuevo,
					"Envie a GAMECARD el mensaje de la cola GET_POKEMON con id: %d",
					getAEnviar->id);
			liberar_conexion(conexion);
		}
	}
}

void enviarASuscriptoresLOCALIZED(broker_localized_pokemon* localizedAEnviar,
		t_list* usersAEnviar) {
	if (list_any_satisfy(usersAEnviar, (void*) esTeam)) {
		//conexion con TEAM
		conexionTeam = crear_conexion(ip_team, puerto_team);
		if (conexionTeam != -1) {
			enviar_cola_Localized_Pokemon(localizedAEnviar, conexionTeam);
			log_info(logEnviarNuevo,
					"Envie a TEAM el mensaje de la cola LOCALIZED_POKEMON con id: %d",
					localizedAEnviar->id);
			liberar_conexion(conexionTeam);
		}
		conexionTeam2 = crear_conexion(ip_team, puerto_team2);
		if (conexionTeam2 != -1) {
			enviar_cola_Localized_Pokemon(localizedAEnviar, conexionTeam2);
			log_info(logEnviarNuevo,
					"Envie a TEAM2 el mensaje de la cola %s con id: %d",
					"LOCALIZED_POKEMON", localizedAEnviar->id);
			liberar_conexion(conexionTeam2);
		}
	}
	if (list_any_satisfy(usersAEnviar, (void*) esGameBoy)) {
		//conexion con GAMEBOY
		conexionGameboy = crear_conexion(ip_gameboy, puerto_gameboy);
		if (conexionGameboy != -1) {
			enviar_cola_Localized_Pokemon(localizedAEnviar, conexionGameboy);
			log_info(logEnviarNuevo,
					"Envie a GAMEBOY el mensaje de la cola LOCALIZED_POKEMON con id: %d",
					localizedAEnviar->id);
			liberar_conexion(conexionGameboy);
		}
	}
	if (list_any_satisfy(usersAEnviar, (void*) esGameCard)) {
		//conexion con GAMECARD
		int conexion = crear_conexion(ip_gamecard, puerto_gamecard);
		if (conexion != -1) {
			enviar_cola_Localized_Pokemon(localizedAEnviar, conexion);
			log_info(logEnviarNuevo,
					"Envie a GAMECARD el mensaje de la cola LOCALIZED_POKEMON con id: %d",
					localizedAEnviar->id);
			liberar_conexion(conexion);
		}
	}

}

void envioColaGetPokemon() {
	t_list *usersConACK = list_create();
	t_list *usersSinACK = list_create();
	char* username;
	particion *mensajeGetEnMemo;
	buddy* mensajeGetEnMemoBuddy;

	bool esSuscriptor(char* userActual) {
		return (!strcmp(userActual, username));
	}

	void llenarUserSinACK(char* userActual) {
		username = string_duplicate(userActual);
		if (!list_any_satisfy(usersConACK, (void*) esSuscriptor)) {
			list_add(usersSinACK, userActual);
		}
	}

	bool menGetQueFalten(particion *part) {
		if (part->tipoMensaje == GET_POKEMON) {
			//me fijo que se hayan mandado a todos los suscriptores
			usersConACK = list_duplicate(part->acknoleged);

			list_iterate(suscriptoresGetPokemon, (void*) llenarUserSinACK);

			if (usersSinACK->elements_count) {
				return 1;
			}
			list_clean(usersSinACK);
			return 0;
		}
		list_clean(usersSinACK);
		return 0;
	}

	bool menGetQueFaltenEnBuddy(buddy* unBuddy) {
		return menGetQueFalten(unBuddy->particion);
	}

	while (1) {

		sem_wait(&suscripcionAColaGET);

		//cambio a un if para que mande la señal al semaforo suscripcionACola porque sino se iba a quedar ahi
		if (suscriptoresGetPokemon->elements_count > 0) {

			sem_wait(&usoMemoria);


			//BUSCO UN MENSAJE QUE NO HAYA ENVIADO
			if (!strcmp(algoritmo_memoria, "PARTICIONES")) {
				if (suscriptoresGetPokemon->elements_count) {
					list_sort(tablaDeParticiones, (void*) baseMasChica);
					mensajeGetEnMemo = list_find(tablaDeParticiones,
							(void*) menGetQueFalten);
					enviarPorTipo(mensajeGetEnMemo, usersSinACK);
				}
			}

			if (!strcmp(algoritmo_memoria, "BS")) {
				//sem_wait(&suscripcionACola);
				if (suscriptoresGetPokemon->elements_count) {
					mensajeGetEnMemoBuddy = list_find(tablaDeParticiones,
							(void*) menGetQueFaltenEnBuddy);

					if (mensajeGetEnMemoBuddy == NULL) {
						particion* particionVacia = NULL;
						enviarPorTipo(particionVacia, usersSinACK);

					} else {
						enviarPorTipo(mensajeGetEnMemoBuddy->particion,
								usersSinACK);
					}

				}
			}

			//clean si lo voy limpiandocada vez que lo uso
			list_clean(usersSinACK);
			list_clean(usersConACK);

			sem_post(&usoMemoria);
		}
		sem_post(&suscripcionAColaGET);
		sleep(2);
	}
}

void envioColaLocalizedPokemon() {
	t_list *usersConACK = list_create();
	t_list *usersSinACK = list_create();
	char* username;
	particion *mensajeLocalizedEnMemo;
	buddy* mensajeLocalizedEnMemoBuddy;

	bool esSuscriptor(char* userActual) {
		return (!strcmp(userActual, username));
	}

	void llenarUserSinACK(char* userActual) {
		username = string_duplicate(userActual);
		if (!list_any_satisfy(usersConACK, (void*) esSuscriptor)) {
			list_add(usersSinACK, userActual);
		}
	}

	bool menLocalizedQueFalten(particion *part) {
		if (part->tipoMensaje == LOCALIZED_POKEMON) {
			//me fijo que se hayan mandado a todos los suscriptores
			usersConACK = list_duplicate(part->acknoleged);

			list_iterate(suscriptoresLocalizedPokemon,
					(void*) llenarUserSinACK);

			if (usersSinACK->elements_count) {
				return 1;
			}
			list_clean(usersSinACK);
			return 0;
		}
		list_clean(usersSinACK);
		return 0;
	}

	bool menLocalizedQueFaltenEnBuddy(buddy* unBuddy) {
		return menLocalizedQueFalten(unBuddy->particion);
	}

	while (1) {

		sem_wait(&suscripcionAColaLOCALIZED);

		//cambio a un if para que mande la señal al semaforo suscripcionACola porque sino se iba a quedar ahi
		if (suscriptoresLocalizedPokemon->elements_count > 0) {

			sem_wait(&usoMemoria);


			//BUSCO UN MENSAJE QUE NO HAYA ENVIADO
			if (!strcmp(algoritmo_memoria, "PARTICIONES")) {
				if (suscriptoresLocalizedPokemon->elements_count) {
					list_sort(tablaDeParticiones, (void*) baseMasChica);
					mensajeLocalizedEnMemo = list_find(tablaDeParticiones,
							(void*) menLocalizedQueFalten);
					enviarPorTipo(mensajeLocalizedEnMemo, usersSinACK);
				}
			}

			if (!strcmp(algoritmo_memoria, "BS")) {
				//sem_wait(&suscripcionACola);
				if (suscriptoresLocalizedPokemon->elements_count) {
					mensajeLocalizedEnMemoBuddy = list_find(tablaDeParticiones,
							(void*) menLocalizedQueFaltenEnBuddy);

					if (mensajeLocalizedEnMemoBuddy == NULL) {
						particion* particionVacia = NULL;
						enviarPorTipo(particionVacia, usersSinACK);
					} else {

						enviarPorTipo(mensajeLocalizedEnMemoBuddy->particion,
								usersSinACK);
					}

				}
			}

			//clean si lo voy limpiandocada vez que lo uso
			list_clean(usersSinACK);
			list_clean(usersConACK);

			sem_post(&usoMemoria);
		}
		sem_post(&suscripcionAColaLOCALIZED);
		sleep(2);
	}
}

void envioColaAppearedPokemon() {
	t_list *usersConACK = list_create();
	t_list *usersSinACK = list_create();
	char* username;
	particion *mensajeAppearedEnMemo;
	buddy* mensajeAppearedEnMemoBuddy;

	bool esSuscriptor(char* userActual) {
		return (!strcmp(userActual, username));
	}

	void llenarUserSinACK(char* userActual) {
		username = string_duplicate(userActual);
		if (!list_any_satisfy(usersConACK, (void*) esSuscriptor)) {
			list_add(usersSinACK, userActual);
		}
	}

	bool menAppearedQueFalten(particion *part) {
		if (part->tipoMensaje == APPEARED_POKEMON) {
			//me fijo que se hayan mandado a todos los suscriptores
			usersConACK = list_duplicate(part->acknoleged);

			list_iterate(suscriptoresAppearedPokemon, (void*) llenarUserSinACK);

			if (usersSinACK->elements_count) {
				return 1;
			}
			list_clean(usersSinACK);
			return 0;
		}
		list_clean(usersSinACK);
		return 0;
	}

	bool menAppearedQueFaltenEnBuddy(buddy* unBuddy) {
		return menAppearedQueFalten(unBuddy->particion);
	}

	while (1) {

		sem_wait(&suscripcionAColaAPPEARED);

		//cambio a un if para que mande la señal al semaforo suscripcionACola porque sino se iba a quedar ahi
		if (suscriptoresAppearedPokemon->elements_count > 0) {

			sem_wait(&usoMemoria);


			//BUSCO UN MENSAJE QUE NO HAYA ENVIADO
			if (!strcmp(algoritmo_memoria, "PARTICIONES")) {
				if (suscriptoresAppearedPokemon->elements_count) {
					list_sort(tablaDeParticiones, (void*) baseMasChica);
					mensajeAppearedEnMemo = list_find(tablaDeParticiones,
							(void*) menAppearedQueFalten);
					enviarPorTipo(mensajeAppearedEnMemo, usersSinACK);
				}
			}

			if (!strcmp(algoritmo_memoria, "BS")) {
				//sem_wait(&suscripcionACola);
				if (suscriptoresAppearedPokemon->elements_count) {
					mensajeAppearedEnMemoBuddy = list_find(tablaDeParticiones,
							(void*) menAppearedQueFaltenEnBuddy);
					if (mensajeAppearedEnMemoBuddy == NULL) {

						particion* particionVacia = NULL;

						enviarPorTipo(particionVacia, usersSinACK);
					} else {
						enviarPorTipo(mensajeAppearedEnMemoBuddy->particion,
								usersSinACK);
					}
				}
			}

			//clean si lo voy limpiandocada vez que lo uso
			list_clean(usersSinACK);
			list_clean(usersConACK);

			sem_post(&usoMemoria);
		}
		sem_post(&suscripcionAColaAPPEARED);
		sleep(2);
	}
}

void envioColaCatchPokemon() {
	t_list *usersConACK = list_create();
	t_list *usersSinACK = list_create();
	char* username;
	particion *mensajeCatchEnMemo;
	buddy* mensajeCatchEnMemoBuddy;

	bool esSuscriptor(char* userActual) {
		return (!strcmp(userActual, username));
	}

	void llenarUserSinACK(char* userActual) {
		username = string_duplicate(userActual);
		if (!list_any_satisfy(usersConACK, (void*) esSuscriptor)) {
			list_add(usersSinACK, userActual);
		}
	}

	bool menCatchQueFalten(particion *part) {
		if (part->tipoMensaje == CATCH_POKEMON) {
			//me fijo que se hayan mandado a todos los suscriptores
			usersConACK = list_duplicate(part->acknoleged);

			list_iterate(suscriptoresCatchPokemon, (void*) llenarUserSinACK);

			if (usersSinACK->elements_count) {
				return 1;
			}
			list_clean(usersSinACK);
			return 0;
		}
		list_clean(usersSinACK);
		return 0;
	}

	bool menCatchQueFaltenEnBuddy(buddy* unBuddy) {
		return menCatchQueFalten(unBuddy->particion);
	}

	while (1) {

		sem_wait(&suscripcionAColaCATCH);

		//cambio a un if para que mande la señal al semaforo suscripcionACola porque sino se iba a quedar ahi
		if (suscriptoresCatchPokemon->elements_count > 0) {

			sem_wait(&usoMemoria);



			//BUSCO UN MENSAJE QUE NO HAYA ENVIADO
			if (!strcmp(algoritmo_memoria, "PARTICIONES")) {
				if (suscriptoresCatchPokemon->elements_count) {
					list_sort(tablaDeParticiones, (void*) baseMasChica);
					mensajeCatchEnMemo = list_find(tablaDeParticiones,
							(void*) menCatchQueFalten);
					enviarPorTipo(mensajeCatchEnMemo, usersSinACK);
				}
			}

			if (!strcmp(algoritmo_memoria, "BS")) {
				//sem_wait(&suscripcionACola);
				if (suscriptoresCatchPokemon->elements_count) {
					mensajeCatchEnMemoBuddy = list_find(tablaDeParticiones,
							(void*) menCatchQueFaltenEnBuddy);

					if (mensajeCatchEnMemoBuddy == NULL) {

						particion* particionVacia = NULL;

						enviarPorTipo(particionVacia, usersSinACK);
					} else {
						enviarPorTipo(mensajeCatchEnMemoBuddy->particion,
								usersSinACK);
					}

				}
			}

			//clean si lo voy limpiandocada vez que lo uso
			list_clean(usersSinACK);
			list_clean(usersConACK);

			sem_post(&usoMemoria);
		}
		sem_post(&suscripcionAColaCATCH);
		sleep(2);
	}
}

void envioColaCaughtPokemon() {
	t_list *usersConACK = list_create();
	t_list *usersSinACK = list_create();
	char* username;
	particion *mensajeCaughtEnMemo;
	buddy* mensajeCaughtEnMemoBuddy;

	bool esSuscriptor(char* userActual) {
		return (!strcmp(userActual, username));
	}

	void llenarUserSinACK(char* userActual) {
		username = string_duplicate(userActual);
		if (!list_any_satisfy(usersConACK, (void*) esSuscriptor)) {
			list_add(usersSinACK, userActual);
		}
	}

	bool menCaughtQueFalten(particion *part) {
		if (part->tipoMensaje == CAUGHT_POKEMON) {
			//me fijo que se hayan mandado a todos los suscriptores
			usersConACK = list_duplicate(part->acknoleged);

			list_iterate(suscriptoresCaughtPokemon, (void*) llenarUserSinACK);

			if (usersSinACK->elements_count) {
				return 1;
			}
			list_clean(usersSinACK);
			return 0;
		}
		list_clean(usersSinACK);
		return 0;
	}

	bool menCaughtQueFaltenEnBuddy(buddy* unBuddy) {
		return menCaughtQueFalten(unBuddy->particion);
	}

	while (1) {

		sem_wait(&suscripcionAColaCAUGHT);

		//cambio a un if para que mande la señal al semaforo suscripcionACola porque sino se iba a quedar ahi
		if (suscriptoresCaughtPokemon->elements_count > 0) {

			sem_wait(&usoMemoria);

			list_sort(tablaDeParticiones, (void*) baseMasChica);
			//BUSCO UN MENSAJE QUE NO HAYA ENVIADO
			if (!strcmp(algoritmo_memoria, "PARTICIONES")) {
				if (suscriptoresCaughtPokemon->elements_count) {
					mensajeCaughtEnMemo = list_find(tablaDeParticiones,
							(void*) menCaughtQueFalten);
					enviarPorTipo(mensajeCaughtEnMemo, usersSinACK);
				}
			}

			if (!strcmp(algoritmo_memoria, "BS")) {
				//sem_wait(&suscripcionACola);
				if (suscriptoresCaughtPokemon->elements_count) {
					mensajeCaughtEnMemoBuddy = list_find(tablaDeParticiones,
							(void*) menCaughtQueFaltenEnBuddy);
					if (mensajeCaughtEnMemoBuddy == NULL) {
						particion* particionVacia = NULL;
						enviarPorTipo(particionVacia, usersSinACK);
					} else {
						enviarPorTipo(mensajeCaughtEnMemoBuddy->particion,
								usersSinACK);
					}

				}
			}

			//clean si lo voy limpiandocada vez que lo uso
			list_clean(usersSinACK);
			list_clean(usersConACK);

			sem_post(&usoMemoria);
		}
		sem_post(&suscripcionAColaCAUGHT);
		sleep(2);
	}
}

int crear_conexion(char *ip, char* puerto) {
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family,
			server_info->ai_socktype, server_info->ai_protocol);

	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen)
			!= 0) {
		//printf("error");
		freeaddrinfo(server_info);
		return -1;
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

void liberar_conexion(int socket_cliente) {
	close(socket_cliente);
}

t_buffer* serializarMensajeColaNEW(broker_new_pokemon* newAEnviar) {

	//serializacion
	//1. uint32_t idMensaje
	//2. uint32_t tamanioNombre;
	//3. char* nombrePokemon;
	//4. uint32_t posX;
	//5. uint32_t posY;
	//6. uint32_t CantidadPokemons;

	t_buffer* mensaje = malloc(sizeof(t_buffer));
	mensaje->size = sizeof(uint32_t) * 5 + newAEnviar->datos->tamanioNombre;
	mensaje->stream = malloc(mensaje->size);

	int offset = 0;

	memcpy(mensaje->stream + offset, &(newAEnviar->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje->stream + offset, &(newAEnviar->datos->tamanioNombre),
			sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje->stream + offset, (newAEnviar->datos->nombrePokemon),
			newAEnviar->datos->tamanioNombre);
	offset += newAEnviar->datos->tamanioNombre;

	memcpy(mensaje->stream + offset, &(newAEnviar->datos->posX),
			sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje->stream + offset, &(newAEnviar->datos->posY),
			sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje->stream + offset, &(newAEnviar->datos->cantidadPokemon),
			sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return mensaje;
}

t_buffer* serializarMensajeColaAPPEARED(
		broker_appeared_pokemon* brokerAppearedPokemon) {

	// serializacion
	//1. uint32_t idMensaje;
	//2. uint32_t tamanioNombre;
	//3. char* nombrePokemon;
	//4. uint32_t posX;
	//5. uint32_t posY;
	//6. uint32_t id_relativo;

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = sizeof(uint32_t) + sizeof(uint32_t) // idMensaje, id
			+ sizeof(uint32_t) * 3 // posX, posY, tamanioNombre
			+ (brokerAppearedPokemon->datos->tamanioNombre); //longitud del strind nombre de pokemon

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	memcpy(buffer->stream + offset, &(brokerAppearedPokemon->id),
			sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset,
			&(brokerAppearedPokemon->datos->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset,
			(brokerAppearedPokemon->datos->nombrePokemon),
			brokerAppearedPokemon->datos->tamanioNombre);
	offset += brokerAppearedPokemon->datos->tamanioNombre;

	memcpy(buffer->stream + offset, &(brokerAppearedPokemon->datos->posX),
			sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(brokerAppearedPokemon->datos->posY),
			sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(brokerAppearedPokemon->id_relativo),sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return buffer;

}

t_buffer* serializarMensajeColaLOCALIZED(
		broker_localized_pokemon* brokerLocalizedPokemon) {

	//serializacion
	//1. uint32_t idMensaje;
	//2. uint32_t tamanioNombre;
	//3. char* nombrePokemon;
	//4. uint32_t cantidadPosiciones;
	//5. uint32_t* posX;
	//6. uint32_t* posY;
	//7. uint32_t idCorrelativo;

	uint32_t posiciones = 0;

	uint32_t posX, posY;

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = sizeof(uint32_t) * 4
			+ ((sizeof(uint32_t) * 2)
					* brokerLocalizedPokemon->datos->cantidadPosiciones)
			+ brokerLocalizedPokemon->datos->tamanioNombre; //longitud del string nombre de pokemon

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	//id del mensaje
	memcpy(buffer->stream + offset, &(brokerLocalizedPokemon->id),
			sizeof(uint32_t));
	offset += sizeof(uint32_t);

	//longitud del string nombre de pokemon
	memcpy(buffer->stream + offset,
			&(brokerLocalizedPokemon->datos->tamanioNombre), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	//string nombre de pokemon
	memcpy(buffer->stream + offset,
			(brokerLocalizedPokemon->datos->nombrePokemon),
			brokerLocalizedPokemon->datos->tamanioNombre);
	offset += brokerLocalizedPokemon->datos->tamanioNombre;

	//cantidad de Posciones
	memcpy(buffer->stream + offset,
			&(brokerLocalizedPokemon->datos->cantidadPosiciones),
			sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for (posiciones = 0;
			posiciones < brokerLocalizedPokemon->datos->cantidadPosiciones;
			posiciones++) {

		posX = brokerLocalizedPokemon->datos->posX[posiciones];
		posY = brokerLocalizedPokemon->datos->posY[posiciones];

		//tengo mis dudas por el &
		memcpy(buffer->stream + offset, &posX, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, &posY, sizeof(uint32_t));
		offset += sizeof(uint32_t);
	}

	//id correlativo
	memcpy(buffer->stream + offset, &(brokerLocalizedPokemon->id_relativo),
			sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return buffer;
}

t_buffer* serializarMensajeColaGET(broker_get_pokemon* getAEnviar) {

	//serializacion
	//1. uint32_t idMensaje;
	//2. uint32_t tamanioNombre;
	//3. char* nombrePokemon;

	t_buffer* mensaje = malloc(sizeof(t_buffer));

	mensaje->size = sizeof(uint32_t) * 2 + getAEnviar->datos->tamanioNombre;
	mensaje->stream = malloc(mensaje->size);

	int offset = 0;

	memcpy(mensaje->stream + offset, &(getAEnviar->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje->stream + offset, &(getAEnviar->datos->tamanioNombre),
			sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje->stream + offset, (getAEnviar->datos->nombrePokemon),
			getAEnviar->datos->tamanioNombre);
	offset += getAEnviar->datos->tamanioNombre;

	return mensaje;
}

t_buffer* serializarMensajeColaCATCH(broker_catch_pokemon* catchAEnviar) {

	// serializacion
	//1. uint32_t id;
	//2. uint32_t tamanioNombre;
	//3. char* nombrePokemon;
	//4. uint32_t posX;
	//5. uint32_t posY;

	t_buffer* mensaje = malloc(sizeof(t_buffer));

	mensaje->size = sizeof(uint32_t) * 4 + (catchAEnviar->datos->tamanioNombre);

	mensaje->stream = malloc(mensaje->size);
	int offset = 0;

	memcpy(mensaje->stream + offset, &(catchAEnviar->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje->stream + offset, &(catchAEnviar->datos->tamanioNombre),
			sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje->stream + offset, (catchAEnviar->datos->nombrePokemon),
			catchAEnviar->datos->tamanioNombre);
	offset += (catchAEnviar->datos->tamanioNombre);

	memcpy(mensaje->stream + offset, &(catchAEnviar->datos->posX),
			sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje->stream + offset, &(catchAEnviar->datos->posY),
			sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return mensaje;
}

t_buffer* serializarMensajeColaCAUGHT(broker_caught_pokemon* caughtAEnviar) {

	// serializacion
	//1. uint32_t id;
	//2. uint32_t puedoAtraparlo;
	//3. uint32_t id_rel;

	t_buffer* mensaje = malloc(sizeof(t_buffer));

	mensaje->size = sizeof(uint32_t) * 3;
	mensaje->stream = malloc(mensaje->size);

	int offset = 0;

	memcpy(mensaje->stream + offset, &(caughtAEnviar->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje->stream + offset, &(caughtAEnviar->datos->puedoAtraparlo),
			sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(mensaje->stream + offset, &(caughtAEnviar->id_relativo),
			sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return mensaje;
}

void enviar_Cola_New_Pokemon(broker_new_pokemon *brokerNewPokemon,
		int socket_cliente) {

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__NEW_POKEMON;
	paquete_a_enviar->tamanio_username = strlen("BROKER") + 1;
	paquete_a_enviar->username = malloc(paquete_a_enviar->tamanio_username);
	paquete_a_enviar->username = "BROKER";

	//serializacion de brokerNewPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer = serializarMensajeColaNEW(brokerNewPokemon);

	paquete_a_enviar->buffer = buffer;

	int tamanio_buffer = 0;

	void* bufferStream = serializar_paquete(paquete_a_enviar, &tamanio_buffer);

	send(socket_cliente, bufferStream, tamanio_buffer, 0);

	free(bufferStream);
	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
}

void enviar_cola_Get_Pokemon(broker_get_pokemon *brokerGetPokemon,
		int socket_cliente) {
	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__GET_POKEMON;
	paquete_a_enviar->tamanio_username = strlen("BROKER") + 1;
	paquete_a_enviar->username = malloc(paquete_a_enviar->tamanio_username);
	paquete_a_enviar->username = "BROKER";
	//serializacion de brokerGetPokemon

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer = serializarMensajeColaGET(brokerGetPokemon);

	paquete_a_enviar->buffer = buffer;

	int tamanio_buffer = 0;

	void* bufferStream = serializar_paquete(paquete_a_enviar, &tamanio_buffer);
	send(socket_cliente, bufferStream, tamanio_buffer, 0);

	free(bufferStream);

	//estos no hacen falta porque no pedimos memoria de stream, el buffer y paquete_a_enviar->buffer son lo mismo
	//free(buffer->stream);
	//free(buffer);
	//free(paquete_a_enviar->buffer->stream);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
}

void enviar_cola_Caught_Pokemon(broker_caught_pokemon *brokerCaughtPokemon,
		int socket_cliente) {

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__CAUGHT_POKEMON;
	paquete_a_enviar->tamanio_username = strlen("BROKER") + 1;
	paquete_a_enviar->username = malloc(paquete_a_enviar->tamanio_username);
	paquete_a_enviar->username = "BROKER";
	//serializacion de brokerCaughtPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer = serializarMensajeColaCAUGHT(brokerCaughtPokemon);

	paquete_a_enviar->buffer = buffer;

	int tamanio_buffer = 0;

	void* bufferStream = serializar_paquete(paquete_a_enviar, &tamanio_buffer);

	send(socket_cliente, bufferStream, tamanio_buffer, 0);

	free(bufferStream);

	//estos no hacen falta porque no pedimos memoria de stream, el buffer y paquete_a_enviar->buffer son lo mismo
	//free(buffer->stream);
	//free(buffer);
	//free(paquete_a_enviar->buffer->stream);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
}

void enviar_cola_Catch_Pokemon(broker_catch_pokemon *brokerCatchPokemon,
		int socket_cliente) {

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__CATCH_POKEMON;
	paquete_a_enviar->tamanio_username = strlen("BROKER") + 1;
	paquete_a_enviar->username = malloc(paquete_a_enviar->tamanio_username);
	paquete_a_enviar->username = "BROKER";

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer = serializarMensajeColaCATCH(brokerCatchPokemon);

	paquete_a_enviar->buffer = buffer;

	int tamanio_buffer = 0;

	void* bufferStream = serializar_paquete(paquete_a_enviar, &tamanio_buffer);
	send(socket_cliente, bufferStream, tamanio_buffer, 0);

	free(bufferStream);

	//estos no hacen falta porque no pedimos memoria de stream, el buffer y paquete_a_enviar->buffer son lo mismo
	//free(buffer->stream);
	//free(buffer);
	//free(paquete_a_enviar->buffer->stream);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
}

void enviar_cola_Localized_Pokemon(
		broker_localized_pokemon *brokerLocalizedPokemon, int socket_cliente) {

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__LOCALIZED_POKEMON;
	paquete_a_enviar->tamanio_username = strlen("BROKER") + 1;
	paquete_a_enviar->username = malloc(paquete_a_enviar->tamanio_username);
	paquete_a_enviar->username = "BROKER";

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer = serializarMensajeColaLOCALIZED(brokerLocalizedPokemon);

	paquete_a_enviar->buffer = buffer;

	int tamanio_buffer = 0;

	void* bufferStream = serializar_paquete(paquete_a_enviar, &tamanio_buffer);
	send(socket_cliente, bufferStream, tamanio_buffer, 0);

	free(bufferStream);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
}

void enviar_cola_Appeared_Pokemon(broker_appeared_pokemon *brokerAppearedPokemon, int socket_cliente) {

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__APPEARED_POKEMON;
	paquete_a_enviar->tamanio_username = strlen("BROKER") + 1;
	paquete_a_enviar->username = malloc(paquete_a_enviar->tamanio_username);
	paquete_a_enviar->username = "BROKER";

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer = serializarMensajeColaAPPEARED(brokerAppearedPokemon);

	paquete_a_enviar->buffer = buffer;

	int tamanio_buffer = 0;

	void* bufferStream = serializar_paquete(paquete_a_enviar, &tamanio_buffer);
	send(socket_cliente, bufferStream, tamanio_buffer, 0);

	free(bufferStream);

	//estos no hacen falta porque no pedimos memoria de stream, el buffer y paquete_a_enviar->buffer son lo mismo
	//free(buffer->stream);
	//free(buffer);
	//free(paquete_a_enviar->buffer->stream);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
}

