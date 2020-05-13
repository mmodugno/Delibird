/*
 * conexiones.c
 *
 *  Created on: 9 may. 2020
 *      Author: utnso
 */


#include "conexiones.h"
#include "utils_team.h"

//ESTO ES PARA SER EL CLIENTE
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

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("error");

	freeaddrinfo(server_info);

	return socket_cliente;
}


pokemon* deserializar_appeared_pokemon(void* stream){
	pokemon* poke = malloc(sizeof(pokemon));

	memcpy(&(poke->tamanio_nombre),stream,sizeof(uint32_t));


	stream+=sizeof(uint32_t);
	memcpy(poke->nombre,stream,poke->tamanio_nombre);

	stream += (poke->tamanio_nombre);
	memcpy(&(poke->posX),stream,sizeof(uint32_t));

	stream+=sizeof(uint32_t);
	memcpy(&(poke->posY),stream,sizeof(uint32_t));

return poke;
}


void* recibir_mensaje(int socket_cliente){
	t_paquete* paquete_recibido=malloc(sizeof(t_paquete));
	paquete_recibido->buffer= malloc(sizeof(t_buffer));


	recv(socket_cliente,&(paquete_recibido->codigo_operacion),sizeof(op_code),0);
	recv(socket_cliente,&(paquete_recibido->buffer->size),sizeof(int),0);
	recv(socket_cliente,(paquete_recibido->buffer->stream),paquete_recibido->buffer->size,0);



	if(paquete_recibido->codigo_operacion == TEAM__APPEARED_POKEMON){

		pokemon* nuevo_pokemon;
		nuevo_pokemon = deserializar_appeared_pokemon(paquete_recibido->buffer->stream);

		free(paquete_recibido->buffer);
		free(paquete_recibido);

		return (void*) nuevo_pokemon;
	}

	// if(paquete_recibido->codigo_operacion == )


}





/*

void iniciar_conexion_gameboy(void)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;


    getaddrinfo(IP, PUERTO, &hints, &servinfo);

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

    while(1)
    	esperar_cliente(socket_servidor);
}



void esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
	pthread_detach(thread);

}


void serve_client(int* socket)
{
	int cod_op;
	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	process_request(cod_op, *socket);
}

void process_request(int cod_op, int cliente_fd) {
	int size;
	void* msg;
		switch (cod_op) {
		case MENSAJE:
			msg = recibir_mensaje(cliente_fd, &size);
			                        //devolver_mensaje(msg, size, cliente_fd);
			free(msg);
			break;
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
		}
}


*/

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}



////////////////////////////////////////////LOGS///////////////////////////////////////////////////////


 t_log* iniciar_log(char* proceso){
	t_config* config = leer_config();
	char* archivo = config_get_string_value(config,"LOG_FILE");
   	return log_create(archivo,proceso,true,LOG_LEVEL_INFO);
   }


void log_cambio_de_cola(char * razon){
	t_log* log = iniciar_log("CAMBIO DE COLA");
	log_info(log,"cambio de cola porque: %s ",razon);
}

void log_movimiento_de_entrenador(entrenador* entrenador){
	t_log* log = iniciar_log("MOVIMIENTO DE ENTRENADOR");
	log_info(log,"entrenador %d: se movio a (%d,%d)",entrenador->id,entrenador->posX,entrenador->posY);
}

void log_atrapar_pokemon(pokemon* poke){
	t_log* log = iniciar_log("ATRAPAR POKEMON");
	log_info(log,"pokemon: %s con posicion (%d, %d)",poke->nombre,poke->posX,poke->posY);
}

void log_intercambio(entrenador* entrenador1,entrenador* entrenador2){
	t_log* log = iniciar_log("INTERCAMBIO");
		log_info(log,"intercambio entre entrenadores %d y %d",entrenador1->id,entrenador2->id);
}



//Error de comunicacion con el broker
void log_comunicacion_fallida(void){
t_log* log = iniciar_log("NO SE PUDO COMUNICAR CON BROKER");
int reconexion = leer_tiempo_de_reconexion();
log_info(log,"se reintentara de comunicar en %d",reconexion);
}


void log_reintentar_comunicacion(void){
t_log* log = iniciar_log("REINTENTANDO CONEXION");
	log_info(log,"conectando con broker ...");
}


//Resultado de reintento de comunicacion con el broker
void log_conexion_exitosa(void){
 t_log* log = iniciar_log("CONEXION");
	log_info(log,"conexion exitosa");
}
void log_fallo_de_conexion(void){
t_log* log = iniciar_log("CONEXION"); //PONERLO COMO ERROR ASI SALE ROJITO
	log_info(log,"fallo de conexion");
}


 /////////////////////////LOGS OBLIGATORIOS//////////////////////////////
 /*
 PENDIENTE - Inicio de algoritmo de detección de deadlock.
 PENDIENTE - Resultado de algoritmo de detección de deadlock.
 PENDIENTE - Llegada de un mensaje (indicando el tipo del mismo y sus datos).
 PENDIENTE - Resultado del Team (especificado anteriormente).
////////////////////////////////////////////////////////////////


*/


