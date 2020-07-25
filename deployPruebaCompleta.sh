#!/bin/bash

#Las commons ya estan instaladas.
#ESTE DEPLOY ES PARA LAS PRUEBAS COMPLETAS. DESPUES DE LAS INDIVIDUALES

# ________________ FileSystem
# Borro el PuntoMontaje para que solo esten los nuevos datos

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


#_______________________________ INICIO DE MODULOS:



#_________________ GameBoy

cd /home/utnso/tp-2020-1c-The-X-Team/GameBoy/Debug

make clean
make all

# no cambia

#__________________ GameCard

cd /home/utnso/tp-2020-1c-The-X-Team/GameCard/Debug

make clean
make all
# no cambia

#_______________________     TEAMS:


cd /home/utnso/tp-2020-1c-The-X-Team/Team/Debug


make clean
make all

unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/home/utnso/tp-2020-1c-The-X-Team/utils/Debug"


#--------- Creo las configs de las pruebas en Team:

#cd /home/utnso/tp-2020-1c-The-X-Team/Team/Debug

if test -e pruebas_FIFO.config; then
    rm pruebas_FIFO.config
fi

#team1

echo -e "POSICIONES_ENTRENADORES=[1|3,2|3,2|2]\n
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



if test -e pruebas_RR.config; then
    rm pruebas_RR.config
fi

#team2
echo -e "POSICIONES_ENTRENADORES=[2|3,6|5,9|9,9|2,2|9]\n
POKEMON_ENTRENADORES=[ , , , , ]\n
OBJETIVOS_ENTRENADORES=[Vaporeon,Jolteon,Flareon,Umbreon,Espeon]\n
TIEMPO_RECONEXION=30\n
RETARDO_CICLO_CPU=5\n
ALGORITMO_PLANIFICACION=RR\n
QUANTUM=1\n
ESTIMACION_INICIAL=5\n
QUANTUM=2\n
ALPHA=0.5\n
IP_BROKER=127.0.0.1\n
PUERTO_BROKER=5003\n
IP_TEAM=127.0.0.2\n
PUERTO_TEAM=5005\n
LOG_FILE=/home/utnso/tp-2020-1c-The-X-Team/Team/log_pruebas_FIFO.txt\n" >> pruebas_RR.config

if test -e pruebas_SJFSD.config; then
    rm pruebas_SJFSD.config
fi

#team1
echo -e "POSICIONES_ENTRENADORES=[1|3,2|3,2|2]\n
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
IP_TEAM=127.0.0.2\n
PUERTO_TEAM=5002\n
LOG_FILE=/home/utnso/tp-2020-1c-The-X-Team/Team/log_pruebas_FIFO.txt\n" >> pruebas_SJFSD.config


if test -e pruebas_SJFCD.config; then
    rm pruebas_SJFCD.config
fi

#team2
echo -e "POSICIONES_ENTRENADORES=[2|3,6|5,9|9,9|2,2|9]\n
POKEMON_ENTRENADORES=[ , , , , ]\n
OBJETIVOS_ENTRENADORES=[Vaporeon,Jolteon,Flareon,Umbreon,Espeon]\n
TIEMPO_RECONEXION=30\n
RETARDO_CICLO_CPU=5\n
ALGORITMO_PLANIFICACION=SJFCD\n
QUANTUM=1\n
ESTIMACION_INICIAL=5\n
QUANTUM=2\n
ALPHA=0.5\n
IP_BROKER=127.0.0.1\n
PUERTO_BROKER=5003\n
IP_TEAM=127.0.0.2\n
PUERTO_TEAM=5005\n
LOG_FILE=/home/utnso/tp-2020-1c-The-X-Team/Team/log_pruebas_FIFO.txt\n" >> pruebas_SJFCD.config



#______________________________________________________________ Broker


cd /home/utnso/tp-2020-1c-The-X-Team/Broker/Debug

make clean
make all

unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/home/utnso/tp-2020-1c-The-X-Team/utils/Debug"



# ___________________ TERMINADO


cd
