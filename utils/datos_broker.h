/*
 * datos_broker.h
 *
 *  Created on: 15 may. 2020
 *      Author: utnso
 */

#ifndef DATOS_BROKER_H_
#define DATOS_BROKER_H_

#include "utils_en_comun.h"

typedef struct {
	uint32_t id;
	localize_pokemon* datos;
	t_queue* suscriptoresQueRespondieron;
} broker_localized_pokemon;

typedef struct {
	uint32_t id;
	new_pokemon* datos;
	t_queue* suscriptoresQueRespondieron;
} broker_new_pokemon;

typedef struct {
	uint32_t id;
	uint32_t id_relativo;
	appeared_pokemon* datos;
	t_queue* suscriptoresQueRespondieron;
} broker_appeared_pokemon;

typedef struct {
	uint32_t id;
	catch_pokemon* datos;
	t_queue* suscriptoresQueRespondieron;
} broker_catch_pokemon;

typedef struct {
	uint32_t id;
	uint32_t id_relativo;
	caught_pokemon* datos;
	t_queue* suscriptoresQueRespondieron;
} broker_caught_pokemon;

typedef struct {
	uint32_t id;
	get_pokemon* datos;
	t_queue* suscriptoresQueRespondieron;
} broker_get_pokemon;


broker_get_pokemon* deserializar_get_pokemon(int socket_cliente);
broker_new_pokemon* deserializar_new_pokemon(int socket_cliente);
broker_appeared_pokemon* deserializar_appeared_pokemon(int socket_cliente);
broker_catch_pokemon* deserializar_catch_pokemon(int socket_cliente);
broker_caught_pokemon* deserializar_caught_pokemon(int socket_cliente);
void serializar_broker_new_pokemon(broker_new_pokemon* brokerNewPokemon, t_buffer* buffer);
void serializar_broker_appeared_pokemon(broker_appeared_pokemon* brokerAppearedPokemon, t_buffer* buffer);
void serializar_broker_catch_pokemon(broker_catch_pokemon* brokerCatchPokemon, t_buffer* buffer);
void serializar_broker_caught_pokemon(broker_caught_pokemon* brokerCaughtPokemon, t_buffer* buffer);
void serializar_broker_get_pokemon(broker_get_pokemon* brokerGetPokemon, t_buffer* buffer);

#endif /* DATOS_BROKER_H_ */
