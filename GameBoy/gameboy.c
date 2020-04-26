#include "gameboy.h"


int main(int argc, char* argv){


	//ESTO ME PARECE QUE SE PUEDE DEJAR COMO INT PORQUE NO SE VA A PASAR POR SOCKET
	int conexionBroker;
	int conexionTeam;
	int conexionGamecard;

	char* ipBroker;
	int puertoBroker;

	char* ipTeam;
	int puertoTeam;

	char* ipGamecard;
	int puertoGamecard;
	////////////////////////////////////////////////////////////////////////////////


	///////////////////////////LOGS OBLIGATRIOS/////////////////////////////////////
	t_log* logConexion=iniciar_logger("Conexion");
	t_log* logSuscipcion=iniciar_logger("Suscripcion");
	t_log* logMensajeNuevo=iniciar_logger("Mensaje Nuevo");
	t_log* logEnviarNuevo= iniciar_logger("Enviar Mensaje");
	////////////////////////////////////////////////////////////////////////////////

	t_config* config = leer_config();

	ipBroker=config_get_string_value(config,"IP_BROKER");
	ipGamecard=config_get_string_value(config,"IP_GAMECARD");
	ipTeam=config_get_string_value(config,"IP_TEAM");

	puertoBroker = config_get_int_value(config,"PUERTO_BROKER");
	puertoGamecard = config_get_int_value(config,"PUERTO_GAMECARD");
	puertoTeam = config_get_int_value(config,"PUERTO_TEAM");

	//TODO
	//////////////HACER CONEXION DEPENDIENDO QUE NOS PASEN POR PARAMETRO////////////



	////////////////////////////////////////////////////////////////////////////////

	//respetar el orden de los parametros
	terminar_programa(conexionBroker,conexionTeam,conexionGamecard,logConexion,logSuscipcion,logMensajeNuevo,logEnviarNuevo,config);

}


t_log* iniciar_logger(char* tipoDeProceso){

	//preguntar por el tipo de LOG_LEVEL
	return log_create("gameboy.log",tipoDeProceso,0,LOG_LEVEL_INFO);
}


t_config* leer_config(void){

	return config_create("gameboy.config");

}

//termino todas las conexiones, logs y archivo de config
//si se agregan mas logs, conexiones o archivos de config, agregar
void terminar_programa(int conexBroker,int conexTeam,int conexGamecard, t_log* logConexion,t_log* logSuscripcion,t_log* logMensajeNuevo,t_log* logEnviarMensaje, t_config* config){

	log_destroy(logConexion);
	log_destroy(logSuscripcion);
	log_destroy(logMensajeNuevo);
	log_destroy(logEnviarMensaje);

	config_destroy(config);

	liberar_conexion(conexBroker);
	liberar_conexion(conexGamecard);
	liberar_conexion(conexTeam);
}
