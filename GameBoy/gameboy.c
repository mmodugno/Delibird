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
	uint32_t tamanioNombre;
	char* nombrePokemon;
	uint32_t posX;
	uint32_t posY;
	uint32_t cantidadPokemon;
};

typedef struct Localize_Pokemon{
	uint32_t tamanioNombre;
	char* nombrePokemon;
	uint32_t cantidadPosiciones;
	uint32_t posX[];
	uint32_t posY[];
};

typedef struct Get_Pokemon{
	uint32_t tamanioNombre;
	char* nombrePokemon;
};

typedef struct Appeared_Pokemon{
	uint32_t tamanioNombre;
	char* nombrePokemon;
	uint32_t posX;
	uint32_t posY;
};

typedef struct Catch_Pokemon{
	uint32_t tamanioNombre;
	char* nombrePokemon;
	uint32_t posX;
	uint32_t posY;
};

typedef struct Caught_Pokemon{
	uint32_t puedoAtraparlo;
};





int main(void){





	return 0;
}
