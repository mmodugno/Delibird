/*
 * utils_gamecard.c
 *
 *  Created on: 15 jun. 2020
 *      Author: utnso
 */

#include"utils_gamecard.h"

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

	if(!strcmp(aux,"Y")){

		printf("NoEntre"); //Deberia delegar un hilo que trate de entrar cada X segundos

	} else {

		modificarArchivoComoConfig(configAux,"OPEN","Y");
	}

	config_destroy(configAux);
}

int buscarIndicePrimerBloqueLibre(){

	int contador = 1;

	char* path = obtener_ruta_bloque(contador);

	while(!estaVacioConRuta(path)){
	contador++;
	memcpy(path,obtener_ruta_bloque(contador),strlen(path));
	}

	FILE* crearArchivo = txt_open_for_append(path);

	txt_close_file(crearArchivo);

	return contador;
}

int obtenerUltimoBloque(t_list* listaBloques) {

	int size = list_size(listaBloques);

	return atoi(list_get(listaBloques,size-1));
}

char* agregarBloqueALista(t_list* bloques, int nuevoBloque) {

	char* nuevosBloques = string_new();

	string_append(&nuevosBloques,"[");

	for(int i = 0; i < list_size(bloques);i++){

	string_append(&nuevosBloques,string_from_format("%d,",atoi(list_get(bloques,i))));

	}

	string_append(&nuevosBloques,string_from_format("%d]",atoi(string_itoa(nuevoBloque))));

	return nuevosBloques;
}

void registrarPokemon(char* nombrePoke, registroDatos* registro) {

	char* path = string_from_format("%s/TallGrass/Files/%s/Metadata.bin",punto_montaje,nombrePoke);

	//FILE* archivoTemporal = fopen(path,"rb+");

	char** bloques;

	t_config* configAux = config_create(path);

	bloques = config_get_array_value(configAux,"BLOCKS");

	t_list* listaBloques = crear_lista(bloques);

	if(list_is_empty(listaBloques)){
		int indiceLibre = buscarIndicePrimerBloqueLibre();

		char* pathLibre = obtener_ruta_bloque(indiceLibre);

		//mutexLock
		FILE* bloqueLibre = txt_open_for_append(pathLibre);

		txt_write_in_file(bloqueLibre,registro_a_string(registro));

	//	bitarray_set_bit(bitArray,indiceLibre-1);

		agregarBloqueParaPokemon(nombrePoke,indiceLibre);

		config_set_value(configAux,"BLOCKS",string_from_format("[%d]",indiceLibre));

		txt_close_file(bloqueLibre);
		//mutexUNlock

	} else {

		int escribio = 0;

		int bloque = obtenerUltimoBloque(listaBloques);

		char* path = obtener_ruta_bloque(bloque);


		FILE* archivoBloque = txt_open_for_append(path);

		if(entraDatoEnBloque(registro,bloque)){


			txt_write_in_file(archivoBloque,registro_a_string(registro));

			escribio = 1;

			txt_close_file(archivoBloque);
		}
		else {

			int tamanio_restante = tamanioRestante(archivoBloque); //Al tamaño maximo le resta el ocupado

			if(tamanio_restante > 0){

			char* primeraParte = string_substring_until(registro_a_string(registro),tamanio_restante);

			txt_write_in_file(archivoBloque,primeraParte);

			}

			int indiceSiguienteLibre = buscarIndicePrimerBloqueLibre();

			char* path = obtener_ruta_bloque(indiceSiguienteLibre);

			FILE* archivoBloqueLibre = txt_open_for_append(path);

			char* segundaParte = string_substring_from(registro_a_string(registro),tamanio_restante);

			txt_write_in_file(archivoBloqueLibre,segundaParte);

			agregarBloqueParaPokemon(nombrePoke,indiceSiguienteLibre);

			txt_close_file(archivoBloqueLibre);

			txt_close_file(archivoBloque);

			escribio = 1;
			}


		if(!escribio){

			int indiceLibre = buscarIndicePrimerBloqueLibre();

			char* pathLibre = obtener_ruta_bloque(indiceLibre);

			//mutexLock
			FILE* bloqueLibre = txt_open_for_append(pathLibre);

			txt_write_in_file(bloqueLibre,string_from_format("%d-%d=%d",registro->posX,registro->posY,registro->cantidad));

			bitarray_set_bit(bitArray,indiceLibre-1);

			char* nuevosBloques = agregarBloqueALista(listaBloques,indiceLibre);

			config_set_value(configAux,"BLOCKS",nuevosBloques);

			txt_close_file(bloqueLibre);

		}
}


	//sleep(tiempo_retardo_operacion);
	//marcar archivo como cerrado;

	//conetar a broker y enviar a cola Appeared pokemon(datos = idRecibido, pokemon, posicionMapa);
	config_destroy(configAux);


}

void leerMetadata(){


	t_config* configAux = config_create("/home/utnso/Escritorio/PuntoMontaje/Metadata/Metadata.bin");

	tamanioBloques =  config_get_int_value(configAux,"BLOCK_SIZE");
	cantidadBloques = config_get_int_value(configAux,"BLOCKS");

	config_destroy(configAux);

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


void crearBitmap(){


	FILE* bitmap = txt_open_for_append("/home/utnso/Escritorio/PuntoMontaje/Metadata/Bitmap.bin");

	fseek(bitmap,0,SEEK_END);

	if(ftell(bitmap) == 0){

	char* puntero_a_bits = malloc(1);

	bitArray = bitarray_create_with_mode(puntero_a_bits,cantidadBloques/8,LSB_FIRST);

	}

	txt_close_file(bitmap);

}

void procesarNewPokemon(char* nombrePoke, registroDatos* registro) {

	char* path  = string_from_format("/home/utnso/Escritorio/PuntoMontaje/TallGrass/Files/%s/Metadata.bin",nombrePoke);

	t_config* configPath = config_create(path);

	verificarExistenciaPokemon(nombrePoke);
	verificarAperturaArchivo(path);

	t_list* listaBloques = crear_lista(config_get_array_value(configPath,"BLOCKS"));

	int yaRegistrado = 0;
	int i = 0;

	char* key = string_from_format("%d-%d",registro->posX,registro->posY);

	while(yaRegistrado == 0 && i < list_size(listaBloques)){


		char* primerFd = obtener_ruta_bloque(atoi(list_get(listaBloques,i)));

		int fd = open(primerFd,O_RDWR);

		struct stat sb;
		fstat(fd,&sb);

		char* file_memory = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);

		char* conjuntoConKey = malloc(sb.st_size);

		int j;

		strcpy(conjuntoConKey,file_memory);

		char* keyCompleta = string_new();

		string_append(&keyCompleta,key);
		string_append(&keyCompleta,"=");
		string_append(&keyCompleta,string_itoa(registro->cantidad));

		if(string_contains(conjuntoConKey,keyCompleta)){

			char** bloques;

			char* nuevoConjunto = string_new();

			bloques = string_split(conjuntoConKey,"\n");
			int k;
			int tamanio = tamanio_array(bloques);

			for(k=0;k < tamanio;k++){

				if(string_contains(bloques[k],key)){

						int valor = (registro->cantidad);

						registroDatos* reg = string_a_registro(bloques[k]);
						reg->cantidad = reg->cantidad + valor;

						bloques[k] = registro_a_string(reg);

						string_append(&nuevoConjunto,bloques[k]); //ya incluye salto de linea en hacer registro

						yaRegistrado = 1;

					} else {

						string_append(&nuevoConjunto,bloques[k]);

						if(k != tamanio-1){
							string_append(&nuevoConjunto,"\n");

						}

					}
				}

				write(fd,nuevoConjunto,sb.st_size);
				break;
			}

		close(fd);
		i++;

	}


	int j = 0;

	//si esta cortado
	while(yaRegistrado == 0 && j < list_size(listaBloques)-1){

	char* primerFd = obtener_ruta_bloque(atoi(list_get(listaBloques,j)));
	char* segundoFd = obtener_ruta_bloque(atoi(list_get(listaBloques,j+1)));

	int fd = open(primerFd,O_RDWR);
	int fd2 = open(segundoFd,O_RDWR);

	struct stat sb;
	fstat(fd,&sb);

	struct stat sb2;
	fstat(fd2,&sb2);

	char* file_memory = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);
	char* file_memory2 = mmap(NULL,sb2.st_size,PROT_READ,MAP_PRIVATE,fd2,0);

	int nuevoTamanio = sb.st_size + sb2.st_size;

	char* conjunto = malloc(nuevoTamanio);

	strcpy(conjunto,file_memory);
	strcpy(conjunto,file_memory2);

	llenarConjunto(conjunto,file_memory,file_memory2,sb,sb2);

	char** bloques;

	char* nuevoConjunto = string_new();

	if(string_contains(conjunto,key)) {

		bloques = string_split(conjunto,"\n");
		int k;
		int tamanio = tamanio_array(bloques);

		for(k=0;k < tamanio;k++){

			if(string_contains(bloques[k],key)){

				int valor = (registro->cantidad);

				registroDatos* reg = string_a_registro(bloques[k]);
				reg->cantidad = reg->cantidad + valor;

				bloques[k] = registro_a_string(reg);

				string_append(&nuevoConjunto,bloques[k]); //ya incluye salto de linea en hacer registro

				yaRegistrado = 1;

			} else {

				string_append(&nuevoConjunto,bloques[k]);

				if(k != tamanio-1){
					string_append(&nuevoConjunto,"\n");

				}

			}
		}

		char* subtring = string_substring_from(nuevoConjunto,tamanioBloques);

		write(fd,nuevoConjunto,tamanioBloques);
		write(fd2,subtring,sb2.st_size);

		close(fd);
		close(fd2);


		break;
		}

		j++;
	}

	if(!yaRegistrado){
	registrarPokemon(nombrePoke,registro);
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

int tamanioRestante(FILE* arch) {

	return tamanioBloques - tamanioArchivo(arch);

}

void agregarBloqueParaPokemon(char* nombrePoke,int indiceSiguienteLibre){


	t_config* configPoke = config_create(string_from_format("/home/utnso/Escritorio/PuntoMontaje/TallGrass/Files/%s/Metadata.bin",nombrePoke));

	char** bloques = config_get_array_value(configPoke,"BLOCKS");

	t_list* listaBloques = crear_lista(bloques);

	config_set_value(configPoke,"BLOCKS",agregarBloqueALista(listaBloques,indiceSiguienteLibre));

	config_save(configPoke);

	config_destroy(configPoke);


}

//////////////////////////////////////FUNCIONES AUXILIARES//////////////////////////////////////

void modificarArchivoComoConfig(t_config* configModif,char* key,char* valor){

	config_set_value(configModif,key,valor);

	config_save(configModif);

	//config_destroy(configModif);

}

void crearDirectorio(char* path ,char* nombreCarpeta){

	char* aux = malloc(strlen(path) + strlen(nombreCarpeta) + 2);

	strcpy(aux,path);
	strcat(aux,nombreCarpeta);

	mkdir(aux,0777);

	free(aux);
}

t_list* crear_lista(char** array){

    t_list* nuevaLista = list_create();
    int i = 0;
    while( array[i] != NULL ){
        list_add(nuevaLista, array[i]);
        i++;
    }
    return nuevaLista;
}

char* registro_a_string(registroDatos* registro) {

	char* key = string_from_format("%d-%d",registro->posX,registro->posY);

	char* value = string_from_format("%d",registro->cantidad);

	char* reg_string = string_from_format("%s=%s\n",key, value);

	//free(key);
	//free(value);

	return reg_string;
}

registroDatos* string_a_registro(char* string) {

	char** keyValor = string_split(string,"=");
	char** posiciones = string_split(keyValor[0],"-");

	int x = atoi(posiciones[0]);
	int y = atoi(posiciones[1]);
	int cantidad = atoi(keyValor[1]);

	registroDatos* aux = hacerRegistro(x,y,cantidad);

	return aux;

}

int tam_registro(registroDatos* registro) {
	char* registro_formateado = registro_a_string(registro);
	int tam_registro = strlen(registro_formateado);
	//free(registro_formateado);
	return tam_registro;
}

bool entraDatoEnBloque(registroDatos* registro, int nroBloque) {
	return tamanioArchivoDadoPath(obtener_ruta_bloque(nroBloque))+tamanioRegistro(registro) < tamanioBloques;
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

char* obtener_ruta_bloque(int nro_bloque) {
	return string_from_format("%s/TallGrass/Blocks/%d.bin",punto_montaje,nro_bloque);
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

void llenarConjunto(char* conjunto,char* file_memory,char* file_memory2,struct stat sb,struct stat sb2) {

		int i,j;

		for(i=0;i<sb.st_size;i++){
					conjunto[i] = file_memory[i];
				}

		for(j=0; j< sb2.st_size;j++){
					conjunto[sb.st_size+j] = file_memory2[j];
				}

}

int tamanio_array(char** bloque){

	int contador = 0;

	while(bloque[contador] != NULL){
		contador++;
	}

	return contador;

}

bool configConKeyCompleta(char* rutaConfig,char* key,int posY) {

			FILE* archBloque = fopen(rutaConfig,"r");

			char ultimo;

			fseek(archBloque,-1,SEEK_END);

			fread(&ultimo,1,1,archBloque);

			txt_close_file(archBloque);

			return ultimo != '=' && atoi(&ultimo) != posY;

}
