/*
 * utils_team.c
 *
 *  Created on: 28 abr. 2020
 *      Author: utnso
 */

#include"utils_team.h"
#include<commons/collections/list.h>



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





//////////////////////////LEO TODA LA CONFIG //////////////////////////////////////////////////
  int leer_retardo_cpu(void){
	 t_config* config = leer_config();
	 int retardo = config_get_int_value(config,"RETARDO_CICLO_CPU");
	  return retardo;
 }


 t_list* obtener_lista_posiciones(void){
	 t_config* config = leer_config();
     char** array_posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES");
     return crear_lista(array_posiciones);

 }

 t_list* obtener_lista_objetivos(void){
	 t_config* config = leer_config();
      char** array_objetivos = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");
     return crear_lista(array_objetivos);

  }


 t_list* obtener_lista_pokemones(void){
	 t_config* config = leer_config();
      char** array_pokemones = config_get_array_value(config,"POKEMON_ENTRENADORES");
       return crear_lista(array_pokemones);
  }

int leer_tiempo_de_reconexion(void){
	t_config* config = leer_config();
	int tiempo_de_reconexion = config_get_int_value(config,"TIEMPO_RECONEXION");
     return tiempo_de_reconexion;
}

int leer_puerto_broker(void){
	 t_config* config = leer_config();
	 int puerto_broker = config_get_int_value(config,"PUERTO_BROKER");
	  return puerto_broker;
}
char* leer_algoritmo_planificacion(void){
	 t_config* config = leer_config();
	 char* algoritmo_planificacion = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
	  return algoritmo_planificacion;
}
int leer_quantum(void){
	 t_config* config = leer_config();
	 int quantum = config_get_int_value(config,"QUANTUM");
	  return quantum;
}
int leer_estimacion_inicial(void){
	 t_config* config = leer_config();
	 int estimacion_inicial = config_get_int_value(config,"ESTIMACION_INICIAL");
	  return estimacion_inicial;
}
char* leer_ip_broker(void){
	 t_config* config = leer_config();
	 char* ip = config_get_string_value(config,"IP_BROKER");
	  return ip;
}


 /////////////////////////////////////////////////////////////////////////////

int conexion;
char* ip; //= leer_ip_broker();
char* puerto; //= leer_puerto_broker();



 entrenador* configurar_entrenador(char* posicion,char* pokemonsconfig, char* objetivosconfig, int id_creada){
	 entrenador* un_entrenador = malloc(sizeof(entrenador));

	 un_entrenador->estado = NEW;
	 un_entrenador->objetivos = crear_lista(string_split(objetivosconfig,"|"));
	 un_entrenador->pokemones = crear_lista(string_split(pokemonsconfig,"|"));
	 un_entrenador->cuantos_puede_cazar = list_size(un_entrenador->objetivos);
	 un_entrenador->id = id_creada;

	 //pthread_t hiloEntrenador;
	 //un_entrenador->hiloDeEntrenador = pthread_create(&hiloEntrenador,NULL,planificarEntrenador,un_entrenador);

	 t_list* posiciones = crear_lista(string_split(posicion,"|"));
	 un_entrenador->posX = atoi(list_get(posiciones,0));
	 un_entrenador->posY = atoi(list_get(posiciones,1));
	 return un_entrenador;
 }


 t_list* hacer_entrenadores(void){
	 t_list* posiciones = obtener_lista_posiciones();
	 t_list* pokemones = obtener_lista_pokemones();
	 t_list* objetivos = obtener_lista_objetivos();

	 t_list* entrenadores = list_create();


	 for(int i=0 ; i< list_size(posiciones) ; i++){
		entrenador* entrenador_listo = configurar_entrenador(list_get(posiciones,i),list_get(pokemones,i),list_get(objetivos,i),i);
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
	 poke->cantidad = 1;
	 return poke;
 }


t_list* calcular_objetivo_global(void){
	t_list* objetivos = list_create();
	t_list* entrenadores = hacer_entrenadores();

	for( int i = 0; i < list_size(entrenadores); i++){
		entrenador* un_entrenador =  list_get(entrenadores,i);

		t_list* objetivos_entrenador = un_entrenador->objetivos;

	 //ACA AGREGAR OTRA F DE LISTA DE STRUCTS y esa pasarla abajo

		list_add_all(objetivos, objetivos_entrenador);

	}
	return objetivos;
}


bool es_de_especie(pokemon* poke,char* nombre){
	return poke->nombre == nombre;
}



 int distancia_entrenador_pokemon(entrenador entrenador, pokemon pokemon){
	int x_final = fabs(entrenador.posX - pokemon.posX);
	int y_final = fabs(entrenador.posY - pokemon.posY);
	return (x_final + y_final);
}

void cambiar_estado_entrenador(entrenador* entrenador,int nuevo_estado){
	entrenador->estado = nuevo_estado;
}

void mover_entrenador(entrenador* entrenador,pokemon* pokemon){
	int tiempo = leer_retardo_cpu();
	while(entrenador->posX != pokemon->posX){
		if(entrenador->posX < pokemon->posX){
			entrenador->posX = entrenador->posX + 1;
			sleep(tiempo);
		}
		else {
			entrenador->posX = entrenador->posX -1;
			sleep(tiempo);
		}
	}
	while(entrenador->posY != pokemon->posY){
		if(entrenador->posY < pokemon->posX){
			entrenador->posY = entrenador->posY + 1;
			sleep(tiempo);
		}
		else {
			entrenador->posY = entrenador->posY -1;
			sleep(tiempo);
		}
	}
}



bool se_puede_planificar(entrenador* entrenador){

return (entrenador->estado == NEW || entrenador->estado == READY);
}



bool puede_cazar(entrenador* entrenador){        //Cambiar a cuando el entrenador termine su exc
	return entrenador->cuantos_puede_cazar > 0;
}

//entrenador* entrenador_mas_cerca(,pokemon* poke){
//	//ordenar la lista entrenadores segun la cercania, sacados de la cola ready?

//}



void planificar_entrenador(t_list* entrenadores){ //entrenadores de la cola de ready
	//t_list* list_filter(t_list*, bool(*condition)(void*));

	//list_filter(entrenadores, se_puede_planificar());

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



