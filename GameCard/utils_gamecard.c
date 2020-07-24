/*
 * utils_gamecard.c
 *
 *  Created on: 15 jun. 2020
 *      Author: utnso
 */
/*
 * utils_gamecard.c
 *
 *  Created on: 15 jun. 2020
 *      Author: utnso
 */

#include"utils_gamecard.h"


int verificarExistenciaPokemon(char* nombrePoke) {

	char* path = "/home/utnso/PuntoMontaje/TallGrass/Files/";

	char* archivo = "/Metadata.bin";

	char* aux = malloc(strlen(path) + strlen(nombrePoke) + strlen(archivo) + 3);

	crearDirectorio(path,nombrePoke);

	strcpy(aux,path);
	strcat(aux,nombrePoke);
	strcat(aux,archivo);

	FILE* metadataPoke = txt_open_for_append(aux);

	int existe = 1;

	if(estaVacio(metadataPoke)){

	txt_write_in_file(metadataPoke,"DIRECTORY=N\n");
	txt_write_in_file(metadataPoke,"SIZE=0\n");
	txt_write_in_file(metadataPoke,"BLOCKS=[]\n"); //CAMBIAR () a corchetes
	txt_write_in_file(metadataPoke,"OPEN=N");

	txt_close_file(metadataPoke);

	existe = 0;

	}



	free(aux);

	return existe;

}

int verificarSiExisteArchivo(char* path) {

	int fd = open(path,O_RDWR);

	int existe = fd;

	close(fd);

	return existe;

}

void verificarAperturaArchivo(char* path) {

	char* aux;

	t_config* configAux = config_create(path);

	aux = config_get_string_value(configAux,"OPEN");

	if(!strcmp(aux,"Y")){

		printf("Abierto, reintentando en: %d segundos \n",tiempo_reintento_operacion);
		fflush(stdout);
		sleep(tiempo_reintento_operacion);
		verificarAperturaArchivo(path);

	} else {
	modificarArchivoComoConfig(configAux,"OPEN","Y");
	}

	config_destroy(configAux);
}

int buscarIndicePrimerBloqueLibre(){

	int contador = 1;
/*
while(!estaVacioConRuta(string_from_format("/home/utnso/Escritorio/PuntoMontaje/TallGrass/Blocks/%d.bin",contador))){
	contador++;
}
*/

	while(bitarray_test_bit(bitArray,contador-1)){
		contador++;
	}


	char* path = obtener_ruta_bloque(contador);

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

		sem_wait(&mutex_bit_array);

		bitarray_set_bit(bitArray,indiceLibre-1);

		actualizar_bitmap();

		sem_post(&mutex_bit_array);

		msync(bitArray->bitarray,1,MS_SYNC);

		config_set_value(configAux,"BLOCKS",string_from_format("[%d]",indiceLibre));

		config_save(configAux);

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

			bitarray_set_bit(bitArray,indiceSiguienteLibre-1);

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

			//bitarray_set_bit(bitArray,indiceLibre-1);

			char* nuevosBloques = agregarBloqueALista(listaBloques,indiceLibre);

			config_set_value(configAux,"BLOCKS",nuevosBloques);

			txt_close_file(bloqueLibre);

		}

		config_destroy(configAux);
}


}

void leerMetadata(){


	t_config* configAux = config_create("/home/utnso/PuntoMontaje/Metadata/Metadata.bin");

	tamanioBloques =  config_get_int_value(configAux,"BLOCK_SIZE");
	cantidadBloques = config_get_int_value(configAux,"BLOCKS");

	config_destroy(configAux);

}

void calcularTamanioMetadata(char* pokemon) {

	char* path  = string_from_format("/home/utnso/PuntoMontaje/TallGrass/Files/%s/Metadata.bin",pokemon);

	t_config* configAux = config_create(path);

	char** bloques = config_get_array_value(configAux,"BLOCKS");

	t_list* listaBloques = crear_lista(bloques);

	int tamanio = 0;

	int i;

	for(i = 0; i < list_size(listaBloques);i++){


		char* pathbBLoque = obtener_ruta_bloque(atoi(list_get(listaBloques,i)));

		int tamanioActual = tamanioArchivoDadoPath(pathbBLoque);

		tamanio = tamanio + tamanioActual;

		}

	modificarArchivoComoConfig(configAux,"SIZE",string_itoa(tamanio));

	config_destroy(configAux);

}


void procesarNewPokemon(char* nombrePoke, registroDatos* registro, int id) {


	char* path  = string_from_format("/home/utnso/PuntoMontaje/TallGrass/Files/%s/Metadata.bin",nombrePoke);

	int existe = verificarExistenciaPokemon(nombrePoke);
	verificarAperturaArchivo(path);

	t_list* listaBloques;

	if(existe){

		t_config* configPath = config_create(path);
		listaBloques = crear_lista(config_get_array_value(configPath,"BLOCKS"));
		config_destroy(configPath);
	}

	else {
		listaBloques = list_create();
	}


	sem_wait(&sem_escritura);

	int yaRegistrado = sumarSiEstaEnBloque(listaBloques,registro);

	if(!yaRegistrado){
	registrarPokemon(nombrePoke,registro);
	}

	sem_post(&sem_escritura);

	t_config* configPath = config_create(path);

	cerrarArchivoMetadataPoke(configPath);

	config_destroy(configPath);

	int conexion = conectarse_con_broker();

	if(conexion < 0){
		printf("No me pude cpnectar para enviar resouesta de New \n");
		fflush(stdout);
		log_info(logFalloConexion,"Fallo conexion con Broker");
	} else {

		printf("Envio APPEARED \n");
		fflush(stdout);
		enviar_appeared(conexion,nombrePoke,registro->posX,registro->posY,id);
	}

	close(conexion);

	calcularTamanioMetadata(nombrePoke);

	sleep(tiempo_retardo_operacion);


}

void procesarCatchPokemon(char* nombrePoke,uint32_t posX, uint32_t posY,int id){


	char* path  = string_from_format("/home/utnso/PuntoMontaje/TallGrass/Files/%s/Metadata.bin",nombrePoke);


	verificarDirectorioPokemon(nombrePoke);
	verificarAperturaArchivo(path);


	t_config* configPath = config_create(path);

	int cantidad = -1;
	registroDatos* regAux = hacerRegistro(posX,posY,cantidad);

	t_list* listaBloques = crear_lista(config_get_array_value(configPath,"BLOCKS"));

	sem_wait(&sem_escritura);

	int yaRegistrado = sumarSiEstaEnBloque(listaBloques,regAux);

	uint32_t resultado;

	if(!yaRegistrado){
		perror("No existe posicion para el pokemon");
		resultado = 0;
	} else {
		printf("Lo atrapaste!");
		fflush(stdout);
		buscarYeliminarCeros(listaBloques);
		resultado = 1;
	}


	sem_wait(&mutex_bit_array);

	eliminarBloquesVacios(nombrePoke);

	actualizar_bitmap();

	sem_post(&mutex_bit_array);

	sem_post(&sem_escritura);

	sleep(tiempo_retardo_operacion);

	config_destroy(configPath);

	configPath = config_create(path);

	cerrarArchivoMetadataPoke(configPath);

	int conexion = conectarse_con_broker();

	if(conexion < 0){
		log_info(logFalloConexion,"Fallo conexion con Broker");
	} else {
		enviar_caught(conexion,resultado,id);
	}

	config_destroy(configPath);

	calcularTamanioMetadata(nombrePoke);

}

void procesarGetPokemon(char* nombrePoke, int id){

	char* path  = string_from_format("/home/utnso/PuntoMontaje/TallGrass/Files/%s/Metadata.bin",nombrePoke);

	t_list* listaRegistros;

	if(verificarSiExisteArchivo(path) == -1){
		listaRegistros = list_create();
		printf("No existe el Pokemon");
	} else {

	verificarAperturaArchivo(path);

	listaRegistros = obtenerPosiciones(nombrePoke);

	//sleep(tiempo_retardo_operacion);

	t_config* configPath = config_create(path);

	cerrarArchivoMetadataPoke(configPath);

	config_destroy(configPath);

	}

	int conexion = conectarse_con_broker();

	if(conexion < 0){
			log_info(logFalloConexion,"Fallo conexion con Broker");
			printf("No me pude cpnectar para enviar resouesta de New \n");
				fflush(stdout);
		} else {

			if(list_is_empty(listaRegistros)) {
				uint32_t* posicionesX;
				uint32_t* posicionesY;

				enviar_localized(conexion,nombrePoke,0,posicionesX,posicionesY,id);
				} else {

					uint32_t paresDePosiciones = list_size(listaRegistros);

					int i = 0;

					uint32_t posicionesX[paresDePosiciones];
					uint32_t posicionesY[paresDePosiciones];

					for(i = 0; i < paresDePosiciones;i ++) {

						registroDatos* registro = list_get(listaRegistros,i);

						posicionesX[i] = registro->posX;
						posicionesY[i] = registro->posY;

					}

					printf("Estoy por mandar Localized \n");
					fflush(stdout);

					enviar_localized(conexion,nombrePoke,paresDePosiciones,posicionesX,posicionesY,id);

					printf("Mande Localized \n");
					fflush(stdout);
				}

		}
	close(conexion);

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


	t_config* configPoke = config_create(string_from_format("/home/utnso/PuntoMontaje/TallGrass/Files/%s/Metadata.bin",nombrePoke));

	char** bloques = config_get_array_value(configPoke,"BLOCKS");

	t_list* listaBloques = crear_lista(bloques);

	config_set_value(configPoke,"BLOCKS",agregarBloqueALista(listaBloques,indiceSiguienteLibre));

	config_save(configPoke);

	config_destroy(configPoke);


}

t_list* obtenerPosiciones(char* nombrePoke){

	int j = 0;

	t_config* configPoke = config_create(string_from_format("/home/utnso/PuntoMontaje/TallGrass/Files/%s/Metadata.bin",nombrePoke));

	char** bloques = config_get_array_value(configPoke,"BLOCKS");

	t_list* listaBloques = crear_lista(bloques);

	char* conjunto = string_new();

	while(j < list_size(listaBloques)){

		char* primerFd = obtener_ruta_bloque(atoi(list_get(listaBloques,j)));

		int fd = open(primerFd,O_RDWR);

		struct stat sb;
		fstat(fd,&sb);

		char* file_memory = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);

		string_append(&conjunto,file_memory);

		close(fd);
		j++;

	}

	char** arrayRegistros = string_split(conjunto,"\n");

	t_list* listaRegistrosString = crear_lista(arrayRegistros);

	t_list* listaRegistros = list_map(listaRegistrosString, (void*) string_a_registro);

	config_destroy(configPoke);

	return listaRegistros;
}

int sumarSiEstaEnBloque(t_list* listaBloques,registroDatos* registro) {

	char* key = string_from_format("%d-%d",registro->posX,registro->posY); //lee posicion a buscar

	int yaRegistrado = 0;

	int i = 0;

	//si la lista es vacia

	if(list_is_empty(listaBloques)) {
	return yaRegistrado;
	}

	if(list_size(listaBloques) == 1 && registro->cantidad == -1){
		char* primerFd = obtener_ruta_bloque(atoi(list_get(listaBloques,i)));

		int fd = open(primerFd,O_RDWR);

		struct stat sb;
		fstat(fd,&sb);

		char* file_memory = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);

		char* conjuntoConKey = malloc(sb.st_size);

		strcpy(conjuntoConKey,file_memory);

		if(string_contains(conjuntoConKey,key)){
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

			}

		close(fd);
	}

	//si no esta cortado
	while(yaRegistrado == 0 && i < list_size(listaBloques)){

		char* primerFd = obtener_ruta_bloque(atoi(list_get(listaBloques,i)));

		int fd = open(primerFd,O_RDWR);

		struct stat sb;
		fstat(fd,&sb);

		char* file_memory = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);

		char* conjuntoConKey = malloc(sb.st_size);

		strcpy(conjuntoConKey,file_memory);

		char* keyCompleta = string_new();

		string_append(&keyCompleta,key);
		string_append(&keyCompleta,"=");
		string_append(&keyCompleta,string_itoa(registro->cantidad));

		//TODO ver esto
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

	return yaRegistrado;

}

void buscarYeliminarCeros(t_list* listaBloques){

	int i;
	char* todasLasPosiciones = string_new();
//ok
	for(i = 0; i < list_size(listaBloques);i++){

		char* primerFd = obtener_ruta_bloque(atoi(list_get(listaBloques,i)));

		int fd = open(primerFd,O_RDWR);

		struct stat sb;
		fstat(fd,&sb);

		char* file_memory = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);

		string_append(&todasLasPosiciones,file_memory);

		close(fd);
	}

	char** arrayPosiciones = string_split(todasLasPosiciones,"\n");

	t_list* listaTodosPosiciones = crear_lista(arrayPosiciones);
//todo ver q onda

	while(list_any_satisfy(listaTodosPosiciones,(void*) tieneCantidadCero)) {
		list_remove_by_condition(listaTodosPosiciones, (void*) tieneCantidadCero);
	}


	char* nuevasPosiciones = listToString(listaTodosPosiciones);

	string_append(&nuevasPosiciones,"\n");

	int cuanto = string_length(nuevasPosiciones);

	for(i = 0; i < list_size(listaBloques);i++){

		char* ruta = obtener_ruta_bloque(atoi(list_get(listaBloques,i)));

		vaciarArchivo(ruta);
	}

	int k = 0;

	while(cuanto > 0){

			char* unFd = obtener_ruta_bloque(atoi(list_get(listaBloques,k)));

			int fd = open(unFd,O_RDWR);

			if(cuanto > tamanioBloques) {

				write(fd,nuevasPosiciones,tamanioBloques);
				cuanto = cuanto - tamanioBloques;

			} else {
				write(fd,nuevasPosiciones,cuanto);
				cuanto = cuanto - cuanto;
			}

			k++;
			close(fd);
	}




}

char* listToString(t_list* lista) {

	char* string = string_new();

	int i;

	for(i = 0; i < list_size(lista);i++){

		char* aSumar = list_get(lista,i);

		string_append(&string,aSumar);

		if(i != list_size(lista)-1){
			string_append(&string,"\n");
		}

	}

	return string;

}

void eliminarBloquesVacios(char* nombrePoke){

	t_config* configPoke = config_create(string_from_format("/home/utnso/PuntoMontaje/TallGrass/Files/%s/Metadata.bin",nombrePoke));

	char** bloques = config_get_array_value(configPoke,"BLOCKS");

	t_list* listaBloques = crear_lista(bloques);

	t_list* nuevaListaBloques = list_create();

	char* stringNuevaLista = string_new();

	int i;

	for(i = 0; i < list_size(listaBloques); i++) {

		int bloqueAux = atoi(list_get(listaBloques,i));
		char* rutaActual = obtener_ruta_bloque(bloqueAux);

		if(!estaVacioConRuta(rutaActual)){
			stringNuevaLista = agregarBloqueALista(nuevaListaBloques,atoi(list_get(listaBloques,i)));
			list_add(nuevaListaBloques,list_get(listaBloques,i));
		}
		else{
					bitarray_clean_bit(bitArray,bloqueAux-1);
		}
	}

	config_set_value(configPoke,"BLOCKS",stringNuevaLista);

	config_save(configPoke);

	config_destroy(configPoke);


}

bool tieneCantidadCero(char* registroString) {

	registroDatos* registro = string_a_registro(registroString);

	bool resultado = registro->cantidad == 0;

	return resultado;

}
//////////////////////////////////////FUNCIONES AUXILIARES//////////////////////////////////////

void modificarArchivoComoConfig(t_config* configModif,char* key,char* valor){

	config_set_value(configModif,key,valor);

	config_save(configModif);

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

void cerrarArchivoMetadataPoke(t_config* configPoke){

	modificarArchivoComoConfig(configPoke,"OPEN","N");

}

void verificarDirectorioPokemon(char* nombrePoke){

	char* ruta = string_from_format("/home/utnso/PuntoMontaje/TallGrass/Files/%s/Metadata.bin",nombrePoke);

	int f = open(ruta,O_RDWR);

	if( f < 0 ){
	perror("No existe el pokemon");
	exit(1);
	}

	close(f);

}

void vaciarArchivo(char* ruta) {

	remove(ruta);

	FILE* arch = txt_open_for_append(ruta);

	txt_close_file(arch);

}


///////////////////////////////////////   BITARRAY   ///////////////////////////////////////

void inicializar_bitarray() {
	//loggear_trace(string_from_format("Inicializando bitarray"));
	FILE* archivo_bitmap = fopen(
			"/home/utnso/PuntoMontaje/Metadata/Bitmap.bin", "r");

	char* bitmap = malloc(cantidadBloques / 8 + 1);
	int resultado_read = 0;

	while (!resultado_read)
		resultado_read = fread(bitmap, sizeof(char),
				sizeof(char) * (cantidadBloques / 8) + 1, archivo_bitmap);

	bitmap[cantidadBloques / 8] = 0;
	bitArray = bitarray_create_with_mode(bitmap, cantidadBloques / 8,
			LSB_FIRST);
	fclose(archivo_bitmap);

}

void inicializar_bitmap() {

	if (estaVacioConRuta(
			"/home/utnso/PuntoMontaje/Metadata/Bitmap.bin")) {
		FILE* archivo_bitmap = fopen(
				"/home/utnso/PuntoMontaje/Metadata/Bitmap.bin",
				"w+");
		fclose(archivo_bitmap);
		truncate("/home/utnso/PuntoMontaje/Metadata/Bitmap.bin",
				cantidadBloques / 8); //te deja el archivo completo en cero.
	}

}

void crearFilesAndBlocks() {

	crearDirectorio("/home/utnso/PuntoMontaje/TallGrass", "/Files");

	metadataFiles = txt_open_for_append(
			"/home/utnso/PuntoMontaje/TallGrass/Files/Metadata.bin");

	if (estaVacio(metadataFiles)) {

		txt_write_in_file(metadataFiles, "DIRECTORY=Y");

		txt_close_file(metadataFiles);

	}

	crearDirectorio("/home/utnso/PuntoMontaje/TallGrass", "/Blocks");

}

void crearBitmap() {

	FILE* bitmap = txt_open_for_append(
			"/home/utnso/PuntoMontaje/Metadata/Bitmap.bin");

	fseek(bitmap, 0, SEEK_END);

	if (ftell(bitmap) == 0) {

		char* puntero_a_bits = malloc(1);

		bitArray = bitarray_create_with_mode(puntero_a_bits,
				cantidadBloques / 8, LSB_FIRST);

	}

	txt_close_file(bitmap);

}

void actualizar_bitmap() {
	FILE* bitmap = fopen(
			"/home/utnso/PuntoMontaje/Metadata/Bitmap.bin", "wb+");
	fwrite(bitArray->bitarray, sizeof(char), sizeof(char) * bitArray->size,
			bitmap);
	fclose(bitmap);
}




///////////////////////////////////////CONEXIONES///////////////////////////////////////


//ESTE ES EL SV PARA RECIBIR MENSAJES
void iniciar_servidor(void)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ipGamecard,puertoGamecard, &hints, &servinfo);

    int activado = 1;


    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

	setsockopt(socket_servidor,SOL_SOCKET,SO_REUSEADDR,&activado,sizeof(activado));

	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    while(1){
    	esperar_cliente(socket_servidor);

    }

}

int conectarse_con_broker(void){
	int conexionBroker = crear_conexion(ipBroker,puertoBroker);
	if(conexionBroker <= 0){
		//log_info(comunicacion_broker_error,"No se pudo conectar con Broker,se realizará la operación por default");
		//broker_default();
		return -1;
	}
	else{
		//log_info(comunicacion_broker_resultado,"me conecte a Broker exitosamente");
		return conexionBroker;
	}

}

void serve_client(int* socketqueMePasan)
{
	int socket = *socketqueMePasan;
	free(socketqueMePasan);
	//sem_wait(&sem_mensaje);
	int cod_op;

	int i = recv(socket, &cod_op, sizeof(op_code), MSG_WAITALL);
	if(i <= 0)
		cod_op = -1;
	process_request(cod_op, socket);
}

void esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	//poner en globales si es necesario

	pthread_t thread;


	socklen_t  tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	//log_info(logConexion," se conectaron a broker");
	int *socketAPasar = malloc(sizeof(int));
	*socketAPasar = socket_cliente;

	pthread_create(&thread,NULL,(void*)serve_client,socketAPasar);
	pthread_detach(thread);

}

void process_request(int cod_op, int cliente_fd) {

	uint32_t tamanio_buffer;
	uint32_t tamanio_username;
	int id;

	char* username;

	recv(cliente_fd, &tamanio_username, sizeof(uint32_t), MSG_WAITALL);

	username = malloc(tamanio_username);
	recv(cliente_fd, username, tamanio_username, MSG_WAITALL);

	recv(cliente_fd, &tamanio_buffer, sizeof(uint32_t), MSG_WAITALL);

	registroConNombre* registroConNombre;

	char *nombre;

	//sem_wait(&sem_mensaje);

	switch (cod_op) {

	case GAMECARD__NEW_POKEMON:

		recv(cliente_fd, &id, sizeof(uint32_t), 0);
		registroConNombre = deserializar_new_pokemon_Gamecard(cliente_fd);
		//TODO guardar este id para que el appeared de este mensaje tenga el id_relativo que id

		procesarNewPokemon(registroConNombre->nombre,
				registroConNombre->registro, id);

		free(registroConNombre);

		break;

	case GAMECARD__CATCH_POKEMON:

		recv(cliente_fd, &id, sizeof(uint32_t), 0);
		registroConNombre = deserializar_catch_pokemon_Gamecard(cliente_fd);

		//TODO guardar este id para que el CAUGHT de este mensaje tenga el id_relativo que id
		procesarCatchPokemon(registroConNombre->nombre,
				registroConNombre->registro->posX,
				registroConNombre->registro->posY,id);

		free(registroConNombre);

		break;

	case GAMECARD__GET_POKEMON:

		recv(cliente_fd, &id, sizeof(uint32_t), 0);
		nombre = deserializar_get_pokemon_Gamecard(cliente_fd);

		//TODO guardar este id para que el Localized de este mensaje tenga el id_relativo que id
		procesarGetPokemon(nombre,id);

		free(nombre);

		break;

	case BROKER__NEW_POKEMON:

		recv(cliente_fd, &id, sizeof(uint32_t), 0);
		//pthread_mutex_unlock(&llegadaMensajesTHREAD);

		registroConNombre = deserializar_new_pokemon_Gamecard(cliente_fd);

		printf("Recibo new de broker \n");
		fflush(stdout);

		int envio_de_ack = crear_conexion(ipBroker,puertoBroker);

		if (envio_de_ack != -1) {

			printf("Envio ACK \n");
			enviarACK(id, envio_de_ack, "GAMECARD");

		}

	//	printf("Por eliminar socket \n");
	//	fflush(stdout);
		//close(envio_de_ack);
		//printf("sockect muerto \n");
//		fflush(stdout);

		sleep(1);

		procesarNewPokemon(registroConNombre->nombre,
				registroConNombre->registro, id);

		free(registroConNombre);
		//////pthread_mutex_unlock(&llegadaMensajesTHREAD);
		break;

	case BROKER__GET_POKEMON:

		sem_wait(&sem_mensaje);

		recv(cliente_fd, &id, sizeof(uint32_t), 0);
		//pthread_mutex_unlock(&llegadaMensajesTHREAD);

		nombre = deserializar_get_pokemon_Gamecard(cliente_fd);

		printf("Recibo get de broker \n");
		fflush(stdout);

		int envio_de_ack_get = crear_conexion(ipBroker,puertoBroker);

		if (envio_de_ack_get != -1) {

			printf("Por enviar ACK \n");
			enviarACK(id, envio_de_ack_get, "GAMECARD");

		}

		printf("Por eliminar socket \n");
		fflush(stdout);
	//	close(envio_de_ack_get);
		printf("sockect muerto \n");
		fflush(stdout);

		sleep(1);

		procesarGetPokemon(nombre,id);

		free(nombre);
		//////pthread_mutex_unlock(&llegadaMensajesTHREAD);
		sem_post(&sem_mensaje);

		break;

	case BROKER__CATCH_POKEMON:

		recv(cliente_fd, &id, sizeof(uint32_t), 0);
		//pthread_mutex_unlock(&llegadaMensajesTHREAD);

		registroConNombre = deserializar_catch_pokemon_Gamecard(cliente_fd);

		printf("Recibo catch de broker \n");
		fflush(stdout);

		int envio_de_ack_catch = crear_conexion(ipBroker,puertoBroker);

		if (envio_de_ack_catch != -1) {

			printf("Por enviar ACK \n");
			enviarACK(id, envio_de_ack_catch, "GAMECARD");

		}

		printf("Por eliminar socket \n");
		fflush(stdout);
	//	close(envio_de_ack_catch);
		printf("sockect muerto \n");
		fflush(stdout);

		sleep(1);

		procesarCatchPokemon(registroConNombre->nombre,registroConNombre->registro->posX,
				registroConNombre->registro->posY,id);

		free(registroConNombre);
		//////pthread_mutex_unlock(&llegadaMensajesTHREAD);
		break;

	case 0:
		pthread_exit(NULL);
	case -1:
		pthread_exit(NULL);

	}
	//sem_post(&sem_mensaje);
}

void* recibir_mensaje(int socket_cliente, int* size)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

//ESTO ES PARA SER EL CLIENTE
int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) != 0){
		//printf("error");
		freeaddrinfo(server_info);
		return -1;
	}


	freeaddrinfo(server_info);

	return socket_cliente;
}

//////////////////////////////////////SERIALIZACIONES Y DESERIALIZACIONES///////////////

registroConNombre* deserializar_new_pokemon_Gamecard(int socket_cliente){


	registroConNombre* registroConName = malloc(sizeof(registroConNombre));
	registroConName->registro = malloc(sizeof(registroDatos));

	int tamanioNombre;

    //void* nombre;
    /////NO HACE FALTA NOMBRE, PODEMOS HACER UN MALLOC DEL NOMBRE DESPUES DE PEDIR EL TAMANIO DEL NOMBRE

    recv(socket_cliente,&(tamanioNombre),sizeof(uint32_t),0);

    registroConName->nombre = malloc(tamanioNombre);

    recv(socket_cliente,registroConName->nombre,tamanioNombre,0);
    //memcpy(newPoke->datos->nombrePokemon,nombre,newPoke->datos->tamanioNombre);

    recv(socket_cliente,&(registroConName->registro->posX),sizeof(uint32_t),0);

    recv(socket_cliente,&(registroConName->registro->posY),sizeof(uint32_t),0);

    recv(socket_cliente,&(registroConName->registro->cantidad),sizeof(uint32_t),0);

    return registroConName;

}

registroConNombre* deserializar_catch_pokemon_Gamecard(int socket_cliente){


	registroConNombre* registroConNombre = malloc(sizeof(registroConNombre));
	registroConNombre->registro = malloc(sizeof(registroDatos));

	int tamanioNombre;

    recv(socket_cliente,&(tamanioNombre),sizeof(uint32_t),0);

    registroConNombre->nombre = malloc(tamanioNombre);

    recv(socket_cliente,registroConNombre->nombre,tamanioNombre,0);

    recv(socket_cliente,&(registroConNombre->registro->posX),sizeof(uint32_t),0);

    recv(socket_cliente,&(registroConNombre->registro->posY),sizeof(uint32_t),0);

    registroConNombre->registro->cantidad = 0;

    return registroConNombre;

}

char* deserializar_get_pokemon_Gamecard(int socket_cliente){

	int tamanioNombre;

    recv(socket_cliente,&(tamanioNombre),sizeof(uint32_t),0);

    char* nombre = malloc(tamanioNombre);

    recv(socket_cliente,nombre,tamanioNombre,0);

    return nombre;

}

void enviar_appeared(int socket_cliente,char* nombrePokemon, int posX,int posY, int id_rel) {

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__APPEARED_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username);
	paquete_a_enviar->username = username;

	t_buffer* buffer = malloc(sizeof(t_buffer));

	broker_appeared_pokemon* brokerAppearedPokemon = malloc(sizeof(broker_appeared_pokemon));
	brokerAppearedPokemon->datos = malloc(sizeof(appeared_pokemon));
	brokerAppearedPokemon->datos->tamanioNombre = strlen(nombrePokemon)+1;
	brokerAppearedPokemon->datos->nombrePokemon = nombrePokemon;
	brokerAppearedPokemon->datos->posX = posX;
	brokerAppearedPokemon->datos->posY = posY;
	brokerAppearedPokemon->id_relativo = id_rel;

	serializar_broker_appeared_pokemon(brokerAppearedPokemon,buffer);

	paquete_a_enviar->buffer= buffer;

	int tamanio_buffer=0;

	void* bufferStream = serializar_paquete(paquete_a_enviar,&tamanio_buffer);
	send(socket_cliente,bufferStream,tamanio_buffer,0);

	free(bufferStream);
	free(brokerAppearedPokemon->datos);
	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);

}

void enviar_caught(int socket_cliente,uint32_t resultado, uint32_t id){

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__CAUGHT_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username)+1;
	paquete_a_enviar->username = username;
	//serializacion de brokerCaughtPokemon

	broker_caught_pokemon* brokerCaughtPokemon = malloc(sizeof(broker_caught_pokemon));
	brokerCaughtPokemon->datos = malloc(sizeof(caught_pokemon));

	brokerCaughtPokemon->datos->puedoAtraparlo = resultado;

	brokerCaughtPokemon->id_relativo = id;

	t_buffer* buffer = malloc(sizeof(t_buffer));
	serializar_broker_caught_pokemon(brokerCaughtPokemon,buffer);

	paquete_a_enviar->buffer = buffer;

	int tamanio_buffer=0;

	void* bufferStream = serializar_paquete(paquete_a_enviar,&tamanio_buffer);

	send(socket_cliente,bufferStream,tamanio_buffer,0);

	free(bufferStream);
	free(brokerCaughtPokemon->datos);
	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
}

void enviar_localized(int socket_cliente, char* nombre, uint32_t paresDePosiciones, uint32_t* posicionesX, uint32_t* posicionesY,uint32_t id) {

	t_paquete* paquete_a_enviar = malloc(sizeof(t_paquete));
	paquete_a_enviar->codigo_operacion = BROKER__LOCALIZED_POKEMON;
	paquete_a_enviar->tamanio_username =strlen(username);
	paquete_a_enviar->username = username;

	t_buffer* buffer = malloc(sizeof(t_buffer));

	broker_localized_pokemon* brokerLocalizedPokemon = malloc(sizeof(broker_appeared_pokemon));
	brokerLocalizedPokemon->datos = malloc(sizeof(localized_pokemon));
	brokerLocalizedPokemon->id_relativo = id;
	brokerLocalizedPokemon->datos->tamanioNombre = strlen(nombre)+1;
	brokerLocalizedPokemon->datos->nombrePokemon = nombre;
	brokerLocalizedPokemon->datos->cantidadPosiciones = paresDePosiciones;
	brokerLocalizedPokemon->datos->posX = posicionesX;
	brokerLocalizedPokemon->datos->posY = posicionesY;

	serializar_broker_localized_pokemon(brokerLocalizedPokemon,buffer);

	paquete_a_enviar->buffer= buffer;

	int tamanio_buffer=0;

	void* bufferStream = serializar_paquete(paquete_a_enviar,&tamanio_buffer);
	send(socket_cliente,bufferStream,tamanio_buffer,0);

	free(bufferStream);
	free(brokerLocalizedPokemon->datos);
	free(paquete_a_enviar->buffer);
	free(paquete_a_enviar);
}

