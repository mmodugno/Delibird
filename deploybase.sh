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

make all


#_________________ GameBoy

cd /home/utnso/tp-2020-1c-The-X-Team/GameBoy/Debug

make all

cd ..

if test -e gameboy.config; then
    rm gameboy.config
fi

echo -e "IP_BROKER=127.0.0.1\n
IP_TEAM=127.0.0.2\n
IP_GAMECARD=127.0.0.3\n
PUERTO_BROKER=5003\n
PUERTO_TEAM=5002\n
PUERTO_GAMECARD=5001\n
LOG_FILE=/home/utnso/gameboy.log" >> gameboy.config

#Ya estamos en la carpeta del debug, entonces compilamos gameboy desde aca:


#make clean
#make all


#__________________ GameCard


cd /home/utnso/tp-2020-1c-The-X-Team/GameCard/Debug

make all

cd ..

if test -e gamecard.config; then
    rm gamecard.config
fi

echo -e "TIEMPO_DE_REINTENTO_CONEXION=10\n
TIEMPO_DE_REINTENTO_OPERACION=5\n
TIEMPO_RETARDO_OPERACION=5\n
PUNTO_MONTAJE_TALLGRASS=/home/utnso/PuntoMontaje\n
IP_BROKER=127.0.0.1\n
PUERTO_BROKER=6009\n" >> gamecard.config

#Ya estamos en la carpeta del debug, entonces compilamos gamecard desde aca:

#cd ../../Debug

#make clean
#make all

#__________________ Teams

cd /home/utnso/tp-2020-1c-The-X-Team/Team/Debug

#consideramos que el Team default es FIFO

make all

cd ../../

cp Team/ Team_RR

cd /home/utnso/tp-2020-1c-The-X-Team/Team


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
LOG_FILE=/home/utnso/tp-2020-1c-The-X-Team/Team/log_pruebas_FIFO.txt\n" >> pruebas_FIFO.config


cd ../Team_RR/


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
LOG_FILE=/home/utnso/tp-2020-1c-The-X-Team/Team/log_pruebas_FIFO.txt\n" >> pruebas_RR.config


#__________________ FIN

cd 
