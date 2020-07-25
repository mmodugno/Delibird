/*
 * utils_team.c
 *
 *  Created on: 28 abr. 2020
 *      Author: utnso
 */

#include"utils_team.h"
#include"utils_en_comun.h"


pthread_mutex_t llegadaMensajesTHREAD = PTHREAD_MUTEX_INITIALIZER;

void variables_globales(){

	config = leer_config();

	IP_TEAM = config_get_string_value(config,"IP_TEAM");
	PUERTO_TEAM = config_get_string_value(config,"PUERTO_TEAM");

	IP_BROKER = config_get_string_value(config,"IP_BROKER");
	PUERTO_BROKER = config_get_string_value(config,"PUERTO_BROKER");

	hacer_entrenadores();

	calcular_objetivo_global();

	pokemones_en_el_mapa = queue_create();

	pokemones_atrapados= list_create();

	//Si planifico con FIFO o RR lo hago mediante COLAS
	if(leer_algoritmo_planificacion() == FIFO || leer_algoritmo_planificacion() == RR){

		entrenadores_block_ready = queue_create();
	}

	else{ //Si planifico con SJF
		lista_entrenadores_block_ready = list_create();
		lista_entrenadores_ready = list_create();
		alpha = leer_alpha();
	}

	entrenadores_ready = queue_create();
	entrenadores_blocked = list_create();
	entrenadores_finalizados = list_create();
	entrenadores_en_deadlock = list_create();

	//Metricas y contador de deadlock:
	cant_deadlocks = 0;
	cant_deadlocks_resueltos = 0;
	entrenador_deadlock=0;
	cambio_contexto =0;
	validar_deadlock = 1;

}



 /////////////////////////////////////////////////////////////////////////////



 entrenador* configurar_entrenador(char* posicion,char* pokemonsconfig, char* objetivosconfig, int id_creada){
	 entrenador* un_entrenador = malloc(sizeof(entrenador));

	 un_entrenador->ciclos_cpu =0;
	 sem_init(&(un_entrenador->sem_entrenador),0,0);
	 sem_init(&(un_entrenador->espera_de_catch),0,0);
	 sem_init(&(un_entrenador->nuevoPoke),0,0);

	 pthread_t hiloEntrenador;


	 un_entrenador->objetivos =crear_lista(string_split(objetivosconfig,"|"));
	 un_entrenador->pokemones =crear_lista(string_split(pokemonsconfig,"|"));


	 un_entrenador->id = id_creada;

	 un_entrenador->id_catch = 0;

	 un_entrenador->rafaga_estimada = leer_estimacion_inicial();

	 un_entrenador->rafaga_real = 1;

	 sacar_pokemones_repetidos(un_entrenador->objetivos,un_entrenador->pokemones);

	 un_entrenador->cuantos_puede_cazar = list_size(un_entrenador->objetivos);
	 t_list* posiciones = crear_lista(string_split(posicion,"|"));
	 un_entrenador->posX = atoi(list_get(posiciones,0));
	 un_entrenador->posY = atoi(list_get(posiciones,1));

	 un_entrenador->hiloDeEntrenador = pthread_create(&hiloEntrenador,NULL,(void*) procedimiento_de_caza,un_entrenador);

	 return un_entrenador;
 }


void sacar_pokemones_repetidos(t_list* objetivos, t_list* pokemones){

	t_list* pokemones_aux = pokemones;
int i;
	for(i = 0; i < list_size(pokemones_aux);i++){
		nombre_pokemon = list_get(pokemones_aux,i);

		if(list_any_satisfy(objetivos, (void*) pokemon_repetido)){
		list_remove_by_condition(objetivos,(void*) pokemon_repetido);
		list_remove_by_condition(pokemones,(void*) pokemon_repetido);
		}

	}
}


void hacer_entrenadores(void){

	entrenadores = list_create();
	entrenadores_new = list_create();

	t_list* posiciones = obtener_lista_posiciones();
	 t_list* pokemones = obtener_lista_pokemones();
	 t_list* objetivos = obtener_lista_objetivos();

int i;
	 for(i=0 ; i< list_size(posiciones) ; i++){
		entrenador* entrenador_listo = configurar_entrenador(list_get(posiciones,i),list_get(pokemones,i),list_get(objetivos,i),i);
		list_add(entrenadores,entrenador_listo);
		list_add(entrenadores_new,entrenador_listo);
		sem_post(&hay_entrenador);
	 }


 }

pokemon* hacer_pokemon(char* nombre, uint32_t posX, uint32_t posY, uint32_t tamanio){

	//printf("Haciendo nuevo Pokemon: %s \n",nombre);
	//fflush(stdout);

	 pokemon* poke = malloc(sizeof(pokemon));
	 poke->nombre = nombre;
	 poke->posX = posX;
	 poke->posY = posY;
	 poke->tamanio_nombre = tamanio;
	 return poke;
 }//tenemos que poner un free


void calcular_objetivo_global(void){

	objetivo_global = dictionary_create();
	printf("Objetivo Global Inicial:  ");
	int i,j;
	for( i = 0; i < list_size(entrenadores);i++){
		//agarro el primer entrenador:
		entrenador* un_entrenador = list_get(entrenadores,i);


		for(j = 0; j< list_size(un_entrenador->objetivos);j++){
			//pongo sus objetivos en el dictionary:
			char* pokemon_a_agregar = list_get(un_entrenador->objetivos,j);
			agregar_un_objetivo(pokemon_a_agregar);

			printf("  %s ",pokemon_a_agregar);

		}
	}
	printf(" \n");


}



void agregar_un_objetivo(char* pokemon_a_agregar){

	if(dictionary_has_key(objetivo_global, pokemon_a_agregar)){ //si el pokemon ya estaba en el diccionario, le sumo 1 a su cantidad
		dictionary_put(objetivo_global,pokemon_a_agregar, dictionary_get(objetivo_global,pokemon_a_agregar)+1);
	}
	else{
		dictionary_put(objetivo_global, pokemon_a_agregar,(void *)1);

	}
}

void algoritmo_aplicado(void){
	switch (leer_algoritmo_planificacion()){
	case FIFO:
		planifico_sin_desalojo();
		break;

	case RR:
		planifico_con_RR();
		break;

	case SJFSD:
		planifico_sin_desalojo();
		break;

	case SJFCD:
		planifico_con_desalojo();
		break;

	 default:
	       printf("\n Algoritmo no reconocido \n");
	       break;
	}

}

void aparece_nuevo_pokemon(pokemon* poke){

	 if (dictionary_has_key(objetivo_global, poke->nombre) && (dictionary_get(objetivo_global, poke->nombre) > 0) ){
		 //ME SIRVE EL POKEMON

		 printf("Agrego pokemon en mapa %s \n",poke->nombre);
		 fflush(stdout);

		 queue_push(pokemones_en_el_mapa,poke);
	 }
	 else{
		 printf("\n El pokemon %s no pertenece al objetivo global. Se descarta \n", poke->nombre);
	 }
}

//FUNCION DEL HILO DEL ENTRENADOR
void procedimiento_de_caza(entrenador* un_entrenador){

while(1){

	sem_wait(&(un_entrenador->nuevoPoke));
	sem_wait(&(un_entrenador->sem_entrenador));

	//log_info(cambioDeCola,"cambio a EXEC de entrenador: %d \n ",entrenador_exec->id);

	if(leer_algoritmo_planificacion() == RR || leer_algoritmo_planificacion() == SJFCD){
		mover_entrenador_RR(un_entrenador,un_entrenador->objetivo_proximo->posX,un_entrenador->objetivo_proximo->posY);
	}
	else{
		mover_entrenador(un_entrenador,un_entrenador->objetivo_proximo->posX,un_entrenador->objetivo_proximo->posY);
	}

	log_info(operacion_de_atrapar,"ATRAPAR POKEMON: %s con posicion (%d, %d) \n",un_entrenador->objetivo_proximo ->nombre,un_entrenador->objetivo_proximo ->posX,un_entrenador->objetivo_proximo ->posY);


	//LA FUNCION APRA CONECTARSE AL BROKER LA HACE EL HILO DE CONEXION CON BROKER
	//if(conectarse_con_broker()!=-1){
		//log_info(comunicacion_broker_resultado,"me conecte a Broker exitosamente");

	int conexionParaCatch = crear_conexion(IP_BROKER,PUERTO_BROKER);

		if(conexionParaCatch>0){

		broker_catch_pokemon *catchAEnviar=malloc(sizeof(broker_catch_pokemon));
		catchAEnviar->datos=malloc(sizeof(catch_pokemon));

		catchAEnviar->datos->tamanioNombre=un_entrenador->objetivo_proximo->tamanio_nombre;
		catchAEnviar->datos->nombrePokemon = malloc(catchAEnviar->datos->tamanioNombre);
		catchAEnviar->datos->nombrePokemon = un_entrenador->objetivo_proximo->nombre;
		catchAEnviar->datos->posX = un_entrenador->objetivo_proximo->posX;
		catchAEnviar->datos->posY= un_entrenador->objetivo_proximo->posY;

		enviar_catch(un_entrenador,catchAEnviar,conexionParaCatch);
		log_info(llegadaDeMensaje,"del catch que envie su ID es %d \n",catchAEnviar->id);
		un_entrenador->id_catch = catchAEnviar->id; //Nos guardamos el ID para identificar los caught

		un_entrenador->ciclos_cpu += 1;


		//libero la conexion con el broker
		//close(conexionParaCatch);
	}else{
		log_info(comunicacion_broker_error,"No se pudo conectar con Broker,se realizará la operación por default \n");
	}




	bloquear_entrenador(un_entrenador);


	//libera la ejecucion mientras espera el catch
	sem_post(&en_ejecucion);

//Si no se puede conectar se confirma el catch.
		if(conectarse_con_broker()==-1){
			confirmacion_de_catch(un_entrenador);
		}

	sem_wait(&(un_entrenador->espera_de_catch));

	sem_wait(&(un_entrenador->sem_entrenador));

	 //Espera que le llegue al sistema una respuesta a su catch

	if(conectarse_con_broker()==-1){
	printf("\n Agarró al pokemon %s \n",un_entrenador->objetivo_proximo->nombre);
	}


	analizar_proxima_cola(un_entrenador); //ANALIZA A QUE COLA O LISTA SE MUEVE


	sem_post(&en_ejecucion);
	sem_post(&hay_entrenador);
}
}

//CONDICION DE DEADLOCK, PARA TODOS LOS ALGORITMOS
void manejar_deadlock(void){
int i,j;
	for(i = 0; i < (list_size(entrenadores_en_deadlock)-1);i++){
		for(j = 1; j < (list_size(entrenadores_en_deadlock)); j++){
			entrenador* entrenador0 = list_get(entrenadores_en_deadlock,i); // otro for para comparar con el resto
			entrenador* entrenador1 = list_get(entrenadores_en_deadlock,j);

		//if(entrenador0->id == entrenador1->id) break;

			nombre_pokemon = list_get(entrenador0->objetivos,0);

			//Si algun pokemon del 1 es el que 0 necesita y viceversa, se planifican:
			if(list_any_satisfy(entrenador1->pokemones,(void*)pokemon_repetido)){

				nombre_pokemon = list_get(entrenador1->objetivos,0);

				if(list_any_satisfy(entrenador0->pokemones,(void*)pokemon_repetido)){

					if(leer_algoritmo_planificacion() == RR){
						planificar_deadlock_RR(entrenador0,entrenador1);

						}
						else{
							planificar_deadlock(entrenador0,entrenador1);
						}

					break;
				}
				printf(" \n No se puede manejar el deadlock con entrenador:%d y entrenador:%d \n",entrenador0->id,entrenador1->id);
				if(entrenador0->id != entrenador1->id) entrenador_deadlock+=2;

				break;
			}
			else{
				printf(" \n No se puede manejar el deadlock con entrenador:%d y entrenador:%d \n",entrenador0->id,entrenador1->id);

				if(entrenador0->id != entrenador1->id) entrenador_deadlock+=2;

				espera_de_deadlock();
				break;
			}
	}
}
}

void planificar_entrenador(void){

	sem_wait(&hay_entrenador);

	proximo_objetivo = queue_peek(pokemones_en_el_mapa);
	queue_pop(pokemones_en_el_mapa); //Si no lo atrapo se vuelve a poner

	if(!list_is_empty(entrenadores_new)){
	entrenador_exec = list_get(list_sorted(entrenadores_new,(void*) primer_entrenador_mas_cerca_de_pokemon) ,0);
	list_remove_by_condition(entrenadores_new,(void*)entrenador_en_exec);

	}
	else{
		entrenador_exec = queue_peek(entrenadores_block_ready);
		queue_pop(entrenadores_block_ready);
	}

	entrenador_exec->objetivo_proximo = proximo_objetivo;
	sem_post(&(entrenador_exec->nuevoPoke));

}

////////////////////////////////////////////////FIFO


void planifico_sin_desalojo(void){

while(1){

//Se fija si hay nuevos pokemones, y alguien para cazarlos
while(validacion_nuevo_pokemon()){

	sem_wait(&en_ejecucion);


	//planifico uno en cada ciclo del fifo
	if(leer_algoritmo_planificacion() == FIFO){
	planificar_entrenador();
	}

	if(leer_algoritmo_planificacion() == SJFSD){
		planificar_entrenador_segun_rafaga();
	}

	cambio_contexto +=1;
	log_info(cambioDeCola,"cambio a EXEC de entrenador: %d \n ",entrenador_exec->id);
	sem_post(&(entrenador_exec->sem_entrenador));

}


//Agregar entrenadores en ready cuando atrapan al pokemon
while(queue_size(entrenadores_ready)>0){
	entrenador_exec = queue_peek(entrenadores_ready);
	queue_pop(entrenadores_ready);

		sem_wait(&en_ejecucion);
		cambio_contexto +=1;
		log_info(cambioDeCola,"cambio a EXEC de entrenador: %d \n ",entrenador_exec->id);

		sem_post(&(entrenador_exec->sem_entrenador));
}



if(validar_deadlock && list_is_empty(entrenadores_new)){
		validar_deadlock=0;
		sem_wait(&en_ejecucion);

		log_info(inicio_deadlock,"Inicio de deteccion de deadlock");

		if(hay_deadlock_multiple()){

			log_info(resultado_deadlock,"Se detectó deadlock multiple \n");

			entrenador_deadlock-=1;


			pthread_create(&hilo_deadlock,NULL,(void *) manejar_deadlock_multiple,NULL);
			sem_post(&en_ejecucion);
			continue;
		}

		if(hay_deadlock()){
			log_info(resultado_deadlock,"Se detectó deadlock \n");
			entrenador_deadlock-=2;
			cant_deadlocks +=1;


		pthread_create(&hilo_deadlock,NULL,(void *) manejar_deadlock,NULL);

		sem_post(&en_ejecucion);
		continue;

		}
	else{
			log_info(resultado_deadlock,"No se detectó nuevo deadlock \n");

			//espera unos segundos y pone el contador validar_deadlock en 1
			pthread_t espera_deadlock;
			pthread_create(&espera_deadlock,NULL,(void *) espera_de_deadlock,NULL);

		}

		sem_post(&en_ejecucion);
	}


if(list_size(entrenadores) == list_size(entrenadores_finalizados)){

	pthread_cancel(hilo_servidor);
	printf("\n FINALIZO EL PROGRAMA \n");

	break;
}

}
}

void deteccion_y_recuperacion_deadlock(){

	if(list_size(entrenadores_en_deadlock)>1){
		log_info(resultado_deadlock,"Se detectó deadlock \n");
		cant_deadlocks +=1;
		//manejar_deadlock();
		cambio_contexto +=1;
		pthread_t hilo_deadlock;
		pthread_create(&hilo_deadlock,NULL,(void *) manejar_deadlock,NULL);

		pthread_join(hilo_deadlock,NULL);

	}else{
		log_info(resultado_deadlock,"No se detectó deadlock \n");
	}

	sem_post(&en_ejecucion);

	pthread_t espera_deadlock;
	pthread_create(&espera_deadlock,NULL,(void *) espera_de_deadlock,NULL);
}



//Funcion de deteccion deadlock
void espera_de_deadlock(void){
	sleep(25);
	validar_deadlock = 1;
}


void mover_entrenador(entrenador* entrenador,int x, int y){

	int tiempo = leer_retardo_cpu();
	while(entrenador->posX != x){
		if(entrenador->posX < x){
			entrenador->posX = entrenador->posX + 1;
			entrenador->ciclos_cpu += 1;
			usleep(tiempo);
		}
		else {
			entrenador->posX = entrenador->posX -1;
			entrenador->ciclos_cpu += 1;
			usleep(tiempo);
		}
	}

	while(entrenador->posY != y){
		if(entrenador->posY < y){
			entrenador->posY = entrenador->posY + 1;
			entrenador->ciclos_cpu += 1;
			usleep(tiempo);
		}
		else {
			entrenador->posY = entrenador->posY -1;
			entrenador->ciclos_cpu += 1;
			usleep(tiempo);
		}
	}

	sleep(tiempo);

	log_info(movimiento_entrenador,"entrenador %d: se movio a (%d,%d)",entrenador->id,entrenador->posX,entrenador->posY);
}


void planificar_deadlock(entrenador* entrenador0,entrenador* entrenador1){
	printf("\n Inicio operacion de deadlock \n ");

	log_info(operacion_de_intercambio,"intercambio entre entrenadores %d y %d \n",entrenador0->id,entrenador1->id);
	entrenador_exec = entrenador0;
	list_remove_by_condition(entrenadores_en_deadlock, (void*)entrenador_en_exec);

	log_info(cambioDeCola,"cambio a EXEC de entrenador: %d \n ",entrenador_exec->id);

	mover_entrenador(entrenador0,entrenador1->posX,entrenador1->posY);
	int retardo = leer_retardo_cpu() * 5;

	sleep(retardo); //IRIA sleep(retardo)

	//intercambio
	nombre_pokemon = list_get(entrenador0->objetivos,0);
	char* poke1 = nombre_pokemon;

	list_remove_by_condition(entrenador1->pokemones,(void*)pokemon_repetido);
	list_remove_by_condition(entrenador0->objetivos,(void*)pokemon_repetido);

	nombre_pokemon = list_get(entrenador1->objetivos,0);
	list_remove_by_condition(entrenador0->pokemones,(void*)pokemon_repetido);
	list_remove_by_condition(entrenador1->objetivos,(void*)pokemon_repetido);

	printf("\n Intercambio de %s y %s \n ",poke1, nombre_pokemon);

	entrenador0->ciclos_cpu += 5;

	cant_deadlocks_resueltos+=1;

	analizar_proxima_cola(entrenador0);

	analizar_proxima_cola(entrenador1);
}

//////////////////////////////////////////////// ROUND ROBIN


void planifico_con_RR(void){


while(1){
	while(validacion_nuevo_pokemon()){
		quantum = leer_quantum();

		if(leer_algoritmo_planificacion() == SJFCD){
				planificar_entrenador_segun_rafaga();
		}
		else{
			planificar_entrenador();
		}
		//Seccion critica
		sem_wait(&en_ejecucion);
		cambio_contexto +=1;
		log_info(cambioDeCola,"cambio a EXEC de entrenador: %d \n ",entrenador_exec->id);

		sem_post(&(entrenador_exec->sem_entrenador));
		//Fin de seccion critica
	}


	while(!queue_is_empty(entrenadores_ready)){
		quantum = leer_quantum();

		entrenador_exec = queue_peek(entrenadores_ready);
		queue_pop(entrenadores_ready);

		proximo_objetivo = entrenador_exec->objetivo_proximo;

		sem_wait(&en_ejecucion);
		cambio_contexto +=1;
		log_info(cambioDeCola,"cambio a EXEC de entrenador: %d \n ",entrenador_exec->id);

		sem_post(&(entrenador_exec->sem_entrenador));
	}

	//Para que no se valide tdo el tiempo, tiene un contador validar_deadlock que se aumenta despues de 10 segundos
	if(validar_deadlock && list_is_empty(entrenadores_new)){
		validar_deadlock=0;
		sem_wait(&en_ejecucion);

		log_info(inicio_deadlock,"Inicio de deteccion de deadlock");

		if(hay_deadlock_multiple()){

			log_info(resultado_deadlock,"Se detectó deadlock multiple ");

			entrenador_deadlock-=1;


			pthread_create(&hilo_deadlock,NULL,(void *) manejar_deadlock_multiple,NULL);
			sem_post(&en_ejecucion);
			continue;
		}

		if(hay_deadlock()){
			log_info(resultado_deadlock,"Se detectó deadlock \n");
			entrenador_deadlock-=2;
			cant_deadlocks +=1;


		pthread_create(&hilo_deadlock,NULL,(void *) manejar_deadlock,NULL);

		sem_post(&en_ejecucion);
		continue;

		}
	else{
			log_info(resultado_deadlock,"No se detectó nuevo deadlock \n");

			//espera unos segundos y pone el contador validar_deadlock en 1
			pthread_t espera_deadlock;
			pthread_create(&espera_deadlock,NULL,(void *) espera_de_deadlock,NULL);

		}

		sem_post(&en_ejecucion);
	}


	if(list_size(entrenadores) == list_size(entrenadores_finalizados)){

		pthread_cancel(hilo_servidor);
		printf("\n FINALIZO EL PROGRAMA \n");

		break;
	}


}
}


void mover_entrenador_RR(entrenador* entrenador,int x, int y){

	int tiempo = leer_retardo_cpu();

	while(entrenador->posX != x){
		while(quantum > 0 && entrenador->posX != x){

		if(entrenador->posX < x){
			entrenador->posX = entrenador->posX + 1;
			sleep(tiempo);
			quantum -=1;
			entrenador->ciclos_cpu += 1;
			log_info(movimiento_entrenador,"entrenador %d: se movio a (%d,%d)",entrenador->id,entrenador->posX,entrenador->posY);
		}
		if(entrenador->posX > x){
			entrenador->posX = entrenador->posX -1;
			sleep(tiempo);
			quantum -=1;
			entrenador->ciclos_cpu += 1;
			log_info(movimiento_entrenador,"entrenador %d: se movio a (%d,%d)",entrenador->id,entrenador->posX,entrenador->posY);
		}
		}

		//No tengo mas Quantum, pero mi posicion es distinta
		if(quantum == 0){
			if(leer_algoritmo_planificacion() == SJFCD){
				list_add(lista_entrenadores_ready, entrenador);
			}
			else{
				queue_push(entrenadores_ready, entrenador);
				log_info(cambioDeCola,"cambio a READY de entrenador: %d \n ",entrenador->id);
			}
			sem_post(&en_ejecucion);
			sem_wait(&(entrenador->sem_entrenador));
	}

	}

	while(entrenador->posY != y){
			while(quantum > 0 && entrenador->posY != y){

			if(entrenador->posY < y){
				entrenador->posY = entrenador->posY + 1;
				sleep(tiempo);
				quantum -=1;
				entrenador->ciclos_cpu += 1;
				log_info(movimiento_entrenador,"entrenador %d: se movio a (%d,%d)",entrenador->id,entrenador->posX,entrenador->posY);
			}
			if(entrenador->posY > y){
				entrenador->posY = entrenador->posY -1;
				sleep(tiempo);
				quantum -=1;
				entrenador->ciclos_cpu += 1;
				log_info(movimiento_entrenador,"entrenador %d: se movio a (%d,%d)",entrenador->id,entrenador->posX,entrenador->posY);}
			}
			//No tengo mas Quantum, pero mi posicion es distinta
			if(quantum == 0){
				if(leer_algoritmo_planificacion() == SJFCD){
					list_add(lista_entrenadores_ready, entrenador);
				}
				else{
					queue_push(entrenadores_ready, entrenador);
					log_info(cambioDeCola,"cambio a READY de entrenador: %d \n ",entrenador->id);
				}
				sem_post(&en_ejecucion);
				sem_wait(&(entrenador->sem_entrenador));
		}

		}

}


void planificar_deadlock_RR(entrenador* entrenador0,entrenador* entrenador1) {

	entrenador_exec = entrenador0;
	list_remove_by_condition(entrenadores_en_deadlock, (void*)entrenador_en_exec);

	int cpu_a_usar = 5;

	int x = entrenador1->posX;
	int y = entrenador1->posY;

	sem_wait(&en_ejecucion);

	log_info(operacion_de_intercambio,"intercambio entre entrenadores %d y %d \n",entrenador0->id,entrenador1->id);

	mover_entrenador(entrenador0,x,y);
	//mover_entrenador_RR(entrenador0,x,y);

	int retardo = leer_retardo_cpu(); //*5
	sleep(retardo); //IRIA sleep(retardo)

	//intercambio

	while(cpu_a_usar > quantum){
		cpu_a_usar -= quantum;

		//printf("\n ------ Realizando algoritmo de Deadlock de entrenadores %d y %d ------\n \n",entrenador0->id,entrenador1->id);

		log_info(cambioDeCola,"cambio a READY a entrenador: %d en espera de Intercambio \n ",entrenador0->id);

		queue_push(entrenadores_ready, entrenador0);

		sem_post(&en_ejecucion);

		sem_wait(&(entrenador0->sem_entrenador));

		quantum = leer_quantum();
	}



	nombre_pokemon = list_get(entrenador0->objetivos,0);
	list_remove_by_condition(entrenador1->pokemones,(void*)pokemon_repetido);
	list_remove_by_condition(entrenador0->objetivos,(void*)pokemon_repetido);

	nombre_pokemon = list_get(entrenador1->objetivos,0);
	list_remove_by_condition(entrenador0->pokemones,(void*)pokemon_repetido);
	list_remove_by_condition(entrenador1->objetivos,(void*)pokemon_repetido);

	printf("\n ------ Terminado algoritmo de Deadlock de entrenadores %d y %d ------\n \n",entrenador0->id,entrenador1->id);

	entrenador0->ciclos_cpu += 5; //Le sumo los 5 ciclos al entrenador por deadlock
	// entrenador1->ciclos_cpu += 5;

	cant_deadlocks_resueltos += 1;

	analizar_proxima_cola(entrenador0);
	analizar_proxima_cola(entrenador1);
	validar_deadlock +=1;
	sem_post(&en_ejecucion);
}


bool validacion_nuevo_pokemon(void){
	if(leer_algoritmo_planificacion() == SJFSD || leer_algoritmo_planificacion() == SJFCD){
	return (hay_pokemon_y_entrenador() ||(!queue_is_empty(pokemones_en_el_mapa) && !list_is_empty(lista_entrenadores_block_ready)));
	}else{
	return (hay_pokemon_y_entrenador() || (!queue_is_empty(pokemones_en_el_mapa)  && !queue_is_empty(entrenadores_block_ready))    );

	}
}


void manejar_deadlock_multiple(){
int i,j;
	for(i = 0; i < (list_size(entrenadores_en_deadlock)-1);i++){
			for(j = 1; j < (list_size(entrenadores_en_deadlock)); j++){
				entrenador* entrenador0 = list_get(entrenadores_en_deadlock,i); // otro for para comparar con el resto
				entrenador* entrenador1 = list_get(entrenadores_en_deadlock,j);

				nombre_pokemon = list_get(entrenador0->objetivos,0);

				//Si algun pokemon del 1 es el que 0 necesita, se planifican:
				if(list_any_satisfy(entrenador1->pokemones,(void*)pokemon_repetido)){

							planificar_deadlock_multiple(entrenador0,entrenador1);

							cant_deadlocks+=1;


							break;
					}
					printf(" \n No se puede manejar el deadlock con entrenador:%d y entrenador:%d \n",entrenador0->id,entrenador1->id);

				}
			break;
		}

	}

void planificar_deadlock_multiple(entrenador* entrenador0,entrenador* entrenador1){


		entrenador_exec = entrenador0;

		list_remove_by_condition(entrenadores_en_deadlock, (void*)entrenador_en_exec);

		int cpu_a_usar = 5;

		int x = entrenador1->posX;
		int y = entrenador1->posY;

		sem_wait(&en_ejecucion);

		log_info(operacion_de_intercambio,"intercambio entre entrenadores %d y %d",entrenador0->id,entrenador1->id);

		//mover_entrenador_RR(entrenador0,x,y);
		mover_entrenador(entrenador0,x,y);


		sleep(leer_retardo_cpu());

		//intercambio

		while(cpu_a_usar > quantum){
			cpu_a_usar -= quantum;


			log_info(cambioDeCola,"cambio a READY a entrenador: %d en espera de Intercambio \n ",entrenador0->id);

			if(leer_algoritmo_planificacion() == SJFCD) list_add(lista_entrenadores_ready,entrenador0);
			else{
			queue_push(entrenadores_ready, entrenador0);
			}

			sem_post(&en_ejecucion);

			sem_wait(&(entrenador0->sem_entrenador));

			quantum = leer_quantum();
		}


		nombre_pokemon = list_get(entrenador0->objetivos,0);


		list_remove_by_condition(entrenador0->objetivos,(void*)pokemon_repetido);
		list_remove_by_condition(entrenador1->pokemones,(void*)pokemon_repetido);

		nombre_pokemon = list_get(entrenador0->pokemones, 0);

		list_remove_by_condition(entrenador0->pokemones,(void*)pokemon_repetido);
		list_add(entrenador1->pokemones,nombre_pokemon);

		printf("\n ------ Terminado algoritmo de Deadlock de entrenadores %d y %d ------\n \n",entrenador0->id,entrenador1->id);

		entrenador0->ciclos_cpu += 5;

		cant_deadlocks_resueltos += 1;

		analizar_proxima_cola(entrenador0);

		validar_deadlock +=1;

		sem_post(&en_ejecucion);
}

//////////////////////////////////////////////// SJFSD


void planificar_entrenador_segun_rafaga(void){

	sem_wait(&hay_entrenador);

	proximo_objetivo = queue_peek(pokemones_en_el_mapa);
	queue_pop(pokemones_en_el_mapa); //Si no lo atrapo se vuelve a poner

	//Juntamos los entrenadores en new y en block_ready, para ver quien esta mas cerca del pokemon


	if(!list_is_empty(entrenadores_new)){
		entrenador_exec = list_get(list_sorted(entrenadores_new,(void*) primer_entrenador_mas_cerca_de_pokemon) ,0);
		list_remove_by_condition(entrenadores_new,(void*)entrenador_en_exec);

		}
	/*
	else{ //PLANIFICO LOS DE LA LISTA BLOCK_READY
		entrenador_exec = list_get(list_sorted(lista_entrenadores_block_ready,(void*) entrenador_con_menor_rafaga) ,0);
		list_remove_by_condition(lista_entrenadores_block_ready,(void*)entrenador_en_exec);
	} */

	else{

		entrenador_exec = list_get(list_sorted(lista_entrenadores_block_ready,(void*) entrenador_con_menor_rafaga) ,0);

		list_remove_by_condition(lista_entrenadores_block_ready,(void*)entrenador_en_exec);

		entrenador_exec->rafaga_estimada = calcular_rafaga_siguiente(entrenador_exec,proximo_objetivo);

		}

	entrenador_exec->objetivo_proximo = proximo_objetivo;

	float distancia_real = distancia_entrenador_pokemon(entrenador_exec,entrenador_exec->objetivo_proximo);

	entrenador_exec->rafaga_real = distancia_real;

	sem_post(&(entrenador_exec->nuevoPoke));

}

bool entrenador_con_menor_rafaga(entrenador* entrenador1, entrenador* entrenador2){

	float rafaga1 = calcular_rafaga_siguiente(entrenador1,proximo_objetivo);
	float rafaga2 = calcular_rafaga_siguiente(entrenador2,proximo_objetivo);
	//bool resultado =  rafaga1 >= rafaga2 ;
	//bool resultado =  rafaga1 <= rafaga2 ;
	bool resultado =  rafaga1 < rafaga2 ;
	return resultado;
}

float calcular_rafaga_siguiente(entrenador* un_entrenador, pokemon* poke){
	//Rafaga siguiente: alpha*rafaga_estimada_anterior + (1-aplha)*rafaga_real_anterior

	return un_entrenador->rafaga_estimada * alpha + (1-alpha) * un_entrenador->rafaga_real ;
}

//////////////////////////////////////////////// SJFCD

void planifico_con_desalojo(void){

while(1){
	while(validacion_nuevo_pokemon()){
		quantum = leer_quantum();

		planificar_entrenador_segun_rafaga();

		//Seccion critica
		sem_wait(&en_ejecucion);
		cambio_contexto +=1;
		log_info(cambioDeCola,"cambio a EXEC de entrenador: %d \n ",entrenador_exec->id);

		sem_post(&(entrenador_exec->sem_entrenador));
		//Fin de seccion critica
	}

/*
	while(!queue_is_empty(entrenadores_ready)){
		quantum = leer_quantum();

		entrenador_exec = queue_peek(entrenadores_ready);
		queue_pop(entrenadores_ready);

		proximo_objetivo = entrenador_exec->objetivo_proximo;

		sem_wait(&en_ejecucion);
		cambio_contexto +=1;
		log_info(cambioDeCola,"cambio a EXEC de entrenador: %d \n ",entrenador_exec->id);

		sem_post(&(entrenador_exec->sem_entrenador));
	}
*/

	while(!list_is_empty(lista_entrenadores_ready)){
		quantum = leer_quantum();

		sem_wait(&en_ejecucion);

		entrenador* anterior_entrenador = entrenador_exec;

		entrenador_exec = list_get(list_sorted(lista_entrenadores_ready,(void*) entrenador_con_menor_rafaga) ,0);

		list_remove_by_condition(lista_entrenadores_ready,(void*)entrenador_en_exec);

		entrenador_exec->rafaga_estimada = calcular_rafaga_siguiente(entrenador_exec,entrenador_exec->objetivo_proximo);

		//float distancia_real = distancia_entrenador_pokemon(entrenador_exec,entrenador_exec->objetivo_proximo);

		//entrenador_exec->rafaga_real = distancia_real;

		if(anterior_entrenador->id != entrenador_exec->id){
			log_info(cambioDeCola,"cambio a READY a entrenador: %d \n ",anterior_entrenador->id);
			cambio_contexto +=1;
			log_info(cambioDeCola,"cambio a EXEC de entrenador: %d \n ",entrenador_exec->id);
		}

		sem_post(&(entrenador_exec->sem_entrenador));


	}


	if(validar_deadlock && list_is_empty(entrenadores_new)){
			validar_deadlock=0;
			sem_wait(&en_ejecucion);

			log_info(inicio_deadlock,"Inicio de deteccion de deadlock");

			if(hay_deadlock_multiple()){

				log_info(resultado_deadlock,"Se detectó deadlock multiple");

				entrenador_deadlock-=1;


				pthread_create(&hilo_deadlock,NULL,(void *) manejar_deadlock_multiple,NULL);
				sem_post(&en_ejecucion);
				continue;
			}

			if(hay_deadlock()){
				log_info(resultado_deadlock,"Se detectó deadlock");
				entrenador_deadlock-=2;



			pthread_create(&hilo_deadlock,NULL,(void *) manejar_deadlock,NULL);
			cant_deadlocks +=1;

			sem_post(&en_ejecucion);
			continue;

			}
		else{
				log_info(resultado_deadlock,"No se detectó nuevo deadlock");

				//espera unos segundos y pone el contador validar_deadlock en 1
				pthread_t espera_deadlock;
				pthread_create(&espera_deadlock,NULL,(void *) espera_de_deadlock,NULL);

			}

			sem_post(&en_ejecucion);
		}



	if(list_size(entrenadores) == list_size(entrenadores_finalizados)){

		pthread_cancel(hilo_servidor);
		printf("\n FINALIZO EL PROGRAMA \n");

		break;
	}


}
}


/////////////////////////////////////////////////METRICAS

void cpu_por_entrenador(void){
	int i;
	for (i= 0; i < list_size(entrenadores); i++){
		entrenador* entrenador = list_get(entrenadores, i);
		printf("Ciclos de cpu entrenador %d: %d \n", entrenador->id, entrenador->ciclos_cpu);
	}
}

void cpu_team(void){

	int cpu_totales = 0;
	int i;
	for(i= 0; i < list_size(entrenadores); i++){
	entrenador* entrenador = list_get(entrenadores, i);
	cpu_totales += entrenador->ciclos_cpu;
	}
	printf("Ciclos de cpu del team: %d \n", cpu_totales);
}

////////////////////////////////////RESPUESTAS DEL CAUGHT

void enviar_catch(entrenador* un_entrenador, broker_catch_pokemon *catchAEnviar, int socketAEnviar) {
	//catchAEnviar=malloc(sizeof(broker_catch_pokemon));

	//int socketAEnviar = crear_conexion(IP_BROKER, PUERTO_BROKER);

	if (socketAEnviar) {

		t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
		paquete_a_enviar->codigo_operacion = BROKER__CATCH_POKEMON;
		paquete_a_enviar->tamanio_username = strlen(username) + 1;
		paquete_a_enviar->username = malloc(paquete_a_enviar->tamanio_username);
		paquete_a_enviar->username = username;

		//serializacion de brokerNewPokemon
		t_buffer* buffer = malloc(sizeof(t_buffer));
		serializar_broker_catch_pokemon(catchAEnviar, buffer);

		paquete_a_enviar->buffer = buffer;

		int tamanio_buffer = 0;

		void* bufferStream = serializar_paquete(paquete_a_enviar,&tamanio_buffer);
		send(socketAEnviar, bufferStream, tamanio_buffer, 0);
		//llenamos el ID del catch que enviamos
		recv(socketAEnviar, &(catchAEnviar->id), sizeof(uint32_t), 0);


		//nos guardamos el id en el entrenador


		free(bufferStream);
		free(paquete_a_enviar->buffer);
		free(paquete_a_enviar);
		//liberar_conexion(socketAEnviar);
	}

}


void enviar_get(char* nombrePokemon, broker_get_pokemon *getAEnviar) {

	int socketAEnviar = crear_conexion(IP_BROKER, PUERTO_BROKER);

	if (socketAEnviar) {

		t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
		paquete_a_enviar->codigo_operacion = BROKER__GET_POKEMON;
		paquete_a_enviar->tamanio_username = strlen(username) + 1;
		paquete_a_enviar->username = malloc(paquete_a_enviar->tamanio_username);
		paquete_a_enviar->username = username;

		getAEnviar->datos->tamanioNombre = strlen(nombrePokemon) + 1;
		getAEnviar->datos->nombrePokemon = malloc(
				getAEnviar->datos->tamanioNombre);
		getAEnviar->datos->nombrePokemon = nombrePokemon;

		//serializacion de brokerNewPokemon
		t_buffer* buffer = malloc(sizeof(t_buffer));

		serializar_broker_get_pokemon(getAEnviar, buffer);

		paquete_a_enviar->buffer = buffer;

		int tamanio_buffer = 0;

		void* bufferStream = serializar_paquete(paquete_a_enviar,
				&tamanio_buffer);

		send(socketAEnviar, bufferStream, tamanio_buffer, 0);

		//llenamos el ID del catch que enviamos
		recv(socketAEnviar, &(getAEnviar->id), sizeof(uint32_t), 0);


		sem_wait(&mutex_lista);

		int id = getAEnviar->id;
		list_add(lista_ids_get, (void *)id);

		sem_post(&mutex_lista);

		free(bufferStream);


		free(paquete_a_enviar->buffer);
		free(paquete_a_enviar);

	}

	//printf("Mande un GET con el socket: %d",socketAEnviar);
	//fflush(stdout);

	//liberar_conexion(socketAEnviar);

	sleep(1);

}


void confirmacion_de_catch(entrenador* un_entrenador){

	proximo_objetivo = un_entrenador->objetivo_proximo;


	dictionary_put(objetivo_global,proximo_objetivo->nombre,dictionary_get(objetivo_global,proximo_objetivo->nombre)-1);

	bool tiene_objetivo = list_any_satisfy(un_entrenador->objetivos , (void*) es_de_especie);

	if(tiene_objetivo){
		list_remove_by_condition(un_entrenador->objetivos , (void*) es_de_especie);
	}else{
		list_add(un_entrenador->pokemones,proximo_objetivo->nombre);
	}

	list_add(pokemones_atrapados,proximo_objetivo);


	disminuir_cuantos_puede_cazar(un_entrenador);

	// aca hago que ya no este mas en blocked, sino que en ready

	desbloquear_entrenador(un_entrenador);

	if(leer_algoritmo_planificacion() == SJFCD) list_add(lista_entrenadores_ready, un_entrenador);
	else{
		queue_push(entrenadores_ready,un_entrenador);
	}


	sem_post(&(un_entrenador->espera_de_catch));

}


void denegar_catch(entrenador* un_entrenador){
	log_info(llegadaDeMensaje,"No se agarró al pokemon %s", un_entrenador->objetivo_proximo->nombre);

	desbloquear_entrenador(un_entrenador);

	if(leer_algoritmo_planificacion() == SJFCD) list_add(lista_entrenadores_ready, un_entrenador);
		else{
			queue_push(entrenadores_ready,un_entrenador);
		}

	sem_post(&(un_entrenador->espera_de_catch));

}



/////////////////////////////////////////FUNCIONES AUX//////////////////////////////////////////////////////


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
        return config_create(archivo_config);
    }


void disminuir_cuantos_puede_cazar(entrenador* un_entrenador){
	int puede_cazar = un_entrenador->cuantos_puede_cazar;
	puede_cazar = puede_cazar - 1;
	un_entrenador->cuantos_puede_cazar = puede_cazar;
}

bool es_de_especie(char* nombre_poke){
	return !strcmp(nombre_poke,proximo_objetivo->nombre);
}

bool pokemon_repetido(char* nombre){
	return !strcmp(nombre,nombre_pokemon);
}

bool puede_cazar(entrenador* entrenador){
	return entrenador->cuantos_puede_cazar > 0;
}


bool primer_entrenador_mas_cerca_de_pokemon(entrenador* entrenador1, entrenador* entrenador2){
	int dist1 = distancia_entrenador_pokemon(entrenador1,proximo_objetivo);
	int dist2 = distancia_entrenador_pokemon(entrenador2,proximo_objetivo);

	bool resultado;
	if(dist1 == dist2) resultado = true;
	else{
	 resultado = distancia_entrenador_pokemon(entrenador1,proximo_objetivo) <= distancia_entrenador_pokemon(entrenador2,proximo_objetivo);
	}

	return resultado;
}

int distancia_entrenador_pokemon(entrenador* un_entrenador, pokemon* un_pokemon){

	int pos_entX =  (un_entrenador->posX);
	int pos_pokX = (un_pokemon->posX);
	int restaX = pos_entX - pos_pokX;

	int pos_entY =  (un_entrenador->posY);
	int pos_pokY = (un_pokemon->posY);
	int restaY = pos_entY - pos_pokY;

	double x_final = fabs(restaX);
	double y_final = fabs(restaY);

	return (int) (x_final + y_final);
}

//SACA OBJETIVO GLOBAL
void quitar_un_objetivo(char* pokemon_a_quitar){
	bool pokemon_repetido(char* nombre){
		return !strcmp(nombre,nombre_pokemon);
	}
	dictionary_put(objetivo_global, pokemon_a_quitar,dictionary_get(objetivo_global,pokemon_a_quitar)-1);
}


bool entrenador_en_exec(entrenador* un_entrenador){
	return (un_entrenador->id) == (entrenador_exec->id);
}

bool cumplio_objetivo(entrenador* un_entrenador){
	return list_is_empty(un_entrenador->objetivos);
}


bool hay_deadlock(void){

	return (entrenador_deadlock==2);
}

bool hay_deadlock_multiple(void){
	return (entrenador_deadlock>2);
}


void analizar_proxima_cola(entrenador* un_entrenador){

	if(puede_cazar(un_entrenador)){

		log_info(cambioDeCola,"cambio a BLOCK-READY de entrenador: %d \n ",un_entrenador->id);

		if(leer_algoritmo_planificacion() == SJFSD || leer_algoritmo_planificacion() == SJFCD){
			list_add(lista_entrenadores_block_ready,un_entrenador);
			}

		else{
		queue_push(entrenadores_block_ready,un_entrenador);
		}

	}
	if(!puede_cazar(un_entrenador)){
			if(cumplio_objetivo(un_entrenador)){
				list_add(entrenadores_finalizados,un_entrenador);
				log_info(cambioDeCola,"cambio a EXIT de entrenador: %d \n ",un_entrenador->id);
			}

			else{
				list_add(entrenadores_en_deadlock,un_entrenador);
				log_info(cambioDeCola,"cambio a BLOCK-DEADLOCK de entrenador: %d \n ",un_entrenador->id);
				entrenador_deadlock+=1;
			}
	}
}

void bloquear_entrenador(entrenador* un_entrenador){
	list_add(entrenadores_blocked,un_entrenador);
	log_info(cambioDeCola,"Cambio a BLOCKED de entrenador: %d \n ",un_entrenador->id);
}



void desbloquear_entrenador(entrenador* un_entrenador){
	entrenador_a_eliminar = un_entrenador;
	list_remove_by_condition(entrenadores_blocked,(void*)entrenador_eliminado);
}

bool entrenador_eliminado(entrenador* un_entrenador){
	return (un_entrenador->id) == (entrenador_a_eliminar->id);
}

bool hay_pokemon_y_entrenador(){
	return (!queue_is_empty(pokemones_en_el_mapa) && !list_is_empty(entrenadores_new));
}


///////////////////////////////////////CONEXIONES///////////////////////////////////////


//ESTE ES EL SV PARA RECIBIR MENSAJES
void iniciar_servidor(void)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP_TEAM, PUERTO_TEAM, &hints, &servinfo);

    int activado = 1;
   	setsockopt(socket_servidor,SOL_SOCKET,SO_REUSEADDR,&activado,sizeof(activado));


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

    //esperar_cliente(socket_servidor);

    while(1){
    		esperar_cliente(socket_servidor);
    }

}

int conectarse_con_broker(void){

	conexionBroker = crear_conexion(IP_BROKER,PUERTO_BROKER);
	if(conexionBroker <= 0){;
			return -1;
		}
		else{
			return conexionBroker;
		}

}


void conexion_broker(void){

	conexionBroker = crear_conexion(IP_BROKER,PUERTO_BROKER);

	if(conexionBroker <= 0){
		log_info(comunicacion_broker_error,"No se pudo conectar con Broker,se realizará la operación por default \n");
		sleep(leer_tiempo_de_reconexion());

		conexion_broker();
	}

	else{
		log_info(comunicacion_broker_resultado,"Conectado con Broker \n");
	}


}


void esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	//poner en globales si es necesario
	pthread_t thread;


	socklen_t  tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	int *socketApasar =malloc(sizeof(int));
	*socketApasar = socket_cliente;

	pthread_create(&thread,NULL,(void*)serve_client,socketApasar);
	pthread_detach(thread);

}

void serve_client(int* socketQueNosPasan)
{
	int socket = *socketQueNosPasan;
	free(socketQueNosPasan);
	//sem_wait(&semaforo_mensaje);
	//pthread_mutex_lock(&llegadaMensajesTHREAD);
	int cod_op;
	int i = recv(socket, &cod_op, sizeof(op_code), MSG_WAITALL);
	if(i <= 0)
		cod_op = -1;
	process_request(cod_op, socket);
	//pthread_mutex_unlock(&llegadaMensajesTHREAD);
	//sem_post(&semaforo_mensaje);
}



void process_request(int cod_op, int cliente_fd) {
	uint32_t tamanio_buffer;
	uint32_t tamanio_username;
	team_appeared_pokemon* appearedRecibido;
	team_caught_pokemon* caughtRecibido;
	broker_localized_pokemon* localizedRecibido;
	broker_appeared_pokemon* appearedRecibidoBROKER;
	uint32_t id;
	pokemon* nuevoPoke;
	char* username;
	int envio_de_ack;

	char* resolucionCatch;

	bool esIDRecibido(uint32_t idIterado){
		return (idIterado == id);
	}



	recv(cliente_fd,&tamanio_username,sizeof(uint32_t),MSG_WAITALL);

	username = malloc(tamanio_username);
	recv(cliente_fd,username,tamanio_username,MSG_WAITALL);

	recv(cliente_fd, &tamanio_buffer, sizeof(uint32_t), MSG_WAITALL);
	//falta los case de los otros tipos de mensajes (get,catch,caught)(localized lo dejamos para despues(es de GameCard)
	switch (cod_op) {

	//sem_wait(&semaforo_mensaje);

		case TEAM__APPEARED_POKEMON:

			pthread_mutex_lock(&llegadaMensajesTHREAD);

			appearedRecibido = deserializar_team_appeared_pokemon(cliente_fd);

			log_info(llegadaDeMensaje,"recibi mensaje appeared pokemon de %s:  \n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d \n",username, appearedRecibido->datos->tamanioNombre, appearedRecibido->datos->nombrePokemon, appearedRecibido->datos->posX, appearedRecibido->datos->posY);

			nuevoPoke = hacer_pokemon(appearedRecibido->datos->nombrePokemon,appearedRecibido->datos->posX,appearedRecibido->datos->posY,appearedRecibido->datos->tamanioNombre);

			aparece_nuevo_pokemon(nuevoPoke);
		free(appearedRecibido);

			pthread_mutex_unlock(&llegadaMensajesTHREAD);

			break;

		case BROKER__CAUGHT_POKEMON:
			//recv(cliente_fd,&(id),sizeof(uint32_t),0);
			pthread_mutex_lock(&llegadaMensajesTHREAD);

			caughtRecibido = deserializar_team_caught_pokemon(cliente_fd);

			envio_de_ack = crear_conexion(IP_BROKER, PUERTO_BROKER);

			if (envio_de_ack != -1) {

			enviarACK(caughtRecibido->id, envio_de_ack, "TEAM");
			//liberar_conexion(envio_de_ack);
			}

			//Comparo entre los IDS de los entrenadores si existe el recibido
			uint32_t id_recibido = caughtRecibido->id_relativo;
			int i;
			for(i = 0; i < list_size(entrenadores);i++){
				entrenador* un_entrenador = list_get(entrenadores,i);
				if(un_entrenador->id_catch == id_recibido){
					un_entrenador->id_catch = 0;
				if(caughtRecibido->datos->puedoAtraparlo){
					resolucionCatch = "se confirma el catch";
				}
				else{ resolucionCatch = "se deniega el catch"; }

			log_info(llegadaDeMensaje,"recibi mensaje CAUGHT de BROKER con id relativo %d:, "
					"id: %d y %s \n",caughtRecibido->id_relativo,caughtRecibido->id,resolucionCatch);

					if(caughtRecibido->datos->puedoAtraparlo) confirmacion_de_catch(un_entrenador);
					else { denegar_catch(un_entrenador); }

					break;
				}else {
					printf("No me sirvio el mensaje\n");
					fflush(stdout);
				}

			}
			free(caughtRecibido);

			pthread_mutex_unlock(&llegadaMensajesTHREAD);

			break;

		case BROKER__LOCALIZED_POKEMON:
			pthread_mutex_lock(&llegadaMensajesTHREAD);

			recv(cliente_fd,&(id),sizeof(uint32_t),0);
			localizedRecibido = deserializar_localized_pokemon(cliente_fd);
			localizedRecibido->id= id;

			envio_de_ack = crear_conexion(IP_BROKER,PUERTO_BROKER);

			if(envio_de_ack != -1){

			enviarACK(localizedRecibido->id,envio_de_ack,"TEAM");
			//liberar_conexion(envio_de_ack);
			}

			id = localizedRecibido->id_relativo;

			//sleep(2);

			sem_wait(&mutex_lista);

			if(list_any_satisfy(lista_ids_get,(void*) esIDRecibido)){

			log_info(llegadaDeMensaje,"recibi mensaje LOCALIZED de BROKER con id relativo: %d, id: %d , nombre: %s, y cantidad de posiciones %d:\n"
			,id,localizedRecibido->id ,localizedRecibido->datos->nombrePokemon, localizedRecibido->datos->cantidadPosiciones);

				int i;
				//Hacemos un appeared por cada pokemon que nos mandan
				for(i = 0; i < localizedRecibido->datos->cantidadPosiciones;i++){

					nuevoPoke = hacer_pokemon(localizedRecibido->datos->nombrePokemon,localizedRecibido->datos->posX[i],localizedRecibido->datos->posY[i],localizedRecibido->datos->tamanioNombre);
					aparece_nuevo_pokemon(nuevoPoke);
				}

			}
			free(localizedRecibido);
			sem_post(&mutex_lista);
			//aca no se muy bien que comparación vamos a hacer
			pthread_mutex_unlock(&llegadaMensajesTHREAD);

			break;

		case BROKER__APPEARED_POKEMON:

			pthread_mutex_lock(&llegadaMensajesTHREAD);

			recv(cliente_fd,&(id),sizeof(uint32_t),0);
			appearedRecibidoBROKER = deserializar_appeared_pokemon(cliente_fd);
			appearedRecibidoBROKER->id= id;

			envio_de_ack = crear_conexion(IP_BROKER,PUERTO_BROKER);

			if(envio_de_ack != -1){

			enviarACK(appearedRecibidoBROKER->id,envio_de_ack,"TEAM");
			//liberar_conexion(envio_de_ack);
			}

			log_info(llegadaDeMensaje,"recibi mensaje appeared pokemon de BROKER:  \n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d \n, con"
					"id relativo: %d e id: %d \n. ",
			appearedRecibidoBROKER->datos->tamanioNombre, appearedRecibidoBROKER->datos->nombrePokemon, appearedRecibidoBROKER->datos->posX, appearedRecibidoBROKER->datos->posY,appearedRecibidoBROKER->id_relativo,appearedRecibidoBROKER->id);

			//sleep(2);

			nuevoPoke = hacer_pokemon(appearedRecibidoBROKER->datos->nombrePokemon,appearedRecibidoBROKER->datos->posX,appearedRecibidoBROKER->datos->posY,appearedRecibidoBROKER->datos->tamanioNombre);

			aparece_nuevo_pokemon(nuevoPoke);

			free(appearedRecibidoBROKER);
			pthread_mutex_unlock(&llegadaMensajesTHREAD);

			break;

		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
		}


}

//MODELO DE COMO RECIBIR MENSAJE, NO LO USAMOS PARA BROKER

void* recibir_mensaje(int socket_cliente, int* size)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}


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

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) != 0){
		//printf("error");
		freeaddrinfo(server_info);
		return -1;
	}


	freeaddrinfo(server_info);

	return socket_cliente;
}


void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}


///////////////////////////////////////LOGS///////////////////////////////////////

 t_log* iniciar_log(char* proceso){
	t_config* config = leer_config();
	char* archivo = config_get_string_value(config,"LOG_FILE");
   	return log_create(archivo,proceso,1,LOG_LEVEL_INFO);
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
 	 if(strcmp(algoritmo_planificacion,"SJFSD") == 0) return SJFSD;
 	 if(strcmp(algoritmo_planificacion,"SJFCD") == 0) return SJFCD;

 return 0;
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

 float leer_alpha(void){

 	 char* alpha_char = config_get_string_value(config,"ALPHA");

 	 char** array_alpha = string_split(alpha_char,".");

 	 int parte_entera = atoi(array_alpha[0]);
 	 int parte_decimal = atoi(array_alpha[1]);

 	 float alpha = parte_entera + 0.1 * parte_decimal;

 	  return alpha;
 }


