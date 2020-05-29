/*
 * gameboy.h
 *
 *  Created on: 29 abr. 2020
 *      Author: utnso
 */

#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include<stdio.h>
#include<stdlib.h>

#include "utils_Gameboy.h"


t_log* iniciar_logger(char*);
void leer_config();
void terminar_programa();

#endif /* GAMEBOY_H_ */
