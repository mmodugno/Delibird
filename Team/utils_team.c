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

	 t_list* posiciones = crear_lista(string_split(posicion,"|"));
	 entrenador->posX = atoi(list_get(posiciones,0));
	 entrenador->posY = atoi(list_get(posiciones,1));
	 return entrenador;
 }


 t_list* hacer_entrenadores(void){
	 t_list* posiciones = obtener_lista_posiciones();
	 t_list* pokemones= obtener_lista_pokemones();
	 t_list* objetivos = obtener_lista_objetivos();

	 t_list* entrenadores = list_create();

	 int i;
	 for(i=0 ; i< list_size(posiciones) ; i++){

		entrenador* entrenador_listo = configurar_entrenador(list_get(posiciones,i),list_get(pokemones,i),list_get(objetivos,i));
		list_add(entrenadores,entrenador_listo);
	 }
	return entrenadores;
 }


 pokemon* hacer_pokemon(char* nombre, uint32_t posX, uint32_t posY){
	 pokemon* poke = malloc(sizeof(pokemon));
	 poke->nombre = nombre;
	 poke->posX = posX;
	 poke->posY = posY;
	 poke->tamanio_nombre = sizeof(nombre);
	 return poke;
 }


 int distancia_entrenador_pokemon(entrenador entrenador, pokemon pokemon){
	int x_final = fabs(entrenador.posX - pokemon.posX);
	int y_final = fabs(entrenador.posY - pokemon.posY);
	return (x_final + y_final);
}


void cambiar_estado_entrenador(entrenador* entrenador,int nuevo_estado){
	entrenador->estado = nuevo_estado;
}






 /*

  void terminar_programa(int conexion, t_log* logger, t_config* config)
 {
 	//Y por ultimo, para cerrar, hay que liberar lo que utilizamos (conexion, log y config) con las funciones de las commons y del TP mencionadas en el enunciado
 	log_destroy(logger);
 	config_destroy(config);
 	liberar_conexion(conexion);
 }
*/



 t_log* iniciar_logger(char* archivo){
   	return log_create(archivo,"iniciar logger",true,LOG_LEVEL_INFO);
   }


