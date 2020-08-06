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

echo -e "IP_BROKER=10.108.32.4
IP_TEAM=10.108.32.3
IP_GAMECARD=10.108.32.2
IP_GAMEBOY=10.108.32.4
PUERTO_BROKER=5003
PUERTO_TEAM=5002
PUERTO_GAMECARD=5001
PUERTO_GAMEBOY=5009
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

echo -e "TIEMPO_DE_REINTENTO_CONEXION=10
TIEMPO_DE_REINTENTO_OPERACION=5
TIEMPO_RETARDO_OPERACION=2
PUNTO_MONTAJE_TALLGRASS=/home/utnso/PuntoMontaje
IP_BROKER=10.108.32.4
IP_GAMECARD=10.108.32.2
PUERTO_BROKER=5003
PUERTO_GAMECARD=5001
LOG_FILE=/home/utnso/gamecard.log" >> gamecard.config



#______________________________________________________________ Teams

cd /home/utnso/tp-2020-1c-The-X-Team/Team/Debug

#consideramos que el Team default es FIFO

make clean
make all

unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/home/utnso/tp-2020-1c-The-X-Team/utils/Debug"



#--------- Creo las configs de las pruebas en Team:

cd /home/utnso/tp-2020-1c-The-X-Team/Team/Debug

if test -e pruebas_FIFO.config; then
    rm pruebas_FIFO.config
fi

echo -e "POSICIONES_ENTRENADORES=[1|3,2|3,3|2]
POKEMON_ENTRENADORES=[Pikachu, , ]
OBJETIVOS_ENTRENADORES=[Pikachu|Squirtle,Pikachu|Gengar,Squirtle|Onix]
TIEMPO_RECONEXION=30
RETARDO_CICLO_CPU=3
ALGORITMO_PLANIFICACION=FIFO
QUANTUM=2
ALPHA=0.5
ESTIMACION_INICIAL=5
IP_BROKER=10.108.32.4
IP_TEAM=10.108.32.3
PUERTO_BROKER=5003
PUERTO_TEAM=5002
LOG_FILE=/home/utnso/tp-2020-1c-The-X-Team/Team/log_pruebas_FIFO.txt" >> pruebas_FIFO.config




if test -e pruebas_RR.config; then
    rm pruebas_RR.config
fi

echo -e "POSICIONES_ENTRENADORES=[1|3,2|3,3|2]
POKEMON_ENTRENADORES=[Pikachu, , ]
OBJETIVOS_ENTRENADORES=[Pikachu|Squirtle,Pikachu|Gengar,Squirtle|Onix]
TIEMPO_RECONEXION=30
RETARDO_CICLO_CPU=3
ALGORITMO_PLANIFICACION=RR
QUANTUM=2
ALPHA=0.5
ESTIMACION_INICIAL=5
IP_BROKER=10.108.32.4
IP_TEAM=10.108.32.3
PUERTO_BROKER=5003
PUERTO_TEAM=5002
LOG_FILE=/home/utnso/tp-2020-1c-The-X-Team/Team/log_pruebas_RR.txt" >> pruebas_RR.config



if test -e pruebas_SJFSD.config; then
    rm pruebas_SJFSD.config
fi

echo -e "POSICIONES_ENTRENADORES=[1|3,2|3,3|2]
POKEMON_ENTRENADORES=[Pikachu, , ]
OBJETIVOS_ENTRENADORES=[Pikachu|Squirtle,Pikachu|Gengar,Squirtle|Onix]
TIEMPO_RECONEXION=30
RETARDO_CICLO_CPU=3
ALGORITMO_PLANIFICACION=SJFSD
QUANTUM=2
ALPHA=0.5
ESTIMACION_INICIAL=5
IP_BROKER=10.108.32.4
IP_TEAM=10.108.32.3
PUERTO_BROKER=5003
PUERTO_TEAM=5002
LOG_FILE=/home/utnso/tp-2020-1c-The-X-Team/Team/log_pruebas_SJFSD.txt" >> pruebas_SJFSD.config



if test -e pruebas_SJFCD.config; then
    rm pruebas_SJFCD.config
fi

echo -e "POSICIONES_ENTRENADORES=[1|3,2|3,3|2]
POKEMON_ENTRENADORES=[Pikachu, , ]
OBJETIVOS_ENTRENADORES=[Pikachu|Squirtle,Pikachu|Gengar,Squirtle|Onix]
TIEMPO_RECONEXION=30
RETARDO_CICLO_CPU=3
ALGORITMO_PLANIFICACION=SJFCD
QUANTUM=1
ALPHA=0.5
ESTIMACION_INICIAL=5
IP_BROKER=10.108.32.4
IP_TEAM=10.108.32.3
PUERTO_BROKER=5003
PUERTO_TEAM=5002
LOG_FILE=/home/utnso/tp-2020-1c-The-X-Team/Team/log_pruebas_SJFCD.txt" >> pruebas_SJFCD.config


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

echo -e "TAMANO_MEMORIA=64
TAMANO_MINIMO_PARTICION=4
ALGORITMO_MEMORIA=PARTICIONES
ALGORITMO_REEMPLAZO=FIFO
ALGORITMO_PARTICION_LIBRE=FF
IP_BROKER=10.108.32.4
IP_TEAM=10.108.32.3
IP_GAMEBOY=10.108.32.4
IP_GAMECARD=10.108.32.2
PUERTO_BROKER=5003
PUERTO_TEAM=5002
PUERTO_TEAM2=5005
PUERTO_GAMEBOY=5009
PUERTO_GAMECARD=5001
FRECUENCIA_COMPACTACION=10
LOG_FILE=/home/utnso/log_broker.log" >> broker.config


cd ../../Consolidacion_LRU/Debug

if test -e broker.config; then
    rm broker.config
fi

echo -e "TAMANO_MEMORIA=64
TAMANO_MINIMO_PARTICION=4
ALGORITMO_MEMORIA=PARTICIONES
ALGORITMO_REEMPLAZO=LRU
ALGORITMO_PARTICION_LIBRE=FF
IP_BROKER=10.108.32.4
PUERTO_BROKER=5003
IP_TEAM=10.108.32.3
PUERTO_TEAM=5002
PUERTO_TEAM2=5005
IP_GAMEBOY=10.108.32.4
PUERTO_GAMEBOY=5009
IP_GAMECARD=10.108.32.2
PUERTO_GAMECARD=5001
FRECUENCIA_COMPACTACION=10
LOG_FILE=/home/utnso/log_broker.log" >> broker.config


cd ../../Compactacion_FIFO/Debug

if test -e broker.config; then
    rm broker.config
fi

echo -e "TAMANO_MEMORIA=64
TAMANO_MINIMO_PARTICION=4
ALGORITMO_MEMORIA=PARTICIONES
ALGORITMO_REEMPLAZO=FIFO
ALGORITMO_PARTICION_LIBRE=FF
IP_BROKER=10.108.32.4
PUERTO_BROKER=5003
IP_TEAM=10.108.32.3
PUERTO_TEAM=5002
PUERTO_TEAM2=5005
IP_GAMEBOY=10.108.32.4
PUERTO_GAMEBOY=5009
IP_GAMECARD=10.108.32.2
PUERTO_GAMECARD=5001
FRECUENCIA_COMPACTACION=1
LOG_FILE=/home/utnso/log_broker.log" >> broker.config


cd ../../Compactacion_LRU/Debug

if test -e broker.config; then
    rm broker.config
fi

echo -e "TAMANO_MEMORIA=64
TAMANO_MINIMO_PARTICION=4
ALGORITMO_MEMORIA=PARTICIONES
ALGORITMO_REEMPLAZO=LRU
ALGORITMO_PARTICION_LIBRE=FF
IP_BROKER=10.108.32.4
PUERTO_BROKER=5003
IP_TEAM=10.108.32.3
PUERTO_TEAM=5002
PUERTO_TEAM2=5005
IP_GAMEBOY=10.108.32.4
PUERTO_GAMEBOY=5009
IP_GAMECARD=10.108.32.2
PUERTO_GAMECARD=5001
FRECUENCIA_COMPACTACION=1
LOG_FILE=/home/utnso/log_broker.log" >> broker.config



cd ../../BS_FIFO/Debug

if test -e broker.config; then
    rm broker.config
fi

echo -e "TAMANO_MEMORIA=64
TAMANO_MINIMO_PARTICION=4
ALGORITMO_MEMORIA=BS
ALGORITMO_REEMPLAZO=FIFO
ALGORITMO_PARTICION_LIBRE=FF
IP_BROKER=10.108.32.4
PUERTO_BROKER=5003
IP_TEAM=10.108.32.3
PUERTO_TEAM=5002
PUERTO_TEAM2=5005
IP_GAMEBOY=10.108.32.4
PUERTO_GAMEBOY=5009
IP_GAMECARD=10.108.32.2
PUERTO_GAMECARD=5001
FRECUENCIA_COMPACTACION=1
LOG_FILE=/home/utnso/log_broker.log" >> broker.config


cd ../../BS_LRU/Debug

if test -e broker.config; then
    rm broker.config
fi

echo -e "TAMANO_MEMORIA=64
TAMANO_MINIMO_PARTICION=4
ALGORITMO_MEMORIA=BS
ALGORITMO_REEMPLAZO=LRU
ALGORITMO_PARTICION_LIBRE=FF
IP_BROKER=10.108.32.4
PUERTO_BROKER=5003
IP_TEAM=10.108.32.3
PUERTO_TEAM=5002
PUERTO_TEAM2=5005
IP_GAMEBOY=10.108.32.4
PUERTO_GAMEBOY=5009
IP_GAMECARD=10.108.32.2
PUERTO_GAMECARD=5001
FRECUENCIA_COMPACTACION=1
LOG_FILE=/home/utnso/log_broker.log" >> broker.config


#__________________ TERMINADO

cd 
