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
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include "utils_Gameboy.h"


t_log* iniciar_logger(char*);
t_config* leer_config(void);
void terminar_programa(int,int,int, t_log*,t_log*,t_log*,t_log*, t_config*);

#endif /* GAMEBOY_H_ */
