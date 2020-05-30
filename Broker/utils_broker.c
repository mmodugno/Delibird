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

	//log_info(logConexion," se conectaron a broker");

	pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
	pthread_detach(thread);

}

void serve_client(int* socket)
{
	int cod_op;
	int i = recv(*socket, &cod_op, sizeof(int), MSG_WAITALL);

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
	int size;
	suscriptor* suscriptor;
	broker_new_pokemon* newRecibido;
	broker_appeared_pokemon* appearedRecibido;
	broker_get_pokemon* getRecibido;
	broker_catch_pokemon* catchRecibido;
	broker_caught_pokemon* caughtRecibido;
	//falta los case de los otros tipos de mensajes (get,catch,caught)(localized lo dejamos para despues(es de GameCard)
	switch (cod_op) {
		case SUSCRIPCION:
			suscriptor = deserializar_suscripcion(cliente_fd,&size);

			log_info(logSuscipcion,"recibi mensaje de suscripcion de %s",suscriptor->nombreDeSuscriptor);

			suscribirACola(suscriptor);
			free(suscriptor);

			break;
		case BROKER__NEW_POKEMON:
			newRecibido = deserializar_new_pokemon(cliente_fd,&size);

			log_info(logMensajeNuevo,"recibi mensaje de NEW_POKEMON /n con tamanio: %d /n nombre: %s /n posX: %d /n posY: %d /n cantidad de pokemones: %d"
					,newRecibido->datos->tamanioNombre,
					newRecibido->datos->nombrePokemon,
					newRecibido->datos->posX,
					newRecibido->datos->posY,
					newRecibido->datos->cantidadPokemon);

			agregarACola(NEW_POKEMON,newRecibido);
			free(newRecibido);

			break;
		case BROKER__APPEARED_POKEMON:
			appearedRecibido = deserializar_appeared_pokemon(cliente_fd,&size);

			log_info(logMensajeNuevo,"recibi mensaje de APPEARED_POKEMON");

			agregarACola(APPEARED_POKEMON,appearedRecibido);
			free(appearedRecibido);

			break;
		case BROKER__GET_POKEMON:
			getRecibido = deserializar_get_pokemon(cliente_fd,&size);

			log_info(logMensajeNuevo,"recibi mensaje de GET_POKEMON");

			agregarACola(GET_POKEMON,getRecibido);
			free(getRecibido);

			break;
		case BROKER__CATCH_POKEMON:
			catchRecibido = deserializar_catch_pokemon(cliente_fd,&size);

			log_info(logMensajeNuevo,"recibi mensaje de CATCH_POKEMON");

			agregarACola(CATCH_POKEMON,catchRecibido);
			free(catchRecibido);

			break;
		case BROKER__CAUGHT_POKEMON:
			caughtRecibido = deserializar_caught_pokemon(cliente_fd,&size);

			log_info(logMensajeNuevo,"recibi mensaje de CAUGHT_POKEMON");

			agregarACola(CAUGHT_POKEMON,caughtRecibido);
			free(caughtRecibido);

			break;
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
		}


}

void* recibir_mensaje(int socket_cliente, int* size)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

//por ahora usamos tipos de dato de broker que van a tener el id_relativo, pero quizas en la cola no lo necesita y sino en la memoria
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
			queue_push(colaLocalizedPokemon,(localize_pokemon*)mensaje);
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





