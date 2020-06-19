/*
 * utils_gamecard.c
 *
 *  Created on: 15 jun. 2020
 *      Author: utnso
 */

#include"utils_gamecard.h"

t_list* crear_lista(char** array){
    t_list* nuevaLista = list_create();
    int i = 0;
    while( array[i] != NULL ){
        list_add(nuevaLista, array[i]);
        i++;
    }
    return nuevaLista;
}

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

bool estaVacioConRuta(char* path) {

	FILE* arch = txt_open_for_append(path);

	bool vacio = estaVacio(arch);

	txt_close_file(arch);

	return vacio;

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
	txt_write_in_file(metadataPoke,"BLOCKS=[]"); //CAMBIAR () a corchetes
	txt_write_in_file(metadata,"\n");
	txt_write_in_file(metadataPoke,"OPEN=N");

	txt_close_file(metadataPoke);

	}

	fclose(metadataPoke);

	free(aux);

}

void verificarAperturaArchivo(char* path) {
//while(1)


	char* aux;

	t_config* configAux = config_create(path);

	aux = config_get_string_value(configAux,"OPEN");

	config_destroy(configAux);

	if(!strcmp(aux,"Y")){

		printf("NoEntre"); //Deberia delegar un hilo que trate de entrar cada X segundos

	} else {

		FILE* archivoTemporal = fopen(path,"rb+");

		char opened = 'Y';

		fseek(archivoTemporal,-1,SEEK_END);

		fwrite(&opened,sizeof(opened),1,archivoTemporal);

		fclose(archivoTemporal);

		printf("Entre");

	}
}

int buscarIndicePrimerBloqueLibre(){

	int contador = 1;

	char* path = string_from_format("/home/utnso/Escritorio/PuntoMontaje/TallGrass/Blocks/%d.bin",contador);

	while(!estaVacioConRuta(path)){
	contador++;
	memcpy(path,string_from_format("/home/utnso/Escritorio/PuntoMontaje/TallGrass/Blocks/%d.bin",contador),strlen(path));
	}

	FILE* crearArchivo = txt_open_for_append(path);

	txt_close_file(crearArchivo);

	return contador;
}


//TODO terminarAgregar
/*void agregarBloqueALista(t_list* bloques, int nuevoBloque) {

	char

	list_add(bloques,nuevoBloque);




}*/

void registrarPokemon(char* nombrePoke, registroDatos* registro) {

	char* path = string_from_format("%s/TallGrass/Files/%s/Metadata.bin",punto_montaje,nombrePoke);

	//FILE* archivoTemporal = fopen(path,"rb+");

	char** bloques;

	t_config* configAux = config_create(path);

	bloques = config_get_array_value(configAux,"BLOCKS");

	t_list* listaBloques = crear_lista(bloques);

	if(list_is_empty(listaBloques)){
		int indiceLibre = buscarIndicePrimerBloqueLibre();

		char* pathLibre = string_from_format("/home/utnso/Escritorio/PuntoMontaje/TallGrass/Blocks/%d.bin",indiceLibre);

		//mutexLock
		FILE* bloqueLibre = txt_open_for_append(pathLibre);

		txt_write_in_file(bloqueLibre,string_from_format("%d-%d=%d",registro->posX,registro->posY,registro->cantidad));

		bitarray_set_bit(bitArray,indiceLibre-1);

		config_set_value(configAux,"BLOCKS",string_from_format("[%d]",indiceLibre));

		txt_close_file(bloqueLibre);
		//mutexUNlock

	} else {

		int escribio = 0;

		for(int i = 0;i < list_size(listaBloques);i++){

		int bloque = list_get(listaBloques);

		if(entraDatoEnBloque(registro,bloque)){

			char* path = string_from_format("/home/utnso/Escritorio/PuntoMontaje/TallGrass/Blocks/%d.bin",bloque);

			FILE* archivoBloque = txt_open_for_append(path);

			txt_write_in_file(archivoBloque,"\n");
			txt_write_in_file(archivoBloque,string_from_format("%d-%d=%d",registro->posX,registro->posY,registro->cantidad));

			escribio = 1;

			txt_close_file(archivoBloque);

			break;

			}

		}

		if(!escribio){

			int indiceLibre = buscarIndicePrimerBloqueLibre();

			char* pathLibre = string_from_format("/home/utnso/Escritorio/PuntoMontaje/TallGrass/Blocks/%d.bin",indiceLibre);

			//mutexLock
			FILE* bloqueLibre = txt_open_for_append(pathLibre);

			txt_write_in_file(bloqueLibre,string_from_format("%d-%d=%d",registro->posX,registro->posY,registro->cantidad));

			bitarray_set_bit(bitArray,indiceLibre-1);

			//TODO terminaragregarAListaBloques
			char* nuevosBloques = agregarAListaBloques(listaBloques,indiceLibre);

			config_set_value(configAux,"BLOCKS",nuevosBloques);

			txt_close_file(bloqueLibre);

		}
	}

	config_destroy(configAux);


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

char* obtener_ruta_bloque(int nro_bloque) {
	return string_from_format("%s%d.bin","/home/utnso/Escritorio/PuntoMontaje/Blocks/",nro_bloque);
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

int tamanioRegistro(registroDatos* registro) {

	int tamanio = strlen(string_from_format("%d-%d=%d",registro->posX,registro->posY,registro->cantidad));

	return tamanio;
}

registroDatos* hacerRegistro(uint32_t x,uint32_t y,uint32_t cant){

	registroDatos* registro = malloc(sizeof(registroDatos));

	registro->posX = x;
	registro->posY = y;
	registro->cantidad = cant;

	return registro;

}

int tamanioArchivo(FILE* arch){

	int tamanio;

	fseek(arch,0,SEEK_END);

	tamanio = ftell(arch);

	return tamanio;

}

int tamanioArchivoDadoPath(char* path){

	FILE* arch = txt_open_for_append(path);

	int tamanio = tamanioArchivo(arch);

	txt_close_file(arch);

	return tamanio;

}

bool entraDatoEnBloque(registroDatos* registro, int nroBloque) {

	return tamanioArchivoDadoPath(string_from_format("/home/utnso/Escritorio/PuntoMontaje/TallGrass/Blocks/%d.bin",nroBloque))+tamanioRegistro(registro) < tamanioBloques;

}


void procesarNewPokemon(char* nombrePoke, registroDatos* registro) {

	char* path  = string_from_format("/home/utnso/Escritorio/PuntoMontaje/TallGrass/Files/%s/Metadata.bin",nombrePoke);

	t_config* configPath = config_create(path);

	verificarExistenciaPokemon(nombrePoke);
	verificarAperturaArchivo(path);
	registrarPokemon(nombrePoke,registro);//poner los whiles despues

	int cantidadPokemon = estaPosicionEnArchivo(registro->posX,registro->posY,path);

	if(cantidadPokemon > 0){

		config_set_value(configPath,string_from_format("%d-%d",registro->posX,registro->posY),string_itoa(cantidadPokemon+registro->cantidad));

	} else{

		config_set_value(configPath,string_from_format("%d-%d",registro->posX,registro->posY),string_itoa(registro->cantidad));
	}

	config_destroy(configPath);
}

int estaPosicionEnArchivo(uint32_t posX,uint32_t posY,char* path){

	FILE* arch = txt_open_for_append(path);

	t_config* configPath = config_create(path);

	txt_close_file(arch);

	char* cantidad = config_get_string_value(configPath,string_from_format("%d-%d",posX,posY));



	if(cantidad != NULL){

	int numero = config_get_int_value(configPath,string_from_format("%d-%d",posX,posY));

	config_destroy(configPath);

	return numero;


	}

	config_destroy(configPath);
	return 0;


}
