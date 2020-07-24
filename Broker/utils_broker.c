/*
 * utils_broker.c
 *l
 *  Created on: 14 may. 2020
 *      Author: utnso
 */

#include "utils_broker.h"

pthread_mutex_t llegadaMensajesTHREAD = PTHREAD_MUTEX_INITIALIZER;

void mostrarParticiones(particion* unaParticion) {
	if (unaParticion != NULL) {
		//este no funciona
		//log_info(almacenadoMemoria,"El dato(ID:%d) %s con base: %d, tamanio: %d y tiempo: %s",unaParticion->idMensaje,colasDeEnum[unaParticion->tipoMensaje],unaParticion->base,unaParticion->tamanioMensaje,unaParticion->timestamp);
		//este me parece que si y quizas es mas descriptivo
		log_info(almacenadoMemoria,
				"base: %d, tamanio: %d, id: %d, tipo %s, libre?: %d con tiempo: %s ",
				unaParticion->base, unaParticion->tamanioMensaje,
				unaParticion->idMensaje, colasDeEnum[unaParticion->tipoMensaje],
				unaParticion->libre, unaParticion->timestamp);
		//printf("base: %d, tamanio: %d, id: %d", unaParticion->base, unaParticion->tamanioMensaje, unaParticion->idMensaje);
	}
}

void iniciarMemoria() {

	memoria = malloc(tamanio_memoria);
	frecuencia = 0;

	if (!strcmp(algoritmo_memoria, "PARTICIONES")) {
		tablaDeParticiones = list_create();

		particion *partLibreInicial = malloc(sizeof(particion));
		//los que tienen idMensaje 0 son particiciones vacias
		partLibreInicial->idMensaje = 0;
		partLibreInicial->tipoMensaje = LIBRE;
		partLibreInicial->base = 0;
		partLibreInicial->tamanioMensaje = tamanio_memoria;
		partLibreInicial->libre = 1;
		partLibreInicial->timestamp = string_new();
		partLibreInicial->acknoleged = list_create();

		list_add(tablaDeParticiones, partLibreInicial);
	}
	if (!strcmp(algoritmo_memoria, "BS")) {
		tablaDeParticiones = list_create();

		buddy* buddyLibreInicial = malloc(sizeof(buddy));
		buddyLibreInicial->particion = malloc(sizeof(particion));

				//los que tienen idMensaje 0 son particiciones vacias
		buddyLibreInicial->particion->idMensaje = 0;
		buddyLibreInicial->particion->tipoMensaje = LIBRE;
		buddyLibreInicial->particion->base = 0;
		buddyLibreInicial->particion->tamanioMensaje = tamanio_memoria;
		buddyLibreInicial->particion->libre = 1;
		buddyLibreInicial->particion->timestamp = string_new();
		buddyLibreInicial->particion->acknoleged = list_create();

		buddyLibreInicial->limite = tamanio_memoria-1;

		//al principio tendría una particion nomás de tamanioMemoria.
		list_add(tablaDeParticiones, buddyLibreInicial);

	}

	idGlobales = 1;

}

uint32_t deserializarAck(int socket_cliente) {
	//uint32_t idAck;

	uint32_t idAck;
	recv(socket_cliente, &idAck, sizeof(uint32_t), 0);
	return idAck;
}

void iniciar_servidor(void)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;



    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip_broker, puerto_broker, &hints, &servinfo);

    int activado = 1;
	setsockopt(socket_servidor,SOL_SOCKET,SO_REUSEADDR,&activado,sizeof(activado));

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    while(1){
    	esperar_cliente(socket_servidor);

    }

}

void esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	pthread_t thread;

	socklen_t  tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
	pthread_detach(thread);

}

void serve_client(int* socket)
{
	//sem_wait(&llegadaMensajes);
//	pthread_mutex_lock(&llegadaMensajesTHREAD);
	int cod_op;
	int i = recv(*socket, &cod_op, sizeof(op_code), MSG_WAITALL);
	if(i <= 0)
		cod_op = -1;
	process_request(cod_op, *socket);

	//liberar_conexion(*socket);
//	pthread_mutex_unlock(&llegadaMensajesTHREAD);
	//sem_post(&llegadaMensajes);
}

/*
 * mensajes que recibimos
 BROKER__NEW_POKEMON = 2,
 BROKER__APPEARED_POKEMON = 3,
 BROKER__CATCH_POKEMON = 4,
 BROKER__CAUGHT_POKEMON = 5,
 BROKER__GET_POKEMON = 6,
 BROKER__LOCALIZED_POKEMON = 13,
 SUSCRIPCION = 11
 */
//TODO ver quienes necesitan devolver el id
void process_request(int cod_op, int cliente_fd) {
	uint32_t tamanio_buffer;
	uint32_t tamanio_username;
	char* username;
	uint32_t tamanioAgregar;
	recv(cliente_fd, &tamanio_username, sizeof(uint32_t), MSG_WAITALL);
	username = malloc(tamanio_username);
	recv(cliente_fd, username, tamanio_username, MSG_WAITALL);
	recv(cliente_fd, &tamanio_buffer, sizeof(uint32_t), MSG_WAITALL);
	uint32_t posiciones;
	char* posicionesString = string_new();
	void *raiz;

	//log_info(logConexion,"recibi una nueva conexion de %s",username);


	uint32_t ackRecibido;
	particion* partEncontrada;
	buddy* buddyEncontrado;

	suscriptor* suscriptor;
	broker_new_pokemon* newRecibido;
	broker_appeared_pokemon* appearedRecibido;
	broker_get_pokemon* getRecibido;
	broker_catch_pokemon* catchRecibido;
	broker_caught_pokemon* caughtRecibido;
	broker_localized_pokemon* localizedRecibido;

	//log_info(logConexion,"%s se conecto al broker",username);
	//falta los case de los otros tipos de mensajes (get,catch,caught)(localized lo dejamos para despues(es de GameCard)

	//if(esGameBoy(username)||esGameCard(username)||esTeam(username)){
		switch (cod_op) {
			case SUSCRIPCION:
				pthread_mutex_lock(&llegadaMensajesTHREAD);

				//sleep(3);

				suscriptor = deserializar_suscripcion(cliente_fd);

				log_info(logSuscipcion,
						"recibi mensaje de suscripcion de %s a la cola %s",
						suscriptor->nombreDeSuscriptor,
						colasDeEnum[(suscriptor->tipoDeCola)]);

				suscribirACola(suscriptor);



				free(suscriptor);
				pthread_mutex_unlock(&llegadaMensajesTHREAD);
				break;

			case BROKER__NEW_POKEMON:
				pthread_mutex_lock(&llegadaMensajesTHREAD);

				newRecibido = deserializar_new_pokemon(cliente_fd);

				//mutex
				sem_wait(&idsDeMensajes);
				newRecibido->id = idGlobales;
				idGlobales++;
				sem_post(&idsDeMensajes);
				//mutex

				log_info(logMensajeNuevo,
									"recibi mensaje de NEW_POKEMON(ID:%d) de %s \n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d \n cantidad de pokemones: %d",
									newRecibido->id,username, newRecibido->datos->tamanioNombre,
									newRecibido->datos->nombrePokemon, newRecibido->datos->posX,
									newRecibido->datos->posY, newRecibido->datos->cantidadPokemon);

				raiz = transformarBrokerNewPokemon(newRecibido, &tamanioAgregar);


				//log_info(logMensajeNuevo,"lo que vale este new a agregar es %d",sizeof(raiz));
				//log_info(logMensajeNuevo,"lo que vale este new puntero de dato es %d",sizeof(new_pokemon*));
				agregarAMemoria(raiz, newRecibido->id, NEW_POKEMON, 0, tamanioAgregar);
				//agregarACola(NEW_POKEMON,newRecibido);

				//free(raiz);
				free(newRecibido);
				pthread_mutex_unlock(&llegadaMensajesTHREAD);
				break;

			case BROKER__APPEARED_POKEMON:

				pthread_mutex_lock(&llegadaMensajesTHREAD);
				appearedRecibido = deserializar_appeared_pokemon(cliente_fd);


				//mutex
				sem_wait(&idsDeMensajes);
				appearedRecibido->id = idGlobales;
				idGlobales++;
				sem_post(&idsDeMensajes);
				//mutex

				log_info(logMensajeNuevo,
								"recibi mensaje de APPEARED_POKEMON(ID:%d) de %s \n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d \n con id_relativo: %d",
								appearedRecibido->id,username, appearedRecibido->datos->tamanioNombre,
								appearedRecibido->datos->nombrePokemon,
								appearedRecibido->datos->posX, appearedRecibido->datos->posY,
								appearedRecibido->id_relativo);

				raiz = transformarBrokerAppearedPokemon(appearedRecibido,
						&tamanioAgregar);
				//log_info(logMensajeNuevo,"lo que vale este appeared a agregar es %d",sizeof(raiz));
				agregarAMemoria(raiz, appearedRecibido->id, APPEARED_POKEMON,
						appearedRecibido->id_relativo, tamanioAgregar);
				//agregarACola(APPEARED_POKEMON,appearedRecibido);

				//free(raiz);

				free(appearedRecibido);
				pthread_mutex_unlock(&llegadaMensajesTHREAD);
				break;

			case BROKER__GET_POKEMON:

				pthread_mutex_lock(&llegadaMensajesTHREAD);
				getRecibido = deserializar_get_pokemon(cliente_fd);


				//mutex
				sem_wait(&idsDeMensajes);
				getRecibido->id = idGlobales;
				idGlobales++;
				sem_post(&idsDeMensajes);
				//mutex

				log_info(logMensajeNuevo,"recibi mensaje de GET_POKEMON(ID:%d) de %s\n con tamanio: %d \n nombre: %s ",
									getRecibido->id,username, getRecibido->datos->tamanioNombre,
									getRecibido->datos->nombrePokemon);

				if (!strcmp(username, "TEAM")) {
					//TODO ver conexiones con los otros sockets (si les tengo que mandar ID
					send(cliente_fd, &(getRecibido->id), sizeof(uint32_t), 0);
				}

				raiz = transformarBrokerGetPokemon(getRecibido, &tamanioAgregar);
				//log_info(logMensajeNuevo,"lo que vale este get a agregar es %d",sizeof(raiz));
				agregarAMemoria(raiz, getRecibido->id, GET_POKEMON, 0, tamanioAgregar);
				//agregarACola(GET_POKEMON,getRecibido);


				//free(raiz);
				free(getRecibido);
				pthread_mutex_unlock(&llegadaMensajesTHREAD);
				break;

			case BROKER__CATCH_POKEMON:

				pthread_mutex_lock(&llegadaMensajesTHREAD);

				catchRecibido = deserializar_catch_pokemon(cliente_fd);



				//mutex
				sem_wait(&idsDeMensajes);
				catchRecibido->id = idGlobales;
				idGlobales++;
				sem_post(&idsDeMensajes);
				//mutex

				log_info(logMensajeNuevo, "recibi mensaje de CATCH_POKEMON(ID:%d) de %s\n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d ",
									catchRecibido->id,username, catchRecibido->datos->tamanioNombre,
									catchRecibido->datos->nombrePokemon, catchRecibido->datos->posX,
									catchRecibido->datos->posY);

				if (!strcmp(username, "TEAM")) {
					//TODO ver conexiones con los otros sockets (si les tengo que mandar ID
					send(cliente_fd, &(catchRecibido->id), sizeof(uint32_t), 0);
				}

				raiz = transformarBrokerCatchPokemon(catchRecibido,&tamanioAgregar);
				//log_info(logMensajeNuevo,"lo que vale este catch a agregar es %d",sizeof(raiz));
				agregarAMemoria(raiz, catchRecibido->id, CATCH_POKEMON, 0,
						tamanioAgregar);
				//agregarACola(CATCH_POKEMON,catchRecibido);


				//free(raiz);
				free(catchRecibido);
				pthread_mutex_unlock(&llegadaMensajesTHREAD);
				break;

			case BROKER__CAUGHT_POKEMON:

				pthread_mutex_lock(&llegadaMensajesTHREAD);

				caughtRecibido = deserializar_caught_pokemon(cliente_fd);


				//mutex
				sem_wait(&idsDeMensajes);
				caughtRecibido->id = idGlobales;
				idGlobales++;
				sem_post(&idsDeMensajes);
				//mutex

				log_info(logMensajeNuevo, "recibi mensaje de CAUGHT_POKEMON(ID:%d) de %s\n con ID_relativo: %d \n puedoAtraparlo: %d ",
										caughtRecibido->id,username, caughtRecibido->id_relativo,
										caughtRecibido->datos->puedoAtraparlo);

				raiz = transformarBrokerCaughtPokemon(caughtRecibido,
						&tamanioAgregar);
				//log_info(logMensajeNuevo,"lo que vale este caught a agregar es %d",sizeof(raiz));
				agregarAMemoria(raiz, caughtRecibido->id, CAUGHT_POKEMON,
						caughtRecibido->id_relativo, tamanioAgregar);
				//agregarACola(CAUGHT_POKEMON,caughtRecibido);

				//free(raiz);
				free(caughtRecibido);
				pthread_mutex_unlock(&llegadaMensajesTHREAD);
				break;

			case BROKER__LOCALIZED_POKEMON:
				pthread_mutex_lock(&llegadaMensajesTHREAD);

				localizedRecibido = deserializar_localized_pokemon(cliente_fd);


				 posiciones = 0;
				 for(posiciones=0;posiciones<localizedRecibido->datos->cantidadPosiciones;posiciones++){
					 char* posicion = string_from_format("(%d;%d)",localizedRecibido->datos->posX[posiciones],localizedRecibido->datos->posY[posiciones]);
					 string_append(&posicionesString,posicion);
				 }



				 /*log_info(logMensajeNuevo,"recibi mensaje de LOCALIZED_POKEMON de %s\n con tamanio: %d\n nombre: %s\n cantidadPosiciones: %d\n con ID_relativo: %d \n "
						 ,username,localizedRecibido->datos->tamanioNombre, localizedRecibido->datos->nombrePokemon,localizedRecibido->datos->cantidadPosiciones,
						 localizedRecibido->id_relativo);*/

				//mutex
				sem_wait(&idsDeMensajes);
				localizedRecibido->id = idGlobales;
				idGlobales++;
				sem_post(&idsDeMensajes);
				//mutex

				log_info(logMensajeNuevo,"recibi mensaje de LOCALIZED_POKEMON(ID:%d) de %s\n con tamanio: %d\n nombre: %s\n cantidadPosiciones: %d\n y Posiciones(x,y): %s\n con ID_relativo: %d \n ",
										localizedRecibido->id,username,localizedRecibido->datos->tamanioNombre, localizedRecibido->datos->nombrePokemon,localizedRecibido->datos->cantidadPosiciones,posicionesString,
										localizedRecibido->id_relativo);

				raiz = transformarBrokerLocalizedPokemon(localizedRecibido,
						&tamanioAgregar);
				//log_info(logMensajeNuevo,"lo que vale este caught a agregar es %d",sizeof(raiz));
				agregarAMemoria(raiz, localizedRecibido->id, LOCALIZED_POKEMON,
						localizedRecibido->id_relativo, tamanioAgregar);
				//agregarACola(CAUGHT_POKEMON,caughtRecibido);

				//free(raiz);
				free(localizedRecibido);
				pthread_mutex_unlock(&llegadaMensajesTHREAD);
				break;

			case ACKNOWLEDGED:

				pthread_mutex_lock(&llegadaMensajesTHREAD);

				ackRecibido = deserializarAck(cliente_fd);

				//TODO comentar esto
				/*
				log_info(confirmacionRecepcion,
						"me llego la confirmacion para el ID:%d pero todavia no busque", ackRecibido);*/

				//mutex porque manejo algo de memoria
				sem_wait(&usoMemoria);

				bool partAck(particion *part) {
					if (/*(!part->libre) &&*/ (ackRecibido == part->idMensaje)) {
						return 1;
					}
					return 0;
				}

				bool buddyAck(buddy* unBuddy){
					return partAck(unBuddy->particion);
				}
				/*
				//TODO comentar, muestra lo que esta en memoria
				list_iterate(tablaDeParticiones,mostrarParticiones);*/

				if(!strcmp(algoritmo_memoria,"PARTICIONES")) partEncontrada = list_find(tablaDeParticiones, (void*) partAck);

				if(!strcmp(algoritmo_memoria,"BS")) {
					buddyEncontrado = list_find(tablaDeParticiones,(void*) buddyAck);
					partEncontrada = buddyEncontrado->particion;
				}

				//si la encontro lo agrega a su tabla de ACK
				if(partEncontrada!=NULL){
					list_add(partEncontrada->acknoleged,username);
					log_info(confirmacionRecepcion,"ACK de %s ID:%d para la cola %s",username,ackRecibido,colasDeEnum[partEncontrada->tipoMensaje]);
				}

				sem_post(&usoMemoria);

				pthread_mutex_unlock(&llegadaMensajesTHREAD);

				break;
			case DESUSCRIBIR:
				pthread_mutex_lock(&llegadaMensajesTHREAD);

				suscriptor = deserializar_suscripcion(cliente_fd);


				log_info(logSuscipcion,
						"recibi mensaje de desuscripcion de %s a la cola %s",
						suscriptor->nombreDeSuscriptor,
						colasDeEnum[(suscriptor->tipoDeCola)]);


				desuscribirACola(suscriptor);


				free(suscriptor);
				pthread_mutex_unlock(&llegadaMensajesTHREAD);


				break;

			case 0:
				pthread_exit(NULL);
				break;
			case -1:

				log_info(almacenadoMemoria,"recibi un menos uno");
				pthread_exit(NULL);
				break;


			free(username);
			//free(posXString);
			//free(posYString);
			//free(posicionesString);


		}


	//pthread_exit(EXIT_SUCCESS);
}

void* recibir_mensaje(int socket_cliente, int* size) {
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

bool esSuscriptor(char* suscAct){
	return !strcmp(suscAct,susAEliminar->nombreDeSuscriptor);
}

void eliminarDeACK(particion* part){
	if(part->tipoMensaje == colaAEliminar){
		list_remove_and_destroy_by_condition(part->acknoleged,(void*) esSuscriptor,free);
	}
}

void suscribirACola(suscriptor* suscrip){
	switch(suscrip->tipoDeCola){
		case NEW_POKEMON:
			sem_wait(&suscripcionAColaNEW);
			list_add(suscriptoresNewPokemon,(char*) suscrip->nombreDeSuscriptor);
			sem_post(&suscripcionAColaNEW);
			break;
		case APPEARED_POKEMON:
			sem_wait(&suscripcionAColaAPPEARED);
			list_add(suscriptoresAppearedPokemon,(char*) suscrip->nombreDeSuscriptor);
			sem_post(&suscripcionAColaAPPEARED);
			break;
		case CATCH_POKEMON:
			sem_wait(&suscripcionAColaCATCH);
			list_add(suscriptoresCatchPokemon,(char*) suscrip->nombreDeSuscriptor);
			sem_post(&suscripcionAColaCATCH);
			break;
		case CAUGHT_POKEMON:
			sem_wait(&suscripcionAColaCAUGHT);
			list_add(suscriptoresCaughtPokemon,(char*) suscrip->nombreDeSuscriptor);
			sem_post(&suscripcionAColaCAUGHT);
			break;
		case GET_POKEMON:
			sem_wait(&suscripcionAColaGET);
			list_add(suscriptoresGetPokemon,(char*) suscrip->nombreDeSuscriptor);
			sem_post(&suscripcionAColaGET);
			break;
		case LOCALIZED_POKEMON:
			sem_wait(&suscripcionAColaLOCALIZED);
			list_add(suscriptoresLocalizedPokemon,(char*) suscrip->nombreDeSuscriptor);
			sem_post(&suscripcionAColaLOCALIZED);
			break;

	}
}



void desuscribirACola(suscriptor* suscrip){



	bool suscriptorAEliminar(char* nombreSus){
		return (!strcmp(nombreSus, suscrip->nombreDeSuscriptor));
	}

	switch(suscrip->tipoDeCola){
		case NEW_POKEMON:{
			sem_wait(&suscripcionAColaNEW);
			list_remove_and_destroy_by_condition(suscriptoresNewPokemon,(void*) suscriptorAEliminar,free);
			sem_wait(&usoMemoria);
			colaAEliminar = NEW_POKEMON;
			susAEliminar = suscrip;
			list_iterate(tablaDeParticiones,(void*) eliminarDeACK);
			sem_post(&usoMemoria);
			sem_post(&suscripcionAColaNEW);
			break;
		}
		case APPEARED_POKEMON:{
			sem_wait(&suscripcionAColaAPPEARED);
			list_remove_and_destroy_by_condition(suscriptoresAppearedPokemon,(void*) suscriptorAEliminar,free);
			sem_wait(&usoMemoria);
			colaAEliminar = APPEARED_POKEMON;
			susAEliminar = suscrip;
			list_iterate(tablaDeParticiones,(void*) eliminarDeACK);
			sem_post(&usoMemoria);
			sem_post(&suscripcionAColaAPPEARED);
			break;
		}
		case CATCH_POKEMON:{
			sem_wait(&suscripcionAColaCATCH);
			list_remove_and_destroy_by_condition(suscriptoresCatchPokemon,(void*) suscriptorAEliminar,free);
			sem_wait(&usoMemoria);
			colaAEliminar = CATCH_POKEMON;
			susAEliminar = suscrip;
			list_iterate(tablaDeParticiones,(void*) eliminarDeACK);
			sem_post(&usoMemoria);
			sem_post(&suscripcionAColaCATCH);
			break;
		}
		case CAUGHT_POKEMON:{
			sem_wait(&suscripcionAColaCAUGHT);
			list_remove_and_destroy_by_condition(suscriptoresCaughtPokemon,(void*)suscriptorAEliminar,free);
			sem_wait(&usoMemoria);
			colaAEliminar = CAUGHT_POKEMON;
			susAEliminar = suscrip;
			list_iterate(tablaDeParticiones,(void*) eliminarDeACK);
			sem_post(&usoMemoria);
			sem_post(&suscripcionAColaCAUGHT);
			break;
		}
		case GET_POKEMON:{
			sem_wait(&suscripcionAColaGET);
			list_remove_and_destroy_by_condition(suscriptoresGetPokemon,(void*) suscriptorAEliminar,free);
			sem_wait(&usoMemoria);
			colaAEliminar = GET_POKEMON;
			susAEliminar = suscrip;
			list_iterate(tablaDeParticiones,(void*) eliminarDeACK);
			sem_post(&usoMemoria);
			sem_post(&suscripcionAColaGET);
			break;
		}
		case LOCALIZED_POKEMON:{
			sem_wait(&suscripcionAColaLOCALIZED);
			list_remove_and_destroy_by_condition(suscriptoresLocalizedPokemon,(void*) suscriptorAEliminar,free);
			sem_wait(&usoMemoria);
			colaAEliminar = LOCALIZED_POKEMON;
			susAEliminar = suscrip;
			list_iterate(tablaDeParticiones,(void*) eliminarDeACK);
			sem_post(&usoMemoria);
			sem_post(&suscripcionAColaLOCALIZED);
			break;
		}
	}
}

void agregarAMemoria(void * dato, uint32_t idMensaje, tipoDeCola tipoMensaje,uint32_t idCorrelativo, uint32_t tamanioAgregar) {
	sem_wait(&usoMemoria);
	if (!strcmp(algoritmo_memoria, "PARTICIONES")) {

		particion *datoAAgregar = crearEntradaParticionBasica(dato, idMensaje,
				tipoMensaje, idCorrelativo, tamanioAgregar);
		//uint32_t desplazamiento = 0;
		particion *particionEncontrada = malloc(sizeof(particion));

		//Para el algoritmo FF/BF
		//particion* particionMasChica = malloc(sizeof(particion));	//Para el algoritmo BF

		if (tamanioAgregar <= tamanio_memoria) {
			if (!strcmp(algoritmo_particion_libre, "FF")) {
				algoritmoFirstFit(datoAAgregar, particionEncontrada);
			}
			if (!strcmp(algoritmo_particion_libre, "BF")) {
				algoritmoBestFit(datoAAgregar, particionEncontrada);
			}
		} else {
			log_info(almacenadoMemoria,
					"El dato a agregar es mas grande que la memoria");
		}
	}
	if (!strcmp(algoritmo_memoria, "BS")) {

			buddy* buddyAAgregar = malloc(sizeof(buddy));

			buddyAAgregar->particion = crearEntradaParticionBasica(dato, idMensaje,
			tipoMensaje, idCorrelativo, tamanioAgregar);

			agregarEnBuddy(buddyAAgregar);

	}
	sem_post(&usoMemoria);
}

particion* crearEntradaParticionBasica(void * dato, uint32_t idMensaje,tipoDeCola tipoMensaje, uint32_t idCorrelativo, uint32_t tamanioAgregar) {
	particion *datoAAgregar = malloc(sizeof(particion));
	datoAAgregar->mensaje = malloc(tamanioAgregar);
	datoAAgregar->mensaje = dato;
	//agregamos tipos de dato raiz como son new_pokemon, localized_pokemon, etc;
	if(tamanioAgregar<tamanio_minimo_particion){
		tamanioAgregar = tamanio_minimo_particion;
	}
	datoAAgregar->tamanioMensaje = tamanioAgregar;
	datoAAgregar->acknoleged = list_create();
	datoAAgregar->tipoMensaje = tipoMensaje;
	datoAAgregar->libre = 0;
	datoAAgregar->timestamp = temporal_get_string_time();//solo tira la hora sin la fecha
	datoAAgregar->idMensaje = idMensaje;
	datoAAgregar->idCorrelativo = idCorrelativo;

	return datoAAgregar;
}

bool baseMasChica(particion *part1, particion* part2) {
	return part1->base < part2->base;
}

void algoritmoBestFit(particion *datoAAgregar, particion *particionChica) {

	uint32_t baseSig = -1;

	bool particionLibreQueEntre(particion *partic) {
		return (partic->libre
				&& (partic->tamanioMensaje >= datoAAgregar->tamanioMensaje));
	}

	void particionMasChica(particion *partic) {
		if (partic->tamanioMensaje < particionChica->tamanioMensaje) {
			particionChica = partic;
		}
	}
	bool partSiguienteALibre(particion *partic) {
		if (partic->base > particionChica->base) {
			baseSig = partic->base;
			return 1;
		}
		return 0;
	}
//Me parece que es list_sorted.
	list_sort(tablaDeParticiones, (void*) baseMasChica);

	t_list* tablaParticionesLibres = list_filter(tablaDeParticiones,(void*)particionLibreQueEntre);
	particionChica = list_get(tablaParticionesLibres, 0);

	list_iterate(tablaParticionesLibres, (void*) particionMasChica);

	if (particionChica != NULL) {
		list_find(tablaDeParticiones, (void*) partSiguienteALibre);
	}

	//si encontro una partcion en toda la memoria
	agregarTablaParticionesYMemoria(datoAAgregar, particionChica, &baseSig);

	list_destroy(tablaParticionesLibres);

}

void agregarTablaParticionesYMemoria(particion *datoAAgregar,
		particion *partElegida, uint32_t* baseSig) {
	//si encontro una partcion en toda la memoria
	if (partElegida != NULL) {
		datoAAgregar->base = partElegida->base;
		if (partElegida->tamanioMensaje != datoAAgregar->tamanioMensaje) {
			partElegida->base = datoAAgregar->base + datoAAgregar->tamanioMensaje ;
			if (*baseSig != -1) {
				partElegida->tamanioMensaje = *baseSig - partElegida->base;
			} else {
				partElegida->tamanioMensaje = tamanio_memoria - partElegida->base;
			}
			//agregar a la tabal de particiones
			list_add(tablaDeParticiones, datoAAgregar);
		} else {
			//"eliminar" la particion libre, en realidad modificamos la particion libre por la que vamos a agregar
			copiarDatos(partElegida, datoAAgregar);
		}
		//agregar a memoria Real
		memcpy(memoria + datoAAgregar->base, datoAAgregar->mensaje,datoAAgregar->tamanioMensaje);

		log_info(almacenadoMemoria,"El dato(ID:%d) %s con base: %d, tamanio: %d y tiempo: %s",
					datoAAgregar->idMensaje,
					colasDeEnum[datoAAgregar->tipoMensaje],
					datoAAgregar->base, datoAAgregar->tamanioMensaje,
					datoAAgregar->timestamp);

		//producirUnMensaje(datoAAgregar->tipoMensaje);
		//para ver como esta la memoria (COMENTAR)
/*
		log_info(almacenadoMemoria,"la memoria quedo asi: ");
		list_iterate(tablaDeParticiones,mostrarParticiones);*/
	}
	else{
		//cuando se llena la memoria o no hay espacio
		//free(partElegida);
		//si no hay ninguna particion en donde entre

		if(!compactarMemoria()){
			//reemplazar si no pudo compactar
			algoritmoReemplazo();
		}


		//volver a ver si entra
		partElegida = NULL;
		if (!strcmp(algoritmo_particion_libre, "FF")) {
			algoritmoFirstFit(datoAAgregar, partElegida);
		}
		if (!strcmp(algoritmo_particion_libre, "BF")) {
			algoritmoBestFit(datoAAgregar, partElegida);
		}

	}

	//para ver como esta la memoria
	//list_iterate(tablaDeParticiones, mostrarParticiones);
}

/*
void producirUnMensaje(tipoDeCola tipo){
	switch (tipo) {
		case NEW_POKEMON:
			//sem_post(&colaNew);
			break;
		case APPEARED_POKEMON:
			//sem_post(&colaAppeared);
			break;
		case CATCH_POKEMON:
			//sem_post(&colaCatch);
			break;
		case CAUGHT_POKEMON:
			//sem_post(&colaCaught);
			break;
		case GET_POKEMON:
			//sem_post(&colaGet);
			break;
		case LOCALIZED_POKEMON:
			//sem_post(&colaLocalized);
			break;
	}
}*/

void algoritmoReemplazo(){

	uint32_t baseAEliminar = -1;

	particion* partAnt = NULL;

	particion* partNueva = NULL;

	char ** timeAct;
	char ** timeElim;

	int horaAct;
	int horaElim;
	int minAct;
	int minElim;
	int segAct;
	int segElim;
	int milAct;
	int milElim;

	bool partLlenas(particion* part) {
		return !(part->libre);
	}

	t_list *tablaParticionesLlenas = list_filter(tablaDeParticiones, (void*) partLlenas);
	particion* partAEliminar = list_get(tablaParticionesLlenas, 0);

	bool particionesAnt(particion* part) {
		return (part->base < partAEliminar->base);
	}

	void partMasVieja(particion* part) {

		timeAct = string_split(part->timestamp, ":");
		timeElim = string_split(partAEliminar->timestamp, ":");

		horaAct = atoi(timeAct[0]);
		horaElim = atoi(timeElim[0]);

		minAct = atoi(timeAct[1]);
		minElim = atoi(timeElim[1]);

		segAct = atoi(timeAct[2]);
		segElim = atoi(timeElim[2]);

		milAct = atoi(timeAct[3]);
		milElim = atoi(timeElim[3]);

		if (horaAct < horaElim) {
			partAEliminar = part;
		} else if (horaAct == horaElim) {
			if (minAct < minElim) {
				partAEliminar = part;
			} else if (minAct == minElim) {
				if (segAct < segElim) {
					partAEliminar = part;
				} else if (segAct == segElim) {
					if (milAct <= milElim) {
						partAEliminar = part;
					}
				}
			}
		}
	}

	bool partSiguienteALibre(particion *partic) {
		return partic->base > partAEliminar->base;
	}

	void particionAnterior(particion * part) {
		partAnt = part;
	}

	bool particionConEsaBase(particion * part) {
		return (part->base == baseAEliminar);
	}

	//se elije cual es la particion que vamos a eliminar (la mas vieja, ya sea para LRU o FIFO
	list_iterate(tablaParticionesLlenas, (void*) partMasVieja);

	//ordenamos todas las pariciones por sus bases
	list_sort(tablaDeParticiones, (void*) baseMasChica);

	particion *partSig = list_find(tablaDeParticiones, (void*) partSiguienteALibre);

	t_list *particionesAnteriores = list_filter(tablaDeParticiones,(void*) particionesAnt);

	list_sort(particionesAnteriores, (void*) baseMasChica);

	list_iterate(particionesAnteriores,(void*) particionAnterior);

	if (partAEliminar != NULL) {

		partNueva = malloc(sizeof(particion));

		partNueva->libre = 1;
		//LIBRE
		partNueva->tipoMensaje = LIBRE;
		partNueva->idCorrelativo = 0;
		partNueva->idMensaje = 0;
		partNueva->base = partAEliminar->base;
		partNueva->tamanioMensaje = partAEliminar->tamanioMensaje;
		partNueva->timestamp = string_new();
		partNueva->acknoleged = list_create();

		//saco la particion a eliminar de la tabla de particiones
		baseAEliminar = partAEliminar->base;
		partAEliminar = list_remove_by_condition(tablaDeParticiones,(void*) particionConEsaBase);

		//consolidacion
		if (partSig != NULL) {
			if (partSig->libre) {
				partNueva->tamanioMensaje += partSig->tamanioMensaje;
				//eliminar partSig de la Tabla de Particiones
				baseAEliminar = partSig->base;
				list_remove_and_destroy_by_condition(tablaDeParticiones,(void*) particionConEsaBase, free);
			}
		}
		if (partAnt != NULL) {
			if (partAnt->libre) {
				//eliminar partAnt de la Tabla de Particiones
				baseAEliminar = partAnt->base;
				partAnt = list_remove_by_condition(tablaDeParticiones,(void*) particionConEsaBase);
				//sumamos el tamaño de esa particion anterior LIBRE
				partNueva->tamanioMensaje += partAnt->tamanioMensaje;
				//movemos la Base a esa
				partNueva->base = partAnt->base;
				free(partAnt);
			}
		}
		log_info(eliminacionMemoria,"eliminamos de la memoria el dato(ID:%d) %s con base: %d, tamanio: %d y tiempo: %s",
					partAEliminar->idMensaje,
					colasDeEnum[partAEliminar->tipoMensaje], partAEliminar->base,
					partAEliminar->tamanioMensaje, partAEliminar->timestamp);

		//libero la particion que quiero eliminar
		eliminarParticion(partAEliminar);
		//agrego la nueva particion LIBRE que quedo de la consolidacion
		list_add(tablaDeParticiones, partNueva);

	}

	//PARA VER COMO QUEDA LA MEMORIA DESPUES DE ELIMINAR (COMENTAR)
	/*
	 list_sort(tablaDeParticiones,baseMasChica);
	 log_info(eliminacionMemoria,"la memoria quedo asi: ");
	 list_iterate(tablaDeParticiones,mostrarParticiones);
	 log_info(eliminacionMemoria," ");*/

	if (tablaParticionesLlenas->elements_count) {
		list_destroy(tablaParticionesLlenas);
	}
	if (tablaParticionesLlenas->elements_count) {
		list_destroy(particionesAnteriores);
	}
}

void algoritmoFirstFit(particion *datoAAgregar, particion *particionEncontrada) {

	//list_iterate(tablaDeParticiones, mostrarParticiones);

	uint32_t baseSig = -1;

	bool primeroLibreQueEntre(particion *partic) {
		return (partic->libre
				&& (partic->tamanioMensaje >= datoAAgregar->tamanioMensaje));
	}

	bool partSiguienteALibre(particion *partic) {
		if (partic->base > particionEncontrada->base) {
			baseSig = partic->base;
			return 1;
		}
		return 0;
	}

	bool partEncontradaEliminar(particion *partic) {
		return (partic == particionEncontrada);
	}

	list_sort(tablaDeParticiones, (void*) baseMasChica);
	particionEncontrada = list_find(tablaDeParticiones, (void*) primeroLibreQueEntre);

	if (particionEncontrada) {
		list_find(tablaDeParticiones, (void*) partSiguienteALibre);
	}

	//si encontro una partcion en toda la memoria
	agregarTablaParticionesYMemoria(datoAAgregar, particionEncontrada,&baseSig);

}

void copiarDatos(particion *target, particion * copiado) {
	list_add_all(target->acknoleged, copiado->acknoleged);
	target->base = copiado->base;
	target->idMensaje = copiado->idMensaje;
	target->libre = copiado->libre;
	target->mensaje = copiado->mensaje;
	target->tamanioMensaje = copiado->tamanioMensaje;
	target->timestamp = copiado->timestamp;
	target->tipoMensaje = copiado->tipoMensaje;
}

int compactarMemoria(){

	bool partLlenas(particion* part) {
		return !(part->libre);
	}
	bool partLibres(particion* part) {
		return (part->libre);
	}
	//nos fijamos la frecuencia
	if (frecuencia == frecuencia_compactacion) {
		frecuencia = 0;
		if(list_count_satisfying(tablaDeParticiones, (void*) partLibres) != 0){
			uint32_t baseAOcupar = 0;
			uint32_t cantLibres = 0;

			void cambiarBases(particion * part) {
				//cambia todas las bases para que esten juntas(solo en la tabla de particiones)
				part->base = baseAOcupar;
				baseAOcupar += part->tamanioMensaje;
				//cambia la memoria
				memcpy(memoria + (part->base), part->mensaje,
						part->tamanioMensaje);
			}

			t_list* partOcupadas = list_filter(tablaDeParticiones, (void*) partLlenas);

			list_sort(tablaDeParticiones,(void*) baseMasChica);
			//muestra como quedo la tabla de particiones  (COMENTAR)
			/*
			 log_info(compactacionMemoria,"antes de compactar la memoria esta asi:");
			 list_iterate(tablaDeParticiones,mostrarParticiones);*/

			//ordenamos las bases ocupadas
			list_sort(partOcupadas, (void*) baseMasChica);
			//cambiamos las bases ocupadas para que esten todas juntas
			list_iterate(partOcupadas, (void*) cambiarBases);

			//destruir y liberar todas las particiones libres
			cantLibres = list_count_satisfying(tablaDeParticiones, (void*) partLibres);

			while (cantLibres > 0) {
				list_remove_and_destroy_by_condition(tablaDeParticiones,(void*) partLibres, free);
				cantLibres--;
			}

			particion *partLibreNueva = malloc(sizeof(particion));
			partLibreNueva->idMensaje = 0;
			partLibreNueva->base = baseAOcupar;
			partLibreNueva->tamanioMensaje = tamanio_memoria - partLibreNueva->base;
			partLibreNueva->idCorrelativo = 0;
			partLibreNueva->tipoMensaje = LIBRE;
			partLibreNueva->libre = 1;
			partLibreNueva->acknoleged = list_create();

			list_add(tablaDeParticiones, partLibreNueva);

			list_destroy(partOcupadas);

			log_info(compactacionMemoria, "Compacte la memoria");

			//muestra como quedo la tabla de particiones
			/*
			log_info(compactacionMemoria,"Despues de compactar la memoria y quedo asi:");
			list_iterate(tablaDeParticiones,mostrarParticiones);*/

			return 1;
		}
	}

	frecuencia++;
	return 0;

}

void eliminarParticion(particion * part){
	if (part->acknoleged != NULL) {
		list_destroy_and_destroy_elements(part->acknoleged, free);
	}
	if (part->mensaje != NULL) {
		free(part->mensaje);
	}
	if (!strcmp(part->timestamp, "")) {
		free(part->timestamp);
	}

	free(part);
}

void terminar_programa() {

	log_destroy(logConexion);
	log_destroy(logSuscipcion);
	log_destroy(logMensajeNuevo);
	log_destroy(logEnviarNuevo);
	log_destroy(confirmacionRecepcion);
	log_destroy(almacenadoMemoria);
	log_destroy(eliminacionMemoria);
	log_destroy(compactacionMemoria);
	log_destroy(dumpCache);

	config_destroy(config);
}

//ver id correlativo si lo usa
broker_new_pokemon* leerdeMemoriaNEW(particion* part) {

	int offset = part->base;

	broker_new_pokemon* newEnMemo = malloc(sizeof(broker_new_pokemon));
	newEnMemo->datos= malloc(sizeof(new_pokemon));

	newEnMemo->id = part->idMensaje;
	//new no tiene id relativo
	//newEnMemo->id_relativo = part->idCorrelativo;


	//deserializacion
	//1. uint32_t tamanioNombre;
	//2. char* nombrePokemon;
	//3. uint32_t posX;
	//4. uint32_t posY;
	//5. uint32_t CantidadPokemons;

	memcpy(&(newEnMemo->datos->tamanioNombre),memoria+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	newEnMemo->datos->nombrePokemon = malloc(newEnMemo->datos->tamanioNombre);

	memcpy((newEnMemo->datos->nombrePokemon),memoria+offset,newEnMemo->datos->tamanioNombre);
	offset+=newEnMemo->datos->tamanioNombre;

	memcpy(&(newEnMemo->datos->posX),memoria+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&(newEnMemo->datos->posY),memoria+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&(newEnMemo->datos->cantidadPokemon),memoria+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);


	if (!strcmp(algoritmo_reemplazo, "LRU")) {
		part->timestamp = temporal_get_string_time();
	}

	return newEnMemo;
}

// ver id correlativo si lo usa
broker_appeared_pokemon* leerdeMemoriaAPPEARED(particion* part) {

	int offset =part->base;

	broker_appeared_pokemon* appEnMemoria = malloc(sizeof(broker_appeared_pokemon));
	appEnMemoria->datos = malloc(sizeof(appeared_pokemon));

	appEnMemoria->id= part->idMensaje;
	//ver id correlativo si lo usa
	appEnMemoria->id_relativo = part->idCorrelativo;


	//deserializacion
	//1. uint32_t tamanioNombre;
	//2. char* nombrePokemon;
	//3. uint32_t posX;
	//4. uint32_t posY;

	memcpy(&(appEnMemoria->datos->tamanioNombre),memoria+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	appEnMemoria->datos->nombrePokemon = malloc(appEnMemoria->datos->tamanioNombre);

	memcpy((appEnMemoria->datos->nombrePokemon),memoria+offset,appEnMemoria->datos->tamanioNombre);
	offset+=appEnMemoria->datos->tamanioNombre;

	memcpy(&(appEnMemoria->datos->posX),memoria+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&(appEnMemoria->datos->posY),memoria+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);



	if (!strcmp(algoritmo_reemplazo, "LRU")) {
		part->timestamp = temporal_get_string_time();
	}

	return appEnMemoria;
}

// ver id correlativo si lo usa
broker_catch_pokemon* leerdeMemoriaCATCH(particion* part) {
	int offset =part->base;

	broker_catch_pokemon* catchEnMemo = malloc(sizeof(broker_catch_pokemon));
	catchEnMemo->datos = malloc(sizeof(catch_pokemon));

	catchEnMemo->id = part->idMensaje;
	//ver id correlativo si lo usa
	//catchEnMemo->id_relativo = part->idCorrelativo;

	//deserializacion
	//1. uint32_t tamanioNombre;
	//2. char* nombrePokemon;
	//3. uint32_t posX;
	//4. uint32_t posY;

	memcpy(&(catchEnMemo->datos->tamanioNombre),memoria+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	catchEnMemo->datos->nombrePokemon = malloc(catchEnMemo->datos->tamanioNombre);

	memcpy((catchEnMemo->datos->nombrePokemon),memoria+offset,catchEnMemo->datos->tamanioNombre);
	offset+=catchEnMemo->datos->tamanioNombre;

	memcpy(&(catchEnMemo->datos->posX),memoria+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&(catchEnMemo->datos->posY),memoria+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);


	if (!strcmp(algoritmo_reemplazo, "LRU")) {
		part->timestamp = temporal_get_string_time();
	}


	return catchEnMemo;
}

// ver id correlativo si lo usa
broker_caught_pokemon* leerdeMemoriaCAUGHT(particion* part) {
	int offset =part->base;

	broker_caught_pokemon * caughtEnMemo = malloc(sizeof(broker_caught_pokemon));
	caughtEnMemo->datos = malloc(sizeof(caught_pokemon));

	caughtEnMemo->id = part->idMensaje;
	//ver id correlativo si lo usa
	caughtEnMemo->id_relativo = part->idCorrelativo;


	//deserializacion
	//1. uint32_t puedoAtraparlo;

	memcpy(&(caughtEnMemo->datos->puedoAtraparlo),memoria+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);


	if (!strcmp(algoritmo_reemplazo, "LRU")) {
		part->timestamp = temporal_get_string_time();
	}

	return caughtEnMemo;
}

// ver id correlativo si lo usa
broker_get_pokemon* leerdeMemoriaGET(particion* part) {
	int offset =part->base;

	broker_get_pokemon* getEnMemo = malloc(sizeof(broker_get_pokemon));
	getEnMemo->datos = malloc(sizeof(get_pokemon));

	getEnMemo->id = part->idMensaje;
	//ver id correlativo si lo usa (este es medio dudoso porque el GAMEBOY le manda a GAMECARD un id relativo
	//getEnMemo->id_relativo = part->idCorrelativo;

	//serializacion
	//1. uint32_t tamanioNombre;
	//2. char* nombrePokemon;

	memcpy(&(getEnMemo->datos->tamanioNombre),memoria+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	getEnMemo->datos->nombrePokemon = malloc(getEnMemo->datos->tamanioNombre);

	memcpy((getEnMemo->datos->nombrePokemon),memoria+offset,getEnMemo->datos->tamanioNombre);
	offset+=getEnMemo->datos->tamanioNombre;


	if (!strcmp(algoritmo_reemplazo, "LRU")) {
		part->timestamp = temporal_get_string_time();
	}

	return getEnMemo;
}

// ver id correlativo si lo usa
broker_localized_pokemon* leerdeMemoriaLOCALIZED(particion* part) {
	int offset =part->base;

	broker_localized_pokemon* localizedEnMemo = malloc(
			sizeof(broker_localized_pokemon));
	localizedEnMemo->datos = malloc(sizeof(localized_pokemon));

	localizedEnMemo->id = part->idMensaje;
	//ver id correlativo si lo usa
	localizedEnMemo->id_relativo = part->idCorrelativo;


	//deserializacion
	//1. uint32_t tamanioNombre;
	//2. char* nombrePokemon;
	//3. uint32_t cantidadPosiciones;
	//4. uint32_t* posX;
	//5. uint32_t* posY;

	uint32_t posiciones;

	memcpy(&(localizedEnMemo->datos->tamanioNombre),memoria+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	localizedEnMemo->datos->nombrePokemon = malloc(localizedEnMemo->datos->tamanioNombre);

	memcpy((localizedEnMemo->datos->nombrePokemon),memoria+offset,localizedEnMemo->datos->tamanioNombre);
	offset+=localizedEnMemo->datos->tamanioNombre;

	memcpy(&(localizedEnMemo->datos->cantidadPosiciones),memoria+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	localizedEnMemo->datos->posX = malloc((localizedEnMemo->datos->cantidadPosiciones)*sizeof(uint32_t));
	localizedEnMemo->datos->posY = malloc((localizedEnMemo->datos->cantidadPosiciones)*sizeof(uint32_t));

	for(posiciones=0;posiciones<localizedEnMemo->datos->cantidadPosiciones;posiciones++){
		//tengo mis dudas por el &
		memcpy(&(localizedEnMemo->datos->posX[posiciones]),memoria+offset,sizeof(uint32_t));
		offset+=sizeof(uint32_t);

		memcpy(&(localizedEnMemo->datos->posY[posiciones]),memoria+offset,sizeof(uint32_t));
		offset+=sizeof(uint32_t);

	}


	if (!strcmp(algoritmo_reemplazo, "LRU")) {
		part->timestamp = temporal_get_string_time();
	}

	return localizedEnMemo;
}

int tamanioBuddy(buddy* unBuddy){
	return unBuddy->limite - unBuddy->particion->base + 1;
}

void dividirEnDos(buddy* unBuddy) {

	buddy* primerNuevoBuddy = malloc(sizeof(buddy));
	primerNuevoBuddy->particion = malloc(sizeof(particion));

	primerNuevoBuddy->particion->idMensaje = 0;
	primerNuevoBuddy->particion->tipoMensaje = LIBRE;
	primerNuevoBuddy->particion->base = unBuddy->particion->base;
	primerNuevoBuddy->particion->tamanioMensaje = tamanioBuddy(unBuddy)/2;
	primerNuevoBuddy->particion->libre = 1;
	primerNuevoBuddy->particion->timestamp = string_new();
	primerNuevoBuddy->particion->acknoleged = list_create();
	primerNuevoBuddy->limite = unBuddy->particion->base + tamanioBuddy(unBuddy)/2 - 1;

	buddy* segundoNuevoBuddy = malloc(sizeof(buddy));
	segundoNuevoBuddy->particion = malloc(sizeof(particion));

	segundoNuevoBuddy->particion->idMensaje = 0;
	segundoNuevoBuddy->particion->tipoMensaje = LIBRE;
	segundoNuevoBuddy->particion->base = primerNuevoBuddy->limite+1;
	segundoNuevoBuddy->particion->tamanioMensaje = tamanioBuddy(unBuddy)/2;
	segundoNuevoBuddy->particion->libre = 1;
	segundoNuevoBuddy->particion->timestamp = string_new();
	segundoNuevoBuddy->particion->acknoleged = list_create();
	segundoNuevoBuddy->limite = unBuddy->limite;

	actualizarTablaParticionesParaBuddy(unBuddy,primerNuevoBuddy,segundoNuevoBuddy);

}

void agregarEnBuddy(buddy* unBuddyParaAgregar){

	t_list* buddiesTotales = tablaDeParticiones;

	uint32_t tamanioParaAgregar = unBuddyParaAgregar->particion->tamanioMensaje;

	uint32_t tamanioIdeal = tamanioIdealBuddy(tamanioParaAgregar);

	bool mayorOIgualAtamanioIdeal(buddy* unBuddy){
		return tamanioBuddy(unBuddy) >=  tamanioIdeal;
	}

	bool estaLibre(buddy* unBuddy){
		return unBuddy->particion->libre;
	}

	t_list* buddiesLibres = list_filter(buddiesTotales,(void*) estaLibre);

	t_list* buddiesQueMeSirven = list_filter(buddiesLibres,(void*) mayorOIgualAtamanioIdeal);

	if (list_size(buddiesQueMeSirven) != 0) {

		buddy* buddyMenor = list_get(buddiesQueMeSirven, 0);
		uint32_t i = 0;

		for (i = 0; i < list_size(buddiesQueMeSirven); i++) {

			buddy* aComparar = list_get(buddiesQueMeSirven, i);

			if (tamanioBuddy(buddyMenor) > tamanioBuddy(aComparar)) {
				buddyMenor = aComparar;
			}

		}

		if (tamanioIdeal == tamanioBuddy(buddyMenor)) {

			memcpy(memoria + buddyMenor->particion->base,
					unBuddyParaAgregar->particion->mensaje,
					unBuddyParaAgregar->particion->tamanioMensaje);

			actualizarComoOcupadoEnLista(buddyMenor,unBuddyParaAgregar);

			log_info(almacenadoMemoria,
					"base: %d libre: %d limite: %d tamanioMensaje: %d",
					buddyMenor->particion->base, buddyMenor->particion->libre,
					buddyMenor->limite,
					unBuddyParaAgregar->particion->tamanioMensaje);

		} else {

			dividirEnDos(buddyMenor);
			agregarEnBuddy(unBuddyParaAgregar);

		}

	} else {

		eliminarVictima();
		agregarEnBuddy(unBuddyParaAgregar);

	}

}

bool hayTamanioDisponiblePara(int tamanio){

	bool estaLibre(buddy* unBuddy){
		return unBuddy->particion->libre;
	}

	//TODO comente esto porque no se si lo usaban
	t_list* listaLibres = list_filter(tablaDeParticiones,(void*) estaLibre);

	t_list* listaTamaniosLibres = list_map(tablaDeParticiones,(void*) tamanioBuddy);

	bool mayorOigualQueTamanioBuscado(int tamanioParaComparar) {
		return tamanioParaComparar >= tamanio;
	}

	return list_any_satisfy(listaTamaniosLibres, (void*) mayorOigualQueTamanioBuscado);

}

void actualizarComoOcupadoEnLista(buddy* unBuddy,buddy* unBuddyParaAgregar) {

	buddy* aComparar;

	int yaLoEncontre = 0;

	int j = 0;

	while(yaLoEncontre == 0){

		aComparar = list_get(tablaDeParticiones,j);

		if(unBuddy->particion->base == aComparar->particion->base) yaLoEncontre = 1;
		else j++;
	}

	buddy* buddyQueSustituye = unBuddy;

	buddyQueSustituye->particion->idMensaje = unBuddyParaAgregar->particion->idMensaje;

	buddyQueSustituye->particion->libre = 0;

	buddyQueSustituye->particion->tipoMensaje = unBuddyParaAgregar->particion->tipoMensaje;

	char* horarioActual = temporal_get_string_time();

	string_append(&buddyQueSustituye->particion->timestamp,horarioActual);

	list_replace(tablaDeParticiones,j,buddyQueSustituye);

}

void actualizarTablaParticionesParaBuddy(buddy* aReemplazar,buddy* primerNuevoBuddy,buddy* segundoNuevoBuddy){

	int j = 0;

	t_list* nuevaLista = list_create();

	buddy* buddysEnTabla;

	int yaLoEncontre = 0;

	while(yaLoEncontre == 0){

		buddysEnTabla = list_get(tablaDeParticiones,j);

		if(buddysEnTabla->particion->base == aReemplazar->particion->base) yaLoEncontre = 1;
		else j++;
	}


	int i;

	for(i = 0; i < j; i++) {
		buddy* aAgregar = list_get(tablaDeParticiones,i);
		list_add(nuevaLista,aAgregar);
	}

	list_add(nuevaLista,primerNuevoBuddy);
	list_add(nuevaLista,segundoNuevoBuddy);

	for(i = j+1; i < list_size(tablaDeParticiones); i++) {
			buddy* aAgregar = list_get(tablaDeParticiones,i);
			list_add(nuevaLista,aAgregar);
	}

	list_clean(tablaDeParticiones);

	list_add_all(tablaDeParticiones,nuevaLista);

}

t_list* listaTamanioBuddiesPosibles() {

	t_list* posiblesTamanios = list_create();

	int tamanioInicial = tamanio_memoria;

	while(tamanioInicial != tamanio_minimo_particion){
		list_add(posiblesTamanios,(void*) tamanioInicial);
		tamanioInicial = tamanioInicial / 2;
	}

	list_add(posiblesTamanios,(void*) tamanioInicial);

	return posiblesTamanios;

}

uint32_t tamanioIdealBuddy(uint32_t tamanio) {

	t_list* tamanioBuddies = listaTamanioBuddiesPosibles();

	bool buddyEsMasGrandeTamanioMensaje(uint32_t tamanioAcomparar){
		 return tamanioAcomparar >= tamanio;
	}

	t_list* dondeEntra = list_filter(tamanioBuddies,(void*) buddyEsMasGrandeTamanioMensaje);

	uint32_t minimo = list_get(dondeEntra,0);
	uint32_t i = 0;

	for(i = 0; i < list_size(dondeEntra);i++){

		if(minimo > list_get(dondeEntra,i)){
			minimo = list_get(dondeEntra,i);
		}

	}

	return minimo;

}

int posicionBuddyTabla(buddy* buddyAconsiderar) {

	int baseAbuscar = buddyAconsiderar->particion->base;

	int base(buddy* unBuddy){
		return unBuddy->particion->base;
	}

	bool mismaBaseQueBusco(buddy* unBuddy){
		return baseAbuscar == unBuddy->particion->base;
	}

	t_list* listaBases = list_map(tablaDeParticiones,(void*) base);

	int i = 0; //posicion del buddy a considerar

	int encontrado = 0;

	while(encontrado == 0 && i < list_size(listaBases)){

		int baseActual = list_get(listaBases,i);

		if(baseActual == baseAbuscar){
			encontrado = 1;
		} else {
			i++;
		}

	}

	return i;


}

void consolidarSiEsPosible(buddy* unBuddy) {

	int posicionBuddy = posicionBuddyTabla(unBuddy);

	bool posicionPAr(int numero) {
		return numero % 2 == 0;
	}

	bool esUltimoBuddy(buddy* unBuddy) {
		return unBuddy->limite == tamanio_memoria-1;
	}

	if (esUltimoBuddy(unBuddy)) {
		consolidarSiEsPosibleAIzquierda(unBuddy);
	} else {

		if (posicionPAr(posicionBuddy)) {
			consolidarSiEsPosibleADerecha(unBuddy); //creo nuevo buddy , ver bases y limites, usar remove y replace
		} else {
			consolidarSiEsPosibleAIzquierda(unBuddy); //creo nuevo buddy, ves bases y limites, usar remove y replace
		}

	}

}

void consolidarSiEsPosibleADerecha(buddy* unBuddy) {

	int posicionBuddy = posicionBuddyTabla(unBuddy);

	int posicionSiguiente = posicionBuddyTabla(unBuddy) + 1;

	buddy* buddySiguiente = list_get(tablaDeParticiones, posicionSiguiente);

	bool ambosLibres(buddy* unBuddy, buddy* otroBuddy) {
		return unBuddy->particion->libre == 1
				&& otroBuddy->particion->libre == 1;
	}

	bool mismoTamanio(buddy* unBuddy, buddy* otroBuddy) {
		return tamanioBuddy(unBuddy) == tamanioBuddy(otroBuddy);
	}

	if (ambosLibres(unBuddy, buddySiguiente)
			&& mismoTamanio(unBuddy, buddySiguiente)) {

		buddy* nuevoBuddy = malloc(sizeof(buddy));
		nuevoBuddy->particion = malloc(sizeof(particion));

		nuevoBuddy->particion->idMensaje = 0;
		nuevoBuddy->particion->tipoMensaje = LIBRE;
		nuevoBuddy->particion->base = unBuddy->particion->base;
		nuevoBuddy->particion->tamanioMensaje = tamanioBuddy(unBuddy) * 2;
		nuevoBuddy->particion->libre = 1;
		nuevoBuddy->particion->timestamp = string_new();
		nuevoBuddy->particion->acknoleged = list_create();
		nuevoBuddy->limite = buddySiguiente->limite;

		int i;

		t_list* nuevaLista = list_create();

		for (i = 0; i < posicionBuddy; i++) {
			list_add(nuevaLista, list_get(tablaDeParticiones, i));
		}

		list_add(nuevaLista, nuevoBuddy);

		for (i = posicionSiguiente + 1; i < list_size(tablaDeParticiones); i++) {
			list_add(nuevaLista, list_get(tablaDeParticiones, i));
		}

		list_clean(tablaDeParticiones);

		list_add_all(tablaDeParticiones, nuevaLista);

		log_info(almacenadoMemoria,
		"se consolida la base1: %d con limite1: %d y la base2: %d con limite2: %d y forman nuevaBase: %d con nuevoLimite: %d",
		unBuddy->particion->base,unBuddy->limite,buddySiguiente->particion->base,buddySiguiente->limite,nuevoBuddy->particion->base,nuevoBuddy->limite
		);
	}
}

void consolidarSiEsPosibleAIzquierda(buddy* unBuddy){

	int posicionBuddy = posicionBuddyTabla(unBuddy);

	int posicionAnterior = posicionBuddyTabla(unBuddy)-1;

	buddy* buddyAnterior = list_get(tablaDeParticiones,posicionAnterior);

	bool ambosLibres(buddy* unBuddy, buddy* otroBuddy){
			return unBuddy->particion->libre == 1 && otroBuddy->particion->libre == 1 ;
	}

	bool mismoTamanio(buddy* unBuddy, buddy* otroBuddy){
			return tamanioBuddy(unBuddy) == tamanioBuddy(otroBuddy);
	}

	if(ambosLibres(unBuddy,buddyAnterior) && mismoTamanio(unBuddy,buddyAnterior)){

		buddy* nuevoBuddy = malloc(sizeof(buddy));
		nuevoBuddy->particion = malloc(sizeof(particion));

		nuevoBuddy->particion->idMensaje = 0;
		nuevoBuddy->particion->tipoMensaje = LIBRE;
		nuevoBuddy->particion->base = buddyAnterior->particion->base;
		nuevoBuddy->particion->tamanioMensaje = tamanioBuddy(unBuddy)*2;
		nuevoBuddy->particion->libre = 1;
		nuevoBuddy->particion->timestamp = string_new();
		nuevoBuddy->particion->acknoleged = list_create();
		nuevoBuddy->limite = unBuddy->limite;

		int i;

		t_list* nuevaLista = list_create();

		for(i = 0; i < posicionAnterior;i++){
			list_add(nuevaLista,list_get(tablaDeParticiones,i));
		}

		list_add(nuevaLista,nuevoBuddy);

		for(i = posicionBuddy+1; i < list_size(tablaDeParticiones);i++){
			list_add(nuevaLista,list_get(tablaDeParticiones,i));
		}

		list_clean(tablaDeParticiones);

		list_add_all(tablaDeParticiones,nuevaLista);

		log_info(almacenadoMemoria,
				"se consolida la base1: %d con limite1: %d y la base2: %d con limite2: %d y forman nuevaBase: %d con nuevoLimite: %d",
				buddyAnterior->particion->base,buddyAnterior->limite,unBuddy->particion->base,unBuddy->limite,nuevoBuddy->particion->base,nuevoBuddy->limite
				);

	}


}

buddy* seleccionarVictima(){

	bool buddyLleno(buddy* unBuddy) {
		return !unBuddy->particion->libre;
	}

	char ** timeAct;
	char ** timeElim;

	int horaAct;
	int horaElim;
	int minAct;
	int minElim;
	int segAct;
	int segElim;
	int milAct;
	int milElim;

	t_list *tablaParticionesLlenas = list_filter(tablaDeParticiones,(void*) buddyLleno);

	buddy* buddyAEliminar = list_get(tablaParticionesLlenas, 0);

	void partMasVieja(buddy* unBuddy) {

			timeAct = string_split(unBuddy->particion->timestamp, ":");
			timeElim = string_split(buddyAEliminar->particion->timestamp, ":");

			horaAct = atoi(timeAct[0]);
			horaElim = atoi(timeElim[0]);

			minAct = atoi(timeAct[1]);
			minElim = atoi(timeElim[1]);

			segAct = atoi(timeAct[2]);
			segElim = atoi(timeElim[2]);

			milAct = atoi(timeAct[3]);
			milElim = atoi(timeElim[3]);

			if (horaAct < horaElim) {
				buddyAEliminar = unBuddy;
			} else if (horaAct == horaElim) {
				if (minAct < minElim) {
					buddyAEliminar = unBuddy;
				} else if (minAct == minElim) {
					if (segAct < segElim) {
						buddyAEliminar = unBuddy;
					} else if (segAct == segElim) {
						if (milAct <= milElim) {
							buddyAEliminar = unBuddy;
						}
					}
				}
			}
		}

		list_iterate(tablaParticionesLlenas,(void*) partMasVieja);


		return buddyAEliminar;

}

void eliminarVictima(){

	buddy* aELiminar = seleccionarVictima();

	int posicionAeliminar = posicionBuddyTabla(aELiminar);

	buddy* nuevoBuddy = malloc(sizeof(buddy));
	nuevoBuddy->particion = malloc(sizeof(particion));

	nuevoBuddy->particion->idMensaje = 0;
	nuevoBuddy->particion->tipoMensaje = LIBRE;
	nuevoBuddy->particion->base = aELiminar->particion->base;
	nuevoBuddy->particion->tamanioMensaje = tamanioBuddy(aELiminar);
	nuevoBuddy->particion->libre = 1;
	nuevoBuddy->particion->timestamp = string_new();
	nuevoBuddy->particion->acknoleged = list_create();
	nuevoBuddy->limite = aELiminar->limite;

	int i;

	t_list* nuevaLista = list_create();

	for(i = 0; i < posicionAeliminar; i++) {
		buddy* aAgregar = list_get(tablaDeParticiones,i);
		list_add(nuevaLista,aAgregar);
	}

	list_add(nuevaLista,nuevoBuddy);

	for(i = posicionAeliminar+1; i < list_size(tablaDeParticiones); i++) {
			buddy* aAgregar = list_get(tablaDeParticiones,i);
			list_add(nuevaLista,aAgregar);
	}

	list_clean(tablaDeParticiones);

	list_add_all(tablaDeParticiones,nuevaLista);

	log_info(almacenadoMemoria,
					"se libera la base: %d y limite: %d",
	aELiminar->particion->base,aELiminar->limite);

	consolidarBuddySystem();

}


void consolidarBuddySystem() {


	while(list_any_satisfy(tablaDeParticiones, (void*) buddyPuedeConsolidar)){

		int i;

		for(i = 0; i < list_size(tablaDeParticiones); i++){
			consolidarSiEsPosible(list_get(tablaDeParticiones,i));
		}

	}


}

bool buddyPuedeConsolidar(buddy* unBuddy){

	bool esUltimoBuddy(buddy* unBuddy) {
		return unBuddy->limite == tamanio_memoria-1;
	}

	if(esUltimoBuddy(unBuddy)){
		return puedeConsolidarIzquierda(unBuddy);
	}
	else {

		if(posicionBuddyTabla(unBuddy) % 2 == 0){
		return puedeConsolidarDerecha(unBuddy);
		}
		else return puedeConsolidarIzquierda(unBuddy);

	}
}

bool puedeConsolidarDerecha(buddy* unBuddy){

	//TODO esto lo comente porque no lo usaban
	//int posicionBuddy = posicionBuddyTabla(unBuddy);

	int posicionSiguiente = posicionBuddyTabla(unBuddy) + 1;

	buddy* buddySiguiente = list_get(tablaDeParticiones, posicionSiguiente);

	bool ambosLibres(buddy* unBuddy, buddy* otroBuddy) {
			return unBuddy->particion->libre == 1
				&& otroBuddy->particion->libre == 1;
		}

	bool mismoTamanio(buddy* unBuddy, buddy* otroBuddy) {
		return tamanioBuddy(unBuddy) == tamanioBuddy(otroBuddy);
	}

	return ambosLibres(unBuddy,buddySiguiente) && mismoTamanio(unBuddy,buddySiguiente);

}

bool puedeConsolidarIzquierda(buddy* unBuddy){
	//TODO esto lo comente porque no lo usaban
	//int posicionBuddy = posicionBuddyTabla(unBuddy);

	int posicionAnterior = posicionBuddyTabla(unBuddy)-1;

	buddy* buddyAnterior = list_get(tablaDeParticiones,posicionAnterior);

		bool ambosLibres(buddy* unBuddy, buddy* otroBuddy){
				return unBuddy->particion->libre == 1 && otroBuddy->particion->libre == 1 ;
		}

		bool mismoTamanio(buddy* unBuddy, buddy* otroBuddy){
				return tamanioBuddy(unBuddy) == tamanioBuddy(otroBuddy);
		}

	return ambosLibres(unBuddy,buddyAnterior) && mismoTamanio(unBuddy,buddyAnterior);

}





















