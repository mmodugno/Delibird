#!/bin/bash

cd
# ________________ Commons Library

if test -d /home/utnso/so-commons-library; then
    cd /home/utnso/so-commons-library/
    sudo make uninstall
    cd ..
    sudo rm -r ./so-commons-library
fi

cd

git clone https://github.com/sisoputnfrba/so-commons-library.git
cd so-commons-library/
    sudo make install

cd


# ________________ FileSystem

if test -d /home/utnso/PuntoMontaje/; then
    sudo rm -r /home/utnso/PuntoMontaje
fi

cd
mkdir -p /home/utnso/PuntoMontaje
cd /home/utnso/PuntoMontaje

mkdir Metadata

cd Metadata/
echo -e "BLOCK_SIZE=64\nBLOCKS=1024\nMAGIC_NUMBER=TALL_GRASS" >> Metadata.bin

cd




# ________________ Shared Library

cd /home/utnso/tp-2020-1c-The-X-Team/utils/Debug

make clean
make all

unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/home/utnso/tp-2020-1c-The-X-Team/utils/Debug"

#_________________ GameBoy

cd /home/utnso/tp-2020-1c-The-X-Team/GameBoy/Debug

make clean
make all

unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/home/utnso/tp-2020-1c-The-X-Team/utils/Debug"


#cd ..

if test -e gameboy.config; then
    rm gameboy.config
fi

echo -e "IP_BROKER=127.0.0.1\n
10.109.32.4\n
IP_GAMECARD=127.0.0.3\n
PUERTO_BROKER=5003\n
PUERTO_TEAM=5002\n
PUERTO_GAMECARD=5001\n
IP_GAMEBOY=127.0.0.9\n
PUERTO_GAMEBOY=5009\n
LOG_FILE=/home/utnso/gameboy.log" >> gameboy.config

#Ya estamos en la carpeta del debug, entonces compilamos gameboy desde aca:


#make clean
#make all


#__________________ GameCard


cd /home/utnso/tp-2020-1c-The-X-Team/GameCard/Debug

make clean
make all

unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/home/utnso/tp-2020-1c-The-X-Team/utils/Debug"

#cd ..

if test -e gamecard.config; then
    rm gamecard.config
fi

echo -e "TIEMPO_DE_REINTENTO_CONEXION=10\n
TIEMPO_DE_REINTENTO_OPERACION=5\n
TIEMPO_RETARDO_OPERACION=5\n
PUNTO_MONTAJE_TALLGRASS=/home/utnso/PuntoMontaje\n
IP_BROKER=127.0.0.1\n
PUERTO_BROKER=6009\n
IP_GAMECARD=127.0.0.3\n
PUERTO_GAMECARD=5001\n" >> gamecard.config

#Ya estamos en la carpeta del debug, entonces compilamos gamecard desde aca:

#cd ../../Debug

#make clean
#make all

#______________________________________________________________ Teams

cd /home/utnso/tp-2020-1c-The-X-Team/Team/Debug

#consideramos que el Team default es FIFO

make clean
make all

unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/home/utnso/tp-2020-1c-The-X-Team/utils/Debug"

#Creamos las distintas carpetas team
#cd ../../
#cp -R Team/ Team_RR
#cp -R Team/ Team_SJFSD
#cp -R Team/ Team_SJFCD

#--------- Creo las configs de las pruebas en Team:

#cd /home/utnso/tp-2020-1c-The-X-Team/Team/Debug

if test -e pruebas_FIFO.config; then
    rm pruebas_FIFO.config
fi

echo -e "POSICIONES_ENTRENADORES=[1|3,2|3,3|2]\n
POKEMON_ENTRENADORES=[Pikachu, , ]\n
OBJETIVOS_ENTRENADORES=[Pikachu|Squirtle,Pikachu|Gengar,Squirtle|Onix]\n
TIEMPO_RECONEXION=30\n
RETARDO_CICLO_CPU=5\n
ALGORITMO_PLANIFICACION=FIFO\n
QUANTUM=2\n
ALPHA=0.5\n
ESTIMACION_INICIAL=5\n
IP_BROKER=127.0.0.1\n
PUERTO_BROKER=5003\n
IP_TEAM=127.0.0.2\n
PUERTO_TEAM=5002\n
LOG_FILE=/home/utnso/tp-2020-1c-The-X-Team/Team/log_pruebas_FIFO.txt\n" >> pruebas_FIFO.config


#cd ../../Team_RR/Debug


if test -e pruebas_RR.config; then
    rm pruebas_RR.config
fi

echo -e "POSICIONES_ENTRENADORES=[1|3,2|3,3|2]\n
POKEMON_ENTRENADORES=[Pikachu, , ]\n
OBJETIVOS_ENTRENADORES=[Pikachu|Squirtle,Pikachu|Gengar,Squirtle|Onix]\n
TIEMPO_RECONEXION=30\n
RETARDO_CICLO_CPU=5\n
ALGORITMO_PLANIFICACION=RR\n
QUANTUM=2\n
ALPHA=0.5\n
ESTIMACION_INICIAL=5\n
IP_BROKER=127.0.0.1\n
PUERTO_BROKER=5003\n
IP_TEAM=127.0.0.2\n
PUERTO_TEAM=5002\n
LOG_FILE=/home/utnso/tp-2020-1c-The-X-Team/Team/log_pruebas_RR.txt\n" >> pruebas_RR.config



#cd ../../Team_SJFSD/Debug

if test -e pruebas_SJFSD.config; then
    rm pruebas_SJFSD.config
fi

echo -e "POSICIONES_ENTRENADORES=[1|3,2|3,3|2]\n
POKEMON_ENTRENADORES=[Pikachu, , ]\n
OBJETIVOS_ENTRENADORES=[Pikachu|Squirtle,Pikachu|Gengar,Squirtle|Onix]\n
TIEMPO_RECONEXION=30\n
RETARDO_CICLO_CPU=5\n
ALGORITMO_PLANIFICACION=SJFSD\n
QUANTUM=2\n
ALPHA=0.5\n
ESTIMACION_INICIAL=5\n
IP_BROKER=127.0.0.1\n
PUERTO_BROKER=5003\n
IP_TEAM=10.109.32.4\n
PUERTO_TEAM=5002\n
LOG_FILE=/home/utnso/tp-2020-1c-The-X-Team/Team/log_pruebas_SJFSD.txt\n" >> pruebas_SJFSD.config



if test -e pruebas_SJFCD.config; then
    rm pruebas_SJFCD.config
fi

echo -e "POSICIONES_ENTRENADORES=[1|3,2|3,3|2]\n
POKEMON_ENTRENADORES=[Pikachu, , ]\n
OBJETIVOS_ENTRENADORES=[Pikachu|Squirtle,Pikachu|Gengar,Squirtle|Onix]\n
TIEMPO_RECONEXION=30\n
RETARDO_CICLO_CPU=5\n
ALGORITMO_PLANIFICACION=SJFCD\n
QUANTUM=2\n
ALPHA=0.5\n
ESTIMACION_INICIAL=5\n
IP_BROKER=127.0.0.1\n
PUERTO_BROKER=5003\n
IP_TEAM=127.0.0.2\n
PUERTO_TEAM=5002\n
LOG_FILE=/home/utnso/tp-2020-1c-The-X-Team/Team/log_pruebas_SJFCD.txt\n" >> pruebas_SJFCD.config


#______________________________________________________________ Broker


cd /home/utnso/tp-2020-1c-The-X-Team/Broker/Debug

make clean
make all

unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/home/utnso/tp-2020-1c-The-X-Team/utils/Debug"

#Creamos las distintas carpetas broker

cd ../../

cp -R Broker/ Consolidacion_FIFO
cp -R Broker/ Consolidacion_LRU
cp -R Broker/ Compactacion_FIFO
cp -R Broker/ Compactacion_LRU
cp -R Broker/ BS_FIFO
cp -R Broker/ BS_LRU

cd  /home/utnso/tp-2020-1c-The-X-Team/Consolidacion_FIFO/Debug

if test -e broker.config; then
    rm broker.config
fi

echo -e "TAMANO_MEMORIA=64\n
TAMANO_MINIMO_PARTICION=4\n
ALGORITMO_MEMORIA=PARTICIONES\n
ALGORITMO_REEMPLAZO=FIFO\n
ALGORITMO_PARTICION_LIBRE=FF\n
IP_BROKER=127.0.0.1\n
PUERTO_BROKER=5003\n
IP_TEAM=127.0.0.2\n
PUERTO_TEAM=5002\n
IP_GAMEBOY=127.0.0.9\n
PUERTO_GAMEBOY=5009\n
IP_GAMECARD=127.0.0.3\n
PUERTO_GAMECARD=5001\n
FRECUENCIA_COMPACTACION=10\n
LOG_FILE=/home/utnso/log_broker.log" >> broker.config


cd ../../Consolidacion_LRU/Debug

if test -e broker.config; then
    rm broker.config
fi

echo -e "TAMANO_MEMORIA=64\n
TAMANO_MINIMO_PARTICION=4\n
ALGORITMO_MEMORIA=PARTICIONES\n
ALGORITMO_REEMPLAZO=LRU\n
ALGORITMO_PARTICION_LIBRE=FF\n
IP_BROKER=127.0.0.1\n
PUERTO_BROKER=5003\n
IP_TEAM=127.0.0.2\n
PUERTO_TEAM=5002\n
IP_GAMEBOY=127.0.0.9\n
PUERTO_GAMEBOY=5009\n
IP_GAMECARD=127.0.0.3\n
PUERTO_GAMECARD=5001\n\n
FRECUENCIA_COMPACTACION=10\n
LOG_FILE=/home/utnso/log_broker.log" >> broker.config


cd ../../Compactacion_FIFO/Debug

if test -e broker.config; then
    rm broker.config
fi

echo -e "TAMANO_MEMORIA=64\n
TAMANO_MINIMO_PARTICION=4\n
ALGORITMO_MEMORIA=PARTICIONES\n
ALGORITMO_REEMPLAZO=FIFO\n
ALGORITMO_PARTICION_LIBRE=FF\n
IP_BROKER=127.0.0.1\n
PUERTO_BROKER=5003\n
IP_TEAM=127.0.0.2\n
PUERTO_TEAM=5002\n
IP_GAMEBOY=127.0.0.9\n
PUERTO_GAMEBOY=5009\n
IP_GAMECARD=127.0.0.3\n
PUERTO_GAMECARD=5001\n\n
FRECUENCIA_COMPACTACION=1\n
LOG_FILE=/home/utnso/log_broker.log" >> broker.config


cd ../../Compactacion_LRU/Debug

if test -e broker.config; then
    rm broker.config
fi

echo -e "TAMANO_MEMORIA=64\n
TAMANO_MINIMO_PARTICION=4\n
ALGORITMO_MEMORIA=PARTICIONES\n
ALGORITMO_REEMPLAZO=LRU\n
ALGORITMO_PARTICION_LIBRE=FF\n
IP_BROKER=127.0.0.1\n
PUERTO_BROKER=5003\n
IP_TEAM=127.0.0.2\n
PUERTO_TEAM=5002\n
IP_GAMEBOY=127.0.0.9\n
PUERTO_GAMEBOY=5009\n
IP_GAMECARD=127.0.0.3\n
PUERTO_GAMECARD=5001\n\n
FRECUENCIA_COMPACTACION=1\n
LOG_FILE=/home/utnso/log_broker.log" >> broker.config



cd ../../BS_FIFO/Debug

if test -e broker.config; then
    rm broker.config
fi

echo -e "TAMANO_MEMORIA=64\n
TAMANO_MINIMO_PARTICION=4\n
ALGORITMO_MEMORIA=BS\n
ALGORITMO_REEMPLAZO=FIFO\n
ALGORITMO_PARTICION_LIBRE=FF\n
IP_BROKER=127.0.0.1\n
PUERTO_BROKER=5003\n
IP_TEAM=127.0.0.2\n
PUERTO_TEAM=5002\n
IP_GAMEBOY=127.0.0.9\n
PUERTO_GAMEBOY=5009\n
IP_GAMECARD=127.0.0.3\n
PUERTO_GAMECARD=5001\n\n
FRECUENCIA_COMPACTACION=1\n
LOG_FILE=/home/utnso/log_broker.log" >> broker.config


cd ../../BS_LRU/Debug

if test -e broker.config; then
    rm broker.config
fi

echo -e "TAMANO_MEMORIA=64\n
TAMANO_MINIMO_PARTICION=4\n
ALGORITMO_MEMORIA=BS\n
ALGORITMO_REEMPLAZO=LRU\n
ALGORITMO_PARTICION_LIBRE=FF\n
IP_BROKER=127.0.0.1\n
PUERTO_BROKER=5003\n
IP_TEAM=127.0.0.2\n
PUERTO_TEAM=5002\n
IP_GAMEBOY=127.0.0.9\n
PUERTO_GAMEBOY=5009\n
IP_GAMECARD=127.0.0.3\n
PUERTO_GAMECARD=5001\n\n
FRECUENCIA_COMPACTACION=1\n
LOG_FILE=/home/utnso/log_broker.log" >> broker.config


#__________________ TERMINADO

cd 
