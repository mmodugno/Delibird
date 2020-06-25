/*
 * utils_broker.c
 *l
 *  Created on: 14 may. 2020
 *      Author: utnso
 */

#include "utils_broker.h"

void mostrarParticiones(particion* unaParticion){
	if(unaParticion != NULL){
		//este no funciona
		//log_info(almacenadoMemoria,"El dato(ID:%d) %s con base: %d, tamanio: %d y tiempo: %s",unaParticion->idMensaje,colasDeEnum[unaParticion->tipoMensaje],unaParticion->base,unaParticion->tamanioMensaje,unaParticion->timestamp);
		//este me parece que si y quizas es mas descriptivo
		log_info(almacenadoMemoria,"base: %d, tamanio: %d, id: %d, tipo: %s", unaParticion->base, unaParticion->tamanioMensaje, unaParticion->idMensaje,colasDeEnum[unaParticion->tipoMensaje]);
		//printf("base: %d, tamanio: %d, id: %d", unaParticion->base, unaParticion->tamanioMensaje, unaParticion->idMensaje);
	}
}

void iniciarMemoria(){

	memoria = malloc(tamanio_memoria);

	tablaDeParticiones = list_create();

	particion *partLibreInicial= malloc(sizeof(particion));
	//los que tienen idMensaje 0 son particiciones vacias
	partLibreInicial->idMensaje = 0;
	partLibreInicial->base = 0;
	partLibreInicial->tamanioMensaje = tamanio_memoria;
	partLibreInicial->libre = 1;

	list_add(tablaDeParticiones,partLibreInicial);

	idGlobales = 1;

}


uint32_t deserializarAck(int socket_cliente){
		//uint32_t idAck;

	    uint32_t idAck;
	    recv(socket_cliente,&idAck,sizeof(uint32_t),0);
	    return idAck;
}


void iniciar_servidor(void){
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip_broker, puerto_broker, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next) {
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

void esperar_cliente(int socket_servidor) {
	struct sockaddr_in dir_cliente;

	socklen_t  tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);



	pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
	pthread_detach(thread);

}

void serve_client(int* socket){
	int cod_op;
	int i = recv(*socket, &cod_op, sizeof(op_code), MSG_WAITALL);
	if(i <= 0)
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
	SUSCRIPCION = 11
*/

void process_request(int cod_op, int cliente_fd) {
	uint32_t tamanio_buffer;
	uint32_t tamanio_username;
	char* username;

	recv(cliente_fd,&tamanio_username,sizeof(uint32_t),MSG_WAITALL);
	username = malloc(tamanio_username);
	recv(cliente_fd, username,tamanio_username,MSG_WAITALL);
	recv(cliente_fd, &tamanio_buffer, sizeof(uint32_t), MSG_WAITALL);

	//log_info(logConexion,"%s se conecto al broker",username);
	//falta los case de los otros tipos de mensajes (get,catch,caught)(localized lo dejamos para despues(es de GameCard)
	switch (cod_op) {
		case SUSCRIPCION:{
			suscriptor* suscriptor;
			suscriptor = deserializar_suscripcion(cliente_fd);

			log_info(logSuscipcion,"recibi mensaje de suscripcion de %s a la cola %s",suscriptor->nombreDeSuscriptor,colasDeEnum[(suscriptor->tipoDeCola)-1]);

			suscribirACola(suscriptor);
			free(suscriptor);

			break;
		}
		case BROKER__NEW_POKEMON:{
			broker_new_pokemon* newRecibido;
			newRecibido = deserializar_new_pokemon(cliente_fd);
			log_info(logMensajeNuevo,"recibi mensaje de NEW_POKEMON de %s \n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d \n cantidad de pokemones: %d"
								,username,newRecibido->datos->tamanioNombre,
								newRecibido->datos->nombrePokemon,
								newRecibido->datos->posX,
								newRecibido->datos->posY,
								newRecibido->datos->cantidadPokemon);
			//mutex
			sem_wait(&idsDeMensajes);
			newRecibido->id = idGlobales;
			idGlobales++;
			sem_post(&idsDeMensajes);
			//mutex


			// Inicializamos la cola de suscriptores ack para que se pueda agregar
			newRecibido->suscriptoresQueRespondieron = queue_create();


			new_pokemon *raiz = transformarBrokerNewPokemon(newRecibido);
			//log_info(logMensajeNuevo,"lo que vale este new a agregar es %d",sizeof(raiz));
			//log_info(logMensajeNuevo,"lo que vale este new puntero de dato es %d",sizeof(new_pokemon*));
			agregarAMemoria(raiz,newRecibido->id,NEW_POKEMON);
			//agregarACola(NEW_POKEMON,newRecibido);

			free(raiz);
			free(newRecibido);

			break;
		}
		case BROKER__APPEARED_POKEMON:{
			broker_appeared_pokemon* appearedRecibido;
			appearedRecibido = deserializar_appeared_pokemon(cliente_fd);

			log_info(logMensajeNuevo,"recibi mensaje de APPEARED_POKEMON de %s \n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d \n con id_relativo: %d"
					,username,appearedRecibido->datos->tamanioNombre,
					appearedRecibido->datos->nombrePokemon,
					appearedRecibido->datos->posX,
					appearedRecibido->datos->posY,appearedRecibido->id_relativo);
			//mutex
			sem_wait(&idsDeMensajes);
			appearedRecibido->id = idGlobales;
			idGlobales++;
			sem_post(&idsDeMensajes);
			//mutex

			// Inicializamos la cola de suscriptores ack para que se pueda agregar
			appearedRecibido->suscriptoresQueRespondieron = queue_create();

			appeared_pokemon *raiz = transformarBrokerAppearedPokemon(appearedRecibido);
			//log_info(logMensajeNuevo,"lo que vale este appeared a agregar es %d",sizeof(raiz));
			agregarAMemoria(raiz,appearedRecibido->id,APPEARED_POKEMON);
			//agregarACola(APPEARED_POKEMON,appearedRecibido);

			free(raiz);
			free(appearedRecibido);

			break;
		}
		case BROKER__GET_POKEMON:{
			broker_get_pokemon* getRecibido;
			getRecibido = deserializar_get_pokemon(cliente_fd);

			log_info(logMensajeNuevo,"recibi mensaje de GET_POKEMON%s\n con tamanio: %d \n nombre: %s "
									,username,getRecibido->datos->tamanioNombre,
									getRecibido->datos->nombrePokemon);

			//mutex
			sem_wait(&idsDeMensajes);
			getRecibido->id = idGlobales;
			idGlobales++;
			sem_post(&idsDeMensajes);
			//mutex

			// Inicializamos la cola de suscriptores ack para que se pueda agregar
			getRecibido->suscriptoresQueRespondieron = queue_create();

			get_pokemon *raiz = transformarBrokerGetPokemon(getRecibido);
			//log_info(logMensajeNuevo,"lo que vale este get a agregar es %d",sizeof(raiz));
			agregarAMemoria(raiz,getRecibido->id,GET_POKEMON);
			//agregarACola(GET_POKEMON,getRecibido);

			free(raiz);
			free(getRecibido);

			break;
		}
		case BROKER__CATCH_POKEMON:{
			broker_catch_pokemon* catchRecibido;
			catchRecibido = deserializar_catch_pokemon(cliente_fd);

			log_info(logMensajeNuevo,"recibi mensaje de CATCH_POKEMON de %s\n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d "
									,username,catchRecibido->datos->tamanioNombre,
									catchRecibido->datos->nombrePokemon,
									catchRecibido->datos->posX,
									catchRecibido->datos->posY);

			//mutex
			sem_wait(&idsDeMensajes);
			catchRecibido->id = idGlobales;
			idGlobales++;
			sem_post(&idsDeMensajes);
			//mutex

			if(!strcmp(username,"TEAM")){
				//TODO ver conexiones con los otros sockets
				send(cliente_fd,&(catchRecibido->id),sizeof(uint32_t),0);
			}

			// Inicializamos la cola de suscriptores ack para que se pueda agregar
			catchRecibido->suscriptoresQueRespondieron = queue_create();

			catch_pokemon *raiz = transformarBrokerCatchPokemon(catchRecibido);
			//log_info(logMensajeNuevo,"lo que vale este catch a agregar es %d",sizeof(raiz));
			agregarAMemoria(raiz, catchRecibido->id,CATCH_POKEMON);
			//agregarACola(CATCH_POKEMON,catchRecibido);

			free(raiz);
			free(catchRecibido);

			break;
		}
		case BROKER__CAUGHT_POKEMON:{
			broker_caught_pokemon* caughtRecibido;
			caughtRecibido = deserializar_caught_pokemon(cliente_fd);

			log_info(logMensajeNuevo,"recibi mensaje de CAUGHT_POKEMON de %s\n con ID_relativo: %d \n puedoAtraparlo: %d "
									,username,caughtRecibido->id_relativo,
									caughtRecibido->datos->puedoAtraparlo);
			//mutex
			sem_wait(&idsDeMensajes);
			caughtRecibido->id = idGlobales;
			idGlobales++;
			sem_post(&idsDeMensajes);
			//mutex

			// Inicializamos la cola de suscriptores ack para que se pueda agregar
			caughtRecibido->suscriptoresQueRespondieron = queue_create();


			caught_pokemon *raiz = transformarBrokerCaughtPokemon(caughtRecibido);
			//log_info(logMensajeNuevo,"lo que vale este caught a agregar es %d",sizeof(raiz));
			agregarAMemoria(raiz,caughtRecibido->id,CAUGHT_POKEMON);
			//agregarACola(CAUGHT_POKEMON,caughtRecibido);

			free(raiz);
			free(caughtRecibido);

			break;
		}
		case ACKNOWLEDGED:{
			uint32_t ackRecibido = deserializarAck(cliente_fd);

			bool buscarIdNew(broker_new_pokemon* brokerNP){
				return (brokerNP->id) == ackRecibido;
			}
			bool buscarIdAppeared(broker_appeared_pokemon* brokerAP){
				return (brokerAP->id) == ackRecibido;
			}
			bool buscarIdCatch(broker_catch_pokemon* brokerCTP){
				return (brokerCTP->id) == ackRecibido;
			}
			bool buscarIdCaught(broker_caught_pokemon* brokerCAP){
				return (brokerCAP->id) == ackRecibido;
			}
			bool buscarIdGet(broker_get_pokemon* brokerGP){
				return (brokerGP->id) == ackRecibido;
			}
			bool buscarIdLocalized(broker_localized_pokemon* brokerLP){
				return (brokerLP->id) == ackRecibido;
			}

			broker_new_pokemon* brokerNP = malloc(sizeof(broker_new_pokemon));
			broker_appeared_pokemon* brokerAP = malloc(sizeof(broker_appeared_pokemon));
			broker_catch_pokemon* brokerCTP = malloc(sizeof(broker_catch_pokemon));
			broker_caught_pokemon* brokerCAP = malloc(sizeof(broker_caught_pokemon));
			broker_get_pokemon* brokerGP = malloc(sizeof(broker_get_pokemon));
			broker_localized_pokemon* brokerLP = malloc(sizeof(broker_localized_pokemon));

			brokerNP  = NULL;
			brokerAP  = NULL;
			brokerCTP = NULL;
			brokerCAP = NULL;
			brokerGP  = NULL;
			brokerLP  = NULL;
			/*

			brokerNP  = (broker_new_pokemon*)list_find(colaNewPokemon, buscarIdNew);
			brokerAP  = (broker_appeared_pokemon*)list_find(colaAppearedPokemon, buscarIdAppeared);
			brokerCTP = (broker_catch_pokemon*)list_find(colaCatchPokemon, buscarIdCatch);
			brokerCAP = (broker_caught_pokemon*)list_find(colaCaughtPokemon, buscarIdCaught);
			brokerGP  = (broker_get_pokemon*)list_find(colaGetPokemon, buscarIdGet);
			brokerLP  = (broker_localized_pokemon*)list_find(colaLocalizedPokemon, buscarIdLocalized);*/

			if(brokerNP != NULL){
				queue_push(brokerNP->suscriptoresQueRespondieron, username);
			}
			if(brokerAP != NULL){
				queue_push(brokerAP->suscriptoresQueRespondieron, username);
			}
			if(brokerCTP != NULL){
				queue_push(brokerCTP->suscriptoresQueRespondieron, username);
			}
			if(brokerCAP != NULL){
				queue_push(brokerCAP->suscriptoresQueRespondieron, username);
			}
			if(brokerGP != NULL){
				queue_push(brokerGP->suscriptoresQueRespondieron, username);
			}
			if(brokerLP != NULL){
				queue_push(brokerLP->suscriptoresQueRespondieron, username);
			}


			break;
		}
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
		}


}




void* recibir_mensaje(int socket_cliente, int* size) {
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

//por ahora usamos tipos de dato de broker que van a tener el id_relativo, el mensaje, su id y la cola de suscriptores que les llego el mensaje
/*void agregarACola(tipoDeCola tipo_de_Cola, void* mensaje){
	switch(tipo_de_Cola){
		case NEW_POKEMON:
			list_add(colaNewPokemon,(broker_new_pokemon*)mensaje);
			break;
		case APPEARED_POKEMON:
			list_add(colaAppearedPokemon,(broker_appeared_pokemon*)mensaje);
			break;
		case CATCH_POKEMON:
			list_add(colaCatchPokemon,(broker_catch_pokemon*)mensaje);
			break;
		case CAUGHT_POKEMON:
			list_add(colaCaughtPokemon,(broker_caught_pokemon*)mensaje);
			break;
		case GET_POKEMON:
			list_add(colaGetPokemon,(broker_get_pokemon*)mensaje);
			break;
		case LOCALIZED_POKEMON:
			//queue_push(colaLocalizedPokemon,(broker_localize_pokemon*) mensaje);
			list_add(colaLocalizedPokemon,(broker_localized_pokemon*) mensaje);
			break;

	}
}*/

void suscribirACola(suscriptor* suscriptor){
	switch(suscriptor->tipoDeCola){
		case NEW_POKEMON:
			queue_push(suscriptoresNewPokemon,(char*) suscriptor->nombreDeSuscriptor);
			break;
		case APPEARED_POKEMON:
			queue_push(suscriptoresAppearedPokemon,(char*) suscriptor->nombreDeSuscriptor);
			break;
		case CATCH_POKEMON:
			queue_push(suscriptoresCatchPokemon,(char*) suscriptor->nombreDeSuscriptor);
			break;
		case CAUGHT_POKEMON:
			queue_push(suscriptoresCaughtPokemon,(char*) suscriptor->nombreDeSuscriptor);
			break;
		case GET_POKEMON:
			queue_push(suscriptoresGetPokemon,(char*) suscriptor->nombreDeSuscriptor);
			break;
		case LOCALIZED_POKEMON:
			queue_push(suscriptoresLocalizedPokemon,(char*) suscriptor->nombreDeSuscriptor);
			break;


	}
}

void agregarAMemoria(void * dato, uint32_t idMensaje,tipoDeCola tipoMensaje){
	particion *datoAAgregar = crearEntradaParticionBasica(dato,idMensaje,tipoMensaje);
	//uint32_t desplazamiento = 0;
	particion *particionEncontrada = malloc(sizeof(particion));	//Para el algoritmo FF/BF
	//particion* particionMasChica = malloc(sizeof(particion));	//Para el algoritmo BF

	if(!strcmp(algoritmo_particion_libre,"FF")){
		algoritmoFirstFit(datoAAgregar,particionEncontrada);
	}
	if(!strcmp(algoritmo_particion_libre,"BF")){
		algoritmoBestFit(datoAAgregar,particionEncontrada);
	}

}



particion* crearEntradaParticionBasica(void * dato, uint32_t idMensaje,tipoDeCola tipoMensaje){
	particion *datoAAgregar = malloc(sizeof(particion));
	datoAAgregar->mensaje = malloc(sizeof(dato));
	datoAAgregar->mensaje = dato;
	//agregamos tipos de dato raiz como son new_pokemon, localized_pokemon, etc;
	//TENER EN CUENTA QUE PUEDE CAMBIAR el tamaño si es broker_tipo_dato  PORQUE TIENE LOS SUSCRIPTORES QUE LE MANDARON ACK
	datoAAgregar->tamanioMensaje= sizeof(dato);
	datoAAgregar->acknoleged = list_create();
	datoAAgregar->tipoMensaje = tipoMensaje;
	datoAAgregar->libre = 0;
	datoAAgregar->timestamp= temporal_get_string_time();//solo tira la hora sin la fecha
	datoAAgregar->idMensaje = idMensaje;

	return datoAAgregar;
}

bool baseMasChica(particion *part1,particion* part2){
	return part1->base < part2->base;
}

void algoritmoBestFit(particion *datoAAgregar,particion *particionChica){

	uint32_t baseSig = -1;

	bool particionLibreQueEntre(particion *partic){
		return (partic->libre && (partic->tamanioMensaje >= datoAAgregar->tamanioMensaje));
	}

	void particionMasChica(particion *partic){
		if(partic->tamanioMensaje<particionChica->tamanioMensaje){
			particionChica = partic;
		}
	}
	bool partSiguienteALibre(particion *partic){
		if(partic->base > particionChica->base){
			baseSig = partic->base;
			return 1;
		}
		return 0;
	}

	list_sort(tablaDeParticiones,baseMasChica);
	t_list* tablaParticionesLibres = list_filter(tablaDeParticiones,particionLibreQueEntre);
	particionChica = list_get(tablaParticionesLibres,0);
	list_iterate(tablaParticionesLibres,particionMasChica);

	list_find(tablaDeParticiones,partSiguienteALibre);

	//si encontro una partcion en toda la memoria
	agregarTablaParticionesYMemoria(datoAAgregar,particionChica,&baseSig);

}

void agregarTablaParticionesYMemoria(particion *datoAAgregar,particion *partElegida,uint32_t* baseSig){
	//si encontro una partcion en toda la memoria
	if(partElegida!=NULL){
		datoAAgregar->base = partElegida->base;
		if(partElegida->tamanioMensaje != datoAAgregar->tamanioMensaje){
			partElegida->base = datoAAgregar->base + datoAAgregar->tamanioMensaje;
			if(*baseSig!=-1){
				partElegida->tamanioMensaje = *baseSig - partElegida->base;
			}
			else{
				partElegida->tamanioMensaje = tamanio_memoria - partElegida->base;
			}
			//agregar a la tabal de particiones
			list_add(tablaDeParticiones,datoAAgregar);
		}
		else{
			//eliminar la particion libre
			//TODO ver si funciona
			copiarDatos(partElegida,datoAAgregar);
		}
		//agregar a memoria Real
		memcpy(memoria+datoAAgregar->base,datoAAgregar->mensaje,datoAAgregar->tamanioMensaje);
	}
	else{
		//cuando se llena la memoria o no hay espacio (por ahora crashea porque no esta hecha la compresion ni el reemplazamiento)
		//free(particionEncontrada);
		//si no hay ninguna particion en donde entre
		//comprimir (ver contador de frecuencia con mutex)

		//volver a ver si entra el dato

		//reemplazar(quien elimina)
		algoritmoReemplazo(datoAAgregar);

		//comprimir(ver contador de frecuencia con mutex)(CREO)

		//volver a ver si entra

	}

	log_info(almacenadoMemoria,"El dato(ID:%d) %s con base: %d, tamanio: %d y tiempo: %s",datoAAgregar->idMensaje,colasDeEnum[datoAAgregar->tipoMensaje],datoAAgregar->base,datoAAgregar->tamanioMensaje,datoAAgregar->timestamp);
	//para ver como esta la memoria
	//list_iterate(tablaDeParticiones, mostrarParticiones);
}

void algoritmoReemplazo(particion *datoAAgregar){
	if(!strcmp(algoritmo_reemplazo,"FIFO")){

	}
	if(!strcmp(algoritmo_reemplazo,"LRU")){

	}
}


void algoritmoFirstFit(particion *datoAAgregar,particion *particionEncontrada){

	//list_iterate(tablaDeParticiones, mostrarParticiones);
	uint32_t baseSig = -1;

	bool primeroLibreQueEntre(particion *partic){
		return (partic->libre && (partic->tamanioMensaje >= datoAAgregar->tamanioMensaje));
	}

	bool partSiguienteALibre(particion *partic){
		if(partic->base > particionEncontrada->base){
			baseSig = partic->base;
			return 1;
		}
		return 0;
	}

	bool partEncontradaEliminar(particion *partic){
		return (partic == particionEncontrada);
	}

	list_sort(tablaDeParticiones,baseMasChica);
	particionEncontrada = list_find(tablaDeParticiones,primeroLibreQueEntre);
	list_find(tablaDeParticiones,partSiguienteALibre);


	//si encontro una partcion en toda la memoria
	agregarTablaParticionesYMemoria(datoAAgregar,particionEncontrada,&baseSig);

}


void copiarDatos(particion *target,particion * copiado){
	target->acknoleged =copiado->acknoleged;
	target->base = copiado->base;
	target->idMensaje =copiado->idMensaje;
	target->libre=copiado->libre;
	target->mensaje=copiado->mensaje;
	target->tamanioMensaje=copiado->tamanioMensaje;
	target->timestamp=copiado->timestamp;
	target->tipoMensaje = copiado->tipoMensaje;
}
