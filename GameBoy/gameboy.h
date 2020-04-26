/*
 * Cliente.h
 *
 *  Created on: 28 feb. 2019
 *      Author: the X team
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
