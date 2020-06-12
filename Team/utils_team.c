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
         return config_create(archivo_config);
     }


void variables_globales(){
	config = leer_config();
	broker_conectado = false;

	hacer_entrenadores();
	calcular_objetivo_global();

	pokemones_en_el_mapa = queue_create();

	pokemones_atrapados= list_create();

	entrenadores_block_ready = queue_create();
	entrenadores_finalizados = list_create();
	entrenadores_en_deadlock = list_create();
	entrenadores_blocked = queue_create();





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

 /////////////////////////////////////////////////////////////////////////////




 entrenador* configurar_entrenador(char* posicion,char* pokemonsconfig, char* objetivosconfig, int id_creada){
	 entrenador* un_entrenador = malloc(sizeof(entrenador));


	 sem_init(&(un_entrenador->sem_entrenador),0,0);
	 sem_init(&(un_entrenador->espera_de_catch),0,0);

	 pthread_t hiloEntrenador;


	 un_entrenador->objetivos =crear_lista(string_split(objetivosconfig,"|"));
	 un_entrenador->pokemones =crear_lista(string_split(pokemonsconfig,"|"));


	 un_entrenador->id = id_creada;

	 sacar_pokemones_repetidos(un_entrenador->objetivos,un_entrenador->pokemones);

	 un_entrenador->cuantos_puede_cazar = list_size(un_entrenador->objetivos);
	 t_list* posiciones = crear_lista(string_split(posicion,"|"));
	 un_entrenador->posX = atoi(list_get(posiciones,0));
	 un_entrenador->posY = atoi(list_get(posiciones,1));

	 un_entrenador->hiloDeEntrenador = pthread_create(&hiloEntrenador,NULL,(void*) procedimiento_de_caza,un_entrenador);

	 return un_entrenador;
 }


void sacar_pokemones_repetidos(t_list* objetivos, t_list* pokemones){
	for(int i = 0; i < list_size(pokemones);i++){
		nombre_pokemon = list_get(pokemones,i);
		//printf("voy sacando a: %s       ",nobmre_objetivoconfig);

		list_remove_by_condition(objetivos,(void*) pokemon_repetido);

	}
}





void hacer_entrenadores(void){

	entrenadores = list_create();
	entrenadores_en_ready = list_create();

	t_list* posiciones = obtener_lista_posiciones();
	 t_list* pokemones = obtener_lista_pokemones();
	 t_list* objetivos = obtener_lista_objetivos();


	 for(int i=0 ; i< list_size(posiciones) ; i++){
		entrenador* entrenador_listo = configurar_entrenador(list_get(posiciones,i),list_get(pokemones,i),list_get(objetivos,i),i);
		list_add(entrenadores,entrenador_listo);
		list_add(entrenadores_en_ready,entrenador_listo);
		sem_post(&hay_entrenador);
	 }


 }


 pokemon* hacer_pokemon(char* nombre, uint32_t posX, uint32_t posY){
	 pokemon* poke = malloc(sizeof(pokemon));
	 poke->nombre = nombre;
	 poke->posX = posX;
	 poke->posY = posY;
	 poke->tamanio_nombre = sizeof(nombre);
	 return poke;
 }//tenemos que poner un free


void calcular_objetivo_global(void){

	objetivo_global = dictionary_create();
	printf("Objetivo Global Inicial:  ");
	for(int i = 0; i < list_size(entrenadores);i++){
		//agarro el primer entrenador:
		entrenador* un_entrenador = list_get(entrenadores,i);


		for(int j = 0; j< list_size(un_entrenador->objetivos);j++){
			//pongo sus objetivos en el dictionary:
			char* pokemon_a_agregar = list_get(un_entrenador->objetivos,j);
			agregar_un_objetivo(pokemon_a_agregar);

			printf("  %s ",pokemon_a_agregar);

		}
	}
	printf(" \n");


}




//La función que se pasa por paremtro recibe (char* key, void* value)

void agregar_un_objetivo(char* pokemon_a_agregar){

	if(dictionary_has_key(objetivo_global, pokemon_a_agregar)){ //si el pokemon ya estaba en el diccionario, le sumo 1 a su cantidad
		dictionary_put(objetivo_global,pokemon_a_agregar, dictionary_get(objetivo_global,pokemon_a_agregar)+1);
	}
	else{
		dictionary_put(objetivo_global, pokemon_a_agregar,(void*)1); // 1 o puntero??

	}
}


void mover_entrenador(entrenador* entrenador,int x, int y){

	int tiempo = leer_retardo_cpu();
	while(entrenador->posX != x){
		if(entrenador->posX < x){
			entrenador->posX = entrenador->posX + 1;
			//sleep(tiempo);
		}
		else {
			entrenador->posX = entrenador->posX -1;

			//sleep(tiempo);
		}
	}

	while(entrenador->posY != y){
		if(entrenador->posY < y){
			entrenador->posY = entrenador->posY + 1;
			//sleep(tiempo);
		}
		else {
			entrenador->posY = entrenador->posY -1;
			//sleep(tiempo);
		}
	}

	sleep(tiempo);

	log_info(movimiento_entrenador,"entrenador %d: se movio a (%d,%d)",entrenador->id,entrenador->posX,entrenador->posY);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void algoritmo_aplicado(void){
	switch (leer_algoritmo_planificacion()){
	case FIFO:
		planifico_con_fifo();
		break;

	case RR:
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

	sem_wait(&(un_entrenador->sem_entrenador));
	log_info(cambioDeCola,"cambio a EXEC de entrenador: %d \n ",entrenador_exec->id);

	mover_entrenador(un_entrenador,un_entrenador->objetivo_proximo->posX,un_entrenador->objetivo_proximo->posY);


	log_info(operacion_de_atrapar,"ATRAPAR POKEMON: %s con posicion (%d, %d)",un_entrenador->objetivo_proximo ->nombre,un_entrenador->objetivo_proximo ->posX,un_entrenador->objetivo_proximo ->posY);


	//enviar_mensaje(broker, CATCH pokemon objetivo_propio


	bloquear_entrenador(un_entrenador);

	log_info(cambioDeCola,"Cambio a BLOCKED de entrenador: %d \n ",un_entrenador->id);



	esperar_respuesta_catch(un_entrenador);

	sem_post(&en_ejecucion);
	//libera la ejecucion mientras espera el catch

	sem_wait(&(un_entrenador->sem_entrenador));
	log_info(cambioDeCola,"cambio a EXEC de entrenador: %d \n ",un_entrenador->id);

	confirmacion_de_catch(un_entrenador); //PRUBA: SOLO EN CASO DEFAULT

	sem_wait(&(un_entrenador->espera_de_catch)); //Espera que le llegue al sistema una respuesta a su catch



	analizar_proxima_cola(un_entrenador); //ANALIZA A QUE COLA O LISTA SE MUEVE


	printf(" \n Entrenador %d: , puede cazar:%d \n",un_entrenador->id, un_entrenador->cuantos_puede_cazar);



	sem_post(&en_ejecucion);
	sem_post(&hay_entrenador);


}

}
//Se ejecuta cuando recibimos un mensaje de appeared_pokemon

void aparece_nuevo_pokemon(pokemon* poke){

	 if (dictionary_has_key(objetivo_global, poke->nombre) && (dictionary_get(objetivo_global, poke->nombre) > 0) ){
		 //ME SIRVE EL POKEMON
		 queue_push(pokemones_en_el_mapa,poke);

	 }

	 else{
		 printf("\n El pokemon %s no pertenece al objetivo global. Se descarta \n", poke->nombre);


	 }


}

void terminar_ejecucion_entrenador(void){
	entrenador_exec = queue_peek(entrenadores_blocked);
	queue_pop(entrenadores_blocked);
	proximo_objetivo = entrenador_exec->objetivo_proximo;
	//log_info(cambioDeCola,"cambio a EXEC de entrenador: %d \n ",entrenador_exec->id);
}


void planificar_entrenador(void){

	sem_wait(&hay_entrenador);

	proximo_objetivo = queue_peek(pokemones_en_el_mapa);
	queue_pop(pokemones_en_el_mapa); //Si no lo atrapo se vuelve a poner

	if(!list_is_empty(entrenadores_en_ready)){
	entrenador_exec = list_get(list_sorted(entrenadores_en_ready,(void*) primer_entrenador_mas_cerca_de_pokemon) ,0);
	list_remove_by_condition(entrenadores_en_ready,(void*)entrenador_en_exec);
	}
	else{
		entrenador_exec = queue_peek(entrenadores_block_ready);
		queue_pop(entrenadores_block_ready);
	}

	entrenador_exec->objetivo_proximo = proximo_objetivo;




}


void planifico_con_fifo(void){

while(1){

//Se fija si hay nuevos pokemones, y alguien para cazarlos
while(validacion_nuevo_pokemon()){

	planificar_entrenador(); //planifico uno en cada ciclo del fifo

	//Seccion critica
	sem_wait(&en_ejecucion);

	sem_post(&(entrenador_exec->sem_entrenador));

	sleep(2);

	//Fin de seccion critica

}

while(queue_size(entrenadores_blocked) > 0){

terminar_ejecucion_entrenador();
sem_wait(&en_ejecucion);
sem_post(&(entrenador_exec->sem_entrenador));

}


while(hay_deadlock()){
	sem_wait(&en_ejecucion);
	manejar_deadlock();
	sem_post(&en_ejecucion);
}

}
}

//Considero que los pokemones pueden estar guardados desde el archivo config

void manejar_deadlock(void){

	for(int i = 0; i < (list_size(entrenadores_en_deadlock)-1);i++){
		for(int j = 1; j < (list_size(entrenadores_en_deadlock)); j++){
			entrenador* entrenador0 = list_get(entrenadores_en_deadlock,i); // otro for para comparar con el resto
			entrenador* entrenador1 = list_get(entrenadores_en_deadlock,j);

			nombre_pokemon = list_get(entrenador0->objetivos,0);

			//Si algun pokemon del 1 es el que 0 necesita y viceversa, se planifican:
			if(list_any_satisfy(entrenador1->pokemones,(void*)pokemon_repetido)){

				nombre_pokemon = list_get(entrenador1->objetivos,0);

				if(list_any_satisfy(entrenador0->pokemones,(void*)pokemon_repetido)){
					planificar_deadlock(entrenador0,entrenador1);


					break;
				}
				printf(" \n No se puede manejar el deadlock con entrenador:%d y entrenador:%d \n",entrenador0->id,entrenador1->id);

			}
	}
}
}
//TODO
void planificar_deadlock(entrenador* entrenador0,entrenador* entrenador1){
	printf("\n Inicio operacion de deadlock \n ");


	entrenador_exec = entrenador0;
	list_remove_by_condition(entrenadores_en_deadlock, (void*)entrenador_en_exec);

	mover_entrenador(entrenador0,entrenador1->posX,entrenador1->posY);
	//int retardo = leer_retardo_cpu() * 5;
	sleep(5); //IRIA sleep(retardo)

	nombre_pokemon = list_get(entrenador0->objetivos,0);
	list_remove_by_condition(entrenador1->pokemones,(void*)pokemon_repetido);
	list_remove_by_condition(entrenador0->objetivos,(void*)pokemon_repetido);

	nombre_pokemon = list_get(entrenador1->objetivos,0);
	list_remove_by_condition(entrenador0->pokemones,(void*)pokemon_repetido);
	list_remove_by_condition(entrenador1->objetivos,(void*)pokemon_repetido);


	analizar_proxima_cola(entrenador0);

	analizar_proxima_cola(entrenador1);
}


bool validacion_nuevo_pokemon(void){
	return (!queue_is_empty(pokemones_en_el_mapa) && !list_is_empty(entrenadores_en_ready)) || (!queue_is_empty(pokemones_en_el_mapa)  && !queue_is_empty(entrenadores_block_ready));
}




//RESPUESTAS DEL CAUGHT

void esperar_respuesta_catch(entrenador* un_entrenador){

	if(!broker_conectado) {
		printf("broker no conectado, realizamos operacion default: \n ");
	}
		else{
			//si el broker esta conectado, depende de el la respuesta
		}

}




void confirmacion_de_catch(entrenador* un_entrenador){

	proximo_objetivo = un_entrenador->objetivo_proximo;


	dictionary_put(objetivo_global,proximo_objetivo->nombre,dictionary_get(objetivo_global,proximo_objetivo->nombre)-1);

	bool tiene_objetivo = list_any_satisfy(un_entrenador->objetivos , (void*) es_de_especie);

	if(tiene_objetivo){
		list_remove_by_condition(un_entrenador->objetivos , (void*) es_de_especie);
	}

	list_add(pokemones_atrapados,proximo_objetivo);
	list_add(un_entrenador->pokemones,proximo_objetivo->nombre);

	disminuir_cuantos_puede_cazar(un_entrenador);

printf("\n Agarró al pokemon %s \n",proximo_objetivo->nombre);
	sem_post(&(un_entrenador->espera_de_catch));


}


void denegar_catch(entrenador* un_entrenador){
	log_info(llegadaDeMensaje,"No se agarró al pokemon %s", un_entrenador->objetivo_proximo->nombre);
	sem_post(&(un_entrenador->espera_de_catch));

}




/////////////////////////////////FUNCIONES AUX//////////////////////////////////////////////////////////////

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

bool puede_cazar(entrenador* entrenador){        //Cambiar a cuando el entrenador termine su exc
	return entrenador->cuantos_puede_cazar > 0;
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


void printear_lista_entrenadores(t_list* lista){

	if(list_is_empty(lista)) printf("Lista vacia");
	for (int i = 0; i < list_size(lista);i++){
		entrenador* entrenador = list_get(lista,i);
		printf("  entrenador: %d ",entrenador->id);
	}
	printf(" \n  ");
}

bool hay_deadlock(void){
	return (list_size(entrenadores_en_deadlock) > 1);
}


void analizar_proxima_cola(entrenador* un_entrenador){
	if(puede_cazar(un_entrenador)){

		log_info(cambioDeCola,"cambio a BLOCK-READY de entrenador: %d \n ",un_entrenador->id);
		queue_push(entrenadores_block_ready,un_entrenador);
	}
	if(!puede_cazar(un_entrenador)){
			if(cumplio_objetivo(un_entrenador)){
				list_add(entrenadores_finalizados,un_entrenador);
				log_info(cambioDeCola,"cambio a EXIT de entrenador: %d \n ",un_entrenador->id);
			}

			else{
				list_add(entrenadores_en_deadlock,un_entrenador);
				log_info(cambioDeCola,"cambio a BLOCK-DEADLOCK de entrenador: %d \n ",un_entrenador->id);
			}
	}

}
void bloquear_entrenador(entrenador* un_entrenador){
	queue_push(entrenadores_blocked,un_entrenador);
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

    while(1){
    	esperar_cliente(socket_servidor);

    }

}
//TODO
bool conectarse_con_broker(void){
	int conexionBroker = crear_conexion(IP_BROKER,PUERTO_BROKER);
		if(conexionBroker <= 0){
			log_info(comunicacion_broker_error,"No se pudo conectar con Broker,se realizará la operación por default");
			//broker_default();
			return false;
		}
		else{
			broker_conectado = true;
			log_info(comunicacion_broker_resultado,"me conecte a Broker exitosamente");
			return true;
		}
}

/*
void catch_pokemon(void){
broker_catch_pokemon *catchPoke = malloc(sizeof(broker_catch_pokemon));
				catchPoke->datos = malloc(sizeof(catch_pokemon));
				//dinamicas
				catchPoke->datos->nombrePokemon = argv[3];
				catchPoke->datos->tamanioNombre = strlen(catchPoke->datos->nombrePokemon)+1;

				//estaticas
				catchPoke->datos->posX = atoi(argv[4]);
				catchPoke->datos->posY = atoi(argv[5]);

				enviar_Broker_Catch_Pokemon(catchPoke,"GAMEBOY",conexionBroker);

				//log_info(logEnviarNuevo,"Mensaje Catch Pokemon \n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d "
									,catchPoke->datos->tamanioNombre,
									catchPoke->datos->nombrePokemon,
									catchPoke->datos->posX,
									catchPoke->datos->posY);
				free(catchPoke);
	}
*/





void esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	//poner en globales si es necesario
	pthread_t thread;


	socklen_t  tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	//log_info(logConexion," se conectaron a broker");

	pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
	pthread_detach(thread);

}

void serve_client(int* socket)
{
	int cod_op;
	int i = recv(*socket, &cod_op, sizeof(op_code), MSG_WAITALL);
	if(i <= 0)
		cod_op = -1;
	process_request(cod_op, *socket);
}

/*
 * mensajes que recibimos
	TEAM__APPEARED_POKEMON
	*/

 //ACA RECIBIMOS EL PAQUETE DEPENDIENDO DEL COD DE OPERACION Y HACEMOS ALGUNA ACCION A PARTIR DEL TIPO DE COD DE OPERACION RECIBIDO

void process_request(int cod_op, int cliente_fd) {
	uint32_t tamanio_buffer;
	uint32_t tamanio_username;
	team_appeared_pokemon* appearedRecibido;
	char* username;

	recv(cliente_fd,&tamanio_username,sizeof(uint32_t),MSG_WAITALL);

	username = malloc(tamanio_username);
	recv(cliente_fd,username,tamanio_username,MSG_WAITALL);

	recv(cliente_fd, &tamanio_buffer, sizeof(uint32_t), MSG_WAITALL);
	//falta los case de los otros tipos de mensajes (get,catch,caught)(localized lo dejamos para despues(es de GameCard)
	switch (cod_op) {

		case TEAM__APPEARED_POKEMON:

			appearedRecibido = deserializar_team_appeared_pokemon(cliente_fd);

			log_info(llegadaDeMensaje,"recibi mensaje appeared pokemon de %s:  \n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d \n",username, appearedRecibido->datos->tamanioNombre, appearedRecibido->datos->nombrePokemon, appearedRecibido->datos->posX, appearedRecibido->datos->posY);

			//printf("recibi mensaje appeared pokemon:  \n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d \n", appearedRecibido->datos->tamanioNombre, appearedRecibido->datos->nombrePokemon, appearedRecibido->datos->posX, appearedRecibido->datos->posY);

			free(appearedRecibido);
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
   	return log_create(archivo,proceso,true,LOG_LEVEL_INFO);
 }


