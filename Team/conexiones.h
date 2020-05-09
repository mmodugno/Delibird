/*
 * conexiones.h
 *
 *  Created on: 9 may. 2020
 *      Author: utnso
 */

#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include "utils_en_comun.h"


/*
typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;



typedef enum
{
	MENSAJE = 1,
	BROKER__NEW_POKEMON = 2,
	BROKER__APPEARED_POKEMON = 3,
	BROKER__CATCH_POKEMON = 4,
	BROKER__CAUGHT_POKEMON = 5,
	BROKER__GET_POKEMON = 6,
	TEAM__APPEARED_POKEMON = 7,
	GAMECARD__NEW_POKEMON = 8,
	GAMECARD__CATCH_POKEMON = 9,
	GAMECARD__GET_POKEMON = 10,
	SUBCRIPCION = 11

}op_code;


*/






#endif /* CONEXIONES_H_ */
