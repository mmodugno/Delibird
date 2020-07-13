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
		log_info(almacenadoMemoria,"base: %d, tamanio: %d, id: %d, tipo %s, libre?: %d con tiempo: %s ", unaParticion->base, unaParticion->tamanioMensaje, unaParticion->idMensaje, colasDeEnum[unaParticion->tipoMensaje],unaParticion->libre,unaParticion->timestamp);
		//printf("base: %d, tamanio: %d, id: %d", unaParticion->base, unaParticion->tamanioMensaje, unaParticion->idMensaje);
	}
}

void iniciarMemoria(){

	memoria = malloc(tamanio_memoria);
	frecuencia = 0;

	if(!strcmp(algoritmo_memoria,"PARTICIONES")){
		tablaDeParticiones = list_create();

		particion *partLibreInicial= malloc(sizeof(particion));
		//los que tienen idMensaje 0 son particiciones vacias
		partLibreInicial->idMensaje = 0;
		partLibreInicial->tipoMensaje = LIBRE;
		partLibreInicial->base = 0;
		partLibreInicial->tamanioMensaje = tamanio_memoria;
		partLibreInicial->libre = 1;
		partLibreInicial->timestamp = string_new();
		partLibreInicial->acknoleged =list_create();

		list_add(tablaDeParticiones,partLibreInicial);
	}
	if(!strcmp(algoritmo_memoria,"BS")){

	}


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
	int cod_op =0;
	int i= recv(*socket, &cod_op, sizeof(op_code), MSG_WAITALL);
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
	BROKER__LOCALIZED_POKEMON = 13,
	SUSCRIPCION = 11
*/

void process_request(int cod_op, int cliente_fd) {
	uint32_t tamanio_buffer;
	uint32_t tamanio_username;
	char* username;
	uint32_t tamanioAgregar;
	recv(cliente_fd,&tamanio_username,sizeof(uint32_t),MSG_WAITALL);
	username = malloc(tamanio_username);
	recv(cliente_fd, username,tamanio_username,MSG_WAITALL);
	recv(cliente_fd, &tamanio_buffer, sizeof(uint32_t), MSG_WAITALL);
	uint32_t posiciones;
	char* posicionesString = string_new();
	char* posXString= string_new();
	char* posYString= string_new();
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


			void *raiz = transformarBrokerNewPokemon(newRecibido,&tamanioAgregar);
			//log_info(logMensajeNuevo,"lo que vale este new a agregar es %d",sizeof(raiz));
			//log_info(logMensajeNuevo,"lo que vale este new puntero de dato es %d",sizeof(new_pokemon*));
			agregarAMemoria(raiz,newRecibido->id,NEW_POKEMON,0,tamanioAgregar);
			//agregarACola(NEW_POKEMON,newRecibido);

			//free(raiz);
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

			void *raiz = transformarBrokerAppearedPokemon(appearedRecibido,&tamanioAgregar);
			//log_info(logMensajeNuevo,"lo que vale este appeared a agregar es %d",sizeof(raiz));
			agregarAMemoria(raiz,appearedRecibido->id,APPEARED_POKEMON,appearedRecibido->id_relativo,tamanioAgregar);
			//agregarACola(APPEARED_POKEMON,appearedRecibido);

			//free(raiz);
			free(appearedRecibido);


			break;
		}
		case BROKER__GET_POKEMON:{
			broker_get_pokemon* getRecibido;
			getRecibido = deserializar_get_pokemon(cliente_fd);

			log_info(logMensajeNuevo,"recibi mensaje de GET_POKEMON de %s\n con tamanio: %d \n nombre: %s "
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

			void *raiz = transformarBrokerGetPokemon(getRecibido,&tamanioAgregar);
			//log_info(logMensajeNuevo,"lo que vale este get a agregar es %d",sizeof(raiz));
			agregarAMemoria(raiz,getRecibido->id,GET_POKEMON,0,tamanioAgregar);
			//agregarACola(GET_POKEMON,getRecibido);

			//free(raiz);
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
				//TODO ver conexiones con los otros sockets (si les tengo que mandar ID
				send(cliente_fd,&(catchRecibido->id),sizeof(uint32_t),0);
			}

			// Inicializamos la cola de suscriptores ack para que se pueda agregar
			catchRecibido->suscriptoresQueRespondieron = queue_create();

			void *raiz = transformarBrokerCatchPokemon(catchRecibido,&tamanioAgregar);
			//log_info(logMensajeNuevo,"lo que vale este catch a agregar es %d",sizeof(raiz));
			agregarAMemoria(raiz, catchRecibido->id,CATCH_POKEMON,0,tamanioAgregar);
			//agregarACola(CATCH_POKEMON,catchRecibido);

			//free(raiz);
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


			void *raiz = transformarBrokerCaughtPokemon(caughtRecibido,&tamanioAgregar);
			//log_info(logMensajeNuevo,"lo que vale este caught a agregar es %d",sizeof(raiz));
			agregarAMemoria(raiz,caughtRecibido->id,CAUGHT_POKEMON,caughtRecibido->id_relativo,tamanioAgregar);
			//agregarACola(CAUGHT_POKEMON,caughtRecibido);

			//free(raiz);
			free(caughtRecibido);
			break;
		}
		case BROKER__LOCALIZED_POKEMON:{
			broker_localized_pokemon* localizedRecibido;
			localizedRecibido = deserializar_localized_pokemon(cliente_fd);

			posiciones = 0;

			for(posiciones=0;posiciones<localizedRecibido->datos->cantidadPosiciones;posiciones++){
				/*
				string_append(&posXString,string_itoa(localizedRecibido->datos->posX[posiciones]));
				string_append(&posYString,string_itoa(localizedRecibido->datos->posY[posiciones]));*/
				//string_itoa(localizedRecibido->datos->posX[posiciones]);
				sprintf(posXString,"%d",localizedRecibido->datos->posX[posiciones]);
				sprintf(posYString,"%d",localizedRecibido->datos->posY[posiciones]);

				string_append(&posicionesString,"(");
				string_append(&posicionesString,posXString);
				string_append(&posicionesString,";");
				string_append(&posicionesString,posYString);
				string_append(&posicionesString,")");
			}


			log_info(logMensajeNuevo,"recibi mensaje de LOCALIZED_POKEMON de %s\n con tamanio: %d\n nombre: %s\n cantidadPosiciones: %d\n y Posiciones(x,y): %s\n con ID_relativo: %d \n "
									,username,localizedRecibido->datos->tamanioNombre, localizedRecibido->datos->nombrePokemon,localizedRecibido->datos->cantidadPosiciones,posicionesString,
									localizedRecibido->id_relativo);
			//mutex
			sem_wait(&idsDeMensajes);
			localizedRecibido->id = idGlobales;
			idGlobales++;
			sem_post(&idsDeMensajes);
			//mutex

			// Inicializamos la cola de suscriptores ack para que se pueda agregar
			localizedRecibido->suscriptoresQueRespondieron = queue_create();


			void *raiz = transformarBrokerLocalizedPokemon(localizedRecibido,&tamanioAgregar);
			//log_info(logMensajeNuevo,"lo que vale este caught a agregar es %d",sizeof(raiz));
			agregarAMemoria(raiz,localizedRecibido->id,LOCALIZED_POKEMON,localizedRecibido->id_relativo,tamanioAgregar);
			//agregarACola(CAUGHT_POKEMON,caughtRecibido);

			//free(raiz);
			free(localizedRecibido);

			break;
		}
		case ACKNOWLEDGED:{


			uint32_t ackRecibido = deserializarAck(cliente_fd);
			log_info(confirmacionRecepcion,"me llego la confirmacion para el ID:%d",ackRecibido);

			bool partAck(particion *part){
				if((!part->libre) &&(ackRecibido==part->idMensaje)){
					return 1;
				}
				return 0;
			}

			particion* partEncontrada=list_find(tablaDeParticiones,partAck);

			list_add(partEncontrada->acknoleged,username);

			break;
		}
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
		}

	free(username);
	//free(posXString);
	//free(posYString);
	free(posicionesString);

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

void agregarAMemoria(void * dato, uint32_t idMensaje,tipoDeCola tipoMensaje, uint32_t idCorrelativo,uint32_t tamanioAgregar){
	sem_wait(&usoMemoria);
	if(!strcmp(algoritmo_memoria,"PARTICIONES")){

		particion *datoAAgregar = crearEntradaParticionBasica(dato,idMensaje,tipoMensaje,idCorrelativo,tamanioAgregar);
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
	if(!strcmp(algoritmo_memoria,"BS")){

	}
	sem_post(&usoMemoria);
}



particion* crearEntradaParticionBasica(void * dato, uint32_t idMensaje,tipoDeCola tipoMensaje, uint32_t idCorrelativo,uint32_t tamanioAgregar){
	particion *datoAAgregar = malloc(sizeof(particion));
	datoAAgregar->mensaje = malloc(tamanioAgregar);
	datoAAgregar->mensaje = dato;
	//agregamos tipos de dato raiz como son new_pokemon, localized_pokemon, etc;
	datoAAgregar->tamanioMensaje= tamanioAgregar;
	datoAAgregar->acknoleged = list_create();
	datoAAgregar->tipoMensaje = tipoMensaje;
	datoAAgregar->libre = 0;
	datoAAgregar->timestamp= temporal_get_string_time();//solo tira la hora sin la fecha
	datoAAgregar->idMensaje = idMensaje;
	datoAAgregar->idCorrelativo = idCorrelativo;

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


	if(particionChica!=NULL){
		list_find(tablaDeParticiones,partSiguienteALibre);
	}

	//si encontro una partcion en toda la memoria
	agregarTablaParticionesYMemoria(datoAAgregar,particionChica,&baseSig);

	list_destroy(tablaParticionesLibres);

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
			//"eliminar" la particion libre, en realidad modificamos la particion libre por la que vamos a agregar
			copiarDatos(partElegida,datoAAgregar);
		}
		//agregar a memoria Real
		memcpy(memoria+datoAAgregar->base,datoAAgregar->mensaje,datoAAgregar->tamanioMensaje);
		log_info(almacenadoMemoria,"El dato(ID:%d) %s con base: %d, tamanio: %d y tiempo: %s",datoAAgregar->idMensaje,colasDeEnum[datoAAgregar->tipoMensaje],datoAAgregar->base,datoAAgregar->tamanioMensaje,datoAAgregar->timestamp);

		//para ver como esta la memoria (COMENTAR)

		log_info(almacenadoMemoria,"la memoria quedo asi: ");
		list_iterate(tablaDeParticiones,mostrarParticiones);
	}
	else{
		//cuando se llena la memoria o no hay espacio (por ahora crashea porque no esta hecha la compresion ni el reemplazo)
		//free(partElegida);
		//ver los pasos para agregar a memoria porque no estoy seguro
		//si no hay ninguna particion en donde entre



		//reemplazar(quien elimina)
		algoritmoReemplazo();

		//comprimir(ver contador de frecuencia con mutex)
		compactarMemoria();

		//volver a ver si entra
		partElegida = NULL;
		if(!strcmp(algoritmo_particion_libre,"FF")){
			algoritmoFirstFit(datoAAgregar,partElegida);
		}
		if(!strcmp(algoritmo_particion_libre,"BF")){
			algoritmoBestFit(datoAAgregar,partElegida);
		}

	}


	//para ver como esta la memoria
	//list_iterate(tablaDeParticiones, mostrarParticiones);
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

	bool partLlenas(particion* part){
		return !(part->libre);
	}

	t_list *tablaParticionesLlenas= list_filter(tablaDeParticiones,partLlenas);
	particion* partAEliminar = list_get(tablaParticionesLlenas,0);

	bool particionesAnt(particion* part){
		return (part->base < partAEliminar->base);
	}

	void partMasVieja(particion* part){

		timeAct = string_split(part->timestamp,":");
		timeElim = string_split(partAEliminar->timestamp,":");

		horaAct = atoi(timeAct[0]);
		horaElim =atoi(timeElim[0]) ;

		minAct =atoi(timeAct[1]) ;
		minElim = atoi(timeElim[1]);

		segAct = atoi(timeAct[2]);
		segElim = atoi(timeElim[2]);

		milAct = atoi(timeAct[3]);
		milElim = atoi(timeElim[3]);

		if(horaAct<horaElim){
			partAEliminar= part;
		}
		else if(horaAct==horaElim){
			if(minAct<minElim){
				partAEliminar= part;
			}
			else if (minAct==minElim) {
				if (segAct<segElim) {
					partAEliminar =part;
				}
				else if (segAct==segElim) {
					if(milAct<=milElim){
						partAEliminar = part;
					}
				}
			}
		}
	}

	bool partSiguienteALibre(particion *partic){
		return partic->base > partAEliminar->base;
	}

	void particionAnterior(particion * part){
		partAnt = part;
	}

	bool particionConEsaBase(particion * part){
		return (part->base == baseAEliminar);
	}


	//se elije cual es la particion que vamos a eliminar (la mas vieja, ya sea para LRU o FIFO
	list_iterate(tablaParticionesLlenas,partMasVieja);


	//ordenamos todas las pariciones por sus bases
	list_sort(tablaDeParticiones,baseMasChica);

	particion *partSig = list_find(tablaDeParticiones,partSiguienteALibre);

	t_list *particionesAnteriores  = list_filter(tablaDeParticiones,particionesAnt);

	list_sort(particionesAnteriores,baseMasChica);

	list_iterate(particionesAnteriores,particionAnterior);

	if(partAEliminar!=NULL){

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
		if(partSig !=NULL){
			if(partSig->libre){
				partNueva->tamanioMensaje += partSig->tamanioMensaje;
				//eliminar partSig de la Tabla de Particiones
				baseAEliminar = partSig->base;
				list_remove_and_destroy_by_condition(tablaDeParticiones,particionConEsaBase,free);
			}
		}
		if(partAnt !=NULL){
			if(partAnt->libre){
				//eliminar partAnt de la Tabla de Particiones
				baseAEliminar = partAnt->base;
				partAnt = list_remove_by_condition(tablaDeParticiones,particionConEsaBase);
				//sumamos el tamaño de esa particion anterior LIBRE
				partNueva->tamanioMensaje+=partAnt->tamanioMensaje;
				//movemos la Base a esa
				partNueva->base = partAnt->base;
				free(partAnt);
			}
		}
		log_info(eliminacionMemoria,"eliminamos de la memoria el dato(ID:%d) %s con base: %d, tamanio: %d y tiempo: %s",partAEliminar->idMensaje,colasDeEnum[partAEliminar->tipoMensaje],partAEliminar->base,partAEliminar->tamanioMensaje,partAEliminar->timestamp);
		//libero la particion que quiero eliminar
		eliminarParticion(partAEliminar);
		//agrego la nueva particion LIBRE que quedo de la consolidacion
		list_add(tablaDeParticiones,partNueva);

	}


	//PARA VER COMO QUEDA LA MEMORIA DESPUES DE ELIMINAR (COMENTAR)
	/*
	list_sort(tablaDeParticiones,baseMasChica);
	log_info(eliminacionMemoria,"la memoria quedo asi: ");
	list_iterate(tablaDeParticiones,mostrarParticiones);
	log_info(eliminacionMemoria," ");*/

	if(tablaParticionesLlenas->elements_count){
		list_destroy(tablaParticionesLlenas);
	}
	if(tablaParticionesLlenas->elements_count){
		list_destroy(particionesAnteriores);
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
	if(particionEncontrada){
		list_find(tablaDeParticiones,partSiguienteALibre);
	}


	//si encontro una partcion en toda la memoria
	agregarTablaParticionesYMemoria(datoAAgregar,particionEncontrada,&baseSig);

}


void copiarDatos(particion *target,particion * copiado){
	list_add_all(target->acknoleged,copiado->acknoleged);
	target->base = copiado->base;
	target->idMensaje =copiado->idMensaje;
	target->libre=copiado->libre;
	target->mensaje=copiado->mensaje;
	target->tamanioMensaje=copiado->tamanioMensaje;
	target->timestamp=copiado->timestamp;
	target->tipoMensaje = copiado->tipoMensaje;
}

void compactarMemoria(){

	bool partLlenas(particion* part){
		return !(part->libre);
	}
	bool partLibres(particion* part){
		return (part->libre);
	}
	//nos fijamos la frecuencia
	if(frecuencia == frecuencia_compactacion){
		frecuencia = 0;
		if(list_count_satisfying(tablaDeParticiones,partLibres) != 0){
			uint32_t baseAOcupar = 0 ;
			uint32_t cantLibres = 0;

			void cambiarBases(particion * part){
				//cambia todas las bases para que esten juntas(solo en la tabla de particiones)
				part->base= baseAOcupar;
				baseAOcupar+=part->tamanioMensaje;
				//cambia la memoria
				memcpy(memoria+(part->base),part->mensaje,part->tamanioMensaje);
			}

			t_list* partOcupadas = list_filter(tablaDeParticiones,partLlenas);

			list_sort(tablaDeParticiones,baseMasChica);
			//muestra como quedo la tabla de particiones TODO (COMENTAR)
			/*
			log_info(compactacionMemoria,"antes de compactar la memoria esta asi:");
			list_iterate(tablaDeParticiones,mostrarParticiones);*/

			//ordenamos las bases ocupadas
			list_sort(partOcupadas,baseMasChica);
			//cambiamos las bases ocupadas para que esten todas juntas
			list_iterate(partOcupadas,cambiarBases);

			//destruir y liberar todas las particiones libres
			cantLibres = list_count_satisfying(tablaDeParticiones,partLibres);

			while(cantLibres>0){
				list_remove_and_destroy_by_condition(tablaDeParticiones,partLibres,free);
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


			list_add(tablaDeParticiones,partLibreNueva);

			list_destroy(partOcupadas);

			log_info(compactacionMemoria,"Compacte la memoria");

			//muestra como quedo la tabla de particiones
			/*
			log_info(compactacionMemoria,"Despues de compactar la memoria y quedo asi:");
			list_iterate(tablaDeParticiones,mostrarParticiones);*/
		}
	}
	else{
		frecuencia++;
	}

}

void eliminarParticion(particion * part){
	if(part->acknoleged != NULL){
		list_destroy_and_destroy_elements(part->acknoleged,free);
	}
	if(part->mensaje!=NULL){
		free(part->mensaje);
	}
	if(!strcmp(part->timestamp,"")){
		free(part->timestamp);
	}

	free(part);
}

