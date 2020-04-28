/*
 * team.c
 *
 *  Created on: 27 abr. 2020
 *      Author: utnso
 */


#include "utils_team.h"


void *sumarMilVeces( void *ptr );

int compartida = 5;
int main(){
     pthread_t hilo1, hilo2;
     char *arg1 = "hilo1: ";
     char arg2 = "hilo2: ";
     int  r1, r2;

     r1 = pthread_create( &hilo1, NULL, sumarMilVeces, (void) arg1);
     r2 = pthread_create( &hilo2, NULL, sumarMilVeces, (void*) arg2);
     pthread_join( hilo2, NULL);
     pthread_join( hilo1, NULL);
     printf("Thread 1 devolvió: %d y el Thread 2: %d\n", r1, r2);
}


void *sumarMilVeces( void *argumento ){
  char *nombre = (char *) argumento;

  int i = 0;
  for (i; i<10; i++) {
    compartida = compartida + 1;
    printf("%s : %d\n",nombre,compartida);
    sleep(1);
  }
}
