#!/bin/sh
unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/home/utnso/tp-2020-1c-The-X-Team/utils/Debug"

./gameboy BROKER NEW_POKEMON Onix 2 8 1
./gameboy BROKER NEW_POKEMON Gengar 6 6 1
./gameboy BROKER NEW_POKEMON Squirtle 5 5 1

./gameboy BROKER NEW_POKEMON Umbreon 10 6 1
./gameboy BROKER NEW_POKEMON Espeon 7 1 1
./gameboy BROKER NEW_POKEMON Vaporeon 4 10 1
