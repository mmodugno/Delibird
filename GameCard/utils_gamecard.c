/*
 * utils_gamecard.c
 *
 *  Created on: 15 jun. 2020
 *      Author: utnso
 */

#include"utils_gamecard.h"

void crearDirectorio(char* path ,char* nombreCarpeta){

	char* aux = malloc(strlen(path) + strlen(nombreCarpeta) + 2);

	strcpy(aux,path);
	strcat(aux,nombreCarpeta);

	mkdir(aux,0777);

	free(aux);
}

bool estaVacio(FILE* arch) {

	fseek(arch,0,SEEK_END);

	return ftell(arch) == 0;

}

void verificarExistenciaPokemon(char* nombrePoke) {

	char* path = "/home/utnso/Escritorio/PuntoMontaje/TallGrass/Files/";

	char* archivo = "/Metadata.bin";

	char* aux = malloc(strlen(path) + strlen(nombrePoke) + strlen(archivo) + 3);

	crearDirectorio(path,nombrePoke);

	strcpy(aux,path);
	strcat(aux,nombrePoke);
	strcat(aux,archivo);

	FILE* metadataPoke = txt_open_for_append(aux);

	if(estaVacio(metadataPoke)){

	txt_write_in_file(metadataPoke,"DIRECTORY=N");
	txt_write_in_file(metadata,"\n");
	txt_write_in_file(metadataPoke,"SIZE=0");
	txt_write_in_file(metadata,"\n");
	txt_write_in_file(metadataPoke,"BLOCKS=()"); //CAMBIAR () a corchetes
	txt_write_in_file(metadata,"\n");
	txt_write_in_file(metadataPoke,"OPEN=N");

	txt_close_file(metadataPoke);

	}

	free(aux);

}

void verificarAperturaArchivo(char* path) {

	FILE* archivoTemporal = fopen(path,"rb");

	char aux;

	fseek(archivoTemporal,-1,SEEK_END);

	fread(&aux,sizeof(char),1,archivoTemporal);

	if(aux == 'Y'){

		printf("NoEntre");

	} else {

		printf("Entre");
	}
}


void crearMetadata(){

	crearDirectorio(punto_montaje,"/Metadata");

	metadata = txt_open_for_append("/home/utnso/Escritorio/PuntoMontaje/Metadata/Metadata.bin");

	if(estaVacio(metadata)){

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

void crearFilesAndBlocks() {

	crearDirectorio("/home/utnso/Escritorio/PuntoMontaje/TallGrass","/Files");

	metadataFiles = txt_open_for_append("/home/utnso/Escritorio/PuntoMontaje/TallGrass/Files/Metadata.bin");

	if(estaVacio(metadataFiles)){

	txt_write_in_file(metadataFiles,"DIRECTORY=Y");

	txt_close_file(metadataFiles);

	}

	crearDirectorio("/home/utnso/Escritorio/PuntoMontaje/TallGrass","/Blocks");


}

void crearMetadataEspecie(char* nombreEspecie) {














}

void crearBitmap(){


	FILE* bitmap = txt_open_for_append("/home/utnso/Escritorio/PuntoMontaje/Metadata/Bitmap.bin");

	fseek(metadata,0,SEEK_END);


	if(ftell(bitmap) == 0){

	char* puntero_a_bits = malloc(1);

	bitArray = bitarray_create_with_mode(puntero_a_bits,cantidadBloques/8,LSB_FIRST);

	}

	txt_close_file(metadata);

}
