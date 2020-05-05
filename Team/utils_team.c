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


 t_log* iniciar_log(char* proceso){
	t_config* config = leer_config();
	char* archivo = config_get_string_value(config,"LOG_FILE");
   	return log_create(archivo,proceso,true,LOG_LEVEL_INFO);
   }




void log_cambio_de_cola(char * razon){
	t_log* log = iniciar_log("CAMBIO DE COLA");
	log_info(log,"cambio de cola porque: %s ",razon);
}

void log_movimiento_de_enntrenador(entrenador* entrenador){
	t_log* log = iniciar_log("MOVIMIENTO DE ENTRENADOR");
	log_info(log,"posicion en x: %d       posicion en y: %d",entrenador->posX,entrenador->posY);
}

void log_atrapar_pokemon(pokemon* poke){
	t_log* log = iniciar_log("ATRAPAR POKEMON");
	log_info(log,"pokemon: %s con posicion en x: %d y posicion en y: %d",poke->nombre,poke->posX,poke->posY);
}

void log_intercambio(entrenador* entrenador1,entrenador* entrenador2){ //Como muestro los entrenadores
	t_log* log = iniciar_log("INTERCAMBIO");
		log_info(log,"intercambio entre entrenadores");
}

/*void log_comunicacion_fallida(void){ NECESITO SABER EL TIEMPO DE RECONECCION

t_log* log = iniciar_log("NO SE PUDO COMUNICAR CON BROKER");
int reconexion =
	log_info(log,"se reintentara de comunicar en %d",reconexion);
}
*/

void log_reintentar_comunicacion(void){

t_log* log = iniciar_log("REINTENTANDO CONEXION");
	log_info(log,"conectando con broker ...");
}

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



