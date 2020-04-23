#include "gameboy.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

typedef struct New_Pokemon{
	uint_32 tamanioNombre;
	char* nombrePokemon;
	uint_32 posX;
	uint_32 posY;
	uint_32 cantidadPokemon;
};

typedef struct Localize_Pokemon{
	uint_32 tamanioNombre;
	char* nombrePokemon;
	uint_32 cantidadPosiciones;
	uint_32 posX[];
	uint_32 posY[];
};

typedef struct Get_Pokemon{
	uint_32 tamanioNombre;
	char* nombrePokemon;
};

typedef struct Appeared_Pokemon{
	uint_32 tamanioNombre;
	char* nombrePokemon;
	uint_32 posX;
	uint_32 posY;
};

typedef struct Catch_Pokemon{
	uint_32 tamanioNombre;
	char* nombrePokemon;
	uint_32 posX;
	uint_32 posY;
};

typedef struct Caught_Pokemon{
	uint_32 puedoAtraparlo;
};





int main(void){





	return 0;
}
