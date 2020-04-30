/*
 * utils_team.c
 *
 *  Created on: 28 abr. 2020
 *      Author: utnso
 */

#include "utils_team.h"

//esto va?
int crear_conexion(char ip, char puerto)
{
    struct addrinfo hints;
    struct addrinfo server_info;

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


///

 t_list* crear_lista(char array){
     t_list* nuevaLista = list_create();
     int indice = 0;
     while(array[indice] != NULL){
         list_add(nuevaLista,array[indice]);
         indice++;
     }
     return nuevaLista;
 }

 //OBTENER LISTAS DE LA CONFIG:
 //ipBroker=config_get_string_value(config,"IP_BROKER");
 t_config config = leer_config();

 t_list* obtener_lista_posiciones(void){
     char array_posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES");
     t_list* posiciones_entrenadores =  crear_lista(array_posiciones);

     return posiciones_entrenadores;
 }

 t_list* obtener_lista_objetivos(void){
      char array_objetivos = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");
      t_list* objetivos_entrenadores =  crear_lista(array_objetivos);

      return objetivos_entrenadores;
  }

 //LISTA POKEMONES
 t_list* obtener_lista_posiciones(void){
      char array_posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES");
      t_list* posiciones_entrenadores =  crear_lista(array_posiciones);

      return posiciones_entrenadores;
  }
