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
void destruir_logs(void);
void suscribirnos_cola_caught();
void suscribirnos_cola_localized();
void iniciar_logs(void);
void terminar_programa(void);
void enviar_get_por_objetivo(char* ,void* );
#endif /* TEAM_TEAMH */
