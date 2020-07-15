/*
 * utils_gameboy.c
 *
 *  Created on: 29 abr. 2020
 *      Author: utnso
 */

#include "utils_Gameboy.h"
#include <sys/time.h>
#include <sys/types.h>


/*
 * Recibe un paquete a serializar, y un puntero a un int en el que dejar
 * el tamaño del stream de bytes serializados que devuelve
 */


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

void enviar_Broker_New_Pokemon(broker_new_pokemon *brokerNewPokemon,char* username , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__NEW_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username)+1;
	paquete_a_enviar->username = username;

	//serializacion de brokerNewPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_broker_new_pokemon(brokerNewPokemon,buffer);

	paquete_a_enviar->buffer= buffer;

	int tamanio_buffer=0;


	void* bufferStream = serializar_paquete(paquete_a_enviar,&tamanio_buffer);

	send(socket_cliente,bufferStream,tamanio_buffer,0);

	free(bufferStream);

	//estos no hacen falta porque no pedimos memoria de stream, el buffer y paquete_a_enviar->buffer son lo mismo
	//free(buffer->stream);
	//free(buffer);
	//free(paquete_a_enviar->buffer->stream);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
}

void enviar_Broker_Appeared_Pokemon(broker_appeared_pokemon *brokerAppearedPokemon,char* username , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__APPEARED_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username)+1;
	paquete_a_enviar->username = username;

	//serializacion de brokerNewPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_broker_appeared_pokemon(brokerAppearedPokemon,buffer);


	paquete_a_enviar->buffer= buffer;

	int tamanio_buffer=0;

	void* bufferStream = serializar_paquete(paquete_a_enviar,&tamanio_buffer);
	send(socket_cliente,bufferStream,tamanio_buffer,0);


	free(bufferStream);

	//estos no hacen falta porque no pedimos memoria de stream, el buffer y paquete_a_enviar->buffer son lo mismo
	//free(buffer->stream);
	//free(buffer);
	//free(paquete_a_enviar->buffer->stream);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
}

void enviar_Broker_Catch_Pokemon(broker_catch_pokemon *brokerCatchPokemon,char* username , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__CATCH_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username)+1;
	paquete_a_enviar->username = username;

	//serializacion de brokerNewPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_broker_catch_pokemon(brokerCatchPokemon,buffer);


	paquete_a_enviar->buffer= buffer;

	int tamanio_buffer=0;

	void* bufferStream = serializar_paquete(paquete_a_enviar,&tamanio_buffer);
	send(socket_cliente,bufferStream,tamanio_buffer,0);


	free(bufferStream);

	//estos no hacen falta porque no pedimos memoria de stream, el buffer y paquete_a_enviar->buffer son lo mismo
	//free(buffer->stream);
	//free(buffer);
	//free(paquete_a_enviar->buffer->stream);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
}

void enviar_Broker_Caught_Pokemon(broker_caught_pokemon *brokerCaughtPokemon,char* username, int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__CAUGHT_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username)+1;
	paquete_a_enviar->username = username;
	//serializacion de brokerCaughtPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_broker_caught_pokemon(brokerCaughtPokemon,buffer);

	paquete_a_enviar->buffer = buffer;

	int tamanio_buffer=0;

	void* bufferStream = serializar_paquete(paquete_a_enviar,&tamanio_buffer);

	send(socket_cliente,bufferStream,tamanio_buffer,0);

	free(bufferStream);

	//estos no hacen falta porque no pedimos memoria de stream, el buffer y paquete_a_enviar->buffer son lo mismo
	//free(buffer->stream);
	//free(buffer);
	//free(paquete_a_enviar->buffer->stream);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
}

void enviar_Broker_Get_Pokemon(broker_get_pokemon *brokerGetPokemon,char* username , int socket_cliente)
{
	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__GET_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username)+1;
	paquete_a_enviar->username = username;
	//serializacion de brokerGetPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_broker_get_pokemon(brokerGetPokemon,buffer);

	paquete_a_enviar->buffer= buffer;

	int tamanio_buffer=0;

	void* bufferStream = serializar_paquete(paquete_a_enviar,&tamanio_buffer);
	send(socket_cliente,bufferStream,tamanio_buffer,0);

	free(bufferStream);

	//estos no hacen falta porque no pedimos memoria de stream, el buffer y paquete_a_enviar->buffer son lo mismo
	//free(buffer->stream);
	//free(buffer);
	//free(paquete_a_enviar->buffer->stream);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
}

void enviar_Team_Appeared_Pokemon(team_appeared_pokemon *teamAppearedPokemon,char* username , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = TEAM__APPEARED_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username)+1;
	paquete_a_enviar->username = malloc(paquete_a_enviar->tamanio_username);
	paquete_a_enviar->username = username;

	//serializacion de teamNewPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_team_appeared_pokemon(teamAppearedPokemon,buffer);

	paquete_a_enviar->buffer= buffer;

	int tamanio_buffer=0;

	void * bufferStream = serializar_paquete(paquete_a_enviar,&tamanio_buffer);
	send(socket_cliente,bufferStream,tamanio_buffer,0);


	free(bufferStream);

	//estos no hacen falta porque no pedimos memoria de stream, el buffer y paquete_a_enviar->buffer son lo mismo
	//free(buffer->stream);
	//free(buffer);
	//free(paquete_a_enviar->buffer->stream);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
}

void enviar_GameCard_New_Pokemon(gameCard_new_pokemon *gameCardNewPokemon,char* username , int socket_cliente)
{


	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = GAMECARD__NEW_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username)+1;
	paquete_a_enviar->username = username;

	//serializacion de gamecardNewPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));
	paquete_a_enviar->buffer = buffer;

	serializar_gameCard_new_pokemon(gameCardNewPokemon,buffer);




	int tamanio_buffer=0;

	void* bufferStream = serializar_paquete(paquete_a_enviar,&tamanio_buffer);
	send(socket_cliente,bufferStream,tamanio_buffer,0);


	free(bufferStream);

	//estos no hacen falta porque no pedimos memoria de stream, el buffer y paquete_a_enviar->buffer son lo mismo
	//free(buffer->stream);
	//free(buffer);
	//free(paquete_a_enviar->buffer->stream);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
}

void enviar_GameCard_Catch_Pokemon(gameCard_catch_pokemon *gameCardCatchPokemon,char* username , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = GAMECARD__CATCH_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username)+1;
	paquete_a_enviar->username = username;

	//serializacion de brokerNewPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_gameCard_catch_pokemon(gameCardCatchPokemon,buffer);

	paquete_a_enviar->buffer= buffer;

	int tamanio_buffer=0;

	void* bufferStream = serializar_paquete(paquete_a_enviar,&tamanio_buffer);
	send(socket_cliente,bufferStream,tamanio_buffer,0);


	free(bufferStream);

	//estos no hacen falta porque no pedimos memoria de stream, el buffer y paquete_a_enviar->buffer son lo mismo
	//free(buffer->stream);
	//free(buffer);
	//free(paquete_a_enviar->buffer->stream);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
}

void enviar_GameCard_Get_Pokemon(gameCard_get_pokemon *gameCardGetPokemon,char* username , int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = GAMECARD__GET_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username)+1;
	paquete_a_enviar->username = username;

	//serializacion de brokerNewPokemon
	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_gameCard_get_pokemon(gameCardGetPokemon,buffer);

	paquete_a_enviar->buffer= buffer;

	int tamanio_buffer=0;

	void* bufferStream = serializar_paquete(paquete_a_enviar,&tamanio_buffer);
	send(socket_cliente,bufferStream,tamanio_buffer,0);


	free(bufferStream);

	//estos no hacen falta porque no pedimos memoria de stream, el buffer y paquete_a_enviar->buffer son lo mismo
	//free(buffer->stream);
	//free(buffer);
	//free(paquete_a_enviar->buffer->stream);

	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
}



//TODO?
//se podria usar este para no repetir codigo, pero no se me ocurrio cómo
void enviar_mensaje(char* mensaje, int socket_cliente)
{

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = MENSAJE;

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size=strlen(mensaje)+1;
	buffer->stream= mensaje;

	paquete_a_enviar->buffer= buffer;

	int tamanio_buffer=0;

	void* bufferStream = serializar_paquete(paquete_a_enviar,&tamanio_buffer);
	send(socket_cliente,bufferStream,tamanio_buffer,0);


	free(bufferStream);

	//estos no hacen falta porque no pedimos memoria de stream, el buffer y paquete_a_enviar->buffer son lo mismo
	//free(buffer->stream);
	//free(buffer);
	//free(paquete_a_enviar->buffer->stream);
	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
}

//TODO?
//podriamos modificar este y usar el server que nos dieron para probar los mensajes anteriores
char* recibir_mensaje(int socket_cliente)
{
	t_paquete* paquete_recibido=malloc(sizeof(t_paquete));
	paquete_recibido->buffer= malloc(sizeof(t_buffer));

	char* mensaje;


	recv(socket_cliente,&(paquete_recibido->codigo_operacion),sizeof(op_code),0);
	recv(socket_cliente,&(paquete_recibido->buffer->size),sizeof(int),0);

	if(paquete_recibido->codigo_operacion == MENSAJE){
		recv(socket_cliente,(paquete_recibido->buffer->stream),paquete_recibido->buffer->size,0);
	}

	mensaje = malloc(paquete_recibido->buffer->size);
	memcpy(mensaje,paquete_recibido->buffer->stream,paquete_recibido->buffer->size);

	free(paquete_recibido->buffer);
	free(paquete_recibido);
	return mensaje;


}


void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

void iniciar_servidor_gameboy(){
	int socket_servidor_gameboy;
	char* ip_gameboy = "127.0.0.9";
	char* puerto_gameboy = "5009";

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip_gameboy, puerto_gameboy, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next) {
        if ((socket_servidor_gameboy = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor_gameboy, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor_gameboy);
            continue;
        }
        break;
    }

	listen(socket_servidor_gameboy, SOMAXCONN);

    freeaddrinfo(servinfo);

    while(flagTerminoSuscripcion == 0){
    	esperar_cliente_gameboy(socket_servidor_gameboy);
    }

    //TODO enviar pedido de Desuscripcion o como se escriba
}

void esperar_cliente_gameboy(int socket_servidor_gameboy) { //Se conecta el Broker al Gameboy para enviarle los mensajes de las colas
	struct sockaddr_in dir_cliente;

	socklen_t* tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente_gameboy = acceptConTimeOut(socket_servidor_gameboy, (void*) &dir_cliente, tam_direccion,segundosSuscripcion);

	pthread_create(&hiloConexion,NULL,(void*)serve_client_gameboy,&socket_cliente_gameboy); // EL hilo q procesa los mensajes
	pthread_detach(hiloConexion);

	if(socket_cliente_gameboy < 0){

		flagTerminoSuscripcion = 1;

	}
	// sem_init(&(semaforoTiempo),0,0);

	//pthread_create(&threadTime,NULL,(void*)analizadorTime,&segundosSuscripcion); // El hilo que maneja el tiempo
	//pthread_detach(threadTime);
}

int acceptConTimeOut(int socket_servidor_gameboy, __SOCKADDR_ARG dir_cliente, socklen_t *__restrict tam_direccion, int timeOut){

	fd_set fds;
	int n;
	struct timeval tv;

	FD_ZERO(&fds);
	FD_SET(socket_servidor_gameboy, &fds);

	tv.tv_sec = timeOut;
	tv.tv_usec = 0;

	n = select(socket_servidor_gameboy+1, &fds, NULL, NULL, &tv);
	if(n == 0) return -2; //timeout
	if(n == -1) return -1; // error

	return accept(socket_servidor_gameboy, (void*) &dir_cliente, tam_direccion);
}

/*void analizadorTime(int* segundos){
	// Semaforo de bloqueo, cuando se cnecta el broker, empiezo a contar los segundos.
	sem_wait(&semaforoTiempo);
	sleep(segundos * 1000);
	// Enviar pedido de desuscripcion

	pthread_exit(NULL);
} */

void serve_client_gameboy(int socket){ // Se reciben los bytes enviados por el Broker
	int cod_op;

	int i = recv(socket, &cod_op, sizeof(op_code), MSG_WAITALL);
	//int i = recv(socket, &cod_op, sizeof(op_code));

	if(i <= 0) cod_op = -1;
	if(i == -2) cod_op = -2;

	process_request_gameboy(cod_op, socket);
}

int recvTimeout(int socket, int *cod_op, int len, int timeOut){

	fd_set fds;
	int n;
	struct timeval tv;


	FD_ZERO(&fds);
	FD_SET(socket, &fds);

	tv.tv_sec = timeOut;
	tv.tv_usec = 0;

	n = select(socket+1, &fds, NULL, NULL, &tv);
	if(n == 0) return -2; //timeout
	if(n == -1) return -1; // error

	return recv(socket, &cod_op, sizeof(op_code), MSG_WAITALL);
}

void process_request_gameboy(int cod_op, int cliente_fd) { //Descifra los mensajes enviados por el Broker y los logea
	uint32_t tamanio_buffer;
	uint32_t tamanio_username;
	char* username;

	recv(cliente_fd,&tamanio_username,sizeof(uint32_t),MSG_WAITALL);
	username = malloc(tamanio_username);
	recv(cliente_fd, username,tamanio_username,MSG_WAITALL);
	recv(cliente_fd, &tamanio_buffer, sizeof(uint32_t), MSG_WAITALL);

	//Leo el ID
	int ID;
	recv(cliente_fd, &ID, sizeof(uint32_t), MSG_WAITALL);

	switch (cod_op) {
		case BROKER__NEW_POKEMON:{
			broker_new_pokemon* newRecibido;
			newRecibido = deserializar_new_pokemon(cliente_fd);
			log_info(logMensajeNuevo,"recibi mensaje de NEW_POKEMON (ID = %d) de %s \n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d \n cantidad de pokemones: %d",
					ID,
					username,
					newRecibido->datos->tamanioNombre,
					newRecibido->datos->nombrePokemon,
					newRecibido->datos->posX,
					newRecibido->datos->posY,
					newRecibido->datos->cantidadPokemon);

			free(newRecibido);
			break;
		}
		case BROKER__APPEARED_POKEMON:{
			broker_appeared_pokemon* appearedRecibido;
			appearedRecibido = deserializar_appeared_pokemon(cliente_fd);

			log_info(logMensajeNuevo,"recibi mensaje de APPEARED_POKEMON (ID = %d) de %s \n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d \n con id_relativo: %d",
					ID,
					username,
					appearedRecibido->datos->tamanioNombre,
					appearedRecibido->datos->nombrePokemon,
					appearedRecibido->datos->posX,
					appearedRecibido->datos->posY,appearedRecibido->id_relativo);

			free(appearedRecibido);
			break;
		}
		case BROKER__GET_POKEMON:{
			broker_get_pokemon* getRecibido;
			getRecibido = deserializar_get_pokemon(cliente_fd);

			log_info(logMensajeNuevo,"recibi mensaje de GET_POKEMON (ID = %d) de %s\n con tamanio: %d \n nombre: %s ",
					ID,
					username,
					getRecibido->datos->tamanioNombre,
					getRecibido->datos->nombrePokemon);

			free(getRecibido);
			break;
		}
		case BROKER__CATCH_POKEMON:{
			broker_catch_pokemon* catchRecibido;
			catchRecibido = deserializar_catch_pokemon(cliente_fd);

			log_info(logMensajeNuevo,"recibi mensaje de CATCH_POKEMON (ID = %d) de %s\n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d ",
					ID,
					username,
					catchRecibido->datos->tamanioNombre,
					catchRecibido->datos->nombrePokemon,
					catchRecibido->datos->posX,
					catchRecibido->datos->posY);

			free(catchRecibido);
			break;
		}
		case BROKER__CAUGHT_POKEMON:{
			broker_caught_pokemon* caughtRecibido;
			caughtRecibido = deserializar_caught_pokemon(cliente_fd);

			log_info(logMensajeNuevo,"recibi mensaje de CAUGHT_POKEMON (ID = %d) de %s\n con ID_relativo: %d \n puedoAtraparlo: %d ",
					ID,
					username,
					caughtRecibido->id_relativo,
					caughtRecibido->datos->puedoAtraparlo);

			free(caughtRecibido);
			break;
		}
		case 0:
			pthread_exit(NULL);
		case -1:
			flagTerminoSuscripcion = 1;
			pthread_exit(NULL);
		case -2:
			flagTerminoSuscripcion = 1;
			pthread_exit(NULL);
		}
}
