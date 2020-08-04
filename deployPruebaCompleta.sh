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
echo -e "BLOCK_SIZE=64\nBLOCKS=1024\nMAGIC_NUMBER=TALL_GRASS\n" >> Metadata.bin

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

echo -e "POSICIONES_ENTRENADORES=[1|3,2|3,2|2]
POKEMON_ENTRENADORES=[Pikachu, , ]
OBJETIVOS_ENTRENADORES=[Pikachu|Squirtle,Pikachu|Gengar,Squirtle|Onix]
TIEMPO_RECONEXION=30
RETARDO_CICLO_CPU=3
ALGORITMO_PLANIFICACION=FIFO
QUANTUM=2
ALPHA=0.5
ESTIMACION_INICIAL=5
IP_BROKER=10.108.32.4
PUERTO_BROKER=5003
IP_TEAM=10.108.32.3
PUERTO_TEAM=5002
LOG_FILE=/home/utnso/tp-2020-1c-The-X-Team/Team/log_pruebas_FIFO.txt" >> pruebas_FIFO.config



if test -e pruebas_RR.config; then
    rm pruebas_RR.config
fi

#team2
echo -e "POSICIONES_ENTRENADORES=[2|3,6|5,9|9,9|2,2|9]
POKEMON_ENTRENADORES=[ , , , , ]
OBJETIVOS_ENTRENADORES=[Vaporeon,Jolteon,Flareon,Umbreon,Espeon]
TIEMPO_RECONEXION=30
RETARDO_CICLO_CPU=3
ALGORITMO_PLANIFICACION=RR
QUANTUM=1
ESTIMACION_INICIAL=5
QUANTUM=2
ALPHA=0.5
IP_BROKER=10.108.32.4
PUERTO_BROKER=5003
IP_TEAM=10.108.32.3
PUERTO_TEAM=5005
LOG_FILE=/home/utnso/tp-2020-1c-The-X-Team/Team/log_pruebas_RR.txt" >> pruebas_RR.config

if test -e pruebas_SJFSD.config; then
    rm pruebas_SJFSD.config
fi

#team1
echo -e "POSICIONES_ENTRENADORES=[1|3,2|3,2|2]
POKEMON_ENTRENADORES=[Pikachu, , ]
OBJETIVOS_ENTRENADORES=[Pikachu|Squirtle,Pikachu|Gengar,Squirtle|Onix]
TIEMPO_RECONEXION=30
RETARDO_CICLO_CPU=3
ALGORITMO_PLANIFICACION=SJFSD
QUANTUM=2
ALPHA=0.5
ESTIMACION_INICIAL=5
IP_BROKER=10.108.32.4
PUERTO_BROKER=5003
IP_TEAM=10.108.32.3
PUERTO_TEAM=5002
LOG_FILE=/home/utnso/tp-2020-1c-The-X-Team/Team/log_pruebas_SJFSD.txt" >> pruebas_SJFSD.config


if test -e pruebas_SJFCD.config; then
    rm pruebas_SJFCD.config
fi

#team2
echo -e "POSICIONES_ENTRENADORES=[2|3,6|5,9|9,9|2,2|9]
POKEMON_ENTRENADORES=[ , , , , ]
OBJETIVOS_ENTRENADORES=[Vaporeon,Jolteon,Flareon,Umbreon,Espeon]
TIEMPO_RECONEXION=30
RETARDO_CICLO_CPU=3
ALGORITMO_PLANIFICACION=SJFCD
QUANTUM=1
ESTIMACION_INICIAL=5
QUANTUM=2
ALPHA=0.5
IP_BROKER=10.108.32.4
PUERTO_BROKER=5003
IP_TEAM=10.108.32.3
PUERTO_TEAM=5005
LOG_FILE=/home/utnso/tp-2020-1c-The-X-Team/Team/log_pruebas_SJFCD.txt" >> pruebas_SJFCD.config



#______________________________________________________________ Broker


cd /home/utnso/tp-2020-1c-The-X-Team/Broker/Debug

make clean
make all

unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/home/utnso/tp-2020-1c-The-X-Team/utils/Debug"



# ___________________ TERMINADO


cd
