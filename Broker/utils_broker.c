/*
 * utils_broker.c
 *
 *  Created on: 14 may. 2020
 *      Author: utnso
 */

#include "utils_broker.h"

void iniciar_servidor(void)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip_broker, puerto_broker, &hints, &servinfo);

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

	socklen_t  tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);



	pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
	pthread_detach(thread);

}

void serve_client(int* socket)
{
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

	log_info(logConexion,"%s se conecto al broker",username);
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
			newRecibido->id = idGlobales;
			idGlobales++;
			//mutex

			new_pokemon *raiz = transformarBrokerNewPokemon(newRecibido);
			agregarAMemoria(raiz,newRecibido->id);
			agregarACola(NEW_POKEMON,newRecibido);

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
			appearedRecibido->id = idGlobales;
			idGlobales++;
			//mutex

			appeared_pokemon *raiz = transformarBrokerAppearedPokemon(appearedRecibido);
			agregarAMemoria(raiz,appearedRecibido->id);
			agregarACola(APPEARED_POKEMON,appearedRecibido);

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
			getRecibido->id = idGlobales;
			idGlobales++;
			//mutex

			get_pokemon *raiz = transformarBrokerGetPokemon(getRecibido);
			agregarAMemoria(raiz,getRecibido->id);
			agregarACola(GET_POKEMON,getRecibido);

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
			catchRecibido->id = idGlobales;
			idGlobales++;
			//mutex

			catch_pokemon *raiz = transformarBrokerCatchPokemon(catchRecibido);
			agregarAMemoria(raiz, catchRecibido->id);
			agregarACola(CATCH_POKEMON,catchRecibido);

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
			caughtRecibido->id = idGlobales;
			idGlobales++;
			//mutex
			caught_pokemon *raiz = transformarBrokerCaughtPokemon(caughtRecibido);
			agregarAMemoria(raiz,caughtRecibido->id);
			agregarACola(CAUGHT_POKEMON,caughtRecibido);

			free(raiz);
			free(caughtRecibido);

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
void agregarACola(tipoDeCola tipo_de_Cola, void* mensaje){
	switch(tipo_de_Cola){
		case NEW_POKEMON:
			queue_push(colaNewPokemon,(broker_new_pokemon*)mensaje);
			break;
		case APPEARED_POKEMON:
			queue_push(colaAppearedPokemon,(broker_appeared_pokemon*)mensaje);
			break;
		case CATCH_POKEMON:
			queue_push(colaCatchPokemon,(broker_catch_pokemon*)mensaje);
			break;
		case CAUGHT_POKEMON:
			queue_push(colaCaughtPokemon,(broker_caught_pokemon*)mensaje);
			break;
		case GET_POKEMON:
			queue_push(colaGetPokemon,(broker_get_pokemon*)mensaje);
			break;
		case LOCALIZED_POKEMON:
			//queue_push(colaLocalizedPokemon,(broker_localize_pokemon*) mensaje);
			queue_push(colaLocalizedPokemon,(localize_pokemon*) mensaje);

			break;


	}
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

void agregarAMemoria(void * dato, uint32_t idMensaje){
	particion *datoAAgregar = crearEntradaParticionBasica(dato,idMensaje);
	uint32_t desplazamiento = 0;
	particion *particionEncontrada = malloc(sizeof(particion));	//Para el algoritmo FF/BF
	particion* particionMasChica = malloc(sizeof(particion));	//Para el algoritmo BF

	if(!strcmp(algoritmo_particion_libre,"FF")){
		algoritmoFirstFit(datoAAgregar,&desplazamiento,particionEncontrada);
	}
	if(!strcmp(algoritmo_particion_libre,"BF")){
		algoritmoBestFit(dato,idMensaje);
	}

	free(datoAAgregar);
	free(particionEncontrada);
	free(particionMasChica);
}



particion* crearEntradaParticionBasica(void * dato, uint32_t idMensaje){
	particion *datoAAgregar = malloc(sizeof(particion));
	datoAAgregar->base = 0;//predeteminado, osea a modificar
	datoAAgregar->mensaje = malloc(sizeof(dato));
	//agregamos tipos de dato raiz como son new_pokemon, localized_pokemon, etc;
	//TENER EN CUENTA QUE PUEDE CAMBIAR el tamaño si es broker_tipo_dato  PORQUE TIENE LOS SUSCRIPTORES QUE LE MANDARON ACK
	datoAAgregar->mensaje= dato;
	datoAAgregar->tamanioMensaje= sizeof(dato);
	datoAAgregar->tiempo= temporal_get_string_time();//solo tira la hora sin la fecha
	datoAAgregar->idMensaje = idMensaje;

	return datoAAgregar;
}

//funcion recursiva
void algoritmoFirstFit(particion *datoAAgregar,uint32_t *desplazamiento,particion *particionEncontrada){

	particionEncontrada = NULL;

	bool estaEnTabla(particion *datoIterado){
		return (*(desplazamiento)<=(datoIterado->base) && (datoIterado->base)<=(*(desplazamiento)+datoAAgregar->tamanioMensaje));
	}

	particionEncontrada = list_find(tablaDeParticiones,(void *)estaEnTabla);
	if(particionEncontrada!=NULL){
		//encontro una particion que esta entre el desplazamiento y el limite
		//por lo tanto no debemos poner el tipo de dato
		if(*(desplazamiento)+datoAAgregar->tamanioMensaje>=tamanio_memoria){
			//si se pasa de la memoria significa que no encontro ningun lugar
			//algortimo de reemplazo
		}
		else{
			//volvemos a buscar otro lugar
			*(desplazamiento)+=(particionEncontrada->base+particionEncontrada->tamanioMensaje);
			algoritmoFirstFit(datoAAgregar,desplazamiento,particionEncontrada);
		}
	}
	else{
		//encontro un lugar donde entra
		//poner en tabla y en la memoria
		datoAAgregar->base = *(desplazamiento);
		list_add(tablaDeParticiones,datoAAgregar);
		memcpy(memoria+datoAAgregar->base,datoAAgregar->mensaje,datoAAgregar->tamanioMensaje);
	}

}

/*
 * */

void algoritmoBestFit(particion *datoAAgregar, particion* particionMasChica){
	//Leer memoria para buscar espacios libres
	//Fijarse si los espacios libres entra el tipo de dato que queremos agregar
	//Evaluar que sea el tamaño mas chico de esos espacios libres
	//Meter en memoria con esa base del tamaño del espacio libre mas chico y ponerlo con el tamaño del dato a agregar

	t_list* particionesLibres = list_create();


	t_list* listaPosicionesLibres = list_create();

	uint32_t i;

	/* recorrer memoria*/
	for(i = 0; i < tamanio_memoria;i++) {
	    void* prueba = malloc(1);
	    memcpy(prueba,memoria+i,1);

	    if(strcmp(prueba,"\0") == 0){
	        /*listaPosicionesLibres.add(posicion)*/
	    	list_add(listaPosicionesLibres,i);
	        //printf("posicion libre en: %d \n",posicion);
	    }
	}
	/* fin recorrer memoria */

	/*armar particionesLibres*/
	/*la listaPosicionesLibres siempre va a estar ordenada porque recorre la memoria en forma ascendente*/

	//arma una lista de particiones libres
	armarParticiones(listaPosicionesLibres,particionesLibres);

	//filtra por las que entre el dato a agregar
	bool entraEnParticion(particionLibre *pLibre){
		return pLibre->tamanio>= datoAAgregar->tamanioMensaje;
	}

	particionesLibres = list_filter(particionesLibres,entraEnParticion);

	//ordena por tamanio
	bool ordenarPorTamanio(particionLibre *pLibre1,particionLibre *pLibre2){
		return pLibre1->tamanio<pLibre2->tamanio;
	}

	list_sort(particionesLibres,ordenarPorTamanio);

	particionLibre *pLibreParaAgregar = list_get(particionesLibres,0);


	//mutex
	datoAAgregar->base = pLibreParaAgregar->base;
	list_add(tablaDeParticiones,datoAAgregar);
	memcpy(memoria+datoAAgregar->base,datoAAgregar->mensaje,datoAAgregar->tamanioMensaje);
	//mutex

	/* Para ver que particion asignar, osea que base, hago un map de listaParticionesLibres con una funcion "sacarTamanio"
	para que todas me devuelvan el tamaño, hago un filter para que tenga el tamaño minimo para copiar mi dato y de
	esas saco la minima
	*/

	free(pLibreParaAgregar);
	//free();
	list_destroy(listaPosicionesLibres);
	list_destroy(particionesLibres);

}

// 9 11 12
void armarParticiones(t_list* listaPosicionesLibres,t_list * listaParticionesLibres) {
	particionLibre* pLibre = malloc(sizeof(particionLibre));
	uint32_t posibleLimite;

	while(list_is_empty(listaPosicionesLibres) != 0) {   //
	    pLibre->base = list_get(listaPosicionesLibres,0);

	    list_remove(listaPosicionesLibres,0); //elimina primer elemento lista
	   // 5 6
	    posibleLimite = list_get(listaPosicionesLibres,0);
	    if(posibleLimite != (pLibre->base+1)) {
	        posibleLimite = pLibre->base;
	    } else {
			uint32_t i = 1;
			uint32_t next = list_get(listaPosicionesLibres, i);
			//*** ver si con un for se soluciona

			for (i = 0; i < list_size(listaPosicionesLibres); i++) {
				next = lista_get(i+1);
				if (posibleLimite+1 == next) {
					posibleLimite = next;
				}
			}
			//
			t_list* listaBasura = list_take_and_remove(listaPosicionesLibres,i);
			list_destroy(listaBasura);
	    }
	    pLibre->limite = posibleLimite;

		if(pLibre->limite == pLibre->base) {
			pLibre->tamanio = 1;
		} else {
			pLibre->tamanio = pLibre->limite - pLibre->base;
		}

		list_add(listaParticionesLibres,pLibre);
	}

}


void leerMemoria(t_list* particionesLibres, uint32_t *pivote, particion* particionAAgregar, t_list tablaDeParticionesCopia){
	//Al leer, voy a meter las particiones libres a la lista particionesLibres
	/*
	 * Primero ordenamos la lista de las particiones en memoria, para que luego podamos
	 * comparar las bases respecto un pivote y así obtener los espacios libres
	 *
	 */

	bool comparadorDeBases(particion* unaParticion, particion* otraParticion){
		// Fijarse si ordena menor a mayor, o al reves!!!!!!!!!!!!!!!!!!!!!!!!!
		return (unaParticion->base < otraParticion->base);
	}

	//mutex para sincronizar
	list_sort(tablaDeParticionesCopia, comparadorDeBases);
	//mutex para sincronizar

	list_iterate(tablaDeParticionesCopia, mostrarParticiones); //Muestro la tabla de particiones para ver como quedo

	//Empezamos a iterar
	particion* particionPivActual = tablaDeParticionesCopia->head->data;
	particion* particionLibreAAgregar = malloc(sizeof(particion));

	if( (particionPivActual->base - *pivote) >= particionAAgregar->tamanioMensaje ){
		particionLibreAAgregar->base = *pivote;
		particionLibreAAgregar->tamanioMensaje = particionPivActual->base - *pivote;
	}



}

void agregarAParticionesLibres(t_list* particionesLibres, particion* unaParticionLibre){
	//Agrego la particionLibre a la lista
}

void mostrarParticiones(particion* unaParticion){
	if(unaParticion != NULL){
		printf("base: %d, tamanio: %d, id: %d, tiempo: %s", unaParticion->base, unaParticion->tamanioMensaje, unaParticion->idMensaje, unaParticion->tiempo);
	}
}

void iniciarMemoria(){

	memoria = malloc(tamanio_memoria);

	tablaDeParticiones = list_create();

	idGlobales = 0;

}





