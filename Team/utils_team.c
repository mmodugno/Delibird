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

	entrenadores_finalizados = list_create();
	entrenadores_en_deadlock = list_create();





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

	 pthread_t hiloEntrenador;


	 un_entrenador->estado = NEW;


	 un_entrenador->objetivos =crear_lista(string_split(objetivosconfig,"|"));
	 un_entrenador->pokemones =crear_lista(string_split(pokemonsconfig,"|"));

	 un_entrenador->cuantos_puede_cazar = list_size(un_entrenador->objetivos);
	 un_entrenador->id = id_creada;

	 sacar_pokemones_repetidos(un_entrenador->objetivos,un_entrenador->pokemones);


	 t_list* posiciones = crear_lista(string_split(posicion,"|"));
	 un_entrenador->posX = atoi(list_get(posiciones,0));
	 un_entrenador->posY = atoi(list_get(posiciones,1));

	 un_entrenador->hiloDeEntrenador = pthread_create(&hiloEntrenador,NULL,(void*) procedimiento_de_caza,un_entrenador);

	 return un_entrenador;
 }


void sacar_pokemones_repetidos(t_list* objetivos, t_list* pokemones){
	for(int i = 0; i < list_size(pokemones);i++){
		nobmre_objetivoconfig = list_get(pokemones,i);
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
 }


void calcular_objetivo_global(void){

	objetivo_global = dictionary_create();
	printf("Objetivo Global:  ");
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

}

void mostrar_objetivo_global(char* key, void*value){
	//SACAR ESTO
	log_info(resultado,"necesito %d del pokemon %s",value, key);
}

void mostrar(void){
	dictionary_iterator(objetivo_global,mostrar_objetivo_global);
}






//La función que se pasa por paremtro recibe (char* key, void* value)

void agregar_un_objetivo(char* pokemon_a_agregar){

	if(dictionary_has_key(objetivo_global, pokemon_a_agregar)){ //si el pokemon ya estaba en el diccionario, le sumo 1 a su cantidad
		dictionary_put(objetivo_global,pokemon_a_agregar, dictionary_get(objetivo_global,pokemon_a_agregar)+1);
	}
	else{
		dictionary_put(objetivo_global, pokemon_a_agregar,(void*) 1); // 1 o puntero??

	}
}

void cambiar_estado_entrenador(entrenador* entrenador,int nuevo_estado){
	entrenador->estado = nuevo_estado;
}

void mover_entrenador(entrenador* entrenador,pokemon* pokemon){


	int tiempo = leer_retardo_cpu();
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
		if(entrenador->posY < pokemon->posY){
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


//Se ejecuta cuando recibimos un mensaje de appeared_pokemon
void aparece_nuevo_pokemon(pokemon* poke){

	sem_wait(&planificando);

	 if (dictionary_has_key(objetivo_global, poke->nombre) && (dictionary_get(objetivo_global, poke->nombre) > 0) ){
		 //ME SIRVE EL POKEMON
		 list_add(pokemones_en_el_mapa,poke);

		 //planificar a un entrenador con este pokemon nuevo

		 sem_wait(&hay_entrenador);
		 planificar_entrenador(poke);

	 }

	 else{
		 printf("El pokemon no pertenece al objetivo global. Se descarta");
	 }
}


void planificar_entrenador(pokemon* un_pokemon){


	proximo_objetivo = un_pokemon;

	entrenador_exec = list_get(list_sorted(entrenadores_en_ready,(void*) primer_entrenador_mas_cerca_de_pokemon) ,0);


	//cambiar_estado_entrenador(entrenador_ready,READY);

	sem_post(&entrenador_listo); //signal


}





void algoritmo_aplicado(void){
	switch (leer_algoritmo_planificacion()){
	case FIFO:
		printf("\n Algoritmo de planificacion = FIFO \n");
		planifico_con_fifo();
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

	sem_wait(&(un_entrenador->sem_entrenador));

	pokemon* objetivo_propio = proximo_objetivo;

	printf(" \n Moviendo entrenador (puede tardar un toque): \n");

	mover_entrenador(un_entrenador,objetivo_propio);



	log_info(operacion,"ATRAPAR POKEMON: %s con posicion (%d, %d)",objetivo_propio->nombre,objetivo_propio->posX,objetivo_propio->posY);

	/* PEDIR UN CATCH
	enviar un mensaje catch al broker, si no se esta conectado efectua el default (podria ser una funcion que se ocupe de eso)
	wait(respuesta de mensaje id)
	cuando se recibe el mensaje CAUGHT se manda un signal y la respuesta

	*/
	confirmacion_de_catch();


//ANALIZAR A QUE COLA O LISTA SE MUEVE




	sem_post(&en_ejecucion);
}

}





void planifico_con_fifo(void){


	sem_init(&en_ejecucion,0,1);

while(1){

	sem_wait(&entrenador_listo);

	sem_wait(&en_ejecucion);
	//Seccion critica
	printf("\n \n Comienzo ejecucion \n \n");




	entrenador* entrenador_a_ejecutar = entrenador_exec;
	list_remove_by_condition(entrenadores_en_ready,(void*)entrenador_en_exec);


	log_info(cambioDeCola,"cambio a EXEC de entrenador: %d \n ",entrenador_a_ejecutar->id);
	//SACAR DE READY

	//cambiar_estado_entrenador(entrenador_a_ejecutar,EXEC);

	sem_post(&(entrenador_a_ejecutar->sem_entrenador));

	sem_post(&planificando);
	//Fin de seccion critica

	if(entrenador_exec->cuantos_puede_cazar >0){
	list_add(entrenadores_en_ready,entrenador_exec);

	if(entrenador_exec->cuantos_puede_cazar == 0){
		if(cumplio_objetivo(entrenador_exec)){
			list_add(entrenadores_finalizados,entrenador_exec);
		}

		else{
			list_add(entrenadores_en_deadlock,entrenador_exec);
		}
	}
}
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


 if(list_any_satisfy(entrenador_exec->objetivos , (void*) es_de_especie)){
	list_remove_by_condition(entrenador_exec->objetivos , (void*) es_de_especie);
 }

 list_add(pokemones_atrapados,proximo_objetivo);
 list_add(entrenador_exec->pokemones,proximo_objetivo);

 disminuir_cuantos_puede_cazar(entrenador_exec);
 if(entrenador_exec->cuantos_puede_cazar == 0){
	 // SE MUEVE A LA LISTA DE BLOQUEADOS
 }

printf("Agarró al pokemon %s",proximo_objetivo->nombre);

}

void denegar_catch(void){
	log_info(llegadaDeMensaje,"No se agarró al pokemon");
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








/////////////////////////////////FUNCIONES AUX//////////////////////////////////////////////////////////////

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


bool entrenador_en_exec(entrenador* un_entrenador){
	return (un_entrenador->id) == (entrenador_exec->id);
}

bool cumplio_objetivo(entrenador* un_entrenador){
	return list_is_empty(un_entrenador->objetivos);
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
	team_appeared_pokemon* appearedRecibido;

	recv(cliente_fd, &tamanio_buffer, sizeof(uint32_t), MSG_WAITALL);
	//falta los case de los otros tipos de mensajes (get,catch,caught)(localized lo dejamos para despues(es de GameCard)
	switch (cod_op) {

		case TEAM__APPEARED_POKEMON:

			appearedRecibido = deserializar_team_appeared_pokemon(cliente_fd);

			log_info(llegadaDeMensaje,"recibi mensaje appeared pokemon:  \n con tamanio: %d \n nombre: %s \n posX: %d \n posY: %d \n", appearedRecibido->datos->tamanioNombre, appearedRecibido->datos->nombrePokemon, appearedRecibido->datos->posX, appearedRecibido->datos->posY);

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

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("error");

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


