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

 entrenador* configurar_entrenador(char* posicion,char* pokemonsconfig, char* objetivosconfig){
	 entrenador* entrenador = malloc(sizeof(entrenador));
	 entrenador->estado = NEW;
	 entrenador->objetivos = crear_lista(string_split(objetivosconfig,"|"));
	 entrenador->pokemones = crear_lista(string_split(pokemonsconfig,"|"));

	 char** posiciones = string_split(posicion,"|");
	 entrenador->posX = atoi(posiciones[0]);
	 entrenador->posY = atoi(posiciones[1]);
	 return entrenador;
 }


 t_list* hacer_entrenadores(void){
	 t_list* posiciones = obtener_lista_posiciones();
	 t_list* pokemones= obtener_lista_pokemones();
	 t_list* objetivos = obtener_lista_objetivos();

	 t_list* entrenadores = list_create();

	 int i;
	 for(i=0 ; i< list_size(posiciones) ; i++){
		// list_add(entrenadores, array[i]);
		 //list_get(t_list*,int index)

		entrenador* entrenador_listo = configurar_entrenador(list_get(posiciones,i),list_get(pokemones,i),list_get(objetivos,i));
		list_add(entrenadores,entrenador_listo);
	 }


	return entrenadores;
 }

char* archivo_log(void){
	char* todo_el_archivo_log = config_get_string_value(config,"LOG_FILE");
	t_list* lista_de_palabras = crear_lista(string_split(todo_el_archivo_log,"/"));
	int tamanio = list_size(lista_de_palabras);
	return list_get(lista_de_palabras,tamanio-1);
}
//LOG_FILE=        "/home/utnso/Team/log_team1.txt"




    //aca deberiamos poner terminar_programa
