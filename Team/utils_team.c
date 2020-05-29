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


	 sem_init(&(un_entrenador->sem_entrenador),0,0);

	 un_entrenador->estado = NEW;
	// t_list* objetivos_config =
	 //t_list* pokemones_config =

	 un_entrenador->objetivos =crear_lista(string_split(objetivosconfig,"|"));
	 un_entrenador->pokemones =crear_lista(string_split(pokemonsconfig,"|"));

	 un_entrenador->cuantos_puede_cazar = list_size(un_entrenador->objetivos);
	 un_entrenador->id = id_creada;

	 sacar_pokemones_repetidos(un_entrenador->objetivos,un_entrenador->pokemones);

	 pthread_t hiloEntrenador;
	 un_entrenador->hiloDeEntrenador = pthread_create(&hiloEntrenador,NULL,procedimiento_de_caza,un_entrenador);

	 t_list* posiciones = crear_lista(string_split(posicion,"|"));
	 un_entrenador->posX = atoi(list_get(posiciones,0));
	 un_entrenador->posY = atoi(list_get(posiciones,1));
	 return un_entrenador;


 }


void sacar_pokemones_repetidos(t_list* objetivos, t_list* pokemones){
	for(int i = 0; i < list_size(pokemones);i++){
		nobmre_objetivoconfig = list_get(pokemones,i);
		//printf("voy sacando a: %s       ",nobmre_objetivoconfig);

		list_remove_by_condition(objetivos,pokemon_repetido);

	}
}


//bool es_de_especie(char* nombre_poke){
//	return strcmp(nombre_poke,proximo_objetivo->nombre);
//}





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
	/*
	for(int i = 0; i < list_size(entrenadores);i++){
		//agarro el primer entrenador:
		entrenador* un_entrenador = list_get(entrenadores,i);



		for(int j = 0; j< list_size(un_entrenador->pokemones);j++){
		//saco los pokemons que tenia del dictionary:
		char* pokemon_a_quitar = list_get(un_entrenador->pokemones,j);
		quitar_un_objetivo(pokemon_a_quitar);

	}
	*/
}




void agregar_un_objetivo(char* pokemon_a_agregar){

	if(dictionary_has_key(objetivo_global, pokemon_a_agregar)){ //si el pokemon ya estaba en el diccionario, le sumo 1 a su cantidad
		dictionary_put(objetivo_global,pokemon_a_agregar, dictionary_get(objetivo_global,pokemon_a_agregar)+1);
	}
	else{
		dictionary_put(objetivo_global, pokemon_a_agregar,1); // 1 o puntero??

	}
}







void cambiar_estado_entrenador(entrenador* entrenador,int nuevo_estado){
	entrenador->estado = nuevo_estado;
}

void mover_entrenador(entrenador* entrenador,pokemon* pokemon){

	printf(" \n inicio de movimiento ed entrenador  \n");
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
	printf(" \n fin de movimiento ed entrenador   \n");
	//log_movimiento_de_entrenador(entrenador); (mostraria la posicion despues de moverse)
}



//Se ejecuta cuando recibimos un mensaje de appeared_pokemon
void aparece_nuevo_pokemon(pokemon* poke){

	 if (dictionary_has_key(objetivo_global, poke->nombre) && (dictionary_get(objetivo_global, poke->nombre) > 0) ){
		 list_add(pokemones_en_el_mapa,poke);

		 //planificar a un entrenador con este pokemon nuevo
		 printf("Se planifica entrenador para la caza");
		 planificar_entrenador(poke);

	 }

	 else{
		 printf("El pokemon no pertenece al objetivo global. Se descarta");
	 }
}


void planificar_entrenador(pokemon* un_pokemon){
	proximo_objetivo = un_pokemon;
	entrenador* entrenador_ready = list_get( list_sorted(list_filter(entrenadores,se_puede_planificar),primer_entrenador_mas_cerca_de_pokemon) ,0);
	cambiar_estado_entrenador(entrenador_ready,READY);
	list_add(entrenadores_en_ready,entrenador_ready);

	sem_post(&entrenador_listo);

	//log_atrapar_pokemon(proximo_objetivo);
}





void algoritmo_aplicado(void){
	switch (leer_algoritmo_planificacion()){
	case FIFO:
		printf("\n Algoritmo de planificacion = FIFO \n ");
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
while(1){
	pthread_mutex_t semaforo_caza;
	pthread_mutex_init(&semaforo_caza,NULL);

	sem_wait(&(un_entrenador->sem_entrenador));

	printf(" \n Se activa a entrenador para la caza \n");

	pthread_mutex_lock(&semaforo_caza);

	printf(" \n Se activa a entrenador para la caza \n");

	//Aca iria otro semaforo para que el catch espere que el entrenador se mueva
	mover_entrenador(un_entrenador,proximo_objetivo);

	//wait pedir un catch  	  ->    cambiar_estado_entrenador(un_entrenador,BLOCK_ESPERANDO);
	denegar_catch();
	//log_atrapar_pokemon(proximo_objetivo);


	//signal (me llega un caught)

	//cosas



	pthread_mutex_unlock(&semaforo_caza);

	//analizar_proximo_estado(un_entrenador);  ESTO IRIA DESPUES DEL CAUGHT



}
}


//RESPUESTAS DEL CAUGHT



void confirmacion_de_catch(void){
	/*Resta 1 del objetivo global
	  si lo contenia en sus objetivos lo saca de la lista
	  lo agrega a la lista de pokemones atrapados (variable global)
	  lo agrega a la lista de los pokemones del entrenador
	*/

	dictionary_put(objetivo_global,proximo_objetivo->nombre,dictionary_get(objetivo_global,proximo_objetivo->nombre)-1);

	entrenador* un_entrenador = list_get(entrenadores_en_ready,0);

 if(list_any_satisfy(un_entrenador->objetivos , es_de_especie)){
	list_remove_by_condition(un_entrenador->objetivos , es_de_especie);
 }

 list_add(pokemones_atrapados,proximo_objetivo);
 list_add(un_entrenador->pokemones,proximo_objetivo);

 disminuir_cuantos_puede_cazar(un_entrenador);

printf("Agarró al pokemon %s",proximo_objetivo->nombre);

}


void denegar_catch(void){
	printf("No se agarró al pokemon");
}




void analizar_proximo_estado(entrenador* un_entrenador){
	if(puede_cazar(un_entrenador)){
		cambiar_estado_entrenador(un_entrenador,BLOCK_READY);
	}
	if(!puede_cazar(un_entrenador)){
		if(list_is_empty(un_entrenador->objetivos)){ cambiar_estado_entrenador(un_entrenador,EXIT);
		}
		else{
			cambiar_estado_entrenador(un_entrenador,BLOCK_DEADLOCK);
		}
}
}


void planifico_con_fifo(void){


while(1){


	sem_wait(&entrenador_listo);
	printf("Comienzo ejecucion \n");

	entrenador* entrenador_a_ejecutar = list_remove(entrenadores_en_ready,0);


	sem_post(&(entrenador_a_ejecutar->sem_entrenador));
	cambiar_estado_entrenador(entrenador_a_ejecutar,EXEC);

	printf("Se activo a entrenador");


}


}

//FUNCIONES AUX

void disminuir_cuantos_puede_cazar(entrenador* un_entrenador){
	un_entrenador->cuantos_puede_cazar -= 1;
}

bool es_de_especie(char* nombre_poke){
	return strcmp(nombre_poke,proximo_objetivo->nombre);
}

bool pokemon_repetido(char* nombre){
	return !strcmp(nombre,nobmre_objetivoconfig);
}

bool puede_cazar(entrenador* entrenador){        //Cambiar a cuando el entrenador termine su exc
	return entrenador->cuantos_puede_cazar > 0;
}


bool se_puede_planificar(entrenador* entrenador){
return (entrenador->estado == NEW || entrenador->estado == BLOCK_READY);
}


bool primer_entrenador_mas_cerca_de_pokemon(entrenador* entrenador1, entrenador* entrenador2){

	bool resultado = distancia_entrenador_pokemon(entrenador1,proximo_objetivo) <= distancia_entrenador_pokemon(entrenador2,proximo_objetivo);
	return resultado;
}

int distancia_entrenador_pokemon(entrenador* un_entrenador, pokemon* un_pokemon){

	int pos_entX =  (un_entrenador->posX);
	int pos_pokX = (un_pokemon->posX);
	int restaX = pos_entX - pos_pokX;

	int pos_entY =  (un_entrenador->posX);
	int pos_pokY = (un_pokemon->posX);
	int restaY = pos_entY - pos_pokY;

	double x_final = fabs(restaX);
	double y_final = fabs(restaY);

	return (int) (x_final + y_final);
}

void quitar_un_objetivo(char* pokemon_a_quitar){
	bool pokemon_repetido(char* nombre){
		return !strcmp(nombre,nobmre_objetivoconfig);
	}
	dictionary_put(objetivo_global, pokemon_a_quitar,dictionary_get(objetivo_global,pokemon_a_quitar)-1);
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



