/*
 * utils_team.c
 *
 *  Created on: 28 abr. 2020
 *      Author: utnso
 */

#include "utils_team.h"
#include<commons/collections/list.h>

///

 t_list* crear_lista(char** array){
     t_list* nuevaLista = list_create();
     int i = 0;
     while( array[i] != NULL ){
         list_add(nuevaLista, array[i]);
         i++;
     }
     return nuevaLista;
 }

 t_config* leer_config(void){
         return config_create("team1.config");
     }

 //OBTENER LISTAS DE LA CONFIG:
 //ipBroker=config_get_string_value(config,"IP_BROKER");

 t_config* config;


 t_list* obtener_lista_posiciones(void){
     char** array_posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES");
     return crear_lista(array_posiciones);

 }

 t_list* obtener_lista_objetivos(void){
      char** array_objetivos = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");
      return crear_lista(array_objetivos);

  }

 //LISTA POKEMONES
 t_list* obtener_lista_pokemones(void){
      char** array_pokemones = config_get_array_value(config,"POSICIONES_ENTRENADORES");
       return crear_lista(array_pokemones);

  }
    //aca deberiamos poner terminar_programa
