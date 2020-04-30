/*
 * team.h
 *
 *  Created on: 30 abr. 2020
 *      Author: utnso
 */

#ifndef TEAM_H_
#define TEAM_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include "utils_team.h"

t_log* iniciar_logger(char*);
t_config* leer_config(void);
//void terminar_programa(int,int,int, t_log*,t_log*,t_log*,t_log*, t_config*);
//t_log* 		log_create(char* file, char *program_name, bool is_active_console, t_log_level level);


#endif /* TEAM_H_ */
