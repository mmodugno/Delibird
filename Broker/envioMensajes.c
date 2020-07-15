/*
 * envioMensajes.c
 *
 *  Created on: 13 jul. 2020
 *      Author: utnso
 */

#include "envioMensajes.h"

void envioColaNewPokemon() {
	t_list *usersSinACK ;
	char* username;

	//TODO ver si este while esta demas
	//while(1){
	sem_wait(&colaNew);

	bool esSuscriptor(char* userActual){
		return (!strcmp(userActual,username));
	}


	void llenarUserSinACK(char* userActual){
		username = string_duplicate(userActual);
		if(!list_any_satisfy(usersSinACK,esSuscriptor)){
			list_add(usersSinACK,userActual);
		}
	}

	bool menNewQueFalten(particion *part) {
		if (part->tipoMensaje == NEW_POKEMON) {
			//me fijo que se hayan mandado a todos los suscriptores
			usersSinACK = list_duplicate(part->acknoleged);
			list_iterate(suscriptoresNewPokemon,llenarUserSinACK);
			if(usersSinACK->elements_count!=0){
				return 1;
			}
			list_clean_and_destroy_elements(usersSinACK,free);
			return 0;
		}
		list_clean_and_destroy_elements(usersSinACK,free);
		return 0;
	}

	sem_wait(&usoMemoria);

	//BUSCO UN MENSAJE QUE NO HAYA ENVIADO
	particion *mensajeNewEnMemo = list_find(tablaDeParticiones,menNewQueFalten);


	enviarPorTipo(mensajeNewEnMemo,usersSinACK);


	//nose si list_destroy_and_destroy_elements() eliminaria los username que siguen estando en la de ack de esa part

	//clean si lo voy limpiandocada vez que lo uso
	list_clean(usersSinACK);
	//destroy si lo uso fuera del while
	//list_destroy(usersSinACK);

	free(username);
	sem_post(&usoMemoria);


}
//por el while
//}

//conectarme con cada uno que necesite mandarle
bool esTeam(char* username){
	return string_contains(username,"TEAM");
}
bool esGameCard(char* username){
	return string_contains(username,"GAMECARD");
}
bool esGameBoy(char* username){
	return string_contains(username,"GAMEBOY");
}

void enviarPorTipo(particion* partAEnviar,t_list* usersAEnviar){
	broker_new_pokemon* newEnMemo;
	broker_get_pokemon* getEnMemo;
	broker_appeared_pokemon* appEnMemo;
	broker_localized_pokemon* localizedEnMemo;
	broker_catch_pokemon* catchEnMemo;
	broker_caught_pokemon* caughtEnMemo;

	if (partAEnviar != NULL) {
		switch (partAEnviar->tipoMensaje) {
			case NEW_POKEMON:
				newEnMemo = leerdeMemoriaNEW(partAEnviar);

				//TODO volver a serializar pero con los ids que necesite


				//TODO mandar ese mensaje a los userAEnviar

				free(newEnMemo->datos->nombrePokemon);
				free(newEnMemo);
				break;
			case GET_POKEMON:
				getEnMemo= leerdeMemoriaGET(partAEnviar);

				//TODO volver a serializar pero con los ids que necesite

				//TODO mandar ese mensaje a los userAEnviar

				free(getEnMemo->datos->nombrePokemon);
				free(getEnMemo);
				break;
			case APPEARED_POKEMON:
				appEnMemo = leerdeMemoriaAPPEARED(partAEnviar);

				//TODO volver a serializar pero con los ids que necesite

				//TODO mandar ese mensaje a los userAEnviar

				free(appEnMemo->datos->nombrePokemon);
				free(appEnMemo);
				break;
			case LOCALIZED_POKEMON:
				localizedEnMemo = leerdeMemoriaLOCALIZED(partAEnviar);

				//TODO volver a serializar pero con los ids que necesite

				//TODO mandar ese mensaje a los userAEnviar


				free(localizedEnMemo->datos->posX);
				free(localizedEnMemo->datos->posY);
				free(localizedEnMemo->datos->nombrePokemon);
				free(localizedEnMemo);
				break;
			case CATCH_POKEMON:
				catchEnMemo = leerdeMemoriaCATCH(partAEnviar);

				//TODO volver a serializar pero con los ids que necesite

				//TODO mandar ese mensaje a los userAEnviar

				free(catchEnMemo->datos->nombrePokemon);
				free(catchEnMemo);
				break;
			case CAUGHT_POKEMON:
				caughtEnMemo = leerdeMemoriaCAUGHT(partAEnviar);

				//TODO volver a serializar pero con los ids que necesite

				//TODO mandar ese mensaje a los userAEnviar


				free(caughtEnMemo);
				break;
		}
	}
}


//TODO revisar porque seguro debemos cambiar que es lo que recibe
void enviarASuscriptores(t_paquete* paqueteAEnviar,t_list* usersAEnviar){
	if (list_any_satisfy(usersAEnviar, esTeam)){
		//conexion con TEAM
		conexionTeam = crear_conexion(ip_team, puerto_team);

		enviarMensaje(paqueteAEnviar,conexionTeam,"TEAM");


		liberar_conexion(conexionTeam);
	}
	if (list_any_satisfy(usersAEnviar, esGameBoy)){
		//conexion con GAMEBOY
		conexionGameboy = crear_conexion(ip_gameboy, puerto_gameboy);

		enviarMensaje(paqueteAEnviar,conexionGameboy,"GAMEBOY");

		liberar_conexion(conexionGameboy);
	}
	if (list_any_satisfy(usersAEnviar, esGameCard)){

		//conexion con GAMECARD
		conexionGamecard = crear_conexion(ip_gamecard, puerto_gamecard);

		enviarMensaje(paqueteAEnviar,conexionGamecard,"GAMECARD");

		liberar_conexion(conexionGamecard);

	}

}


void enviarMensaje(t_paquete* paqueteAEnviar,int socket_cliente,char* suscriptorAEnviar){
	if(socket_cliente!=-1){

		//TODO ver como mostrar lo que esta en el paquete, quizas necesitemos el dato para mostrarlo pero es mas complicado

		log_info(logEnviarNuevo,"Envie a %s el mensaje ",suscriptorAEnviar);

	}
	else{
		log_info(logEnviarNuevo,"No pude enviar el mensaje a %s porque no esta en linea",suscriptorAEnviar);
	}
}


//TODO si funciona NEW, replicar
void envioColaGetPokemon() {
	sem_wait(&colaGet);
	sem_wait(&usoMemoria);



	sem_post(&usoMemoria);
}

//TODO si funciona NEW, replicar
void envioColaLocalizedPokemon() {
	sem_wait(&colaLocalized);
	sem_wait(&usoMemoria);




	sem_post(&usoMemoria);
}

//TODO si funciona NEW, replicar
void envioColaAppearedPokemon() {
	sem_wait(&colaAppeared);
	sem_wait(&usoMemoria);




	sem_post(&usoMemoria);
}

//TODO si funciona NEW, replicar
void envioColaCatchPokemon() {
	sem_wait(&colaCatch);
	sem_wait(&usoMemoria);



	sem_post(&usoMemoria);
}

//TODO si funciona NEW, replicar
void envioColaCaughtPokemon() {
	sem_wait(&colaCaught);
	sem_wait(&usoMemoria);



	sem_post(&usoMemoria);
}


int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) != 0){
		//printf("error");
		freeaddrinfo(server_info);
		return -1;
	}


	freeaddrinfo(server_info);

	return socket_cliente;
}

void liberar_conexion(int socket_cliente){
	close(socket_cliente);
}


t_buffer* serializarMensajeColaNEW(broker_new_pokemon* newAEnviar){

	//serializacion
	//1. uint32_t idMensaje
	//2. uint32_t tamanioNombre;
	//3. char* nombrePokemon;
	//4. uint32_t posX;
	//5. uint32_t posY;
	//6. uint32_t CantidadPokemons;


	t_buffer* mensaje = malloc(sizeof(t_buffer));
	mensaje->size = sizeof(uint32_t)*5 + newAEnviar->datos->tamanioNombre;
	mensaje->stream = malloc(mensaje->size);

	int offset = 0;

	memcpy(mensaje->stream+offset,&(newAEnviar->id),sizeof (uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(mensaje->stream+offset,&(newAEnviar->datos->tamanioNombre),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(mensaje->stream+offset,(newAEnviar->datos->nombrePokemon),newAEnviar->datos->tamanioNombre);
	offset+=newAEnviar->datos->tamanioNombre;

	memcpy(mensaje->stream+offset,&(newAEnviar->datos->posX),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(mensaje->stream+offset,&(newAEnviar->datos->posY),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(mensaje->stream+offset,&(newAEnviar->datos->cantidadPokemon),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	return mensaje;
}

t_buffer* serializarMensajeColaAPPEARED(broker_appeared_pokemon* brokerAppearedPokemon)
{

	// serializacion
	//1. uint32_t idMensaje;
	//2. uint32_t tamanioNombre;
	//3. char* nombrePokemon;
	//4. uint32_t posX;
	//5. uint32_t posY;
	//6. uint32_t id_relativo;

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size= sizeof(uint32_t)+sizeof(uint32_t) // idMensaje, id
			+ sizeof(uint32_t)*3 // posX, posY, tamanioNombre
			+ (brokerAppearedPokemon->datos->tamanioNombre); //longitud del strind nombre de pokemon

	buffer->stream = malloc(buffer->size);
	int offset = 0;


	memcpy(buffer->stream+offset,&(brokerAppearedPokemon->id),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(brokerAppearedPokemon->datos->tamanioNombre),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,(brokerAppearedPokemon->datos->nombrePokemon), brokerAppearedPokemon->datos->tamanioNombre);
	offset+=brokerAppearedPokemon->datos->tamanioNombre;

	memcpy(buffer->stream+offset,&(brokerAppearedPokemon->datos->posX),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(brokerAppearedPokemon->datos->posY),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer->stream+offset,&(brokerAppearedPokemon->id_relativo),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	return buffer;

}

t_buffer* serializarMensajeColaLOCALIZED(broker_localized_pokemon* brokerLocalizedPokemon)
{

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

	buffer->size = sizeof(uint32_t)*4 + ((sizeof(uint32_t)*2)*brokerLocalizedPokemon->datos->cantidadPosiciones)
			+ brokerLocalizedPokemon->datos->tamanioNombre; //longitud del string nombre de pokemon

	buffer->stream = malloc(buffer->size);
	int offset = 0;

	//id del mensaje
	memcpy(buffer->stream + offset, &(brokerLocalizedPokemon->id),sizeof(uint32_t));
	offset += sizeof(uint32_t);

	//longitud del string nombre de pokemon
	memcpy(buffer->stream + offset, &(brokerLocalizedPokemon->datos->tamanioNombre),sizeof(uint32_t));
	offset += sizeof(uint32_t);

	//string nombre de pokemon
	memcpy(buffer->stream + offset, (brokerLocalizedPokemon->datos->nombrePokemon),brokerLocalizedPokemon->datos->tamanioNombre);
	offset += brokerLocalizedPokemon->datos->tamanioNombre;

	//cantidad de Posciones
	memcpy(buffer->stream + offset,&(brokerLocalizedPokemon->datos->cantidadPosiciones), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for (posiciones = 0;posiciones < brokerLocalizedPokemon->datos->cantidadPosiciones;posiciones++) {

		posX = brokerLocalizedPokemon->datos->posX[posiciones];
		posY = brokerLocalizedPokemon->datos->posY[posiciones];

		//tengo mis dudas por el &
		memcpy(buffer->stream + offset, &posX, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, &posY, sizeof(uint32_t));
		offset += sizeof(uint32_t);
	}

	//id correlativo
	memcpy(buffer->stream + offset, &(brokerLocalizedPokemon->id_relativo),sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return buffer;
}


t_buffer* serializarMensajeColaGET(broker_get_pokemon* getAEnviar){

	//serializacion
	//1. uint32_t idMensaje;
	//2. uint32_t tamanioNombre;
	//3. char* nombrePokemon;

	t_buffer* mensaje = malloc(sizeof(t_buffer));

	mensaje->size = sizeof(uint32_t)*2 + getAEnviar->datos->tamanioNombre;
	mensaje->stream = malloc(mensaje->size);

	int offset = 0;

	memcpy(mensaje->stream+offset,&(getAEnviar->id),sizeof (uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(mensaje->stream+offset,&(getAEnviar->datos->tamanioNombre),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(mensaje->stream+offset,(getAEnviar->datos->nombrePokemon),getAEnviar->datos->tamanioNombre);
	offset+=getAEnviar->datos->tamanioNombre;

	return mensaje;
}

t_buffer* serializarMensajeColaCATCH(broker_catch_pokemon* catchAEnviar){

	// serializacion
	//1. uint32_t id;
	//2. uint32_t tamanioNombre;
	//3. char* nombrePokemon;
	//4. uint32_t posX;
	//5. uint32_t posY;

	t_buffer* mensaje = malloc(sizeof(t_buffer));

	mensaje->size= sizeof(uint32_t)*4 + catchAEnviar->datos->tamanioNombre;

	mensaje->stream = malloc(mensaje->size);
	int offset = 0;

	memcpy(mensaje->stream+offset,&(catchAEnviar->id),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(mensaje->stream+offset,&(catchAEnviar->datos->tamanioNombre),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(mensaje->stream+offset,(catchAEnviar->datos->nombrePokemon),catchAEnviar->datos->tamanioNombre);
	offset+=(catchAEnviar->datos->tamanioNombre);

	memcpy(mensaje->stream+offset,&(catchAEnviar->datos->posX),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(mensaje->stream+offset,&(catchAEnviar->datos->posY),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	return mensaje;
}

t_buffer* serializarMensajeColaCAUGHT(broker_caught_pokemon* caughtAEnviar){

	// serializacion
	//1. uint32_t id;
	//2. uint32_t puedoAtraparlo;
	//3. uint32_t id_rel;

	t_buffer* mensaje = malloc(sizeof(t_buffer));

	mensaje->size = sizeof(uint32_t)*3;
	mensaje->stream = malloc(mensaje->size);

	int offset = 0;

	memcpy(mensaje->stream+offset,&(caughtAEnviar->id),sizeof (uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(mensaje->stream+offset,&(caughtAEnviar->datos->puedoAtraparlo),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(mensaje->stream+offset,&(caughtAEnviar->id_relativo),sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	return mensaje;
}
