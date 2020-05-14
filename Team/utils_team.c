/*
 * utils_team.c
 *
 *  Created on: 28 abr. 2020
 *      Author: utnso
 */

#include"utils_team.h"



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


void variables_globales(){
	config = leer_config();
	hacer_entrenadores();
	calcular_objetivo_global();

	pokemones_en_el_mapa = list_create();
	pokemones_atrapados= list_create();

	entrenadores_en_ready= list_create();



}



//////////////////////////LEO TODA LA CONFIG //////////////////////////////////////////////////

  int leer_retardo_cpu(void){
	 int retardo = config_get_int_value(config,"RETARDO_CICLO_CPU");
	  return retardo;
 }

 t_list* obtener_lista_posiciones(void){
     char** array_posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES");
     return crear_lista(array_posiciones);
 }

 t_list* obtener_lista_objetivos(void){
      char** array_objetivos = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");
     return crear_lista(array_objetivos);
  }

 t_list* obtener_lista_pokemones(void){
      char** array_pokemones = config_get_array_value(config,"POKEMON_ENTRENADORES");
       return crear_lista(array_pokemones);
  }

int leer_tiempo_de_reconexion(void){
	int tiempo_de_reconexion = config_get_int_value(config,"TIEMPO_RECONEXION");
     return tiempo_de_reconexion;
}

int leer_puerto_broker(void){
	int puerto_broker = config_get_int_value(config,"PUERTO_BROKER");
	  return puerto_broker;
}
int leer_algoritmo_planificacion(void){
	 char* algoritmo_planificacion = config_get_string_value(config,"ALGORITMO_PLANIFICACION");

	 if(strcmp(algoritmo_planificacion,"FIFO") == 0) return FIFO;
	 if(strcmp(algoritmo_planificacion,"RR") == 0) return RR;
	 //if(strcmp(algoritmo_planificacion,"FIFO") == 0) return FIFO;

	 return algoritmo_planificacion;

}
int leer_quantum(void){
	 int quantum = config_get_int_value(config,"QUANTUM");
	  return quantum;
}
int leer_estimacion_inicial(void){
	 int estimacion_inicial = config_get_int_value(config,"ESTIMACION_INICIAL");
	  return estimacion_inicial;
}
char* leer_ip_broker(void){
	 char* ip = config_get_string_value(config,"IP_BROKER");
	 return ip;
}

 /////////////////////////////////////////////////////////////////////////////



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


void hacer_entrenadores(void){
	entrenadores = list_create();

	t_list* posiciones = obtener_lista_posiciones();
	 t_list* pokemones = obtener_lista_pokemones();
	 t_list* objetivos = obtener_lista_objetivos();




	 for(int i=0 ; i< list_size(posiciones) ; i++){
		entrenador* entrenador_listo = configurar_entrenador(list_get(posiciones,i),list_get(pokemones,i),list_get(objetivos,i),i);
		list_add(entrenadores,entrenador_listo);

	 }

 }


 pokemon* hacer_pokemon(char* nombre, uint32_t posX, uint32_t posY){
	 pokemon* poke = malloc(sizeof(pokemon));
	 poke->nombre = nombre;
	 poke->posX = posX;
	 poke->posY = posY;
	 poke->tamanio_nombre = sizeof(nombre);
	 return poke;
 }


void calcular_objetivo_global(void){

	objetivo_global = dictionary_create();

	for(int i = 0; i < list_size(entrenadores);i++){
		//agarro el primer entrenador:
		entrenador* un_entrenador = list_get(entrenadores,i);


		for(int j = 0; j< list_size(un_entrenador->objetivos);j++){
			//pongo sus objetivos en el dictionary:
			char* pokemon_a_agregar = list_get(un_entrenador->objetivos,j);
			agregar_un_objetivo(pokemon_a_agregar);
		}
	}

	//LE SACO LOS POKEMONES QUE YA TENIAN
	for(int i = 0; i < list_size(entrenadores);i++){
		//agarro el primer entrenador:
		entrenador* un_entrenador = list_get(entrenadores,i);



		for(int j = 0; j< list_size(un_entrenador->pokemones);j++){
		//saco los pokemons que tenia del dictionary:
		char* pokemon_a_quitar = list_get(un_entrenador->pokemones,j);
		quitar_un_objetivo(pokemon_a_quitar);

	}
}

}


void agregar_un_objetivo(char* pokemon_a_agregar){

	if(dictionary_has_key(objetivo_global, pokemon_a_agregar)){ //si el pokemon ya estaba en el diccionario, le sumo 1 a su cantidad
		dictionary_put(objetivo_global,pokemon_a_agregar, dictionary_get(objetivo_global,pokemon_a_agregar)+1);
	}
	else{
		dictionary_put(objetivo_global, pokemon_a_agregar,1); // 1 o puntero??

	}
}


void quitar_un_objetivo(char* pokemon_a_quitar){
	dictionary_put(objetivo_global, pokemon_a_quitar,dictionary_get(objetivo_global,pokemon_a_quitar)-1);
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

	//log_movimiento_de_entrenador(entrenador); (mostraria la posicion despues de moverse)
}


void aparece_nuevo_pokemon(char* nombre,int posicionX, int posicionY){
	pokemon* nuevo_pokemon = malloc(sizeof(pokemon));
	nuevo_pokemon = hacer_pokemon(nombre,posicionX,posicionY);

	list_add(pokemones_en_el_mapa,nuevo_pokemon);

	//planificar a un entrenador con este pokemon nuevo
	planificar_entrenador(nuevo_pokemon);
}


void planificar_entrenador(pokemon* un_pokemon){
	proximo_objetivo = un_pokemon;
	entrenador* entrenador_ready = list_get( list_sorted(list_filter( entrenadores,se_puede_planificar),primer_entrenador_mas_cerca_de_pokemon) ,0);
	cambiar_estado_entrenador(entrenador_ready,READY);
	list_add(entrenadores_en_ready,entrenador_ready);
}


/* bool puede_cazar(entrenador* entrenador){        //Cambiar a cuando el entrenador termine su exc
	return entrenador->cuantos_puede_cazar > 0;
}
*/



bool se_puede_planificar(entrenador* entrenador){
return (entrenador->estado == NEW || entrenador->estado == BLOCK_READY);
}



bool primer_entrenador_mas_cerca_de_pokemon(entrenador* entrenador1, entrenador* entrenador2){
	return distancia_entrenador_pokemon(entrenador1,proximo_objetivo) <= distancia_entrenador_pokemon(entrenador2,proximo_objetivo);
}


int distancia_entrenador_pokemon(entrenador* un_entrenador, pokemon* un_pokemon){
	int x_final = fabs(un_entrenador->posX - un_pokemon->posX);
	int y_final = fabs(un_entrenador->posY - un_pokemon->posY);
	return (x_final + y_final);
}



void algoritmo_aplicado(void){
	switch (leer_algoritmo_planificacion()){
	case FIFO:
		printf("\n Algoritmo de planificacion = FIFO");
		//planifico_con_fifo();
		break;

	case RR:
		printf("\n Algoritmo de planificacion = RR");
		//planifico_con_RR();
		break;

	 default:
	       printf("\n Algoritmo no reconocido \n");
	       break;
	}
}



//FUNCION DEL HILO DEL ENTRENADOR
void procedimiento_de_caza(entrenador* un_entrenador){
	//semaforo
	cambiar_estado_entrenador(un_entrenador,EXEC);
	mover_entrenador(un_entrenador,proximo_objetivo);
	//pedir un catch

	cambiar_estado_entrenador(un_entrenador,BLOCK_ESPERANDO);

	//wait(respuesta de catch)
	//cosas

	cambiar_estado_entrenador(un_entrenador,BLOCK_READY);

	//se vuelve a bloquear
}





void planifico_con_fifo(void){

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



