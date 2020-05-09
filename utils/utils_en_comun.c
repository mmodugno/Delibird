/*
 * Utils_en_comun.c
 *
 *  Created on: 6 may. 2020
 *      Author: utnso
 */
#include <stdlib.h>
#include "utils_en_comun.h"

//TODO
void* serializar_paquete(t_paquete* paquete, int *bytes){
	(*bytes) = sizeof(op_code)+sizeof(int)+(paquete->buffer->size);
	int offset = 0;
	void* stream_a_enviar=malloc(*bytes);

	memcpy(stream_a_enviar+offset,&(paquete->codigo_operacion),sizeof(op_code));
	offset+=sizeof(op_code);

	memcpy(stream_a_enviar+offset,&(paquete->buffer->size),sizeof(int));
	offset+=sizeof(int);


	memcpy(stream_a_enviar+offset,(paquete->buffer->stream),paquete->buffer->size);

	return stream_a_enviar;
}

