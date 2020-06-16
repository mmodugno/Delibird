/*
 * utils_gamecard.c
 *
 *  Created on: 15 jun. 2020
 *      Author: utnso
 */

#include"utils_gamecard.h"

void crearDirectorio(char* path ,char* nombreCarpeta){

	char* aux = malloc(strlen(path) + strlen(nombreCarpeta));

	strcpy(aux,path);
	strcat(aux,nombreCarpeta);

	mkdir(aux,0777);


	free(aux);
}

void crearMetadata(){

	crearDirectorio("/home/utnso/Escritorio/","tall-grass");

	crearDirectorio(punto_montaje,"/Metadata");

	metadata = txt_open_for_append("/home/utnso/Escritorio/tall-grass/Metadata/Metadata.bin");

	fseek(metadata,0,SEEK_END);

	if(ftell(metadata) == 0){

	char* aux = malloc(sizeof(10));
	char* aux2 = malloc(sizeof(10));

	sprintf(aux,"%d",tamanioBloques);
	sprintf(aux2,"%d",cantidadBloques);

	txt_write_in_file(metadata,"BLOCK_SIZE=");
	txt_write_in_file(metadata,aux);
	txt_write_in_file(metadata,"\n");
	txt_write_in_file(metadata,"BLOCKS=");
	txt_write_in_file(metadata,aux2);
	txt_write_in_file(metadata,"\n");
	txt_write_in_file(metadata,"MAGIC_NUMBER=TALL_GRASS");

	free(aux);
	free(aux2);

	txt_close_file(metadata);

	}

}

void crearMetadataEspecie(char* nombreEspecie) {














}

void crearBitmap(){


	FILE* bitmap = txt_open_for_append("/home/utnso/Escritorio/tall-grass/Metadata/Bitmap.bin");

	fseek(metadata,0,SEEK_END);


	if(ftell(bitmap) == 0){

	char* puntero_a_bits = malloc(1);

	bitArray = bitarray_create_with_mode(puntero_a_bits,cantidadBloques/8,LSB_FIRST);

	}

}
