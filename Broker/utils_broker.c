/*
 * utils_broker.c
 *l
 *  Created on: 14 may. 2020
 *      Author: utnso
 */

#include "utils_broker.h"

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

void iniciar_servidor(void) {
	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip_broker, puerto_broker, &hints, &servinfo);

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((socket_servidor = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1)
			continue;

		if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_servidor);
			continue;
		}
		break;
	}

	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);

	while (1) {
		esperar_cliente(socket_servidor);
	}
}

void esperar_cliente(int socket_servidor) {
	struct sockaddr_in dir_cliente;

	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente,
			&tam_direccion);

	pthread_create(&thread, NULL, (void*) serve_client, &socket_cliente);
	pthread_detach(thread);

}

void serve_client(int* socket) {

	sem_wait(&llegadaMensajes);
	int cod_op = 0;
	int i = recv(*socket, &cod_op, sizeof(op_code), MSG_WAITALL);
	if (i <= 0)
		cod_op = -1;
	process_request(cod_op, *socket);
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

	uint32_t ackRecibido;
	particion* partEncontrada;

	suscriptor* suscriptor;
	broker_new_pokemon* newRecibido;
	broker_appeared_pokemon* appearedRecibido;
	broker_get_pokemon* getRecibido;
	broker_catch_pokemon* catchRecibido;
	broker_caught_pokemon* caughtRecibido;
	broker_localized_pokemon* localizedRecibido;

	//log_info(logConexion,"%s se conecto al broker",username);
	//falta los case de los otros tipos de mensajes (get,catch,caught)(localized lo dejamos para despues(es de GameCard)
	switch (cod_op) {
		case SUSCRIPCION:


			suscriptor = deserializar_suscripcion(cliente_fd);

			log_info(logSuscipcion,
					"recibi mensaje de suscripcion de %s a la cola %s",
					suscriptor->nombreDeSuscriptor,
					colasDeEnum[(suscriptor->tipoDeCola)]);

			sem_wait(&suscripcionACola);
			suscribirACola(suscriptor);
			sem_post(&suscripcionACola);

			free(suscriptor);

			break;

		case BROKER__NEW_POKEMON:


			newRecibido = deserializar_new_pokemon(cliente_fd);
			log_info(logMensajeNuevo,
					"recibi mensaje de NEW_POKEMON de %s \n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d \n cantidad de pokemones: %d",
					username, newRecibido->datos->tamanioNombre,
					newRecibido->datos->nombrePokemon, newRecibido->datos->posX,
					newRecibido->datos->posY, newRecibido->datos->cantidadPokemon);
			//mutex
			sem_wait(&idsDeMensajes);
			newRecibido->id = idGlobales;
			idGlobales++;
			sem_post(&idsDeMensajes);
			//mutex

			raiz = transformarBrokerNewPokemon(newRecibido, &tamanioAgregar);


			//log_info(logMensajeNuevo,"lo que vale este new a agregar es %d",sizeof(raiz));
			//log_info(logMensajeNuevo,"lo que vale este new puntero de dato es %d",sizeof(new_pokemon*));
			agregarAMemoria(raiz, newRecibido->id, NEW_POKEMON, 0, tamanioAgregar);
			//agregarACola(NEW_POKEMON,newRecibido);

			//free(raiz);
			free(newRecibido);

			break;

		case BROKER__APPEARED_POKEMON:


			appearedRecibido = deserializar_appeared_pokemon(cliente_fd);

			log_info(logMensajeNuevo,
					"recibi mensaje de APPEARED_POKEMON de %s \n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d \n con id_relativo: %d",
					username, appearedRecibido->datos->tamanioNombre,
					appearedRecibido->datos->nombrePokemon,
					appearedRecibido->datos->posX, appearedRecibido->datos->posY,
					appearedRecibido->id_relativo);
			//mutex
			sem_wait(&idsDeMensajes);
			appearedRecibido->id = idGlobales;
			idGlobales++;
			sem_post(&idsDeMensajes);
			//mutex

			raiz = transformarBrokerAppearedPokemon(appearedRecibido,
					&tamanioAgregar);
			//log_info(logMensajeNuevo,"lo que vale este appeared a agregar es %d",sizeof(raiz));
			agregarAMemoria(raiz, appearedRecibido->id, APPEARED_POKEMON,
					appearedRecibido->id_relativo, tamanioAgregar);
			//agregarACola(APPEARED_POKEMON,appearedRecibido);

			//free(raiz);

			free(appearedRecibido);

			break;

		case BROKER__GET_POKEMON:


			getRecibido = deserializar_get_pokemon(cliente_fd);

			log_info(logMensajeNuevo,
					"recibi mensaje de GET_POKEMON de %s\n con tamanio: %d \n nombre: %s ",
					username, getRecibido->datos->tamanioNombre,
					getRecibido->datos->nombrePokemon);

			//mutex
			sem_wait(&idsDeMensajes);
			getRecibido->id = idGlobales;
			idGlobales++;
			sem_post(&idsDeMensajes);
			//mutex

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

			break;

		case BROKER__CATCH_POKEMON:



			catchRecibido = deserializar_catch_pokemon(cliente_fd);

			log_info(logMensajeNuevo,
					"recibi mensaje de CATCH_POKEMON de %s\n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d ",
					username, catchRecibido->datos->tamanioNombre,
					catchRecibido->datos->nombrePokemon, catchRecibido->datos->posX,
					catchRecibido->datos->posY);

			//mutex
			sem_wait(&idsDeMensajes);
			catchRecibido->id = idGlobales;
			idGlobales++;
			sem_post(&idsDeMensajes);
			//mutex

			if (!strcmp(username, "TEAM")) {
				//TODO ver conexiones con los otros sockets (si les tengo que mandar ID
				send(cliente_fd, &(catchRecibido->id), sizeof(uint32_t), 0);
			}

			raiz = transformarBrokerCatchPokemon(catchRecibido,
					&tamanioAgregar);
			//log_info(logMensajeNuevo,"lo que vale este catch a agregar es %d",sizeof(raiz));
			agregarAMemoria(raiz, catchRecibido->id, CATCH_POKEMON, 0,
					tamanioAgregar);
			//agregarACola(CATCH_POKEMON,catchRecibido);


			//free(raiz);
			free(catchRecibido);
			break;

		case BROKER__CAUGHT_POKEMON:



			caughtRecibido = deserializar_caught_pokemon(cliente_fd);

			log_info(logMensajeNuevo,
					"recibi mensaje de CAUGHT_POKEMON de %s\n con ID_relativo: %d \n puedoAtraparlo: %d ",
					username, caughtRecibido->id_relativo,
					caughtRecibido->datos->puedoAtraparlo);
			//mutex
			sem_wait(&idsDeMensajes);
			caughtRecibido->id = idGlobales;
			idGlobales++;
			sem_post(&idsDeMensajes);
			//mutex

			raiz = transformarBrokerCaughtPokemon(caughtRecibido,
					&tamanioAgregar);
			//log_info(logMensajeNuevo,"lo que vale este caught a agregar es %d",sizeof(raiz));
			agregarAMemoria(raiz, caughtRecibido->id, CAUGHT_POKEMON,
					caughtRecibido->id_relativo, tamanioAgregar);
			//agregarACola(CAUGHT_POKEMON,caughtRecibido);

			//free(raiz);
			free(caughtRecibido);
			break;

		case BROKER__LOCALIZED_POKEMON:


			localizedRecibido = deserializar_localized_pokemon(cliente_fd);


			 posiciones = 0;
			 for(posiciones=0;posiciones<localizedRecibido->datos->cantidadPosiciones;posiciones++){
				 char* posicion = string_from_format("(%d;%d)",localizedRecibido->datos->posX[posiciones],localizedRecibido->datos->posY[posiciones]);
				 string_append(&posicionesString,posicion);
			 }

			 log_info(logMensajeNuevo,"recibi mensaje de LOCALIZED_POKEMON de %s\n con tamanio: %d\n nombre: %s\n cantidadPosiciones: %d\n y Posiciones(x,y): %s\n con ID_relativo: %d \n "
						 ,username,localizedRecibido->datos->tamanioNombre, localizedRecibido->datos->nombrePokemon,localizedRecibido->datos->cantidadPosiciones,posicionesString,
						 localizedRecibido->id_relativo);

			 /*log_info(logMensajeNuevo,"recibi mensaje de LOCALIZED_POKEMON de %s\n con tamanio: %d\n nombre: %s\n cantidadPosiciones: %d\n con ID_relativo: %d \n "
					 ,username,localizedRecibido->datos->tamanioNombre, localizedRecibido->datos->nombrePokemon,localizedRecibido->datos->cantidadPosiciones,
					 localizedRecibido->id_relativo);*/

			//mutex
			sem_wait(&idsDeMensajes);
			localizedRecibido->id = idGlobales;
			idGlobales++;
			sem_post(&idsDeMensajes);
			//mutex

			raiz = transformarBrokerLocalizedPokemon(localizedRecibido,
					&tamanioAgregar);
			//log_info(logMensajeNuevo,"lo que vale este caught a agregar es %d",sizeof(raiz));
			agregarAMemoria(raiz, localizedRecibido->id, LOCALIZED_POKEMON,
					localizedRecibido->id_relativo, tamanioAgregar);
			//agregarACola(CAUGHT_POKEMON,caughtRecibido);

			//free(raiz);
			free(localizedRecibido);

			break;

		case ACKNOWLEDGED:



			ackRecibido = deserializarAck(cliente_fd);
			log_info(confirmacionRecepcion,
					"me llego la confirmacion para el ID:%d", ackRecibido);

			bool partAck(particion *part) {
				if ((!part->libre) && (ackRecibido == part->idMensaje)) {
					return 1;
				}
				return 0;
			}

				uint32_t ackRecibido = deserializarAck(cliente_fd);

			partEncontrada = list_find(tablaDeParticiones, partAck);

			list_add(partEncontrada->acknoleged, username);

			//mutex porque manejo algo de memoria
			sem_wait(&usoMemoria);

			//busco si hay una particion con ese ID
			particion* partEncontrada=list_find(tablaDeParticiones,partAck);

			//si la encontro lo agrega a su tabla de ACK
			if(partEncontrada!=NULL){
				list_add(partEncontrada->acknoleged,username);
				log_info(confirmacionRecepcion,"ACK de %s ID:%d para la cola %s",username,ackRecibido,colasDeEnum[partEncontrada->tipoMensaje]);
			}

			sem_post(&usoMemoria);


			break;
		case DESUSCRIBIR:

			suscriptor = deserializar_suscripcion(cliente_fd);

/*
			log_info(logSuscipcion,
					"recibi mensaje de desuscripcion de %s a la cola %s",
					suscriptor->nombreDeSuscriptor,
					colasDeEnum[(suscriptor->tipoDeCola) - 1]);*/

			sem_wait(&suscripcionACola);
			desuscribirACola(suscriptor);
			sem_post(&suscripcionACola);

			free(suscriptor);

			break;

			break;
		case 0:
			pthread_exit(NULL);
			break;
		case -1:
			pthread_exit(NULL);
			break;


		free(username);
		//free(posXString);
		//free(posYString);
		//free(posicionesString);

	}
	sem_post(&llegadaMensajes);
}

void* recibir_mensaje(int socket_cliente, int* size) {
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void suscribirACola(suscriptor* suscriptor){
	switch(suscriptor->tipoDeCola){
		case NEW_POKEMON:
			list_add(suscriptoresNewPokemon,(char*) suscriptor->nombreDeSuscriptor);
			break;
		case APPEARED_POKEMON:
			list_add(suscriptoresAppearedPokemon,(char*) suscriptor->nombreDeSuscriptor);
			break;
		case CATCH_POKEMON:
			list_add(suscriptoresCatchPokemon,(char*) suscriptor->nombreDeSuscriptor);
			break;
		case CAUGHT_POKEMON:
			list_add(suscriptoresCaughtPokemon,(char*) suscriptor->nombreDeSuscriptor);
			break;
		case GET_POKEMON:
			list_add(suscriptoresGetPokemon,(char*) suscriptor->nombreDeSuscriptor);
			break;
		case LOCALIZED_POKEMON:
			list_add(suscriptoresLocalizedPokemon,(char*) suscriptor->nombreDeSuscriptor);
			break;

	}
}



void desuscribirACola(suscriptor* suscrip){

	//TODO ver porque no existe el suscriptor
	bool suscriptorAEliminar(suscriptor* susc){
		return (!strcmp(susc->nombreDeSuscriptor, suscrip->nombreDeSuscriptor));
	}

	switch(suscrip->tipoDeCola){
		case NEW_POKEMON:{
			list_remove_and_destroy_by_condition(suscriptoresNewPokemon,suscriptorAEliminar,free);
			break;
		}
		case APPEARED_POKEMON:{
			list_remove_and_destroy_by_condition(suscriptoresAppearedPokemon,suscriptorAEliminar,free);
			break;
		}
		case CATCH_POKEMON:{

			list_remove_and_destroy_by_condition(suscriptoresCatchPokemon,suscriptorAEliminar,free);
			break;
		}
		case CAUGHT_POKEMON:{
			list_remove_and_destroy_by_condition(suscriptoresCaughtPokemon,suscriptorAEliminar,free);
			break;
		}
		case GET_POKEMON:{
			list_remove_and_destroy_by_condition(suscriptoresGetPokemon,suscriptorAEliminar,free);
			break;
		}
		case LOCALIZED_POKEMON:{
			list_remove_and_destroy_by_condition(suscriptoresLocalizedPokemon,suscriptorAEliminar,free);
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
		particion *particionEncontrada = malloc(sizeof(particion));	//Para el algoritmo FF/BF
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

	list_sort(tablaDeParticiones, baseMasChica);

	t_list* tablaParticionesLibres = list_filter(tablaDeParticiones,particionLibreQueEntre);
	particionChica = list_get(tablaParticionesLibres, 0);

	list_iterate(tablaParticionesLibres, particionMasChica);

	if (particionChica != NULL) {
		list_find(tablaDeParticiones, partSiguienteALibre);
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

		producirUnMensaje(datoAAgregar->tipoMensaje);
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
}

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

	t_list *tablaParticionesLlenas = list_filter(tablaDeParticiones, partLlenas);
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
	list_iterate(tablaParticionesLlenas, partMasVieja);

	//ordenamos todas las pariciones por sus bases
	list_sort(tablaDeParticiones, baseMasChica);

	particion *partSig = list_find(tablaDeParticiones, partSiguienteALibre);

	t_list *particionesAnteriores = list_filter(tablaDeParticiones,particionesAnt);

	list_sort(particionesAnteriores, baseMasChica);

	list_iterate(particionesAnteriores, particionAnterior);

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
		partAEliminar = list_remove_by_condition(tablaDeParticiones,particionConEsaBase);

		//consolidacion
		if (partSig != NULL) {
			if (partSig->libre) {
				partNueva->tamanioMensaje += partSig->tamanioMensaje;
				//eliminar partSig de la Tabla de Particiones
				baseAEliminar = partSig->base;
				list_remove_and_destroy_by_condition(tablaDeParticiones,particionConEsaBase, free);
			}
		}
		if (partAnt != NULL) {
			if (partAnt->libre) {
				//eliminar partAnt de la Tabla de Particiones
				baseAEliminar = partAnt->base;
				partAnt = list_remove_by_condition(tablaDeParticiones,particionConEsaBase);
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

	list_sort(tablaDeParticiones, baseMasChica);
	particionEncontrada = list_find(tablaDeParticiones, primeroLibreQueEntre);

	if (particionEncontrada) {
		list_find(tablaDeParticiones, partSiguienteALibre);
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
		if(list_count_satisfying(tablaDeParticiones, partLibres) != 0){
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

			t_list* partOcupadas = list_filter(tablaDeParticiones, partLlenas);

			list_sort(tablaDeParticiones, baseMasChica);
			//muestra como quedo la tabla de particiones  (COMENTAR)
			/*
			 log_info(compactacionMemoria,"antes de compactar la memoria esta asi:");
			 list_iterate(tablaDeParticiones,mostrarParticiones);*/

			//ordenamos las bases ocupadas
			list_sort(partOcupadas, baseMasChica);
			//cambiamos las bases ocupadas para que esten todas juntas
			list_iterate(partOcupadas, cambiarBases);

			//destruir y liberar todas las particiones libres
			cantLibres = list_count_satisfying(tablaDeParticiones, partLibres);

			while (cantLibres > 0) {
				list_remove_and_destroy_by_condition(tablaDeParticiones,partLibres, free);
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

/*
void iniciarBuddySystem() {
	root = malloc(sizeof(buddy));
	root->size = tamanio_memoria;
	root->freeSpace = root->size;
	root->padre = NULL;
	root->hijoIzq = NULL;
	root->hijoDer = NULL;
	root->next = NULL;
	root->back = NULL;
	root->ocupado = false;

	buddy* freeList[cantidadNiveles(root)];

	colaVictimaBuddy = list_create();

	alocarMemoria(7, freeList, cantidadNiveles(root));
}

void alocarMemoria(int tamNuevoMensaje, buddy* freeList[], cantNivelesTotales) {
	int nivelMensaje = nivelParticion(tamNuevoMensaje);

	if (freeList[nivelMensaje] != NULL) { //encontre el buddy
		buddyAAlocar = freeList[nivelMensaje];
		buddyAAlocar->ocupado = true;
		buddyAAlocar->freeSpace = buddyAAlocar->size - tamNuevoMensaje;

		//Saco el buddy de la freeList
		if (freeList[nivelMensaje]->next != NULL) {
			freeList[nivelMensaje] = freeList[nivelMensaje]->next;
		} else {
			freeList[nivelMensaje] = NULL;
		}

		// Lo agrego a la estructura correspondiente de seleccionDeVictima TODO

	} else {
		// voy a tener que splitear la memoria y volver a buscar el nodo libre
		int aux = nivelMensaje + 1;
		bool spliteado = false;

		while (spliteado != true && aux <= cantNivelesTotales) {
			if (freeList[aux] != NULL) {
				splitMemory(freeList[aux]);

				//lo saco de la freeList
				if (freeList[aux]->next != NULL) {
					freeList[aux] = freeList[aux]->next;
				} else {
					freeList[aux] = NULL;
				}

				spliteado = true;
				alocarMemoria(tamNuevoMensaje, freeList, cantNivelesTotales);
			}
		}

		if (spliteado == false) {
			searchAndDestroy(); //busco una victima y la hago boleta. Veo si puedo mergear.
		}

	}
}

void seleccionarVictimaFIFO() {
	buddy* buddyVictima = list_get(colaVictimaBuddy, 0);

	//Si tiene hijos los hago boleta
	if (tieneHijos(buddyVictima)) {
		matarHijos(buddyVictima); //Free cada hijo
	}

	//Aniquilo a la victima
	eliminarBuddy(buddyVictima);

	//Mergeo en lo posible
	mergeTree(); //TODO
}

void mergeTree() {
	//TODO
}

bool tieneHijos(buddy* unBuddy) {
	return (unBuddy->hijoIzq != NULL && unBuddy->hijoDer != NULL);
}

void matarHijos(buddy* unBuddy) {
	if (!tieneHijos(unBuddy)) {
		eliminarBuddy(unBuddy);
	} else if (unBuddy->hijoIzq != NULL) {
		matarHijos(unBuddy->hijoIzq);
	} else if (unBuddy->hijoDer != NULL) {
		matarHijos(unBuddy->hijoDer);
	}
}

void eliminarBuddy(buddy* unBuddy) {
	free(unBuddy);
}
/*
void splitMemory(buddy* unBuddy) {
	buddy *nuevoIzq = malloc(sizeof(buddy));
	buddy *nuevoDer = malloc(sizeof(buddy));

	nuevoIzq->parent = unBuddy;
	nuevoDer->parent = unBuddy;

	nuevoIzq->size = unBuddy->size / 2;
	nuevoDer->size = unBuddy->size / 2;

	nuevoIzq->freeSpace = unBuddy->size / 2;
	nuevoDer->freeSpace = unBuddy->size / 2;

	nuevoIzq->ocupado = false;
	nuevoDer->ocupado = false;

	nuevoIzq->left = NULL;
	nuevoIzq->right = NULL;

	nuevoDer->left = NULL;
	nuevoDer->right = NULL;

	//asigno los hijos al buddy
	unBuddy->left = nuevoIzq;
	unBuddy->right = nuevoDer;
	unBuddy->ocupado = true;

	//agrego los hijos a la freeList
	int nivelNuevosBuddys = nivelParticion(nuevoIzq->size);

	if (freeList[nivelNuevosBuddys] != NULL) {
		while (freeList[nivelNuevosBuddys]->next != NULL) {
			freeList[nivelNuevosBuddys] = freeList[nivelNuevosBuddys]->next;
		}

		freeList[nivelNuevosBuddys]->next = nuevoIzq;
		nuevoIzq->back = freeList[nivelNuevosBuddys];
		nuevoIzq->next = nuevoDer;
		nuevoDer->back = nuevoIzq;
		nuevoDer->next = NULL;
	} else {
		freeList[nivelNuevosBuddys] = nuevoIzq;
		nuevoIzq->back = freeList[nivelNuevosBuddys];
		nuevoIzq->next = nuevoDer;
		nuevoDer->back = nuevoIzq;
		nuevoDer->next = NULL;
	}
}

int cantidadNiveles(buddy unBuddy) {
	int valorPotencia = 1;
	int level = 0;
	while (valorPotencia < unBuddy->size) {
		valorPotencia = valorPotencia * 2;
		level++;
	}
	return level;
}

int nivelParticion(int tamNuevoMensaje) {
	int valorPotencia = 1;
	int level = 0;
	while (valorPotencia < tamNuevoMensaje) {
		valorPotencia = valorPotencia * 2;
		level++;
	}
	return level;
}*/

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

	t_list* buddiesLibres = list_filter(buddiesTotales,estaLibre);

	t_list* buddiesQueMeSirven = list_filter(buddiesLibres,mayorOIgualAtamanioIdeal);

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

	t_list* listaLibres = list_filter(tablaDeParticiones,estaLibre);

	t_list* listaTamaniosLibres = list_map(tablaDeParticiones,tamanioBuddy);

	bool mayorOigualQueTamanioBuscado(int tamanioParaComparar) {
		return tamanioParaComparar >= tamanio;
	}

	return list_any_satisfy(listaTamaniosLibres, mayorOigualQueTamanioBuscado);

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

	buddyQueSustituye->particion->libre = 0;

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
		list_add(posiblesTamanios,tamanioInicial);
		tamanioInicial = tamanioInicial / 2;
	}

	list_add(posiblesTamanios,tamanioInicial);

	return posiblesTamanios;

}

uint32_t tamanioIdealBuddy(uint32_t tamanio) {

	t_list* tamanioBuddies = listaTamanioBuddiesPosibles();

	bool buddyEsMasGrandeTamanioMensaje(uint32_t tamanioAcomparar){
		 return tamanioAcomparar >= tamanio;
	}

	t_list* dondeEntra = list_filter(tamanioBuddies,buddyEsMasGrandeTamanioMensaje);

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

	t_list* listaBases = list_map(tablaDeParticiones,base);

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
		return numero%2 == 0;
	}

	if(posicionPAr(posicionBuddy)){
		consolidarSiEsPosibleADerecha(unBuddy); //creo nuevo buddy , ver bases y limites, usar remove y replace
	} else {
		consolidarSiEsPosibleAIzquierda(unBuddy); //creo nuevo buddy, ves bases y limites, usar remove y replace
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

	t_list *tablaParticionesLlenas = list_filter(tablaDeParticiones,buddyLleno);

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

		list_iterate(tablaParticionesLlenas, partMasVieja);


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

	consolidarSiEsPosible(nuevoBuddy);

}
