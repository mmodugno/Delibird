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





//////////////////////////LEO TODA LA CONFIG //////////////////////////////////////////////////77
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
      char** array_pokemones = config_get_array_value(config,"POSICIONES_ENTRENADORES");
       return crear_lista(array_pokemones);
  }



char* leer_ip_broker(void){
	t_config* config = leer_config();
	char* ipBroker=config_get_string_value(config,"IP_BROKER");
     return ipBroker;
}


/*
	int tiempo_de_reconexion;
	int puertoBroker;
    char* algoritmoPlanificacion;
     puertoBroker = config_get_int_value(config,"PUERTO_BROKER");
     algoritmoPlanificacion = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
     tiempo_de_reconexion = config_get_int_value(config,"TIEMPO_RECONEXION");
*/
//uint32_t quantum; int
  //uint32_t estimacion_inicial; int

 /////////////////////////////////////////////////////////////////////////////





 entrenador* configurar_entrenador(char* posicion,char* pokemonsconfig, char* objetivosconfig){
	 entrenador* entrenador = malloc(sizeof(entrenador));
	 entrenador->estado = NEW;
	 entrenador->objetivos = crear_lista(string_split(objetivosconfig,"|"));
	 entrenador->pokemones = crear_lista(string_split(pokemonsconfig,"|"));
	 entrenador->cuantos_puede_cazar = list_size(entrenador->objetivos);

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


void mover_entrenador(entrenador* entrenador,pokemon* pokemon){
	//int tiempo = retardo_cpu();
	while(entrenador->posX != pokemon->posX){
		if(entrenador->posX < pokemon->posX){
			entrenador->posX = entrenador->posX + 1;
			//sleep(tiempo);
		}
		else {
			entrenador->posX = entrenador->posX -1;
			//sleep(tiempo);
		}
	}
	while(entrenador->posY != pokemon->posY){
		if(entrenador->posY < pokemon->posX){
			entrenador->posY = entrenador->posY + 1;
			//sleep(tiempo);
		}
		else {
			entrenador->posY = entrenador->posY -1;
			//sleep(tiempo);
		}
	}
	printf("listo");
}


bool se_puede_planificar(entrenador* entrenador){

if(entrenador->cuantos_puede_cazar == 0) return false;
if(entrenador->cuantos_puede_cazar > 0){
	return (entrenador->estado == NEW || entrenador->estado == READY);
	}
else return false;
}







///////////////////////////////////////////////////////////////////////////////////////////////////


 t_log* iniciar_log(void){
	t_config* config = leer_config();
	char* archivo = config_get_string_value(config,"LOG_FILE");
   	return log_create(archivo,"iniciar logger",true,LOG_LEVEL_INFO);
   }





  //void cambio_de_cola(char * razon){
//	 log_info(log,razon);
 //}

 /////////////////////////LOGS OBLIGATORIOS//////////////////////////////
 /*Cambio de un entrenador de cola de planificación (indicando la razón del porqué).
 Movimiento de un entrenador (indicando la ubicación a la que se movió).
 Operación de atrapar (indicando la ubicación y el pokemon a atrapar).
 Operación de intercambio (indicando entrenadores involucrados).
 Inicio de algoritmo de detección de deadlock.
 Resultado de algoritmo de detección de deadlock.
 Llegada de un mensaje (indicando el tipo del mismo y sus datos).
 Resultado del Team (especificado anteriormente).
 Errores de comunicación con el Broker (indicando que se realizará la operación por default).
 Inicio de proceso de reintento de comunicación con el Broker.
 Resultado de proceso de reintento de comunicación con el Broker.*/
////////////////////////////////////////////////////////////////





 /*
  void terminar_programa(int conexion, t_log* logger, t_config* config)
 {
 	//Y por ultimo, para cerrar, hay que liberar lo que utilizamos (conexion, log y config) con las funciones de las commons y del TP mencionadas en el enunciado
 	log_destroy(logger);
 	config_destroy(config);
 	liberar_conexion(conexion);
 }
*/



