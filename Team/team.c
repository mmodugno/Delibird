/*
 * team.c
 *
 *  Created on: 27 abr. 2020
 *      Author: utnso
 */


#include "team.h"


int main(int argc, char* argv[]){

	if(argc == 1){
			printf("No hay parametros suficientes\n");
			return 2;
		}
	if(argc == 2){
			archivo_config = argv[1];
			}

	cambioDeCola = iniciar_log("CAMBIO DE COLA"); //Detallar razon
	movimiento_entrenador = iniciar_log("MOVIMIENTO DE ENTRENADOR");//indicando la ubicación a la que se movió).
	operacion_de_atrapar = iniciar_log("OPERACION ATRAPAR"); //log_info(operacion_de_atrapar,"pokemon: %s con posicion (%d, %d)",poke->nombre,poke->posX,poke->posY);
	operacion_de_intercambio = iniciar_log("OPERACION INTERCAMBIO"); //log_info(log,"intercambio entre entrenadores %d y %d",entrenador1->id,entrenador2->id);
	inicio_deadlock = iniciar_log("DETECCION_DEADLOCK");
	resultado_deadlock = iniciar_log("RESULTADO_DE_DEADLOCK");
	llegadaDeMensaje = iniciar_log("NUEVO MENSAJE");//INDICAR TIPO DEL MENSAJE Y SUS DATOS
	resultado = iniciar_log("RESULTADO TEAM"); //al final
	comunicacion_broker_error = iniciar_log("ERROR EN COMUNICACION"); //se realizara los resultados por default
	comunicacion_broker_reintento = iniciar_log("REINTENTO DE COMUNICACION");
	comunicacion_broker_resultado = iniciar_log("RESULTADO DE COMUNICACION");

	printf(" Arrancando \n \n");

	sem_init(&hay_entrenador,0,0);
	sem_init(&en_ejecucion,0,1); //Empieza en 1 porque se puede ejecutar solo 1 por vez

	///////////////////////////////////////////
	variables_globales();
	char* algoritmo = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
	printf("Algoritmo de planificacion = %s \n \n",algoritmo);

	conectarse_con_broker();

	pthread_t hilo_principal;
	pthread_create(&hilo_principal,NULL,(void *) algoritmo_aplicado,NULL);


	iniciar_servidor();





/*
	pokemon* squirte = hacer_pokemon("Squirtle", 0,0);
	aparece_nuevo_pokemon(squirte);

	pokemon* pikachu = hacer_pokemon("Pikachu", 6, 7);
	aparece_nuevo_pokemon(pikachu);

	pokemon* pikachu2 = hacer_pokemon("Pikachu", 8, 3);
	aparece_nuevo_pokemon(pikachu2);


	pokemon* charmander = hacer_pokemon("Charmander", 0,0);
	aparece_nuevo_pokemon(charmander);




*/





 	pthread_join(hilo_principal,NULL);

 	terminar_programa();
}


    //aca deberiamos poner terminar_programa
void terminar_programa(void){
	//Y por ultimo, para cerrar, hay que liberar lo que utilizamos (conexion, log y config) con las funciones de las commons y del TP mencionadas en el enunciado
	log_destroy(cambioDeCola);
	log_destroy(llegadaDeMensaje);
	log_destroy(movimiento_entrenador);
	log_destroy(operacion_de_atrapar);
	log_destroy(operacion_de_intercambio);
	log_destroy(resultado);
	log_destroy(inicio_deadlock);
	log_destroy(resultado_deadlock);
	log_destroy(comunicacion_broker_error);
	log_destroy(comunicacion_broker_reintento);
	log_destroy(comunicacion_broker_resultado);

	config_destroy(config);
	//liberar_conexion(conexion);
}


