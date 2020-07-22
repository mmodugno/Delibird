/*
 ============================================================================
 Name        : GameCard.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include"GameCard.h"


int main(int argc, char* argv[]){

		if(argc  == 1){
			printf("No hay parametros suficientes");
			return 2;
		}

		nombreConfig = argv[1];

		sem_init(&sem_mensaje,0,1);

		leer_config();

		username = malloc(strlen("GAMECARD"+1));
		username = "GAMECARD";

		sem_init(&sem_conexion,0,1);

		sem_init(&mutex_bit_array,0,1);

		logArchivoAbierto = iniciar_logger("ARCHIVO ABIERTO");
		logFalloConexion = iniciar_logger("FALLO CONEXION");

		printf("Empezo el programa \n");


		crearDirectorio("/home/utnso/Escritorio/","PuntoMontaje");

		leerMetadata();

		crearDirectorio(punto_montaje,"/TallGrass");

		crearFilesAndBlocks();

		sem_init(&sem_escritura,0,1);

		inicializar_bitmap();

		inicializar_bitarray();


		//t_list* listaAux = list_create();

		//list_add(listaAux,"1");
		//ist_add(listaAux,"2");

		//buscarYeliminarCeros(listaAux);

		//verificarDirectorioPokemon("Pikachu");

		//registroDatos* registro1 = hacerRegistro(4,1,1);
		//registroDatos* registro2 = hacerRegistro(4,2,1);
		//registroDatos* registro3 = hacerRegistro(4,3,1);



		//procesarNewPokemon("Pikachu",registro1);
		//procesarNewPokemon("Charmander",registro1);


		//procesarCatchPokemon("Charmander",413,17);


		//procesarNewPokemon("Messi",registro1);
		//procesarGetPokemon("Charmander");3

		/*pthread_t hilo_servidor;

		pthread_create(&hilo_servidor,NULL,(void *) iniciar_servidor,NULL);*/

		int i;

		for(i = 0; i < bitarray_get_max_bit(bitArray);i++){
			if(bitarray_test_bit(bitArray,i) == 0){
				printf("Libre en: %d \n",i);
			}
		}

		suscribirnos_cola_catch();
		suscribirnos_cola_new();
		suscribirnos_cola_get();

		iniciar_servidor();




		printf(" \n Terminado  \n" );

		terminar_programa();

}



void leer_config(void){

	config = config_create(nombreConfig);

	tiempo_reintento_conexion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_CONEXION");
	tiempo_reintento_operacion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_OPERACION");
	tiempo_retardo_operacion = config_get_int_value(config,"TIEMPO_RETARDO_OPERACION");

	punto_montaje = config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS");

	ipBroker = config_get_string_value(config,"IP_BROKER");

	puertoBroker = config_get_int_value(config,"PUERTO_BROKER");
}


t_log* iniciar_logger(char* tipoDeProceso){

	//preguntar por el tipo de LOG_LEVEL
	return log_create("gameboy.log",tipoDeProceso,0,LOG_LEVEL_INFO);
}


void terminar_programa(void){

	log_destroy(logArchivoAbierto);
	log_destroy(logFalloConexion);
	config_destroy(config);

	//munmap(bmap, size);

	bitarray_destroy(bitArray);
	//liberar_conexion(conexionBroker);
}


//  cola de mensajes NEW_POKEMON, CATCH_POKEMON  y GET_POKEMON.
void suscribirnos_cola_catch(){
	int suscripcionGet;
	suscriptor* meSuscriboGet = malloc(sizeof(suscriptor));


	meSuscriboGet->nombreDeSuscriptor = "GAMECARD";
	meSuscriboGet->tamanioNombreSucriptor = strlen(meSuscriboGet->nombreDeSuscriptor) + 1;

	meSuscriboGet->tipoDeCola = CAUGHT_POKEMON;

	suscripcionGet = crear_conexion(IP_BROKER,PUERTO_BROKER);


	if(suscripcionGet != -1){
		enviar_pedido_suscripcion(meSuscriboGet, suscripcionGet);
		liberar_conexion(suscripcionGet);
		}
			free(meSuscriboGet);
}

void suscribirnos_cola_new(){
	int suscripcionNew;
	suscriptor* meSuscriboNew = malloc(sizeof(suscriptor));


	meSuscriboNew->nombreDeSuscriptor = "GAMECARD";
	meSuscriboNew->tamanioNombreSucriptor = strlen(meSuscriboNew->nombreDeSuscriptor) + 1;

	meSuscriboNew->tipoDeCola = NEW_POKEMON;

	suscripcionNew = crear_conexion(IP_BROKER,PUERTO_BROKER);


	if(suscripcionNew != -1){
		enviar_pedido_suscripcion(meSuscriboNew, suscripcionNew);
		liberar_conexion(suscripcionNew);
		}
			free(meSuscriboNew);
}

void suscribirnos_cola_get(){
	int suscripcionGet;
	suscriptor* meSuscriboGet = malloc(sizeof(suscriptor));


	meSuscriboGet->nombreDeSuscriptor = "GAMECARD";
	meSuscriboGet->tamanioNombreSucriptor = strlen(meSuscriboGet->nombreDeSuscriptor) + 1;

	meSuscriboGet->tipoDeCola = GET_POKEMON;

	suscripcionGet = crear_conexion(IP_BROKER,PUERTO_BROKER);


	if(suscripcionGet != -1){
	enviar_pedido_suscripcion(meSuscriboGet, suscripcionGet);
	liberar_conexion(suscripcionGet);
		}
			free(meSuscriboGet);
}
