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

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	log_info(logConexion," se conectaron a broker");

	pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
	pthread_detach(thread);

}

void serve_client(int* socket)
{
	int cod_op;
	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) != 0)
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
	void* msg;
	//falta los case de los otros tipos de mensajes (get,catch,caught)(localized lo dejamos para despues(es de GameCard)
	switch (cod_op) {
		case SUSCRIPCION:
			suscriptor* suscriptor = deserializar_suscripcion(cliente_fd,&size);

			log_info(logSuscipcion,"recibi mensaje de suscripcion de %s",suscriptor->nombreDeSuscriptor);

			suscribirACola(suscriptor);
			free(suscriptor);

			break;
		case BROKER__NEW_POKEMON:
			new_pokemon* newRecibido = deserializar_new_pokemon(cliente_fd,&size);

			log_info(logMensajeNuevo,"recibi mensaje de NEW_POKEMON");

			agregarACola(NEW_POKEMON,newRecibido);
			free(newRecibido);

			break;
		case BROKER__APPEARED_POKEMON:
			appeared_pokemon* appearedRecibido = deserializar_appeared_pokemon(cliente_fd,&size);

			log_info(logMensajeNuevo,"recibi mensaje de APPEARED_POKEMON");

			agregarACola(APPEARED_POKEMON,appearedRecibido);
			free(appearedRecibido);

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

void agregarACola(tipoDeCola tipo_de_Cola, void* mensaje){
	switch(tipo_de_Cola){
		case NEW_POKEMON:
			queue_push(colaNewPokemon,(new_pokemon*)mensaje);
			break;
		case APPEARED_POKEMON:
			queue_push(colaAppearedPokemon,(appeared_pokemon*)mensaje);
			break;
		case CATCH_POKEMON:
			queue_push(colaCatchPokemon,(catch_pokemon*)mensaje);
			break;
		case CAUGHT_POKEMON:
			queue_push(colaCaughtPokemon,(caught_pokemon*)mensaje);
			break;
		case GET_POKEMON:
			queue_push(colaGetPokemon,(get_pokemon*)mensaje);
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





