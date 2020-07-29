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


		crearDirectorio("/home/utnso/","PuntoMontaje");

		leerMetadata();

		crearDirectorio(punto_montaje,"/TallGrass");

		crearFilesAndBlocks();

		sem_init(&sem_escritura,0,1);

		inicializar_bitmap();

		inicializar_bitarray();

		//sem_init(&sem_mensaje,0,1);


		//t_list* listaAux = list_create();

		//list_add(listaAux,"1");
		//list_add(listaAux,"2");


		//registroDatos* registro1 = hacerRegistro(4,1,5);
	//	registroDatos* registro2 = hacerRegistro(4,2,1);
		//registroDatos* registro3 = hacerRegistro(4,3,1);

		//procesarNewPokemon("Squirtle",registro2,99);
		//procesarNewPokemon("Squirtle",registro1,99);
		//procesarCatchPokemon("Squirtle",4,2,99);
		/*procesarCatchPokemon("Squirtle",4,1,99);
		procesarCatchPokemon("Squirtle",4,1,99);*/

		//procesarNewPokemon("Messi",registro1);
		//procesarGetPokemon("Gedasdsngar",1);

		/*pthread_t hilo_servidor;

		pthread_create(&hilo_servidor,NULL,(void *) iniciar_servidor,NULL);*/

		/*
		int i;

		for(i = 0; i < 100 ;i++){
			if(bitarray_test_bit(bitArray,i) == 0){
				printf("Libre en: %d \n",i);
			}
		}
		*/

		suscribirnos_cola_catch();
		sleep(1);
		suscribirnos_cola_new();
		sleep(1);
		suscribirnos_cola_get();
		sleep(1);

		iniciar_servidor();

		return 1;

}

void leer_config(void){

	config = config_create(nombreConfig);

	tiempo_reintento_conexion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_CONEXION");
	tiempo_reintento_operacion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_OPERACION");
	tiempo_retardo_operacion = config_get_int_value(config,"TIEMPO_RETARDO_OPERACION");

	punto_montaje = config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS");

	ipBroker = config_get_string_value(config,"IP_BROKER");

	puertoBroker = config_get_string_value(config,"PUERTO_BROKER");

	ipGamecard = config_get_string_value(config,"IP_GAMECARD");
	puertoGamecard = config_get_string_value(config,"PUERTO_GAMECARD");


}


t_log* iniciar_logger(char* tipoDeProceso){

	//preguntar por el tipo de LOG_LEVEL
	return log_create("gameboy.log",tipoDeProceso,0,LOG_LEVEL_INFO);
}


//  cola de mensajes NEW_POKEMON, CATCH_POKEMON  y GET_POKEMON.
void suscribirnos_cola_catch(){
	int suscripcionGet;
	suscriptor* meSuscriboGet = malloc(sizeof(suscriptor));

	meSuscriboGet->nombreDeSuscriptor = "GAMECARD";
	meSuscriboGet->tamanioNombreSucriptor = strlen(meSuscriboGet->nombreDeSuscriptor) + 1;

	meSuscriboGet->tipoDeCola = CATCH_POKEMON;

	suscripcionGet = crear_conexion(ipBroker,puertoBroker);

	if(suscripcionGet != -1){
			enviar_pedido_suscripcion(meSuscriboGet, suscripcionGet);
			//printf("Mande a cola CATCH_POKEMON con el socket:%d \n",suscripcionGet);
			//fflush(stdout);

			close(suscripcionGet);
		}
			free(meSuscriboGet);
}

void suscribirnos_cola_new(){
	int suscripcionNew;
	suscriptor* meSuscriboNew = malloc(sizeof(suscriptor));


	meSuscriboNew->nombreDeSuscriptor = "GAMECARD";
	meSuscriboNew->tamanioNombreSucriptor = strlen(meSuscriboNew->nombreDeSuscriptor) + 1;

	meSuscriboNew->tipoDeCola = NEW_POKEMON;



	suscripcionNew = crear_conexion(ipBroker,puertoBroker);


	if(suscripcionNew != -1){
			enviar_pedido_suscripcion(meSuscriboNew, suscripcionNew);
		//	printf("Mande a cola NEW con el socket: %d \n",suscripcionNew);
		//	fflush(stdout);

		close(suscripcionNew);
		}
			free(meSuscriboNew);
}

void suscribirnos_cola_get(){
	int suscripcionGet;
	suscriptor* meSuscriboGet = malloc(sizeof(suscriptor));


	meSuscriboGet->nombreDeSuscriptor = "GAMECARD";
	meSuscriboGet->tamanioNombreSucriptor = strlen(meSuscriboGet->nombreDeSuscriptor) + 1;

	meSuscriboGet->tipoDeCola = GET_POKEMON;

	suscripcionGet = crear_conexion(ipBroker,puertoBroker);


	if(suscripcionGet != -1){
			enviar_pedido_suscripcion(meSuscriboGet, suscripcionGet);
			//printf("Mande a cola GET con el socket: %d \n",suscripcionGet);
			//fflush(stdout);

			close(suscripcionGet);
		}
			free(meSuscriboGet);
}
