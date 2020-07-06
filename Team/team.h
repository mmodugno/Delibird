//*
// * team.h
// *
// *  Created on: 28 abr. 2020
// *      Author: utnso
// */
//
#ifndef TEAM_TEAMH
#define TEAM_TEAMH

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include"utils_team.h"


void imprimir_metricas(void);
void loggear_metricas(void);
//t_log         log_create(char* file, char program_name, bool is_active_console, t_log_level level);

void iniciar_logs(void);
void terminar_programa(void);
#endif /* TEAM_TEAMH */
